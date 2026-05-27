#ifndef SPECIFICATION_MANAGER_H
#define SPECIFICATION_MANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include "types.h"

namespace mscs {

class SpecificationManager : public QObject {
    Q_OBJECT

public:
    explicit SpecificationManager(QObject *parent = nullptr);
    ~SpecificationManager();

    // Загрузка конфигурации экспорта из файла
    bool loadExportConfig(const QString& filePath, ExportConfig& config);
    
    // Сохранение конфигурации экспорта в файл
    bool saveExportConfig(const QString& filePath, const ExportConfig& config);
    
    // Загрузка конфигурации спецификатора из файла
    bool loadSpecifierConfig(const QString& filePath, SpecifierConfig& config);
    
    // Сохранение конфигурации спецификатора в файл
    bool saveSpecifierConfig(const QString& filePath, const SpecifierConfig& config);
    
    // Получение списка всех конфигураций для модуля
    QList<QString> getExportConfigs(ModuleType module);
    QList<QString> getSpecifierConfigs(ModuleType module);
    
    // Создание новой конфигурации по умолчанию
    ExportConfig createDefaultExportConfig(ModuleType module);
    SpecifierConfig createDefaultSpecifierConfig(ModuleType module);
    
    // Валидация конфигурации
    bool validateExportConfig(const ExportConfig& config, QStringList& errors);
    bool validateSpecifierConfig(const SpecifierConfig& config, QStringList& errors);
    
    // Связывание спецификатора с экспортом
    bool linkSpecifierToExport(SpecifierConfig& specifier, const ExportConfig& exportConfig);

signals:
    void configLoaded(const QString& configName);
    void configSaved(const QString& configName);
    void configError(const QString& error);

private:
    QString getModulePath(ModuleType module, bool isExport);
};

} // namespace mscs

#endif // SPECIFICATION_MANAGER_H
