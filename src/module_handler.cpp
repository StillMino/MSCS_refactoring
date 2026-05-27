#include "module_handler.h"
#include <QDir>
#include <QSettings>
#include <QDebug>

namespace mscs {

ModuleHandler::ModuleHandler(QObject *parent)
    : QObject(parent)
    , m_activeModule(ModuleType::AEC)
{
    // Регистрация встроенных модулей
    registerModule(ModuleType::AEC, "Строительные решения", "");
    registerModule(ModuleType::Piping, "Трубопроводы", "");
    registerModule(ModuleType::Cable, "Кабельное хозяйство", "");
    registerModule(ModuleType::HVAC, "Вентиляция", "");
}

ModuleHandler::~ModuleHandler() {
}

bool ModuleHandler::registerModule(ModuleType type, const QString& name, const QString& path) {
    ModuleInfo info;
    info.name = name;
    info.path = path;
    info.available = path.isEmpty() || QDir(path).exists();
    
    m_modules[type] = info;
    return info.available;
}

QString ModuleHandler::getModuleName(ModuleType type) const {
    auto it = m_modules.find(type);
    if (it != m_modules.end()) {
        return it->name;
    }
    return tr("Неизвестный модуль");
}

QString ModuleHandler::getModulePath(ModuleType type) const {
    auto it = m_modules.find(type);
    if (it != m_modules.end()) {
        return it->path;
    }
    return "";
}

QList<ModuleType> ModuleHandler::getRegisteredModules() const {
    return m_modules.keys();
}

bool ModuleHandler::loadDefaultSettings(ModuleType type) {
    // Загрузка настроек модуля из реестра или файла конфигурации
    QSettings settings("CSoft", "Model Studio CS");
    settings.beginGroup(moduleName(type));
    
    // Здесь можно загрузить настройки по умолчанию
    
    settings.endGroup();
    
    emit moduleSettingsLoaded(type);
    return true;
}

bool ModuleHandler::isModuleAvailable(ModuleType type) const {
    auto it = m_modules.find(type);
    if (it != m_modules.end()) {
        return it->available;
    }
    return false;
}

void ModuleHandler::setActiveModule(ModuleType type) {
    if (m_activeModule != type && isModuleAvailable(type)) {
        m_activeModule = type;
        emit moduleActivated(type);
    }
}

} // namespace mscs
