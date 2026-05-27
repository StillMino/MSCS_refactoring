#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QToolBar>
#include <QAction>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QHeaderView>

namespace mscs {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_isModified(false)
    , m_isNewConfig(false)
    , m_currentModule(ModuleType::AEC)
{
    setupUi();
    
    m_specManager = new SpecificationManager(this);
    m_moduleHandler = new ModuleHandler(this);
    
    loadModules();
    
    setWindowTitle("MSCS Specification Configurator - Model Studio CS");
    resize(1200, 800);
}

MainWindow::~MainWindow() {
    delete m_specManager;
    delete m_moduleHandler;
}

void MainWindow::setupUi() {
    setupMenuBar();
    setupToolBar();
    setupCentralWidget();
    setupStatusBar();
}

void MainWindow::setupMenuBar() {
    QMenuBar* menuBar = this->menuBar();
    
    // Меню Файл
    QMenu* fileMenu = menuBar->addMenu(tr("Файл"));
    fileMenu->addAction(tr("Новая конфигурация"), this, &MainWindow::onNewConfig, QKeySequence::New);
    fileMenu->addAction(tr("Открыть..."), this, &MainWindow::onOpenConfig, QKeySequence::Open);
    fileMenu->addAction(tr("Сохранить"), this, &MainWindow::onSaveConfig, QKeySequence::Save);
    fileMenu->addAction(tr("Сохранить как..."), this, &MainWindow::onSaveAsConfig, QKeySequence::SaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Выход"), this, &MainWindow::onExit, QKeySequence::Quit);
    
    // Меню Модули
    QMenu* moduleMenu = menuBar->addMenu(tr("Модули"));
    moduleMenu->addAction(tr("Загрузить модуль"), this, &MainWindow::onLoadModule);
    moduleMenu->addAction(tr("Обновить список"), this, &MainWindow::onRefreshConfigs, QKeySequence::Refresh);
    
    // Меню Правка
    QMenu* editMenu = menuBar->addMenu(tr("Правка"));
    editMenu->addAction(tr("Добавить профиль"), this, &MainWindow::onAddProfile);
    editMenu->addAction(tr("Редактировать профиль"), this, &MainWindow::onEditProfile);
    editMenu->addAction(tr("Клонировать профиль"), this, &MainWindow::onCloneProfile);
    editMenu->addAction(tr("Удалить профиль"), this, &MainWindow::onDeleteProfile);
    editMenu->addSeparator();
    editMenu->addAction(tr("Сохранить изменения"), this, &MainWindow::onSaveChanges);
    
    // Меню Вид
    QMenu* viewMenu = menuBar->addMenu(tr("Вид"));
    viewMenu->addAction(tr("Предпросмотр экспорта"), this, &MainWindow::onExportPreview);
    
    // Меню Справка
    QMenu* helpMenu = menuBar->addMenu(tr("Справка"));
    helpMenu->addAction(tr("О программе"), this, &MainWindow::onAbout);
    helpMenu->addAction(tr("Настройки"), this, &MainWindow::onSettings);
}

void MainWindow::setupToolBar() {
    QToolBar* toolBar = addToolBar("Main Toolbar");
    toolBar->setMovable(false);
    
    QAction* newAction = toolBar->addAction(tr("Новая"));
    connect(newAction, &QAction::triggered, this, &MainWindow::onNewConfig);
    
    QAction* openAction = toolBar->addAction(tr("Открыть"));
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenConfig);
    
    QAction* saveAction = toolBar->addAction(tr("Сохранить"));
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveConfig);
    
    toolBar->addSeparator();
    
    QAction* addProfileAction = toolBar->addAction(tr("Добавить профиль"));
    connect(addProfileAction, &QAction::triggered, this, &MainWindow::onAddProfile);
    
    QAction* editProfileAction = toolBar->addAction(tr("Редактировать"));
    connect(editProfileAction, &QAction::triggered, this, &MainWindow::onEditProfile);
    
    QAction* deleteProfileAction = toolBar->addAction(tr("Удалить"));
    connect(deleteProfileAction, &QAction::triggered, this, &MainWindow::onDeleteProfile);
    
    toolBar->addSeparator();
    
    QLabel* moduleLabel = new QLabel(tr("Модуль:"));
    toolBar->addWidget(moduleLabel);
    
