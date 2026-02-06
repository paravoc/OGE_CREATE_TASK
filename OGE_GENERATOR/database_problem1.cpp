#include "database_problem1.h"
#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>
#include <sstream>
#include <ctime>
#include <algorithm>

using namespace std;
using namespace OGE;

bool DatabaseProblem1::create_tables() {
    if (!db) return false;

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

    // Выполняем SQL
    char* error_msg = nullptr;
    int rc = sqlite3_exec(db, sql_words.c_str(), nullptr, nullptr, &error_msg);
    if (rc != SQLITE_OK) {
        cerr << "SQL ошибка при создании таблицы слов: " << error_msg << endl;
        sqlite3_free(error_msg);
        return false;
    }

    rc = sqlite3_exec(db, sql_encodings.c_str(), nullptr, nullptr, &error_msg);
    if (rc != SQLITE_OK) {
        cerr << "SQL ошибка при создании таблицы кодировок: " << error_msg << endl;
        sqlite3_free(error_msg);
        return false;
    }

    rc = sqlite3_exec(db, sql_templates.c_str(), nullptr, nullptr, &error_msg);
    if (rc != SQLITE_OK) {
        cerr << "SQL ошибка при создании таблицы шаблонов: " << error_msg << endl;
        sqlite3_free(error_msg);
        return false;
    }

    return true;
}

bool DatabaseProblem1::seed_data() {
    // Создаем таблицы если их нет
    if (!create_tables()) {
        return false;
    }

    // Проверяем, есть ли уже данные
    string check_sql = "SELECT COUNT(*) FROM problem1_words";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, check_sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    bool has_data = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        has_data = (count > 0);
    }
    sqlite3_finalize(stmt);

    if (has_data) {
        return true; // Данные уже есть
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
        if (!add_encoding(name, bits, desc)) {
            cerr << "Ошибка вставки кодировки: " << name << endl;
        }
    }

    // Вставляем слова
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

    // Вставляем шаблоны
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
        if (!add_template(prefix, suffix, delimiter)) {
            cerr << "Ошибка вставки шаблона" << endl;
        }
    }

    return true;
}

vector<DatabaseProblem1::WordRecord> DatabaseProblem1::get_words(const string& category) {
    vector<WordRecord> words;

    string sql = "SELECT word, length, category FROM problem1_words";
    if (!category.empty()) {
        sql += " WHERE category = ?";
    }
    sql += " ORDER BY RANDOM()";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << endl;
        return words;
    }

    if (!category.empty()) {
        sqlite3_bind_text(stmt, 1, category.c_str(), -1, SQLITE_TRANSIENT);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        WordRecord record;
        record.word = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        record.length = sqlite3_column_int(stmt, 1);
        record.category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        words.push_back(record);
    }

    sqlite3_finalize(stmt);
    return words;
}

vector<DatabaseProblem1::EncodingRecord> DatabaseProblem1::get_encodings() {
    vector<EncodingRecord> encodings;

    const char* sql = "SELECT name, bits_per_char, description FROM problem1_encodings ORDER BY RANDOM()";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << endl;
        return encodings;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        EncodingRecord record;
        record.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        record.bits_per_char = sqlite3_column_int(stmt, 1);
        record.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        encodings.push_back(record);
    }

    sqlite3_finalize(stmt);
    return encodings;
}

bool DatabaseProblem1::add_word(const string& word, const string& category) {
    int length = static_cast<int>(word.length());

    const char* sql = "INSERT OR IGNORE INTO problem1_words (word, length, category) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, word.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, length);
    sqlite3_bind_text(stmt, 3, category.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}

bool DatabaseProblem1::add_encoding(const string& name, int bits, const string& desc) {
    const char* sql = "INSERT OR IGNORE INTO problem1_encodings (name, bits_per_char, description) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, bits);
    sqlite3_bind_text(stmt, 3, desc.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}

vector<DatabaseProblem1::TextTemplate> DatabaseProblem1::get_templates() {
    vector<TextTemplate> templates;

    const char* sql = "SELECT prefix, suffix, delimiter FROM problem1_text_templates ORDER BY RANDOM()";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << endl;
        return templates;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TextTemplate template_rec;
        template_rec.prefix = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        template_rec.suffix = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        template_rec.delimiter = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        templates.push_back(template_rec);
    }

    sqlite3_finalize(stmt);
    return templates;
}

bool DatabaseProblem1::add_template(const std::string& prefix, const std::string& suffix,
    const std::string& delimiter) {
    const char* sql = "INSERT INTO problem1_text_templates (prefix, suffix, delimiter) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, prefix.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, suffix.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, delimiter.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}

int DatabaseProblem1::get_word_count(const string& category) {
    string sql = "SELECT COUNT(*) FROM problem1_words";
    if (!category.empty()) {
        sql += " WHERE category = ?";
    }

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << endl;
        return 0;
    }

    if (!category.empty()) {
        sqlite3_bind_text(stmt, 1, category.c_str(), -1, SQLITE_TRANSIENT);
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}

int DatabaseProblem1::get_encoding_count() {
    const char* sql = "SELECT COUNT(*) FROM problem1_encodings";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << endl;
        return 0;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}

bool DatabaseProblem1::clear_data() {
    if (!db) return false;

    bool success = true;

    const char* queries[] = {
        "DELETE FROM problem1_words",
        "DELETE FROM problem1_encodings",
        "DELETE FROM problem1_text_templates"
    };

    for (const char* query : queries) {
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << endl;
            success = false;
            continue;
        }

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE) {
            cerr << "Ошибка выполнения запроса: " << sqlite3_errmsg(db) << endl;
            success = false;
        }
    }

    return success;
}

bool DatabaseProblem1::tables_exist() const {
    if (!db) return false;

    const char* sql = "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name IN ('problem1_words', 'problem1_encodings', 'problem1_text_templates')";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        return false;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count == 3;
}