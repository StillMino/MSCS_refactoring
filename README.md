# MSCS Specification Configurator

Утилита для динамической настройки и создания спецификаций для Model Studio CS (nanoCAD).

## Описание

Современный интерфейс для работы с XML-конфигурациями спецификаций Model Studio CS. Утилита работает отдельно от целевого ПО и позволяет удобно настраивать:

- **Настройки экспорта** - профили экспорта данных спецификаций
- **Настройки спецификатора** - отображение и редактирование данных в предпросмотре MSCS

## Поддерживаемые модули

- Строительные решения (AEC)
- Трубопроводы (Piping)
- Кабельное хозяйство (Cable)
- Вентиляция (HVAC)

## Структура проекта

```
MSCS_SpecConfigurator/
├── CMakeLists.txt          # Конфигурация сборки
├── include/                # Заголовочные файлы
│   ├── types.h            # Типы данных и структуры
│   ├── mainwindow.h       # Главное окно
│   ├── xml_parser.h       # Парсер XML
│   ├── specification_manager.h  # Менеджер спецификаций
│   ├── export_config.h    # Конфигурация экспорта
│   ├── specifier_config.h # Конфигурация спецификатора
│   ├── database_connector.h  # Подключение к БД
│   └── module_handler.h   # Обработчик модулей
├── src/                   # Исходный код
│   ├── main.cpp          # Точка входа
│   ├── mainwindow.cpp    # Реализация главного окна
│   ├── xml_parser.cpp    # Парсинг XML
│   ├── specification_manager.cpp
│   ├── database_connector.cpp
│   └── module_handler.cpp
├── ui/                    # UI файлы Qt Designer
│   ├── mainwindow.ui
│   ├── export_config_dialog.ui
│   ├── specifier_config_dialog.ui
│   └── profile_selector.ui
└── resources/            # Ресурсы
    └── icons/           # Иконки
```

## Требования

- C++17
- Qt 5.15+ или Qt 6.x
- CMake 3.16+
- Компоненты Qt: Widgets, Xml, Sql

## Сборка

### Windows

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### Linux

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Расположение конфигураций

По умолчанию утилита работает с конфигурациями в следующих папках:

- **Экспорт**: `%APPDATA%\CSoft\Model Studio CS\<Module>\Export\`
- **Спецификатор**: `%APPDATA%\CSoft\Model Studio CS\<Module>\Specifications\`

Где `<Module>` - один из: `AEC`, `Piping`, `Cable`, `HVAC`

## Возможности

- ✅ Просмотр существующих конфигураций экспорта и спецификатора
- ✅ Создание новых конфигураций
- ✅ Редактирование профилей экспорта
- ✅ Настройка видимых и редактируемых полей
- ✅ Связывание спецификатора с профилями экспорта
- ✅ Поддержка PostgreSQL и MS SQL Server
- ✅ Современный темный интерфейс (Fusion style)
- ✅ Поиск по конфигурациям
- ✅ Валидация настроек

## Лицензия

Проект распространяется под лицензией MIT.

## Контакты

Разработано для Model Studio CS на базе nanoCAD.