#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QDir>

namespace mscs {

// Типы модулей Model Studio CS
enum class ModuleType {
    AEC,            // Строительные решения
    Piping,         // Трубопроводы
    Cable,          // Кабельное хозяйство
    HVAC,           // Вентиляция
    Unknown
};

// Поле набора данных (Field из XML)
struct DatasetField {
    QString caption;        // caption="Количество"
    QString data;           // data="GESN_QTY"
    int type = 0;           // type="0" (0=текст, 1=число, 2=дата)
    int aggregate = 0;      // aggregate="0" (0=нет, 1=сумма, 2=среднее...)
    bool visible = true;    // visible="1"
    QString format;         // format=""
    QString style;          // style=""
};

// Тип объекта (Type из XML)
struct ObjectType {
    QString name;           // name="aec_stg"
};

// Поле группировки (GroupField из XML)
struct GroupField {
    int id = 0;             // id="2"
    int idsort = -1;        // idsort="-1"
    int direction = 0;      // direction="0" (0=по возрастанию, 1=по убыванию)
};

// Поле сортировки (SortField из XML)
struct SortField {
    int id = 0;             // id="1"
    int direction = 0;      // direction="1" (0=по возрастанию, 1=по убыванию)
};

// Расширенный параметр (Parameter из Extended)
struct ExtendedParameter {
    QString name;           // name="PX_PARSE_ASSEMBLIES"
    QString value;          // value="0"
    QString caption;        // caption="Учитывать объекты внутри сборок"
    QString comment;        // comment=""
};

// Настройки таблицы (Table из XML)
struct TableSettings {
    QString caption;                    // caption="Набор данных 1"
    QString filter;                     // filter="[BOM_INCLUDE]="Да" or [BOM_INCLUDE]="1""
    QString resultFilter;               // result.filter=""
    bool aggregated = false;            // aggregated="1"
    QList<ObjectType> types;            // Types/Type
    QList<DatasetField> fields;         // Fields/Field
};

// Настройки представления (View из XML)
struct ViewSettings {
    QList<GroupField> groupFields;      // GroupFields/GroupField
    QList<SortField> sortFields;        // SortFields/SortField
};

// Профиль набора данных (Dataset из XML)
struct DatasetProfile {
    int assemblyGrouping = 0;           // assemblyGrouping="0"
    int assemblyFilter = 0;             // assemblyFilter="0"
    QString binding = "Fields";         // binding="Fields"
    QString relationType;               // relationType=""
    QString join = "outer";             // join="outer"
    int hierarchy = 0;                  // hierarchy="0"
    TableSettings table;
    ViewSettings view;
};

// Формат отчета (ReportFormat из XML)
struct ReportFormat {
    int application = 0;                // application="0"
    int title = 0;                      // title="0"
    QString parser;                     // parser=""
    bool outlining = false;             // outlining="0"
    int headersStyle = 2;               // headers.style="2"
    bool headersBold = true;            // headers.bold="1"
    int tableSeparate = 0;              // table.separate="0"
    int tableOffset = 50;               // table.offset="50"
    int tableOffsetDir = 0;             // table.offset.dir="0"
    int groupsStyle = 2;                // groups.style="2"
    bool groupsBold = true;             // groups.bold="1"
    bool groupsColumn = true;           // groups.column="1"
    bool groupsSlant = false;           // groups.slant="0"
    bool groupsUnderline = false;       // groups.underline="0"
    QString macros;                     // macros=""
    QString templateFile;               // template="Model Studio CS 3D.dwt"
    bool useFullpathTemplate = false;   // usefullpathtemplate="0"
    int encoding = 0;                   // encoding="0"
    QString worksheet;                  // worksheet=""
    bool wrap = false;                  // wrap="0"
    QString xmlApplication;             // xml.application=""
    QString xmlArguments;               // xml.arguments=""
    QString xmlScript;                  // xml.script=""
    bool identifiersOut = false;        // identifiers.out="0"
    bool xmlWaitResults = true;         // xml.wait.results="1"
    bool totalsBold = false;            // totals.bold="0"
    bool totalsItalic = false;          // totals.italic="0"
    bool totalsUnderline = false;       // totals.underline="0"
    QString totalsComment;              // totals.comment=""
    bool totalsCommentColumn = true;    // totals.comment.column="1"
    QChar csvDivider = ';';             // csv.divider=";"
};

// Профиль экспорта (полная структура Report из XML)
struct ExportProfile {
    QString fileName;                   // Имя файла без расширения
    DatasetProfile datasetProfile;
    ReportFormat reportFormat;
    QList<ExtendedParameter> extendedParams;
    
