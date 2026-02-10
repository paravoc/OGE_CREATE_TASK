#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <sqlite3.h>

using namespace std;

namespace OGE {
    struct DatabaseInfo {
        size_t file_size;
        int total_tables;
        unordered_map<int, bool> tables_created;
    };

    struct SQLDeleter {
        void operator()(sqlite3* db_ptr) const {
            if (db_ptr) sqlite3_close(db_ptr);
        }
    };

    class DatabaseManager {
    protected:
        const string db_path = "database/problems.db";
        unique_ptr<sqlite3, SQLDeleter> db = nullptr;
        bool is_open = false;
        DatabaseInfo db_info{};

    public:
        DatabaseManager() = default;
        explicit DatabaseManager(const string& custom_path) : db_path(custom_path) {}
        virtual ~DatabaseManager() = default;

        bool open_or_create();
        void close();
        bool is_connected() const { return is_open; }

        sqlite3* get_connection() { return db.get(); }
        const string& get_db_path() const { return db_path; }

        bool execute_sql(const string& sql);
        vector<vector<string>> query(const string& sql) const;

        virtual bool create_stats_table() = 0; 
        virtual string get_table_name() = 0;   


        virtual void record_generation(int problem_num, const string& task_type) = 0;
        virtual void record_solution(int problem_num, bool correct, double time_sec) = 0;


        virtual unordered_map<string, int> get_generation_stats() const = 0;
        virtual int get_total_generated() const = 0;

        DatabaseInfo get_info() const;

        static bool database_exists(const string& path);
        static bool remove_database(const string& path);
    };
}