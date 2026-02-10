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
            if (db_ptr) {
                sqlite3_close(db_ptr);
            }
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
        virtual ~DatabaseManager() = default;

        bool open();
        bool open_or_create();
        void close();
        bool is_connected() const { return is_open; }

        sqlite3* get_connection() { return db.get(); }
        const string& get_db_path() const { return db_path; }

        bool execute_sql(const string& sql);
        vector<vector<string>> query(const string& sql);

        virtual bool create_table_problem() = 0;
        virtual string take_table_problem() = 0;

        virtual void insert_data_in_table() = 0;
        virtual vector<string> generate_problems(int numbers) = 0;
        virtual string generate_problem() = 0;

        virtual vector<string> get_problems_stats() = 0;
        virtual string get_problem_stats() = 0;

        DatabaseInfo get_info() const;
    };

}