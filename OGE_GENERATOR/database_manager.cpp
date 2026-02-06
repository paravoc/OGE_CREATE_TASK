#include "database_manager.h"
#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>
#include <sstream>
#include <ctime>
#include <unordered_map>

using namespace std;
using namespace OGE;
namespace fs = filesystem;

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
        fs::path db_path_obj(path);
        auto parent_dir = db_path_obj.parent_path();
        if (!parent_dir.empty() && !fs::exists(parent_dir)) {
            fs::create_directories(parent_dir);
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
        fs::copy_file(backup_path, db_path, fs::copy_options::overwrite_existing);
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

    if (!db) return info;

    // Получаем размер файла
    if (db_path != ":memory:") {
        try {
            info.file_size = fs::file_size(db_path);
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
    }

    // Проверяем наличие схем для разных задач
    for (int i = 1; i <= 16; i++) {
        string check_sql = "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name LIKE 'problem" +
            to_string(i) + "_%'";
        auto check_result = const_cast<DatabaseManager*>(this)->query(check_sql);

        bool has_schema = (!check_result.empty() && !check_result[0].empty() &&
            stoi(check_result[0][0]) > 0);
        info.schemas_created[i] = has_schema;
    }

    return info;
}