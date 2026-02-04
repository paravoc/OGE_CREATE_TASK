#include "database_manager.h"
#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>
#include <sstream>
#include <ctime>

using namespace std;
using namespace OGE;

DatabaseManager::DatabaseManager() : db(nullptr), is_open(false) {}

DatabaseManager::~DatabaseManager() {
    close();
}

bool DatabaseManager::open(const string& path) {
    if (is_open) {
        close();
    }

    db_path = path.empty() ? ":memory:" : path;

    int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc != SQLITE_OK) {
        cerr << "Не удалось открыть БД: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    // Включаем поддержку внешних ключей
    execute_sql("PRAGMA foreign_keys = ON");
    // Включаем журналирование WAL для лучшей производительности
    execute_sql("PRAGMA journal_mode = WAL");

    is_open = true;
    return true;
}

bool DatabaseManager::open_or_create(const string& path) {
    if (path.empty()) {
        return open(":memory:");
    }

    // Проверяем, существует ли файл
    ifstream test_file(path);
    bool file_exists = test_file.good();
    test_file.close();

    if (!file_exists) {
        // Создаем директорию если нужно
        filesystem::path db_path_obj(path);
        auto parent_dir = db_path_obj.parent_path();
        if (!parent_dir.empty() && !filesystem::exists(parent_dir)) {
            filesystem::create_directories(parent_dir);
        }
    }

    if (!open(path)) {
        return false;
    }

    return true;
}

void DatabaseManager::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
    is_open = false;
}

bool DatabaseManager::execute_sql(const string& sql) {
    if (!db) return false;

    char* error_msg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error_msg);

    if (rc != SQLITE_OK) {
        cerr << "SQL ошибка: " << error_msg << endl;
        cerr << "Запрос: " << sql << endl;
        sqlite3_free(error_msg);
        return false;
    }

    return true;
}

vector<vector<string>> DatabaseManager::query(const string& sql) {
    vector<vector<string>> results;

    if (!db) return results;

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << endl;
        return results;
    }

    int column_count = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        vector<string> row;
        for (int i = 0; i < column_count; i++) {
            const char* text = reinterpret_cast<const char*>(
                sqlite3_column_text(stmt, i));
            row.push_back(text ? text : "");
        }
        results.push_back(row);
    }

    sqlite3_finalize(stmt);
    return results;
}

bool DatabaseManager::create_schema() {
    if (!is_open) return false;

    // Создаем все таблицы
    return create_problem1_tables();
}

bool DatabaseManager::create_problem1_tables() {
    // Таблица слов
    string sql_words = R"(
        CREATE TABLE IF NOT EXISTS problem1_words (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            word TEXT NOT NULL,
            length INTEGER NOT NULL,
            category TEXT NOT NULL,
            language TEXT DEFAULT 'ru',
            usage_count INTEGER DEFAULT 0,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            last_used_at TIMESTAMP
        );
        
        CREATE INDEX IF NOT EXISTS idx_words_category ON problem1_words(category);
        CREATE INDEX IF NOT EXISTS idx_words_length ON problem1_words(length);
    )";

    // Таблица кодировок
    string sql_encodings = R"(
        CREATE TABLE IF NOT EXISTS problem1_encodings (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            bits_per_char INTEGER NOT NULL,
            description TEXT,
            is_common INTEGER DEFAULT 1,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
    )";

    // Таблица шаблонов текста
    string sql_templates = R"(
        CREATE TABLE IF NOT EXISTS problem1_text_templates (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            prefix TEXT,
            suffix TEXT,
            delimiter TEXT NOT NULL,
            usage_count INTEGER DEFAULT 0,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
    )";

    return execute_sql(sql_words) &&
        execute_sql(sql_encodings) &&
        execute_sql(sql_templates);
}

bool DatabaseManager::seed_default_data() {
    return seed_problem1_data();
}

