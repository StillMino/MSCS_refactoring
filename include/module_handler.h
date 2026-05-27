#ifndef MODULE_HANDLER_H
#define MODULE_HANDLER_H

#include <QObject>
#include <QMap>
#include "types.h"

namespace mscs {

class ModuleHandler : public QObject {
    Q_OBJECT

public:
    explicit ModuleHandler(QObject *parent = nullptr);
    ~ModuleHandler();

    // Регистрация модуля
    bool registerModule(ModuleType type, const QString& name, const QString& path);
    
    // Получение информации о модуле
    QString getModuleName(ModuleType type) const;
    QString getModulePath(ModuleType type) const;
    QList<ModuleType> getRegisteredModules() const;
    
    // Загрузка настроек модуля по умолчанию
    bool loadDefaultSettings(ModuleType type);
    
    // Проверка доступности модуля
    bool isModuleAvailable(ModuleType type) const;
    
    // Переключение активного модуля
    void setActiveModule(ModuleType type);
    ModuleType getActiveModule() const { return m_activeModule; }

signals:
    void moduleActivated(ModuleType type);
    void moduleSettingsLoaded(ModuleType type);

private:
    struct ModuleInfo {
        QString name;
        QString path;
        bool available;
    };
    
    QMap<ModuleType, ModuleInfo> m_modules;
    ModuleType m_activeModule;
};

} // namespace mscs

#endif // MODULE_HANDLER_H
