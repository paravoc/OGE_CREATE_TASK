#include"database_manager.h"
#include<iostream>

using namespace OGE;
using namespace std;

bool DatabaseManager::open_or_create() {
	if (is_open) return true;

	close();

	sqlite3* raw_db = nullptr;
	int res_raw_db = sqlite3_open(db_path.c_str(), &raw_db);

	if (res_raw_db != SQLITE_OK) {
		if(!res_raw_db)
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

vector<vector<string>> DatabaseManager::query(const string& sql) {
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