bool DatabaseManager::seed_problem1_data() {
    // Создаем таблицы если их нет
    if (!create_problem1_tables()) {
        return false;
    }

    // Проверяем, есть ли уже данные
    auto check_sql = "SELECT COUNT(*) FROM problem1_words";
    auto result = query(check_sql);

    if (!result.empty() && !result[0].empty() && result[0][0] != "0") {
        // Данные уже есть
        return true;
    }

    // Вставляем кодировки
    vector<tuple<string, int, string>> encodings = {
    {"КОИ-8", 8, "кодировка для русского алфавита (1 байт на символ)"},
    {"UTF-8", 8, "универсальная кодировка (1 байт на символ)"},
    {"Windows-1251", 8, "кодировка Windows для кириллицы (1 байт на символ)"},
    {"UTF-16", 16, "16-битная кодировка Unicode (2 байта на символ)"},
    {"UTF-32", 32, "32-битная кодировка Unicode (4 байта на символ)"},
    {"CP866", 8, "кодировка DOS для русского (1 байт на символ)"},
    {"ОГЭБУС", 24, "кодировка Mac для кириллицы (3 байта на символ)"},
    {"Мемокод-2024", 80, "кодировка для мемов (10 байт на символ)"},
    {"TikTok-Encoding", 48, "кодировка для коротких видео (6 байт на символ)"},
    {"Гигачад-Код", 64, "ультрасовременная кодировка (8 байт на символ)"},
    {"Киберпанк-2077", 16, "футуристическая кодировка (2 байта на символ)"},
    {"Аниме-Кодировка", 8, "кодировка для аниме (1 байт на символ)"},
    {"Котокод", 24, "кодировка для котов (3 байта на символ)"},
    {"ИИ-Генератор", 24, "нейросетевая кодировка (4 байта на символ)"},
    {"Крипто-Алфавит", 256, "блокчейн кодировка (32 байта на символ)"}
    };

    for (const auto& [name, bits, desc] : encodings) {
        string sql = "INSERT OR IGNORE INTO problem1_encodings (name, bits_per_char, description) VALUES ('"
            + name + "', " + to_string(bits) + ", '" + desc + "')";
        if (!execute_sql(sql)) {
            cerr << "Ошибка вставки кодировки: " << name << endl;
        }
    }


    vector<tuple<string, string>> words = {
        // Современные мемы и интернет-культура
        {"Криндж", "мемы"}, {"Майнкрафт", "мемы"}, {"Стикер", "мемы"},
        {"Мемас", "мемы"}, {"Хайп", "мемы"}, {"Стрим", "мемы"},
        {"Блог", "мемы"}, {"Тикток", "мемы"}, {"Ютуб", "мемы"},
        {"Дискорд", "мемы"}, {"Телеграм", "мемы"}, {"Мемасик", "мемы"},
        {"Лайк", "мемы"}, {"Репост", "мемы"}, {"Хештег", "мемы"},
        {"Контент", "мемы"}, {"Вайб", "мемы"}, {"Пруф", "мемы"},

        // Выдуманные смешные реки
        {"Речка-Беспечаль", "реки"}, {"Водопад-Смех", "реки"},
        {"Ручей-Забугорный", "реки"}, {"Поток-Мемасиков", "реки"},
        {"Река-Приколюха", "реки"}, {"Ручеек-Кринжовый", "реки"},
        {"Водица-Веселуха", "реки"}, {"Поток-Хайповый", "реки"},
        {"Речка-Тиктоковая", "реки"}, {"Ручей-Дискордный", "реки"},

        // Выдуманные смешные города
        {"Кринджбург", "города"}, {"Мемасиковск", "города"},
        {"Хайпоград", "города"}, {"Вайбсити", "города"},
        {"Стримтаун", "города"}, {"Лайкполь", "города"},
        {"Репоствиль", "города"}, {"Контентбург", "города"},
        {"Ютубоград", "города"}, {"Телеграмск", "города"},
        {"Дискордбург", "города"}, {"Гифкополь", "города"},
        {"Эмодживиль", "города"}, {"Стикерград", "города"},

        // Выдуманные смешные страны
        {"Мемастан", "страны"}, {"Кринжляндия", "страны"},
        {"Хайпомир", "страны"}, {"Вайбрия", "страны"},
        {"Стримленд", "страны"}, {"Лайкия", "страны"},
        {"Репостия", "страны"}, {"Контентланд", "страны"},
        {"Ютубия", "страны"}, {"Телеграмия", "страны"},
        {"Дискордия", "страны"}, {"Гифкостан", "страны"},
        {"Эмоджиссия", "страны"}, {"Стикерия", "страны"},

        // Настоящие страны
        {"Россия", "страны"}, {"Германия", "страны"}, {"Франция", "страны"},
        {"Италия", "страны"}, {"Испания", "страны"}, {"Португалия", "страны"},
        {"Япония", "страны"}, {"Китай", "страны"}, {"Индия", "страны"},
        {"Канада", "страны"}, {"Бразилия", "страны"}, {"Австралия", "страны"},
        {"Мексика", "страны"}, {"Аргентина", "страны"}, {"Египет", "страны"},
        {"Турция", "страны"}, {"Греция", "страны"}, {"Швеция", "страны"},
        {"Норвегия", "страны"}, {"Финляндия", "страны"}, {"Польша", "страны"},

        // Технические термины
        {"Процессор", "техника"}, {"Видеокарта", "техника"}, {"Оперативка", "техника"},
        {"Жесткий диск", "техника"}, {"SSD", "техника"}, {"Материнка", "техника"},
        {"Блок питания", "техника"}, {"Кулер", "техника"}, {"Корпус", "техника"},
        {"Монитор", "техника"}, {"Клавиатура", "техника"}, {"Мышка", "техника"},
        {"Колонки", "техника"}, {"Наушники", "техника"}, {"Вебкамера", "техника"},
        {"Маршрутизатор", "техника"}, {"Свитч", "техника"}, {"Модем", "техника"},

        // IT термины
        {"Алгоритм", "IT"}, {"Структура", "IT"}, {"Функция", "IT"},
        {"Переменная", "IT"}, {"Массив", "IT"}, {"Список", "IT"},
        {"Дерево", "IT"}, {"Граф", "IT"}, {"Хэш", "IT"},
        {"Поток", "IT"}, {"Процесс", "IT"}, {"Память", "IT"},
        {"Кэш", "IT"}, {"Буфер", "IT"}, {"Интерфейс", "IT"},
        {"API", "IT"}, {"SDK", "IT"}, {"Фреймворк", "IT"},
        {"Библиотека", "IT"}, {"Компилятор", "IT"}, {"Интерпретатор", "IT"},
        {"Отладчик", "IT"}, {"Тестирование", "IT"}, {"Разработка", "IT"},

        // Математические определения
        {"Функция", "математика"}, {"Производная", "математика"}, {"Интеграл", "математика"},
        {"Дифференциал", "математика"}, {"Уравнение", "математика"}, {"Неравенство", "математика"},
        {"Матрица", "математика"}, {"Вектор", "математика"}, {"Тензор", "математика"},
        {"Градиент", "математика"}, {"Дивергенция", "математика"}, {"Ротор", "математика"},
        {"Предел", "математика"}, {"Ряд", "математика"}, {"Сумма", "математика"},
        {"Произведение", "математика"}, {"Факториал", "математика"}, {"Комбинаторика", "математика"},
        {"Вероятность", "математика"}, {"Статистика", "математика"}, {"Теория", "математика"},

        // Программирование
        {"Python", "программирование"}, {"Java", "программирование"}, {"Cplusplus", "программирование"},
        {"Csharp", "программирование"}, {"JavaScript", "программирование"}, {"TypeScript", "программирование"},
        {"PHP", "программирование"}, {"Ruby", "программирование"}, {"Go", "программирование"},
        {"Rust", "программирование"}, {"Kotlin", "программирование"}, {"Swift", "программирование"},
        {"SQL", "программирование"}, {"HTML", "программирование"}, {"CSS", "программирование"},
        {"React", "программирование"}, {"Vue", "программирование"}, {"Angular", "программирование"},
        {"Django", "программирование"}, {"Flask", "программирование"}, {"Spring", "программирование"},

        // Операционные системы
        {"Windows", "ОС"}, {"Linux", "ОС"}, {"macOS", "ОС"},
        {"Android", "ОС"}, {"iOS", "ОС"}, {"Ubuntu", "ОС"},
        {"Debian", "ОС"}, {"Fedora", "ОС"}, {"Arch", "ОС"},
        {"CentOS", "ОС"}, {"RedHat", "ОС"}, {"FreeBSD", "ОС"},

        // Базы данных
        {"MySQL", "БД"}, {"PostgreSQL", "БД"}, {"MongoDB", "БД"},
        {"Redis", "БД"}, {"SQLite", "БД"}, {"Oracle", "БД"},
        {"MariaDB", "БД"}, {"Cassandra", "БД"}, {"Elasticsearch", "БД"},
        {"Firebase", "БД"}, {"DynamoDB", "БД"}, {"Neo4j", "БД"}
    };


    for (const auto& [word, category] : words) {
        if (!add_word(word, category)) {
            cerr << "Ошибка вставки слова: " << word << endl;
        }
    }

    vector<tuple<string, string, string>> templates = {
    {"", "— популярные языки программирования", ", "},
    {"", "— современные технологии", "| "},
    {"", "— операционные системы", ", "},
    {"", "— базы данных", ", "},
    {"В списке: ", "", "| "},
    {"Категория: ", "", "; "},
    {"", "— математические понятия", ", "},
    {"", "— технические термины", "; "},
    {"Набор: ", "", ", "},
    {"", "— элементы программирования", "; "},
    {"Группа: ", "", ", "},
    {"", "— страны мира", "; "},
    {"Коллекция: ", "", ", "},
    {"", "— компоненты компьютера", "; "},
    {"Список: ", "", ", "},
    {"", "— IT концепции", "; "},
    {"Каталог: ", "", ", "},
    {"", "— вычислительные термины", "; "},
    {"Перечень: ", "", ", "},
    {"", "— алгоритмы и структуры", " || "}
    };

    for (const auto& [prefix, suffix, delimiter] : templates) {
        string sql = "INSERT INTO problem1_text_templates (prefix, suffix, delimiter) VALUES ('"
            + prefix + "', '" + suffix + "', '" + delimiter + "')";
        if (!execute_sql(sql)) {
            cerr << "Ошибка вставки шаблона" << endl;
        }
    }

    return true;
}

