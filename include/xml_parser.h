#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QDomDocument>
#include <QFile>
#include <QDir>
#include "types.h"

namespace mscs {

class XmlParser {
public:
    XmlParser() = default;
    ~XmlParser() = default;

    // ==================== Профили экспорта ====================
    
    // Загрузка профиля экспорта из файла
    bool loadExportProfile(const QString& filePath, ExportProfile& profile);
    
    // Сохранение профиля экспорта в файл
    bool saveExportProfile(const QString& filePath, const ExportProfile& profile);
    
    // Парсинг XML в структуру ExportProfile
    bool parseExportProfile(const QDomDocument& doc, ExportProfile& profile);
    
    // Создание XML документа из ExportProfile
    QDomDocument createExportProfileXml(const ExportProfile& profile);
    
    // Получение всех профилей экспорта из папки
    QList<ExportProfile> loadAllExportProfiles(const QString& folderPath, ModuleType module);
    
    // ==================== Профили спецификатора ====================
    
    // Загрузка профиля спецификатора из файла
    bool loadSpecifierProfile(const QString& filePath, SpecifierProfile& profile);
    
    // Сохранение профиля спецификатора в файл
    bool saveSpecifierProfile(const QString& filePath, const SpecifierProfile& profile);
    
    // Парсинг XML в структуру SpecifierProfile
    bool parseSpecifierProfile(const QDomElement& root, SpecifierProfile& profile);
    
    // Создание XML документа из SpecifierProfile
    QDomDocument createSpecifierProfileXml(const SpecifierProfile& profile);
    
    // Получение всех профилей спецификатора из папки
    QList<SpecifierProfile> loadAllSpecifierProfiles(const QString& folderPath, ModuleType module);
    
    // ==================== Вспомогательные методы ====================
    
    // Валидация XML структуры
    bool validateStructure(const QDomDocument& doc, const QString& expectedRoot);
    
    // Определение типа модуля по пути или содержимому
    ModuleType detectModuleType(const QString& path);
    
    // Поиск профиля экспорта по имени
    ExportProfile findExportProfileByName(const QString& folderPath, const QString& name);
    
    // Поиск профиля спецификатора по имени
    SpecifierProfile findSpecifierProfileByName(const QString& folderPath, const QString& name);

private:
    // Чтение атрибутов с значениями по умолчанию
    QString readAttribute(const QDomElement& elem, const QString& attrName, const QString& defaultValue = "");
    int readIntAttribute(const QDomElement& elem, const QString& attrName, int defaultValue = 0);
    bool readBoolAttribute(const QDomElement& elem, const QString& attrName, bool defaultValue = false);
    
    // Запись атрибутов
    void writeAttribute(QDomElement& elem, const QString& attrName, const QString& value);
    void writeAttribute(QDomElement& elem, const QString& attrName, int value);
    void writeAttribute(QDomElement& elem, const QString& attrName, bool value);
    
    // Парсинг вложенных структур
    void parseTableSettings(const QDomElement& tableElem, TableSettings& table);
    void parseViewSettings(const QDomElement& viewElem, ViewSettings& view);
    void parseReportFormat(const QDomElement& formatElem, ReportFormat& format);
    void parseExtendedParams(const QDomElement& extendedElem, QList<ExtendedParameter>& params);
    
    // Запись вложенных структур
    void writeTableSettings(QDomElement& parent, const TableSettings& table, QDomDocument& doc);
    void writeViewSettings(QDomElement& parent, const ViewSettings& view, QDomDocument& doc);
    void writeReportFormat(QDomElement& parent, const ReportFormat& format, QDomDocument& doc);
    void writeExtendedParams(QDomElement& parent, const QList<ExtendedParameter>& params, QDomDocument& doc);
};

} // namespace mscs

#endif // XML_PARSER_H
