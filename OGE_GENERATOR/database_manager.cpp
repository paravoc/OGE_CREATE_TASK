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
        {"ОГЭБУС", 24, "кодировка Mac для кириллицы (3 байт на символ)"}
    };

    for (const auto& [name, bits, desc] : encodings) {
        string sql = "INSERT OR IGNORE INTO problem1_encodings (name, bits_per_char, description) VALUES ('"
            + name + "', " + to_string(bits) + ", '" + desc + "')";
        if (!execute_sql(sql)) {
            cerr << "Ошибка вставки кодировки: " << name << endl;
        }
    }

    // Вставляем слова (реки, города, горы)
    vector<tuple<string, string>> words = {
        // Очень короткие слова (1-3 символа)
        {"Обь", "реки"}, {"Дон", "реки"}, {"Урал", "реки"},
        {"Нил", "реки"}, {"По", "реки"}, {"Яуза", "реки"},

        // Средние слова (4-6 символов)
        {"Лена", "реки"}, {"Кама", "реки"}, {"Нева", "реки"},
        {"Днепр", "реки"}, {"Дунай", "реки"}, {"Тибр", "реки"},
        {"Рейн", "реки"}, {"Сена", "реки"}, {"Темза", "реки"},

        // Длинные слова (7-10 символов)
        {"Волга", "реки"}, {"Амур", "реки"}, {"Ангара", "реки"},
        {"Енисей", "реки"}, {"Иртыш", "реки"}, {"Амазонка", "реки"},
        {"Миссисипи", "реки"}, {"Северная Двина", "реки"},

        // Города разной длины
        {"Рим", "города"}, {"Париж", "города"}, {"Лондон", "города"},
        {"Берлин", "города"}, {"Мадрид", "города"}, {"Варшава", "города"},
        {"Стамбул", "города"}, {"Санкт-Петербург", "города"},
        {"Екатеринбург", "города"}, {"Владивосток", "города"},

        // Горы
        {"Арарат", "горы"}, {"Казбек", "горы"}, {"Эльбрус", "горы"},
        {"Монблан", "горы"}, {"Эверест", "горы"}, {"Килиманджаро", "горы"},

        // Страны
        {"Иран", "страны"}, {"Куба", "страны"}, {"Ирак", "страны"},
        {"Чад", "страны"}, {"Оман", "страны"}, {"Ливан", "страны"},
        {"Ямайка", "страны"}, {"Канада", "страны"}, {"Бразилия", "страны"},
        {"Австралия", "страны"}, {"Великобритания", "страны"},

        // Животные (для разнообразия)
        {"Лев", "животные"}, {"Тигр", "животные"}, {"Медведь", "животные"},
        {"Слон", "животные"}, {"Жираф", "животные"}, {"Крокодил", "животные"},
        {"Бегемот", "животные"}, {"Носорог", "животные"},

        // Цвета
        {"Синий", "цвета"}, {"Красный", "цвета"}, {"Зелёный", "цвета"},
        {"Жёлтый", "цвета"}, {"Фиолетовый", "цвета"}, {"Оранжевый", "цвета"}
    };

    for (const auto& [word, category] : words) {
        if (!add_word(word, category)) {
            cerr << "Ошибка вставки слова: " << word << endl;
        }
    }

    // Вставляем шаблоны текста
    vector<tuple<string, string, string>> templates = {
        {"", "— реки", ", "},
        {"", "— города", ", "},
        {"", "— горы", ", "},
        {"", "— страны", ", "},
        {"Список: ", "", ", "},
        {"Перечислите: ", "", "; "},
        {"", "являются крупнейшими", ", "},
        {"", "— известные объекты", ", "}
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