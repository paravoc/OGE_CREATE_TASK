#pragma once
#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <memory>
#include <vector>

// Forward declaration
struct sqlite3;

using namespace std;

namespace OGE {

    class DatabaseManager {
    private:
        sqlite3* db = nullptr;
        string db_path;
        bool is_open = false;

    public:
        DatabaseManager();
        ~DatabaseManager();

        // Управление соединением
        bool open(const string& path = ""); // Если пусто - in-memory
        bool open_or_create(const string& path);
        void close();
        bool is_connected() const { return is_open; }

        // Получить соединение для передачи в ProblemType1
        sqlite3* get_connection() { return db; }

        // Создание схемы
        bool create_schema();
        bool create_problem1_tables();

        // Заполнение данными
        bool seed_default_data();
        bool seed_problem1_data();

        // CRUD операции
        bool execute_sql(const string& sql);
        vector<vector<string>> query(const string& sql);

        // Для ProblemType1
        struct WordRecord {
            string word;
            int length;
            string category;
        };

        struct EncodingRecord {
            string name;
            int bits_per_char;
            string description;
        };

        vector<WordRecord> get_words(const string& category = "");
        vector<EncodingRecord> get_encodings();
        bool add_word(const string& word, const string& category);
        bool add_encoding(const string& name, int bits, const string& desc);

        // Бэкап
        bool backup(const string& backup_path);
        bool restore(const string& backup_path);

        // Информация
        struct DatabaseInfo {
            string path;
            size_t file_size;
            int total_tables;
            bool schema_created;
        };

        DatabaseInfo get_info() const;
    };

} // namespace OGE

#endif // DATABASE_MANAGER_H