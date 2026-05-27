#ifndef SPECIFICATION_MANAGER_H
#define SPECIFICATION_MANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include "types.h"
#include "xml_parser.h"

namespace mscs {

class SpecificationManager : public QObject {
    Q_OBJECT

public:
    explicit SpecificationManager(QObject *parent = nullptr);
    ~SpecificationManager();

    // Загрузка профиля экспорта из файла
    bool loadExportProfile(const QString& filePath, ExportProfile& profile);
    
    // Сохранение профиля экспорта в файл
    bool saveExportProfile(const QString& filePath, const ExportProfile& profile);
    
    // Загрузка профиля спецификатора из файла
    bool loadSpecifierProfile(const QString& filePath, SpecifierProfile& profile);
    
    // Сохранение профиля спецификатора в файл
    bool saveSpecifierProfile(const QString& filePath, const SpecifierProfile& profile);
    
    // Получение списка всех профилей для модуля
    QList<ExportProfile> getExportProfiles(ModuleType module);
    QList<SpecifierProfile> getSpecifierProfiles(ModuleType module);
    
    // Создание нового профиля по умолчанию
    ExportProfile createDefaultExportProfile(ModuleType module);
    SpecifierProfile createDefaultSpecifierProfile(ModuleType module);
    
    // Валидация профиля
    bool validateExportProfile(const ExportProfile& profile, QStringList& errors);
    bool validateSpecifierProfile(const SpecifierProfile& profile, QStringList& errors);
    
    // Связывание спецификатора с экспортом
    bool linkSpecifierToExport(SpecifierProfile& specifier, const ExportProfile& exportProfile);

signals:
    void profileLoaded(const QString& profileName);
    void profileSaved(const QString& profileName);
    void profileError(const QString& error);

private:
    XmlParser m_parser;
    QString getModulePath(ModuleType module, bool isExport);
};

} // namespace mscs

#endif // SPECIFICATION_MANAGER_H
