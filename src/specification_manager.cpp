#include "specification_manager.h"
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

bool SpecificationManager::loadExportProfile(const QString& filePath, ExportProfile& profile) {
    return m_parser.loadExportProfile(filePath, profile);
}

bool SpecificationManager::saveExportProfile(const QString& filePath, const ExportProfile& profile) {
    return m_parser.saveExportProfile(filePath, profile);
}

bool SpecificationManager::loadSpecifierProfile(const QString& filePath, SpecifierProfile& profile) {
    return m_parser.loadSpecifierProfile(filePath, profile);
}

bool SpecificationManager::saveSpecifierProfile(const QString& filePath, const SpecifierProfile& profile) {
    return m_parser.saveSpecifierProfile(filePath, profile);
}

QList<ExportProfile> SpecificationManager::getExportProfiles(ModuleType module) {
    QString path = getModulePath(module, true);
    return m_parser.loadAllExportProfiles(path, module);
}

QList<SpecifierProfile> SpecificationManager::getSpecifierProfiles(ModuleType module) {
    QString path = getModulePath(module, false);
    return m_parser.loadAllSpecifierProfiles(path, module);
}

ExportProfile SpecificationManager::createDefaultExportProfile(ModuleType module) {
    ExportProfile profile;
    
    switch (module) {
        case ModuleType::AEC:
            profile.fileName = "AEC_Default";
            profile.datasetProfile.table.caption = "Набор данных AEC";
            break;
        case ModuleType::Piping:
            profile.fileName = "Piping_Default";
            profile.datasetProfile.table.caption = "Набор данных Piping";
            break;
        case ModuleType::Cable:
            profile.fileName = "Cable_Default";
            profile.datasetProfile.table.caption = "Набор данных Cable";
            break;
        case ModuleType::HVAC:
            profile.fileName = "HVAC_Default";
            profile.datasetProfile.table.caption = "Набор данных HVAC";
            break;
        default:
            profile.fileName = "Default";
            profile.datasetProfile.table.caption = "Набор данных";
    }
    
    // Добавить поле по умолчанию
    DatasetField field;
    field.caption = "Наименование";
    field.data = "PART_NAME";
    field.type = 0;
    field.aggregate = 0;
    field.visible = true;
    profile.datasetProfile.table.fields.append(field);
    
    return profile;
}

SpecifierProfile SpecificationManager::createDefaultSpecifierProfile(ModuleType module) {
    SpecifierProfile profile;
    
    switch (module) {
        case ModuleType::AEC:
            profile.profileName = "AEC_Specifier";
            break;
        case ModuleType::Piping:
            profile.profileName = "Piping_Specifier";
            break;
        case ModuleType::Cable:
            profile.profileName = "Cable_Specifier";
            break;
        case ModuleType::HVAC:
            profile.profileName = "HVAC_Specifier";
            break;
        default:
            profile.profileName = "Default_Specifier";
    }
    
    profile.positionField = "BOM_NUMBER";
    profile.outputProfile = profile.profileName;
    profile.positionStart = 1;
    profile.positionStep = 1;
    profile.positionDivider = ".";
    
    return profile;
}

bool SpecificationManager::validateExportProfile(const ExportProfile& profile, QStringList& errors) {
    bool valid = true;
    
    if (profile.fileName.isEmpty()) {
        errors << tr("Имя файла профиля не может быть пустым");
        valid = false;
    }
    
    if (profile.datasetProfile.table.fields.isEmpty()) {
        errors << tr("Профиль должен содержать хотя бы одно поле");
        valid = false;
    }
    
    for (const auto& field : profile.datasetProfile.table.fields) {
        if (field.data.isEmpty()) {
            errors << tr("Поле должно иметь имя данных (data attribute)");
            valid = false;
        }
    }
    
    return valid;
}

bool SpecificationManager::validateSpecifierProfile(const SpecifierProfile& profile, QStringList& errors) {
    bool valid = true;
    
    if (profile.profileName.isEmpty()) {
        errors << tr("Имя профиля не может быть пустым");
        valid = false;
    }
    
    if (profile.outputProfile.isEmpty()) {
        errors << tr("Не указан профиль экспорта для спецификатора");
        valid = false;
    }
    
    return valid;
}

bool SpecificationManager::linkSpecifierToExport(SpecifierProfile& specifier, const ExportProfile& exportProfile) {
    specifier.outputProfile = exportProfile.fileName;
    specifier.linkedExportProfileFile = exportProfile.fileName;
    return true;
}

QString SpecificationManager::getModulePath(ModuleType module, bool isExport) {
    QString base = DefaultPaths::getBasePath();
    
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
