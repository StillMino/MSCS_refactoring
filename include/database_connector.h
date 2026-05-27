#ifndef DATABASE_CONNECTOR_H
#define DATABASE_CONNECTOR_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "types.h"

namespace mscs {

class DatabaseConnector : public QObject {
    Q_OBJECT

public:
    explicit DatabaseConnector(QObject *parent = nullptr);
    ~DatabaseConnector();

    // Подключение к базе данных
    bool connect(const DatabaseConnection& connection);
    void disconnect();
    
    // Проверка состояния подключения
    bool isConnected() const;
    DatabaseType getDatabaseType() const { return m_connection.type; }
    
    // Выполнение запросов
    QSqlQuery executeQuery(const QString& query);
    QSqlQuery executeQuery(const QString& query, const QVariantList& params);
    
    // Получение данных для спецификации
    QList<SpecificationItem> getSpecificationData(const QString& tableName, 
                                                   const QStringList& fields,
                                                   const QString& filter = "");
    
    // Транзакции
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    
    // Последняя ошибка
    QString lastError() const { return m_lastError; }

signals:
    void connected();
    void disconnected();
    void error(const QString& error);

private:
    QString buildConnectionString(const DatabaseConnection& connection);
    QString escapeIdentifier(const QString& identifier);

private:
    QSqlDatabase m_db;
    DatabaseConnection m_connection;
    QString m_lastError;
};

} // namespace mscs

#endif // DATABASE_CONNECTOR_H
