#include "database_manager.h"
#include<iostream>

struct EmptyDB : OGE::DatabaseManager {
    bool create_table_problem() override { return true; }
    string take_table_problem() override { return ""; }
    void insert_data_in_table() override {}
    vector<string> generate_problems(int) override { return {}; }
    string generate_problem() override { return ""; }
    vector<string> get_problems_stats() override { return {}; }
    string get_problem_stats() override { return ""; }
};

int main() {
    EmptyDB db; 
    cout << db.open_or_create();
    db.close();
    return 0;
}