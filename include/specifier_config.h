#ifndef SPECIFIER_CONFIG_H
#define SPECIFIER_CONFIG_H

#include <QObject>
#include "types.h"

namespace mscs {

class SpecifierConfig : public QObject {
    Q_OBJECT

public:
    explicit SpecifierConfig(QObject *parent = nullptr);
    
    // Геттеры и сеттеры
    QString id() const { return m_id; }
    void setId(const QString& id) { m_id = id; }
    
    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }
    
    ModuleType moduleType() const { return m_moduleType; }
    void setModuleType(ModuleType type) { m_moduleType = type; }
    
    QString exportProfileRef() const { return m_exportProfileRef; }
    void setExportProfileRef(const QString& ref) { m_exportProfileRef = ref; }
    
    QString exportProfileRefOut() const { return m_exportProfileRefOut; }
    void setExportProfileRefOut(const QString& ref) { m_exportProfileRefOut = ref; }
    
    QList<QString> visibleFields() const { return m_visibleFields; }
    void setVisibleFields(const QList<QString>& fields) { m_visibleFields = fields; }
    
    QList<QString> editableFields() const { return m_editableFields; }
    void setEditableFields(const QList<QString>& fields) { m_editableFields = fields; }
    
    bool allowEditing() const { return m_allowEditing; }
    void setAllowEditing(bool allow) { m_allowEditing = allow; }
    
    // Управление полями
    void addVisibleField(const QString& field);
    void removeVisibleField(const QString& field);
    void addEditableField(const QString& field);
    void removeEditableField(const QString& field);

private:
    QString m_id;
    QString m_name;
    ModuleType m_moduleType;
    QString m_exportProfileRef;
    QString m_exportProfileRefOut;
    QList<QString> m_visibleFields;
    QList<QString> m_editableFields;
    bool m_allowEditing;
};

} // namespace mscs

#endif // SPECIFIER_CONFIG_H
