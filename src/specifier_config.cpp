#include "specifier_config.h"

namespace mscs {

SpecifierConfig::SpecifierConfig(QObject *parent) 
    : QObject(parent), m_moduleType(ModuleType::Unknown), m_allowEditing(true) {
}

void SpecifierConfig::addVisibleField(const QString& field) {
    if (!m_visibleFields.contains(field)) {
        m_visibleFields.append(field);
    }
}

void SpecifierConfig::removeVisibleField(const QString& field) {
    m_visibleFields.removeAll(field);
}

void SpecifierConfig::addEditableField(const QString& field) {
    if (!m_editableFields.contains(field)) {
        m_editableFields.append(field);
    }
}

void SpecifierConfig::removeEditableField(const QString& field) {
    m_editableFields.removeAll(field);
}

} // namespace mscs
