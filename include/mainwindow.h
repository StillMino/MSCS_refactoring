#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QTextEdit>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "types.h"
#include "xml_parser.h"
#include "specification_manager.h"
#include "module_handler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace mscs {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Меню
    void onNewConfig();
    void onOpenConfig();
    void onSaveConfig();
    void onSaveAsConfig();
    void onExit();
    
    void onLoadModule();
    void onRefreshConfigs();
    
    void onAbout();
    void onSettings();
    
    // Работа с конфигурациями
    void onModuleChanged(int index);
    void onExportConfigSelected(const QString& configName);
    void onSpecifierConfigSelected(const QString& configName);
    
    // Редактирование профилей
    void onAddProfile();
    void onEditProfile();
    void onDeleteProfile();
    void onCloneProfile();
    
    // Сохранение изменений
    void onSaveChanges();
    void onDiscardChanges();
    
    // Предпросмотр
    void onPreviewChanged();
    void onExportPreview();
    
    // Поиск
    void onSearchTextChanged(const QString& text);

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupCentralWidget();
    void setupStatusBar();
    
    void loadModules();
    void loadExportConfigs(ModuleType module);
    void loadSpecifierConfigs(ModuleType module);
    
    void showExportConfigDetails(const ExportProfile& profile);
    void showSpecifierConfigDetails(const SpecifierProfile& profile);
    
    void updateProfileList();
    void updateFieldTable();
    void updateSettingsEditor();
    
    bool checkUnsavedChanges();
    void setModified(bool modified);
    
    QString getAppDataPath();
    QString getConfigPath(ModuleType module, bool isExport);

private:
    Ui::MainWindow* ui;
    
    // Элементы интерфейса
    QComboBox* m_moduleCombo;
    QTabWidget* m_mainTabs;
    
    // Панель экспорта
    QListWidget* m_exportConfigList;
    QTreeWidget* m_profileTree;
    QTableWidget* m_fieldTable;
    QTextEdit* m_settingsEditor;
    
    // Панель спецификатора
    QListWidget* m_specifierConfigList;
    QComboBox* m_exportProfileRefCombo;
    QComboBox* m_exportProfileRefOutCombo;
    QListWidget* m_visibleFieldsList;
    QListWidget* m_editableFieldsList;
    
    // Общие элементы
    QLineEdit* m_searchEdit;
    QLabel* m_statusLabel;
    
    // Менеджеры
    SpecificationManager* m_specManager;
    ModuleHandler* m_moduleHandler;
    XmlParser m_xmlParser;
    
    // Текущее состояние
    ModuleType m_currentModule;
    ExportProfile m_currentExportProfile;
    SpecifierProfile m_currentSpecifierProfile;
    bool m_isModified;
    bool m_isNewConfig;
};

} // namespace mscs

#endif // MAINWINDOW_H
