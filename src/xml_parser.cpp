#include "xml_parser.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>

namespace mscs {

bool XmlParser::loadFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл:" << filePath;
        return false;
    }
    
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    
    if (!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
        qWarning() << "Ошибка парсинга XML:" << errorMsg 
                   << "строка:" << errorLine << "столбец:" << errorColumn;
        file.close();
        return false;
    }
    
    file.close();
    return true;
}

bool XmlParser::saveToFile(const QString& filePath) {
    // Этот метод будет вызываться с уже готовым документом
    // Реализация в методах createExportConfigXml/createSpecifierConfigXml
    return true;
}

ExportConfig XmlParser::parseExportConfig(const QDomDocument& doc) {
    ExportConfig config;
    
    QDomElement root = doc.documentElement();
    config.name = root.attribute("name", root.tagName());
    config.id = root.attribute("id", QUuid::createUuid().toString());
    config.moduleType = detectModuleType(doc);
    
    // Парсинг профилей
    QDomNodeList profileNodes = root.elementsByTagName("Profile");
    for (int i = 0; i < profileNodes.count(); ++i) {
        QDomElement profileElem = profileNodes.at(i).toElement();
        ExportProfile profile;
        profile.id = profileElem.attribute("id", QString::number(i));
        profile.name = profileElem.attribute("name", "Профиль " + QString::number(i));
        profile.description = profileElem.attribute("description", "");
        profile.enabled = profileElem.attribute("enabled", "true") == "true";
        
        // Парсинг полей
        QDomNodeList fieldNodes = profileElem.elementsByTagName("Field");
        for (int j = 0; j < fieldNodes.count(); ++j) {
            profile.fields.append(fieldNodes.at(j).toElement().text());
        }
        
        // Парсинг настроек
        QDomNodeList settingsNodes = profileElem.elementsByTagName("Settings");
        if (!settingsNodes.isEmpty()) {
            profile.settings = parseSettings(settingsNodes.at(0).toElement());
        }
        
        config.profiles.append(profile);
    }
    
    // Настройки экспорта по умолчанию
    QDomNodeList defaultNodes = root.elementsByTagName("DefaultProfile");
    if (!defaultNodes.isEmpty()) {
        config.defaultProfile = defaultNodes.at(0).toElement().text();
    }
    
    return config;
}

SpecifierConfig XmlParser::parseSpecifierConfig(const QDomDocument& doc) {
    SpecifierConfig config;
    
    QDomElement root = doc.documentElement();
    config.name = root.attribute("name", root.tagName());
    config.id = root.attribute("id", QUuid::createUuid().toString());
    config.moduleType = detectModuleType(doc);
    
    // Ссылки на профили экспорта
    config.exportProfileRef = readTextElement(root, "ExportProfileRef", "");
    config.exportProfileRefOut = readTextElement(root, "ExportProfileRefOut", "");
    
    // Разрешение редактирования
    config.allowEditing = root.attribute("allowEditing", "true") == "true";
    
    // Видимые поля
    QDomNodeList visibleNodes = root.elementsByTagName("VisibleFields");
    if (!visibleNodes.isEmpty()) {
        QDomElement visibleElem = visibleNodes.at(0).toElement();
        QDomNodeList fieldNodes = visibleElem.elementsByTagName("Field");
        for (int i = 0; i < fieldNodes.count(); ++i) {
            config.visibleFields.append(fieldNodes.at(i).toElement().text());
        }
    }
    
    // Редактируемые поля
    QDomNodeList editableNodes = root.elementsByTagName("EditableFields");
    if (!editableNodes.isEmpty()) {
        QDomElement editableElem = editableNodes.at(0).toElement();
        QDomNodeList fieldNodes = editableElem.elementsByTagName("Field");
        for (int i = 0; i < fieldNodes.count(); ++i) {
            config.editableFields.append(fieldNodes.at(i).toElement().text());
        }
    }
    
    // Настройки представления
    QDomNodeList viewSettingsNodes = root.elementsByTagName("ViewSettings");
    if (!viewSettingsNodes.isEmpty()) {
        config.viewSettings = parseSettings(viewSettingsNodes.at(0).toElement());
    }
    
    return config;
}

QDomDocument XmlParser::createExportConfigXml(const ExportConfig& config) {
    QDomDocument doc("ExportConfig");
    
    QDomElement root = doc.createElement("ExportConfig");
    root.setAttribute("name", config.name);
    root.setAttribute("id", config.id);
    doc.appendChild(root);
    
    // Профили
    for (const auto& profile : config.profiles) {
        QDomElement profileElem = doc.createElement("Profile");
        profileElem.setAttribute("id", profile.id);
        profileElem.setAttribute("name", profile.name);
        profileElem.setAttribute("description", profile.description);
        profileElem.setAttribute("enabled", profile.enabled ? "true" : "false");
        
        // Поля
        for (const auto& field : profile.fields) {
            QDomElement fieldElem = doc.createElement("Field");
            fieldElem.appendChild(doc.createTextNode(field));
            profileElem.appendChild(fieldElem);
        }
        
        // Настройки
        if (!profile.settings.isEmpty()) {
            QDomElement settingsElem = doc.createElement("Settings");
            writeSettings(settingsElem, profile.settings);
            profileElem.appendChild(settingsElem);
        }
        
        root.appendChild(profileElem);
    }
    
    // Профиль по умолчанию
    if (!config.defaultProfile.isEmpty()) {
        QDomElement defaultElem = doc.createElement("DefaultProfile");
        defaultElem.appendChild(doc.createTextNode(config.defaultProfile));
        root.appendChild(defaultElem);
    }
    
    return doc;
}