    m_moduleCombo = new QComboBox();
    m_moduleCombo->setMinimumWidth(200);
    toolBar->addWidget(m_moduleCombo);
    
    connect(m_moduleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModuleChanged);
    
    toolBar->addSeparator();
    
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText(tr("Поиск..."));
    m_searchEdit->setMaximumWidth(200);
    toolBar->addWidget(m_searchEdit);
    
    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &MainWindow::onSearchTextChanged);
}

void MainWindow::setupCentralWidget() {
    QWidget* centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    m_mainTabs = new QTabWidget();
    mainLayout->addWidget(m_mainTabs);
    
    // Вкладка Экспорт
    QWidget* exportTab = new QWidget();
    QHBoxLayout* exportLayout = new QHBoxLayout(exportTab);
    
    // Левая панель - список конфигураций
    QWidget* leftPanel = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftPanel->setMaximumWidth(250);
    
    QLabel* exportConfigLabel = new QLabel(tr("Конфигурации экспорта:"));
    leftLayout->addWidget(exportConfigLabel);
    
    m_exportConfigList = new QListWidget();
    leftLayout->addWidget(m_exportConfigList);
    
    connect(m_exportConfigList, &QListWidget::currentTextChanged,
            this, &MainWindow::onExportConfigSelected);
    
    // Центральная панель - профили
    QWidget* centerPanel = new QWidget();
    QVBoxLayout* centerLayout = new QVBoxLayout(centerPanel);
    
    QLabel* profileLabel = new QLabel(tr("Профили экспорта:"));
    centerLayout->addWidget(profileLabel);
    
    m_profileTree = new QTreeWidget();
    m_profileTree->setHeaderLabels({"Имя", "ID", "Статус"});
    m_profileTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    centerLayout->addWidget(m_profileTree);
    
    // Правая панель - детали профиля
    QWidget* rightPanel = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightPanel->setMinimumWidth(400);
    
    QGroupBox* fieldGroup = new QGroupBox(tr("Поля"));
    QVBoxLayout* fieldLayout = new QVBoxLayout(fieldGroup);
    
    m_fieldTable = new QTableWidget();
    m_fieldTable->setColumnCount(3);
    m_fieldTable->setHorizontalHeaderLabels({"Поле", "Тип", "Описание"});
    m_fieldTable->horizontalHeader()->setStretchLastSection(true);
    fieldLayout->addWidget(m_fieldTable);
    
    rightLayout->addWidget(fieldGroup);
    
    QGroupBox* settingsGroup = new QGroupBox(tr("Настройки"));
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);
    
    m_settingsEditor = new QTextEdit();
    m_settingsEditor->setPlaceholderText(tr("Настройки в формате JSON..."));
    settingsLayout->addWidget(m_settingsEditor);
    
    rightLayout->addWidget(settingsGroup);
    
    exportLayout->addWidget(leftPanel);
    exportLayout->addWidget(centerPanel, 1);
    exportLayout->addWidget(rightPanel, 1);
    
    m_mainTabs->addTab(exportTab, tr("Экспорт"));
    
    // Вкладка Спецификатор
    QWidget* specifierTab = new QWidget();
    QHBoxLayout* specifierLayout = new QHBoxLayout(specifierTab);
    
    // Левая панель - список конфигураций спецификатора
    QWidget* specLeftPanel = new QWidget();
    QVBoxLayout* specLeftLayout = new QVBoxLayout(specLeftPanel);
    specLeftPanel->setMaximumWidth(250);
    
    QLabel* specifierConfigLabel = new QLabel(tr("Конфигурации спецификатора:"));
    specLeftLayout->addWidget(specifierConfigLabel);
    
    m_specifierConfigList = new QListWidget();
    specLeftLayout->addWidget(m_specifierConfigList);
    
    connect(m_specifierConfigList, &QListWidget::currentTextChanged,
            this, &MainWindow::onSpecifierConfigSelected);
    
    // Центральная панель - настройки спецификатора
    QWidget* specCenterPanel = new QWidget();
    QVBoxLayout* specCenterLayout = new QVBoxLayout(specCenterPanel);
    
    QGroupBox* profileRefGroup = new QGroupBox(tr("Ссылки на профили экспорта"));
    QVBoxLayout* profileRefLayout = new QVBoxLayout(profileRefGroup);
    
    m_exportProfileRefCombo = new QComboBox();
    m_exportProfileRefCombo->setPlaceholderText(tr("Профиль для спецификатора"));
    profileRefLayout->addWidget(new QLabel(tr("Для спецификатора:")));
    profileRefLayout->addWidget(m_exportProfileRefCombo);
    
    m_exportProfileRefOutCombo = new QComboBox();
    m_exportProfileRefOutCombo->setPlaceholderText(tr("Профиль для экспорта"));
    profileRefLayout->addWidget(new QLabel(tr("Для экспорта:")));
    profileRefLayout->addWidget(m_exportProfileRefOutCombo);
    
    specCenterLayout->addWidget(profileRefGroup);
    
    QSplitter* fieldsSplitter = new QSplitter(Qt::Horizontal);
    
    QWidget* visibleFieldsWidget = new QWidget();
    QVBoxLayout* visibleLayout = new QVBoxLayout(visibleFieldsWidget);
    visibleLayout->addWidget(new QLabel(tr("Видимые поля:")));
    m_visibleFieldsList = new QListWidget();
    m_visibleFieldsList->setSelectionMode(QAbstractItemView::MultiSelection);
    visibleLayout->addWidget(m_visibleFieldsList);
    fieldsSplitter->addWidget(visibleFieldsWidget);
    
    QWidget* editableFieldsWidget = new QWidget();
    QVBoxLayout* editableLayout = new QVBoxLayout(editableFieldsWidget);
    editableLayout->addWidget(new QLabel(tr("Редактируемые поля:")));
    m_editableFieldsList = new QListWidget();
    m_editableFieldsList->setSelectionMode(QAbstractItemView::MultiSelection);
    editableLayout->addWidget(m_editableFieldsList);
    fieldsSplitter->addWidget(editableFieldsWidget);
    
    specCenterLayout->addWidget(fieldsSplitter);
    
    specifierLayout->addWidget(specLeftPanel);
    specifierLayout->addWidget(specCenterPanel, 1);
    
    m_mainTabs->addTab(specifierTab, tr("Спецификатор"));
}

