#include "database_connector.h"
#include <QSqlDriver>
#include <QDebug>

namespace mscs {

DatabaseConnector::DatabaseConnector(QObject *parent)
    : QObject(parent)
{
}

DatabaseConnector::~DatabaseConnector() {
    disconnect();
}

bool DatabaseConnector::connect(const DatabaseConnection& connection) {
    m_connection = connection;
    
    QString driverName;
    switch (connection.type) {
        case DatabaseType::PostgreSQL:
            driverName = "QPSQL";
            break;
        case DatabaseType::MSSQL:
            driverName = "QODBC";
            break;
        default:
            m_lastError = tr("Неизвестный тип базы данных");
            emit error(m_lastError);
            return false;
    }
    
    if (!QSqlDatabase::drivers().contains(driverName)) {
        m_lastError = tr("Драйвер %1 не найден").arg(driverName);
        emit error(m_lastError);
        return false;
    }
    
    m_db = QSqlDatabase::addDatabase(driverName);
    
    QString connectionString = buildConnectionString(connection);
    m_db.setDatabaseName(connectionString);
    
    if (connection.useIntegratedAuth) {
        // Windows Authentication для MS SQL
        m_db.setUserName("");
        m_db.setPassword("");
    } else {
        m_db.setUserName(connection.username);
        m_db.setPassword(connection.password);
    }
    
    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        qWarning() << "Ошибка подключения к БД:" << m_lastError;
        emit error(m_lastError);
        return false;
    }
    
    emit connected();
    return true;
}

void DatabaseConnector::disconnect() {
    if (m_db.isOpen()) {
        m_db.close();
        emit disconnected();
    }
    m_db = QSqlDatabase();
}

bool DatabaseConnector::isConnected() const {
    return m_db.isOpen() && m_db.isValid();
}

QSqlQuery DatabaseConnector::executeQuery(const QString& query) {
    QSqlQuery result(m_db);
    if (!result.exec(query)) {
        m_lastError = result.lastError().text();
        qWarning() << "Ошибка выполнения запроса:" << m_lastError;
        emit error(m_lastError);
    }
    return result;
}

QSqlQuery DatabaseConnector::executeQuery(const QString& query, const QVariantList& params) {
    QSqlQuery result(m_db);
    
    for (const auto& param : params) {
        result.addBindValue(param);
    }
    
    if (!result.exec(query)) {
        m_lastError = result.lastError().text();
        qWarning() << "Ошибка выполнения запроса:" << m_lastError;
        emit error(m_lastError);
    }
    
    return result;
}

QList<SpecificationItem> DatabaseConnector::getSpecificationData(const QString& tableName,
                                                                  const QStringList& fields,
                                                                  const QString& filter) {
    QList<SpecificationItem> items;
    
    if (!isConnected()) {
        return items;
    }
    
    // Формирование запроса
    QString fieldList = fields.join(", ");
    if (fieldList.isEmpty()) {
        fieldList = "*";
    }
    
    QString query = QString("SELECT %1 FROM %2").arg(fieldList, escapeIdentifier(tableName));
    
    if (!filter.isEmpty()) {
        query += " WHERE " + filter;
    }
    
    QSqlQuery result = executeQuery(query);
    
    while (result.next()) {
        SpecificationItem item;
        item.id = result.value(0).toString();
        
        for (int i = 0; i < fields.count() && i < result.record().count(); ++i) {
            item.properties[fields[i]] = result.value(i);
        }
        
        items.append(item);
    }
    
    return items;
}

bool DatabaseConnector::beginTransaction() {
    if (!isConnected()) {
        return false;
    }
    return m_db.transaction();
}

bool DatabaseConnector::commitTransaction() {
    if (!isConnected()) {
        return false;
    }
    return m_db.commit();
}

bool DatabaseConnector::rollbackTransaction() {
    if (!isConnected()) {
        return false;
    }
    return m_db.rollback();
}

QString DatabaseConnector::buildConnectionString(const DatabaseConnection& connection) {
    switch (connection.type) {
        case DatabaseType::PostgreSQL: {
            return QString("host=%1 port=%2 dbname=%3 user=%4 password=%5")
                .arg(connection.host)
                .arg(connection.port)
                .arg(connection.databaseName)
                .arg(connection.username)
                .arg(connection.password);
        }
        case DatabaseType::MSSQL: {
            if (connection.useIntegratedAuth) {
                return QString("DRIVER={SQL Server};SERVER=%1;DATABASE=%2;Trusted_Connection=Yes;")
                    .arg(connection.host)
                    .arg(connection.databaseName);
            } else {
                return QString("DRIVER={SQL Server};SERVER=%1;DATABASE=%2;UID=%3;PWD=%4;")
                    .arg(connection.host)
                    .arg(connection.databaseName)
                    .arg(connection.username)
                    .arg(connection.password);
            }
        }
        default:
            return "";
    }
}

QString DatabaseConnector::escapeIdentifier(const QString& identifier) {
    // Экранирование имен таблиц и колонок
    return "\"" + identifier.replace("\"", "\"\"") + "\"";
}

} // namespace mscs
