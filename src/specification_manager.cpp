#include "specification_manager.h"
#include "xml_parser.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QDebug>

namespace mscs {

SpecificationManager::SpecificationManager(QObject *parent)
    : QObject(parent)
{
}

SpecificationManager::~SpecificationManager() {
}

bool SpecificationManager::loadExportConfig(const QString& filePath, ExportConfig& config) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit configError(tr("Не удалось открыть файл: ") + filePath);
        return false;
    }
    
    XmlParser parser;
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        emit configError(tr("Ошибка парсинга XML"));
        file.close();
        return false;
    }
    file.close();
    
    config = parser.parseExportConfig(doc);
    emit configLoaded(config.name);
    
    return true;
}

bool SpecificationManager::saveExportConfig(const QString& filePath, const ExportConfig& config) {
    XmlParser parser;
    QDomDocument doc = parser.createExportConfigXml(config);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        emit configError(tr("Не удалось сохранить файл: ") + filePath);
        return false;
    }
    
    QTextStream out(&file);
    doc.save(out, 4);
    file.close();
    
    emit configSaved(config.name);
    return true;
}

bool SpecificationManager::loadSpecifierConfig(const QString& filePath, SpecifierConfig& config) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit configError(tr("Не удалось открыть файл: ") + filePath);
        return false;
    }
    
    XmlParser parser;
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        emit configError(tr("Ошибка парсинга XML"));
        file.close();
        return false;
    }
    file.close();
    
    config = parser.parseSpecifierConfig(doc);
    emit configLoaded(config.name);
    
    return true;
}

bool SpecificationManager::saveSpecifierConfig(const QString& filePath, const SpecifierConfig& config) {
    XmlParser parser;
    QDomDocument doc = parser.createSpecifierConfigXml(config);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        emit configError(tr("Не удалось сохранить файл: ") + filePath);
        return false;
    }
    
    QTextStream out(&file);
    doc.save(out, 4);
    file.close();
    
    emit configSaved(config.name);
    return true;
}

QList<QString> SpecificationManager::getExportConfigs(ModuleType module) {
    QList<QString> configs;
    QString path = getModulePath(module, true);
    
    QDir dir(path);
    if (!dir.exists()) {
        return configs;
    }
    
    dir.setNameFilters(QStringList() << "*.xml");
    QFileInfoList files = dir.entryInfoList();
    
    for (const auto& file : files) {
        configs.append(file.baseName());
    }
    
    return configs;
}

QList<QString> SpecificationManager::getSpecifierConfigs(ModuleType module) {
    QList<QString> configs;
    QString path = getModulePath(module, false);
    
    QDir dir(path);
    if (!dir.exists()) {
        return configs;
    }
    
    dir.setNameFilters(QStringList() << "*.xml");
    QFileInfoList files = dir.entryInfoList();
    
    for (const auto& file : files) {
        configs.append(file.baseName());
    }
    
    return configs;
}

ExportConfig SpecificationManager::createDefaultExportConfig(ModuleType module) {
    ExportConfig config;
    config.id = QUuid::createUuid().toString();
    config.moduleType = module;
    
    switch (module) {
        case ModuleType::AEC:
            config.name = "AEC Export Config";
            break;
        case ModuleType::Piping:
            config.name = "Piping Export Config";
            break;
        case ModuleType::Cable:
            config.name = "Cable Export Config";
            break;
        case ModuleType::HVAC:
            config.name = "HVAC Export Config";
            break;
        default:
            config.name = "Default Export Config";
    }
    
    // Профиль по умолчанию
    ExportProfile defaultProfile;
    defaultProfile.id = QUuid::createUuid().toString();
    defaultProfile.name = "Default Profile";
    defaultProfile.description = "Профиль экспорта по умолчанию";
    defaultProfile.enabled = true;
    defaultProfile.fields << "Name" << "Type" << "Quantity" << "Unit";
    
    config.profiles.append(defaultProfile);
    config.defaultProfile = defaultProfile.name;
    
    return config;
}

SpecifierConfig SpecificationManager::createDefaultSpecifierConfig(ModuleType module) {
    SpecifierConfig config;
    config.id = QUuid::createUuid().toString();
    config.moduleType = module;
    
    switch (module) {
        case ModuleType::AEC:
            config.name = "AEC Specifier Config";
            break;
        case ModuleType::Piping:
            config.name = "Piping Specifier Config";
            break;
        case ModuleType::Cable:
            config.name = "Cable Specifier Config";
            break;
        case ModuleType::HVAC:
            config.name = "HVAC Specifier Config";
            break;
        default:
            config.name = "Default Specifier Config";
    }
    
    config.allowEditing = true;
    config.visibleFields << "Name" << "Type" << "Quantity" << "Unit";
    config.editableFields << "Quantity" << "Unit";
    
    return config;
}

bool SpecificationManager::validateExportConfig(const ExportConfig& config, QStringList& errors) {
    bool valid = true;
    
    if (config.name.isEmpty()) {
        errors << tr("Имя конфигурации не может быть пустым");
        valid = false;
    }
    
    if (config.profiles.isEmpty()) {
        errors << tr("Конфигурация должна содержать хотя бы один профиль");
        valid = false;
    }
    
    for (const auto& profile : config.profiles) {
        if (profile.name.isEmpty()) {
            errors << tr("Имя профиля не может быть пустым");
            valid = false;
        }
        
        if (profile.fields.isEmpty()) {
            errors << tr("Профиль '%1' должен содержать хотя бы одно поле").arg(profile.name);
            valid = false;
        }
    }
    
    return valid;
}

bool SpecificationManager::validateSpecifierConfig(const SpecifierConfig& config, QStringList& errors) {
    bool valid = true;
    
    if (config.name.isEmpty()) {
        errors << tr("Имя конфигурации не может быть пустым");
        valid = false;
    }
    
    if (config.exportProfileRef.isEmpty()) {
        errors << tr("Не указан профиль экспорта для спецификатора");
        valid = false;
    }
    
    if (config.exportProfileRefOut.isEmpty()) {
        errors << tr("Не указан профиль экспорта для экспорта");
        valid = false;
    }
    
    return valid;
}

bool SpecificationManager::linkSpecifierToExport(SpecifierConfig& specifier, const ExportConfig& exportConfig) {
    if (exportConfig.profiles.isEmpty()) {
        return false;
    }
    
    // Установка первого профиля как значения по умолчанию
    specifier.exportProfileRef = exportConfig.profiles.first().id;
    specifier.exportProfileRefOut = exportConfig.profiles.first().id;
    
    return true;
}

QString SpecificationManager::getModulePath(ModuleType module, bool isExport) {
    QString base = qgetenv("APPDATA");
    if (base.isEmpty()) {
        base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    base += "/CSoft/Model Studio CS/";
    
    switch (module) {
        case ModuleType::AEC:
            base += "AEC/";
            break;
        case ModuleType::Piping:
            base += "Piping/";
            break;
        case ModuleType::Cable:
            base += "Cable/";
            break;
        case ModuleType::HVAC:
            base += "HVAC/";
            break;
        default:
            base += "AEC/";
    }
    
    base += isExport ? "Export" : "Specifications";
    return base;
}

} // namespace mscs