QDomDocument XmlParser::createSpecifierConfigXml(const SpecifierConfig& config) {
    QDomDocument doc("SpecifierConfig");
    
    QDomElement root = doc.createElement("SpecifierConfig");
    root.setAttribute("name", config.name);
    root.setAttribute("id", config.id);
    root.setAttribute("allowEditing", config.allowEditing ? "true" : "false");
    doc.appendChild(root);
    
    // Ссылки на профили экспорта
    if (!config.exportProfileRef.isEmpty()) {
        QDomElement refElem = doc.createElement("ExportProfileRef");
        refElem.appendChild(doc.createTextNode(config.exportProfileRef));
        root.appendChild(refElem);
    }
    
    if (!config.exportProfileRefOut.isEmpty()) {
        QDomElement refOutElem = doc.createElement("ExportProfileRefOut");
        refOutElem.appendChild(doc.createTextNode(config.exportProfileRefOut));
        root.appendChild(refOutElem);
    }
    
    // Видимые поля
    if (!config.visibleFields.isEmpty()) {
        QDomElement visibleElem = doc.createElement("VisibleFields");
        for (const auto& field : config.visibleFields) {
            QDomElement fieldElem = doc.createElement("Field");
            fieldElem.appendChild(doc.createTextNode(field));
            visibleElem.appendChild(fieldElem);
        }
        root.appendChild(visibleElem);
    }
    
    // Редактируемые поля
    if (!config.editableFields.isEmpty()) {
        QDomElement editableElem = doc.createElement("EditableFields");
        for (const auto& field : config.editableFields) {
            QDomElement fieldElem = doc.createElement("Field");
            fieldElem.appendChild(doc.createTextNode(field));
            editableElem.appendChild(fieldElem);
        }
        root.appendChild(editableElem);
    }
    
    // Настройки представления
    if (!config.viewSettings.isEmpty()) {
        QDomElement viewSettingsElem = doc.createElement("ViewSettings");
        writeSettings(viewSettingsElem, config.viewSettings);
        root.appendChild(viewSettingsElem);
    }
    
    return doc;
}

ExportProfile XmlParser::getExportProfileByName(const QDomDocument& doc, const QString& profileName) {
    ExportProfile profile;
    QDomElement root = doc.documentElement();
    QDomNodeList profileNodes = root.elementsByTagName("Profile");
    
    for (int i = 0; i < profileNodes.count(); ++i) {
        QDomElement profileElem = profileNodes.at(i).toElement();
        QString name = profileElem.attribute("name", "");
        if (name == profileName) {
            profile.id = profileElem.attribute("id", QString::number(i));
            profile.name = name;
            profile.description = profileElem.attribute("description", "");
            profile.enabled = profileElem.attribute("enabled", "true") == "true";
            
            QDomNodeList fieldNodes = profileElem.elementsByTagName("Field");
            for (int j = 0; j < fieldNodes.count(); ++j) {
                profile.fields.append(fieldNodes.at(j).toElement().text());
            }
            
            QDomNodeList settingsNodes = profileElem.elementsByTagName("Settings");
            if (!settingsNodes.isEmpty()) {
                profile.settings = parseSettings(settingsNodes.at(0).toElement());
            }
            
            break;
        }
    }
    
    return profile;
}

void XmlParser::updateExportProfile(QDomDocument& doc, const ExportProfile& profile) {
    QDomElement root = doc.documentElement();
    QDomNodeList profileNodes = root.elementsByTagName("Profile");
    
    for (int i = 0; i < profileNodes.count(); ++i) {
        QDomElement profileElem = profileNodes.at(i).toElement();
        QString id = profileElem.attribute("id", "");
        if (id == profile.id) {
            profileElem.setAttribute("name", profile.name);
            profileElem.setAttribute("description", profile.description);
            profileElem.setAttribute("enabled", profile.enabled ? "true" : "false");
            
            // Обновление полей
            QDomNodeList fieldNodes = profileElem.elementsByTagName("Field");
            for (int j = fieldNodes.count() - 1; j >= 0; --j) {
                profileElem.removeChild(fieldNodes.at(j));
            }
            
            for (const auto& field : profile.fields) {
                QDomElement fieldElem = doc.createElement("Field");
                fieldElem.appendChild(doc.createTextNode(field));
                profileElem.appendChild(fieldElem);
            }
            
            // Обновление настроек
            QDomNodeList settingsNodes = profileElem.elementsByTagName("Settings");
            if (!settingsNodes.isEmpty()) {
                profileElem.removeChild(settingsNodes.at(0));
            }
            
            if (!profile.settings.isEmpty()) {
                QDomElement settingsElem = doc.createElement("Settings");
                writeSettings(settingsElem, profile.settings);
                profileElem.appendChild(settingsElem);
            }
            
            break;
        }
    }
}

