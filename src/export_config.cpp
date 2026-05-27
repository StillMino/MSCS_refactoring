#include "export_config.h"

namespace mscs {

ExportConfig::ExportConfig(QObject *parent) 
    : QObject(parent), m_moduleType(ModuleType::Unknown) {
}

void ExportConfig::addProfile(const ExportProfile& profile) {
    m_profiles.append(profile);
}

void ExportConfig::removeProfile(const QString& profileId) {
    for (int i = 0; i < m_profiles.size(); ++i) {
        if (m_profiles[i].fileName == profileId) {
            m_profiles.removeAt(i);
            break;
        }
    }
}

ExportProfile ExportConfig::getProfile(const QString& profileId) const {
    for (const auto& profile : m_profiles) {
        if (profile.fileName == profileId || profile.displayName() == profileId) {
            return profile;
        }
    }
    return ExportProfile();
}

void ExportConfig::updateProfile(const ExportProfile& profile) {
    for (int i = 0; i < m_profiles.size(); ++i) {
        if (m_profiles[i].fileName == profile.fileName) {
            m_profiles[i] = profile;
            break;
        }
    }
}

} // namespace mscs
