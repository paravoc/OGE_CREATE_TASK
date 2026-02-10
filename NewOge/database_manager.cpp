#include"database_manager.h"
#include<iostream>
#include <filesystem> 


using namespace OGE;
using namespace std;
namespace fs = std::filesystem;

bool DatabaseManager::open_or_create() {
	if (is_open) return true;

	close();

	sqlite3* raw_db = nullptr;
	int res_raw_db = sqlite3_open(db_path.c_str(), &raw_db);

	if (res_raw_db != SQLITE_OK) {
		if(raw_db)
			sqlite3_close(raw_db);
		is_open = false;
		return false;
	}
	db.reset(raw_db);
	is_open = true;

	execute_sql("PRAGMA foreign_keys = ON;");
	execute_sql("PRAGMA journal_mode = WAL;");

	const string meta_sql = R"(
    CREATE TABLE IF NOT EXISTS database_meta (
        key TEXT PRIMARY KEY,
        value TEXT
    );
		)";

	if (!execute_sql(meta_sql)) {
		close();
		return false;
	}
	return true;
}

void DatabaseManager::close() {
	db.reset();
	is_open = false;
}

bool DatabaseManager::execute_sql(const string& sql) {
	if (!is_open) {
		return false;
	}

	char* error_message = nullptr;

	int result_query = sqlite3_exec(db.get(), sql.c_str(), nullptr, nullptr, &error_message);

	if (result_query != SQLITE_OK) {
		if (error_message) {
			cerr << error_message << endl;
			cerr << sql << endl;
		}
		return false;
	}
	return true;
}

vector<vector<string>> DatabaseManager::query(const string& sql) const  {
	vector<vector<string>> results;
	if (!is_open or !db) {
		return results;
	}

	sqlite3_stmt* stmt = nullptr;

	if (sqlite3_prepare_v2(db.get(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		return results;
	}

	int cols = sqlite3_column_count(stmt);


	int step_result;
	while ((step_result =sqlite3_step(stmt)) == SQLITE_ROW) {
		vector<string> row;
		row.reserve(cols);

		for (int i = 0; i < cols; ++i) {
			const unsigned char* text = sqlite3_column_text(stmt, i);
			if (text) {
				row.emplace_back(reinterpret_cast<const char*>(text));
			}
			else {
				row.emplace_back("");
			}
		}
		results.emplace_back(move(row));
	}
	sqlite3_finalize(stmt);
	return results;
}

DatabaseInfo DatabaseManager::get_info() const {
	DatabaseInfo info{};

	try {
		if (fs::exists(db_path)) {
			info.file_size = fs::file_size(db_path);
		}
		else {
			info.file_size = 0;
		}
	}
	catch (...) {
		info.file_size = 0;
	}
	
	if (is_open) {
		auto tables_result = query("SELECT COUNT(*) FROM sqlite_master WHERE type='table';");
		if (!tables_result.empty() && !tables_result[0].empty()) {
			try {
				info.total_tables = stoi(tables_result[0][0]);
			}
			catch (...) {
				info.total_tables = 0;
			}
		}

		auto problem_tables = query(
			R"(
				SELECT name FROM sqlite_master 
				WHERE type='table' AND name LIKE 'problem_%' 
				ORDER BY name;  
			)"
		);

		for (const auto& row : problem_tables) {
			if (!row.empty()) {
				const string& table_name = row[0];
				if (table_name.length() > 8 && table_name.substr(0, 8) == "problem_") {
					try {
						int problem_num = stoi(table_name.substr(8));
						if (problem_num >= 1 && problem_num <= 16) { 
							info.tables_created[problem_num] = true;
						}
					}
					catch (...) {

					}
				}
			}
		}
	}
	 else {
		info.total_tables = 0;
		info.tables_created.clear();
		}

	return info;
}