bool XmlParser::validateStructure(const QDomDocument& doc, const QString& expectedRoot) {
    QDomElement root = doc.documentElement();
    return root.tagName() == expectedRoot;
}

QList<ExportProfile> XmlParser::getAllProfiles(const QDomDocument& doc) {
    QList<ExportProfile> profiles;
    QDomElement root = doc.documentElement();
    QDomNodeList profileNodes = root.elementsByTagName("Profile");
    
    for (int i = 0; i < profileNodes.count(); ++i) {
        QDomElement profileElem = profileNodes.at(i).toElement();
        ExportProfile profile;
        profile.id = profileElem.attribute("id", QString::number(i));
        profile.name = profileElem.attribute("name", "Профиль " + QString::number(i));
        profile.description = profileElem.attribute("description", "");
        profile.enabled = profileElem.attribute("enabled", "true") == "true";
        
        QDomNodeList fieldNodes = profileElem.elementsByTagName("Field");
        for (int j = 0; j < fieldNodes.count(); ++j) {
            profile.fields.append(fieldNodes.at(j).toElement().text());
        }
        
        QDomNodeList settingsNodes = profileElem.elementsByTagName("Settings");
        if (!settingsNodes.isEmpty()) {
            profile.settings = parseSettings(settingsNodes.at(0).toElement());
        }
        
        profiles.append(profile);
    }
    
    return profiles;
}

QString XmlParser::readTextElement(const QDomElement& parent, const QString& tagName, const QString& defaultValue) {
    QDomNodeList nodes = parent.elementsByTagName(tagName);
    if (!nodes.isEmpty()) {
        return nodes.at(0).toElement().text();
    }
    return defaultValue;
}

void XmlParser::writeTextElement(QDomElement& parent, const QString& tagName, const QString& value) {
    QDomDocument doc = parent.ownerDocument();
    QDomElement elem = doc.createElement(tagName);
    elem.appendChild(doc.createTextNode(value));
    parent.appendChild(elem);
}

QMap<QString, QVariant> XmlParser::parseSettings(const QDomElement& settingsElement) {
    QMap<QString, QVariant> settings;
    QDomNodeList settingNodes = settingsElement.childNodes();
    
    for (int i = 0; i < settingNodes.count(); ++i) {
        QDomNode node = settingNodes.at(i);
        if (node.isElement()) {
            QDomElement elem = node.toElement();
            QString key = elem.attribute("key", elem.tagName());
            QString value = elem.text();
            QString type = elem.attribute("type", "string");
            
            if (type == "int") {
                settings[key] = value.toInt();
            } else if (type == "double") {
                settings[key] = value.toDouble();
            } else if (type == "bool") {
                settings[key] = (value == "true" || value == "1");
            } else {
                settings[key] = value;
            }
        }
    }
    
    return settings;
}

void XmlParser::writeSettings(QDomElement& parent, const QMap<QString, QVariant>& settings) {
    QDomDocument doc = parent.ownerDocument();
    
    for (auto it = settings.begin(); it != settings.end(); ++it) {
        QDomElement settingElem = doc.createElement("Setting");
        settingElem.setAttribute("key", it.key());
        
        QString value;
        QString type;
        
        if (it.value().canConvert<int>()) {
            value = QString::number(it.value().toInt());
            type = "int";
        } else if (it.value().canConvert<double>()) {
            value = QString::number(it.value().toDouble(), 'f', 6);
            type = "double";
        } else if (it.value().canConvert<bool>()) {
            value = it.value().toBool() ? "true" : "false";
            type = "bool";
        } else {
            value = it.value().toString();
            type = "string";
        }
        
        settingElem.setAttribute("type", type);
        settingElem.appendChild(doc.createTextNode(value));
        parent.appendChild(settingElem);
    }
}

ModuleType XmlParser::detectModuleType(const QDomDocument& doc) {
    QDomElement root = doc.documentElement();
    QString moduleName = root.attribute("module", "");
    
    if (moduleName.contains("AEC", Qt::CaseInsensitive) || 
        moduleName.contains("Building", Qt::CaseInsensitive)) {
        return ModuleType::AEC;
    } else if (moduleName.contains("Piping", Qt::CaseInsensitive) ||
               moduleName.contains("Pipeline", Qt::CaseInsensitive)) {
        return ModuleType::Piping;
    } else if (moduleName.contains("Cable", Qt::CaseInsensitive) ||
               moduleName.contains("Electrical", Qt::CaseInsensitive)) {
        return ModuleType::Cable;
    } else if (moduleName.contains("HVAC", Qt::CaseInsensitive) ||
               moduleName.contains("Ventilation", Qt::CaseInsensitive)) {
        return ModuleType::HVAC;
    }
    
    return ModuleType::Unknown;
}

} // namespace mscs