void MainWindow::setupStatusBar() {
    QStatusBar* status = statusBar();
    
    m_statusLabel = new QLabel(tr("Готов"));
    status->addWidget(m_statusLabel);
    
    QLabel* pathLabel = new QLabel(getAppDataPath());
    status->addPermanentWidget(pathLabel);
}

void MainWindow::loadModules() {
    m_moduleCombo->clear();
    m_moduleCombo->addItem("Строительные решения (AEC)", QVariant::fromValue(ModuleType::AEC));
    m_moduleCombo->addItem("Трубопроводы (Piping)", QVariant::fromValue(ModuleType::Piping));
    m_moduleCombo->addItem("Кабельное хозяйство (Cable)", QVariant::fromValue(ModuleType::Cable));
    m_moduleCombo->addItem("Вентиляция (HVAC)", QVariant::fromValue(ModuleType::HVAC));
    
    loadExportConfigs(m_currentModule);
    loadSpecifierConfigs(m_currentModule);
}

void MainWindow::loadExportConfigs(ModuleType module) {
    m_exportConfigList->clear();
    
    QString configPath = getConfigPath(module, true);
    QDir dir(configPath);
    
    if (!dir.exists()) {
        m_statusLabel->setText(tr("Папка конфигураций не найдена: ") + configPath);
        return;
    }
    
    QStringList filters;
    filters << "*.xml";
    dir.setNameFilters(filters);
    
    QFileInfoList files = dir.entryInfoList();
    for (const auto& file : files) {
        QListWidgetItem* item = new QListWidgetItem(file.baseName());
        item->setData(Qt::UserRole, file.filePath());
        m_exportConfigList->addItem(item);
    }
    
    m_statusLabel->setText(tr("Загружено конфигураций: %1").arg(files.count()));
}

void MainWindow::loadSpecifierConfigs(ModuleType module) {
    m_specifierConfigList->clear();
    
    QString configPath = getConfigPath(module, false);
    QDir dir(configPath);
    
    if (!dir.exists()) {
        m_statusLabel->setText(tr("Папка конфигураций не найдена: ") + configPath);
        return;
    }
    
    QStringList filters;
    filters << "*.xml";
    dir.setNameFilters(filters);
    
    QFileInfoList files = dir.entryInfoList();
    for (const auto& file : files) {
        QListWidgetItem* item = new QListWidgetItem(file.baseName());
        item->setData(Qt::UserRole, file.filePath());
        m_specifierConfigList->addItem(item);
    }
}

