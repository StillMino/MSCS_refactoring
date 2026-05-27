#ifndef EXPORT_CONFIG_H
#define EXPORT_CONFIG_H

#include <QObject>
#include "types.h"

namespace mscs {

class ExportConfig : public QObject {
    Q_OBJECT

public:
    explicit ExportConfig(QObject *parent = nullptr);
    
    // Геттеры и сеттеры
    QString id() const { return m_id; }
    void setId(const QString& id) { m_id = id; }
    
    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }
    
    ModuleType moduleType() const { return m_moduleType; }
    void setModuleType(ModuleType type) { m_moduleType = type; }
    
    QList<ExportProfile> profiles() const { return m_profiles; }
    void setProfiles(const QList<ExportProfile>& profiles) { m_profiles = profiles; }
    
    QString defaultProfile() const { return m_defaultProfile; }
    void setDefaultProfile(const QString& profile) { m_defaultProfile = profile; }
    
    // Управление профилями
    void addProfile(const ExportProfile& profile);
    void removeProfile(const QString& profileId);
    ExportProfile getProfile(const QString& profileId) const;
    void updateProfile(const ExportProfile& profile);

private:
    QString m_id;
    QString m_name;
    ModuleType m_moduleType;
    QList<ExportProfile> m_profiles;
    QString m_defaultProfile;
};

} // namespace mscs

#endif // EXPORT_CONFIG_H