vector<DatabaseManager::WordRecord> DatabaseManager::get_words(const string& category) {
    vector<WordRecord> words;

    string sql = "SELECT word, length, category FROM problem1_words";
    if (!category.empty()) {
        sql += " WHERE category = '" + category + "'";
    }
    sql += " ORDER BY RANDOM()"; // Для случайной выборки

    auto results = query(sql);

    for (const auto& row : results) {
        if (row.size() >= 3) {
            WordRecord record;
            record.word = row[0];
            record.length = stoi(row[1]);
            record.category = row[2];
            words.push_back(record);
        }
    }

    return words;
}

vector<DatabaseManager::EncodingRecord> DatabaseManager::get_encodings() {
    vector<EncodingRecord> encodings;

    string sql = "SELECT name, bits_per_char, description FROM problem1_encodings ORDER BY RANDOM()";
    auto results = query(sql);

    for (const auto& row : results) {
        if (row.size() >= 3) {
            EncodingRecord record;
            record.name = row[0];
            record.bits_per_char = stoi(row[1]);
            record.description = row[2];
            encodings.push_back(record);
        }
    }

    return encodings;
}

bool DatabaseManager::add_word(const string& word, const string& category) {
    int length = static_cast<int>(word.length());

    string sql = "INSERT OR IGNORE INTO problem1_words (word, length, category) VALUES ('"
        + word + "', " + to_string(length) + ", '" + category + "')";

    return execute_sql(sql);
}