void MainWindow::showExportConfigDetails(const ExportProfile& config) {
    m_currentExportProfile = config;
    updateProfileList();
}

void MainWindow::showSpecifierConfigDetails(const SpecifierProfile& config) {
    m_currentSpecifierProfile = config;
    
    // Заполнение комбобоксов профилями
    m_exportProfileRefCombo->clear();
    m_exportProfileRefOutCombo->clear();
    
    for (const auto& profile : m_currentExportProfile.datasetProfile.table.fields) {
        m_exportProfileRefCombo->addItem(profile.name, profile.id);
        m_exportProfileRefOutCombo->addItem(profile.name, profile.id);
    }
    
    // Установка текущих значений
    int refIndex = m_exportProfileRefCombo->findData(config.exportProfileRef);
    if (refIndex >= 0) {
        m_exportProfileRefCombo->setCurrentIndex(refIndex);
    }
    
    int refOutIndex = m_exportProfileRefOutCombo->findData(config.exportProfileRefOut);
    if (refOutIndex >= 0) {
        m_exportProfileRefOutCombo->setCurrentIndex(refOutIndex);
    }
    
    // Заполнение списков полей
    m_visibleFieldsList->clear();
    m_editableFieldsList->clear();
    
    for (const auto& field : config.visibleFields) {
        m_visibleFieldsList->addItem(field);
    }
    
    for (const auto& field : config.editableFields) {
        m_editableFieldsList->addItem(field);
    }
}

void MainWindow::updateProfileList() {
    m_profileTree->clear();
    
    for (const auto& profile : m_currentExportProfile.datasetProfile.table.fields) {
        QTreeWidgetItem* item = new QTreeWidgetItem(m_profileTree);
        item->setText(0, profile.name);
        item->setText(1, profile.id);
        item->setText(2, profile.enabled ? tr("Активен") : tr("Неактивен"));
        item->setData(0, Qt::UserRole, QVariant::fromValue(profile));
    }
    
    updateFieldTable();
}

void MainWindow::updateFieldTable() {
    m_fieldTable->setRowCount(0);
    
    // Заполняется при выборе профиля
}

void MainWindow::updateSettingsEditor() {
    // Обновление редактора настроек
}

bool MainWindow::checkUnsavedChanges() {
    if (m_isModified) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            tr("Несохраненные изменения"),
            tr("Есть несохраненные изменения. Сохранить?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );
        
        if (reply == QMessageBox::Save) {
            onSaveChanges();
            return true;
        } else if (reply == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::setModified(bool modified) {
    m_isModified = modified;
    setWindowModified(modified);
    m_statusLabel->setText(modified ? tr("Изменено") : tr("Готов"));
}

QString MainWindow::getAppDataPath() {
    QString appData = qgetenv("APPDATA");
    if (appData.isEmpty()) {
        appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    return appData + "/CSoft/Model Studio CS";
}

QString MainWindow::getConfigPath(ModuleType module, bool isExport) {
    QString base = getAppDataPath() + "/";
    
    switch (module) {
        case ModuleType::AEC:
            base += "AEC/";
            break;
        case ModuleType::Piping:
            base += "Piping/";
            break;
        case ModuleType::Cable:
            base += "Cable/";
            break;
        case ModuleType::HVAC:
            base += "HVAC/";
            break;
        default:
            base += "AEC/";
    }
    
    base += isExport ? "Export" : "Specifications";
    return base;
}

// Слоты меню
void MainWindow::onNewConfig() {
    if (!checkUnsavedChanges()) return;
    
    m_isNewConfig = true;
    m_currentExportProfile = ExportProfile();
    m_currentSpecifierProfile = SpecifierProfile();
    m_isModified = false;
    
    m_profileTree->clear();
    m_fieldTable->setRowCount(0);
    m_settingsEditor->clear();
    
    m_statusLabel->setText(tr("Создана новая конфигурация"));
}

void MainWindow::onOpenConfig() {
    if (!checkUnsavedChanges()) return;
    
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Открыть конфигурацию"),
        getConfigPath(m_currentModule, true),
        tr("XML файлы (*.xml)")
    );
    
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось открыть файл"));
        return;
    }
    
    QDomDocument doc;
    QString errorMsg;
    if (!doc.setContent(&file, &errorMsg)) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Ошибка парсинга XML: ") + errorMsg);
        file.close();
        return;
    }
    file.close();
    
    // Парсинг в зависимости от типа
    if (fileName.contains("Specifications", Qt::CaseInsensitive)) {
        m_currentSpecifierProfile = [m_xmlParser.parseSpecifierProfile(doc)](QDomDocument& doc) { SpecifierProfile p; m_xmlParser.parseSpecifierProfile(doc.documentElement(), p); return p; }(doc);
        showSpecifierConfigDetails(m_currentSpecifierProfile);
    } else {
        m_currentExportProfile = m_xmlParser.parseExportProfile(doc);
        showExportConfigDetails(m_currentExportProfile);
    }
    
    m_isModified = false;
    m_statusLabel->setText(tr("Открыто: ") + fileName);
}

void MainWindow::onSaveConfig() {
    onSaveChanges();
}

void MainWindow::onSaveAsConfig() {
    QString defaultName = m_currentExportProfile.name.isEmpty() ? 
                          "new_config.xml" : m_currentExportProfile.name + ".xml";
    
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Сохранить как"),
        getConfigPath(m_currentModule, true) + "/" + defaultName,
        tr("XML файлы (*.xml)")
    );
    
    if (fileName.isEmpty()) return;
    
    if (!fileName.endsWith(".xml")) {
        fileName += ".xml";
    }
    
    QDomDocument doc = m_xmlParser.createExportProfileXml(m_currentExportProfile);
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось сохранить файл"));
        return;
    }
    
    QTextStream out(&file);
    doc.save(out, 4);
    file.close();
    
    m_isModified = false;
    m_statusLabel->setText(tr("Сохранено: ") + fileName);
}

