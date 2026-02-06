#pragma once
#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

// Forward declaration
struct sqlite3;

namespace OGE {

    class DatabaseManager {
    private:
        sqlite3* db = nullptr;
        std::string db_path;
        bool is_open = false;

    public:
        DatabaseManager();
        ~DatabaseManager();

        // Управление соединением
        bool open(const std::string& path = ""); // Если пусто - in-memory
        bool open_or_create(const std::string& path);
        void close();
        bool is_connected() const { return is_open; }

        // Получить соединение для передачи в ProblemType1
        sqlite3* get_connection() { return db; }
        const std::string& get_db_path() const { return db_path; }

        // CRUD операции (общие для всех задач)
        bool execute_sql(const std::string& sql);
        std::vector<std::vector<std::string>> query(const std::string& sql);

        // Бэкап
        bool backup(const std::string& backup_path);
        bool restore(const std::string& backup_path);

        // Информация
        struct DatabaseInfo {
            std::string path;
            size_t file_size;
            int total_tables;
            std::unordered_map<int, bool> schemas_created;
        };

        DatabaseInfo get_info() const;
    };

} // namespace OGE

#endif // DATABASE_MANAGER_H