bool DatabaseManager::add_encoding(const string& name, int bits, const string& desc) {
    string sql = "INSERT OR IGNORE INTO problem1_encodings (name, bits_per_char, description) VALUES ('"
        + name + "', " + to_string(bits) + ", '" + desc + "')";

    return execute_sql(sql);
}

bool DatabaseManager::backup(const string& backup_path) {
    if (!db || !is_open) return false;

    sqlite3* backup_db = nullptr;
    int rc = sqlite3_open(backup_path.c_str(), &backup_db);

    if (rc != SQLITE_OK) {
        cerr << "Не удалось открыть БД для бэкапа: " << backup_path << endl;
        return false;
    }

    sqlite3_backup* backup = sqlite3_backup_init(backup_db, "main", db, "main");
    if (!backup) {
        cerr << "Не удалось инициализировать бэкап: " << sqlite3_errmsg(backup_db) << endl;
        sqlite3_close(backup_db);
        return false;
    }

    rc = sqlite3_backup_step(backup, -1); // Копируем всю БД
    sqlite3_backup_finish(backup);

    bool success = (rc == SQLITE_DONE);
    sqlite3_close(backup_db);

    return success;
}

bool DatabaseManager::restore(const string& backup_path) {
    if (!is_open) return false;

    // Закрываем текущее соединение
    close();

    // Копируем файл
    try {
        filesystem::copy_file(backup_path, db_path, filesystem::copy_options::overwrite_existing);
    }
    catch (const exception& e) {
        cerr << "Ошибка восстановления: " << e.what() << endl;
        return false;
    }

    // Открываем заново
    return open(db_path);
}

DatabaseManager::DatabaseInfo DatabaseManager::get_info() const {
    DatabaseInfo info;
    info.path = db_path;
    info.schema_created = false;

    if (!db) return info;

    // Получаем размер файла
    if (db_path != ":memory:") {
        try {
            info.file_size = filesystem::file_size(db_path);
        }
        catch (...) {
            info.file_size = 0;
        }
    }
    else {
        info.file_size = 0;
    }

    // Проверяем существование таблиц
    string sql = "SELECT COUNT(*) FROM sqlite_master WHERE type='table'";
    auto result = const_cast<DatabaseManager*>(this)->query(sql);

    if (!result.empty() && !result[0].empty()) {
        info.total_tables = stoi(result[0][0]);
        info.schema_created = (info.total_tables > 0);
    }

    return info;
}