void MainWindow::onExit() {
    if (checkUnsavedChanges()) {
        qApp->quit();
    }
}

void MainWindow::onLoadModule() {
    // Загрузка дополнительного модуля
    QMessageBox::information(this, tr("Инфо"), tr("Функция загрузки модуля в разработке"));
}

void MainWindow::onRefreshConfigs() {
    if (!checkUnsavedChanges()) return;
    
    loadExportConfigs(m_currentModule);
    loadSpecifierConfigs(m_currentModule);
    
    m_statusLabel->setText(tr("Конфигурации обновлены"));
}

void MainWindow::onAbout() {
    QMessageBox::about(
        this,
        tr("О программе"),
        tr("<h2>MSCS Specification Configurator</h2>"
           "<p>Версия 1.0.0</p>"
           "<p>Утилита для настройки спецификаций Model Studio CS</p>"
           "<p>Основано на nanoCAD</p>")
    );
}

void MainWindow::onSettings() {
    QMessageBox::information(this, tr("Настройки"), tr("Настройки в разработке"));
}

void MainWindow::onModuleChanged(int index) {
    if (!checkUnsavedChanges()) return;
    
    ModuleType newModule = m_moduleCombo->currentData().value<ModuleType>();
    if (newModule != m_currentModule) {
        m_currentModule = newModule;
        loadExportConfigs(m_currentModule);
        loadSpecifierConfigs(m_currentModule);
        
        m_statusLabel->setText(tr("Переключен модуль: ") + m_moduleCombo->currentText());
    }
}

void MainWindow::onExportConfigSelected(const QString& configName) {
    if (!checkUnsavedChanges()) return;
    
    QListWidgetItem* currentItem = m_exportConfigList->currentItem();
    if (!currentItem) return;
    
    QString filePath = currentItem->data(Qt::UserRole).toString();
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось открыть файл конфигурации"));
        return;
    }
    
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();
    
    m_currentExportProfile = m_xmlParser.parseExportProfile(doc);
    showExportConfigDetails(m_currentExportProfile);
    
    m_statusLabel->setText(tr("Выбрана конфигурация экспорта: ") + configName);
}

