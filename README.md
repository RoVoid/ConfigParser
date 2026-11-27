# Config Parser

## Учебный репозиторий

**Проект не рассчитан на публичное использование!!!**

Небольшая библиотека для чтения конфигурации из INI-подобного файла

## Требования

- CMake 3.31+
- Компилятор с поддержкой C++20
- Ninja

## Сборка

```bash
cmake -B build -S . -G Ninja
cmake --build build
```

## Запуск

```bash
ConfigParser.exe [--config <path>] [--set key=value ...]
```

| Опция                   | Описание                                               |
| ----------------------- | ------------------------------------------------------ |
| `-h`, `--help`          | показать справку                                       |
| `-c`, `--config <path>` | путь к файлу конфигурации, по умолчанию `config.ini`   |
| `-s`, `--set key=value` | переопределить значение; можно указывать несколько раз |

```bash
ConfigParser.exe --config server.ini --set port=9000 -s "debug = false"
```

## Формат файла

```ini
# комментарий
host = 0.0.0.0
port = 8080
debug = true
```

- одна пара `key = value` на строку, пробелы вокруг ключа, `=` и значения игнорируются
- пустые строки и строки, начинающиеся с `#`, пропускаются
- строка без `=` это ошибка

## Библиотека

```cpp
#include <config/config.h>

config::Config cfg;
cfg.add<std::string>("host", "127.0.0.1");
cfg.add<int>("port", 3000);
cfg.add<bool>("debug", false);

cfg.load("config.ini");
cfg.set("port", "9000");

int port = cfg.get<int>("port");
```

| Метод                   | Описание                                                        |
| ----------------------- | --------------------------------------------------------------- |
| `add<T>(name, default)` | объявить ключ, его тип и значение по умолчанию                  |
| `load(filename)`        | прочитать файл; каждое значение разбирается в объявленный тип   |
| `set(name, text)`       | задать значение из текста                                       |
| `get<T>(name)`          | получить значение; `T` должен совпадать с типом из `add`        |
| `contains(name)`        | объявлен ли ключ                                                |
| `values()`              | все значения (`std::unordered_map<std::string, config::Value>`) |

Ошибки сообщаются исключением `std::runtime_error`:

| Сообщение                                                  | Причина                                    |
| ---------------------------------------------------------- | ------------------------------------------ |
| `Cannot open config file: ...`                             | файл не открылся                           |
| `Invalid line in ...`                                      | в строке файла нет `=`                     |
| `Unknown key: ...`                                         | ключ не объявлен через `add`               |
| `Invalid value for key: ...` / `Invalid bool for key: ...` | значение не разбирается в объявленный тип  |
| `Wrong type for key: ...`                                  | `get<T>` с типом, отличным от объявленного |

## Структура

```
ConfigParser/
├── CMakeLists.txt               корень: общие настройки и add_subdirectory
├── config/                      библиотека config (target config::config)
│   ├── CMakeLists.txt
│   ├── include/config.h         интерфейс библиотеки
│   └── src/config.cpp           реализация
└── app/                         исполняемый ConfigParser (здесь же подключается Boost)
    ├── CMakeLists.txt
    ├── main.cpp
    └── config.ini               пример конфигурации, копируется рядом с exe
```