    // Для удобства в UI
    QString displayName() const {
        return fileName.isEmpty() ? "Без имени" : fileName;
    }
};

// Профиль спецификатора (SpecificationProfile из XML)
struct SpecifierProfile {
    QString profileName;                // profile="Общие_Профиль экспорта для утилиты 1"
    QString positionField;              // position.field="BOM_NUMBER"
    QString commentField;               // comment.field=""
    bool positionLock = false;          // position.lock="0"
    bool commentLock = false;           // comment.lock="0"
    int groupColumn = 2;                // group.column="2"
    QString outputProfile;              // output.profile="Общие_Профиль экспорта для утилиты 1"
    int numberingMethod = 0;            // numbering.method="0"
    int numberingMode = 0;              // numbering.mode="0"
    QString numberingFormula;           // numbering.formula=""
    int positionStart = 1;              // position.start="1"
    int positionStep = 1;               // position.step="1"
    QString positionPrefix;             // position.prefix=""
    QString positionSuffix;             // position.suffix=""
    QString positionDivider = ".";      // position.divider="."
    
    // Ссылка на файл профиля экспорта (для UI)
    QString linkedExportProfileFile;
    
    QString displayName() const {
        return profileName.isEmpty() ? "Без имени" : profileName;
    }
};

// Перечисление типов баз данных
enum class DatabaseType {
    PostgreSQL,
    MSSQL,
    Unknown
};

// Структура подключения к БД
struct DatabaseConnection {
    DatabaseType type;
    QString host;
    int port;
    QString databaseName;
    QString username;
    QString password;
    bool useIntegratedAuth = false;  // Для MS SQL
};

// Пути по умолчанию для разных модулей
struct DefaultPaths {
    static QString getBasePath() {
        // В Windows: %APPDATA%\CSoft\Model Studio CS\
        // В Linux/macOS для тестирования можно использовать домашнюю директорию
#ifdef Q_OS_WIN
        return QString::fromLocal8Bit(qgetenv("APPDATA")) + "/CSoft/Model Studio CS/";
#else
        return QDir::homePath() + "/.csoft/model-studio-cs/";
#endif
    }
    
    static QString getExportPath(ModuleType module) {
        QString base = getBasePath();
        switch (module) {
            case ModuleType::AEC:
                return base + "AEC/Export";
            case ModuleType::Piping:
                return base + "Piping/Export";
            case ModuleType::Cable:
                return base + "Cable/Export";
            case ModuleType::HVAC:
                return base + "HVAC/Export";
            default:
                return base + "AEC/Export";
        }
    }

    static QString getSpecifierPath(ModuleType module) {
        QString base = getBasePath();
        switch (module) {
            case ModuleType::AEC:
                return base + "AEC/Specifications";
            case ModuleType::Piping:
                return base + "Piping/Specifications";
            case ModuleType::Cable:
                return base + "Cable/Specifications";
            case ModuleType::HVAC:
                return base + "HVAC/Specifications";
            default:
                return base + "AEC/Specifications";
        }
    }
    
    static QString moduleToFolder(ModuleType module) {
        switch (module) {
            case ModuleType::AEC: return "AEC";
            case ModuleType::Piping: return "Piping";
            case ModuleType::Cable: return "Cable";
            case ModuleType::HVAC: return "HVAC";
            default: return "AEC";
        }
    }
};

} // namespace mscs

// Регистрация типов для использования в QVariant и мета-объектной системе Qt
Q_DECLARE_METATYPE(mscs::ModuleType)
Q_DECLARE_METATYPE(mscs::ExportProfile)
Q_DECLARE_METATYPE(mscs::SpecifierProfile)
Q_DECLARE_METATYPE(mscs::DatabaseType)

#endif // TYPES_H