void MainWindow::onSpecifierConfigSelected(const QString& configName) {
    if (!checkUnsavedChanges()) return;
    
    QListWidgetItem* currentItem = m_specifierConfigList->currentItem();
    if (!currentItem) return;
    
    QString filePath = currentItem->data(Qt::UserRole).toString();
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось открыть файл конфигурации"));
        return;
    }
    
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();
    
    m_currentSpecifierProfile = [m_xmlParser.parseSpecifierProfile(doc)](QDomDocument& doc) { SpecifierProfile p; m_xmlParser.parseSpecifierProfile(doc.documentElement(), p); return p; }(doc);
    
    // Сначала нужно загрузить связанный экспорт
    showSpecifierConfigDetails(m_currentSpecifierProfile);
    
    m_statusLabel->setText(tr("Выбрана конфигурация спецификатора: ") + configName);
}

void MainWindow::onAddProfile() {
    ExportProfile newProfile;
    newProfile.id = QUuid::createUuid().toString();
    newProfile.name = tr("Новый профиль");
    newProfile.enabled = true;
    
    m_currentExportProfile.datasetProfile.table.fields.append(newProfile);
    updateProfileList();
    setModified(true);
    
    m_statusLabel->setText(tr("Добавлен новый профиль"));
}

void MainWindow::onEditProfile() {
    QTreeWidgetItem* currentItem = m_profileTree->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, tr("Предупреждение"), tr("Выберите профиль для редактирования"));
        return;
    }
    
    ExportProfile profile = currentItem->data(0, Qt::UserRole).value<ExportProfile>();
    
    // Здесь должен быть диалог редактирования
    QMessageBox::information(this, tr("Инфо"), tr("Редактирование профиля: ") + profile.name);
}

void MainWindow::onDeleteProfile() {
    QTreeWidgetItem* currentItem = m_profileTree->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, tr("Предупреждение"), tr("Выберите профиль для удаления"));
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Подтверждение"),
        tr("Удалить выбранный профиль?"),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        ExportProfile profile = currentItem->data(0, Qt::UserRole).value<ExportProfile>();
        
        auto it = std::find_if(m_currentExportProfile.datasetProfile.table.fields.begin(), 
                               m_currentExportProfile.datasetProfile.table.fields.end(),
                               [&profile](const ExportProfile& p) { return p.id == profile.id; });
        
        if (it != m_currentExportProfile.datasetProfile.table.fields.end()) {
            m_currentExportProfile.datasetProfile.table.fields.erase(it);
            updateProfileList();
            setModified(true);
            
            m_statusLabel->setText(tr("Профиль удален"));
        }
    }
}

void MainWindow::onCloneProfile() {
    QTreeWidgetItem* currentItem = m_profileTree->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, tr("Предупреждение"), tr("Выберите профиль для клонирования"));
        return;
    }
    
    ExportProfile original = currentItem->data(0, Qt::UserRole).value<ExportProfile>();
    
    ExportProfile cloned = original;
    cloned.id = QUuid::createUuid().toString();
    cloned.name = original.name + tr(" (копия)");
    
    m_currentExportProfile.datasetProfile.table.fields.append(cloned);
    updateProfileList();
    setModified(true);
    
    m_statusLabel->setText(tr("Профиль склонирован"));
}

void MainWindow::onSaveChanges() {
    // Сбор данных из UI
    // Сохранение в файл
    
    QDomDocument doc = m_xmlParser.createExportProfileXml(m_currentExportProfile);
    
    QString filePath = getConfigPath(m_currentModule, true) + "/" + 
                       m_currentExportProfile.name + ".xml";
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось сохранить файл"));
        return;
    }
    
    QTextStream out(&file);
    doc.save(out, 4);
    file.close();
    
    setModified(false);
    m_statusLabel->setText(tr("Изменения сохранены"));
}

void MainWindow::onDiscardChanges() {
    if (QMessageBox::question(this, tr("Подтверждение"), 
                              tr("Отменить все изменения?")) == QMessageBox::Yes) {
        setModified(false);
        m_statusLabel->setText(tr("Изменения отменены"));
    }
}

void MainWindow::onPreviewChanged() {
    // Обновление предпросмотра
}

void MainWindow::onExportPreview() {
    QMessageBox::information(this, tr("Предпросмотр"), tr("Функция предпросмотра в разработке"));
}

void MainWindow::onSearchTextChanged(const QString& text) {
    // Поиск по конфигурациям
    for (int i = 0; i < m_exportConfigList->count(); ++i) {
        QListWidgetItem* item = m_exportConfigList->item(i);
        item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
    }
}

} // namespace mscs
