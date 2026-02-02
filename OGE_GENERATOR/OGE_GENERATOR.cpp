#include "database_manager.h"
#include "problem_type1.h"
#include <iostream>

int main() {
    setlocale(LC_ALL, "ru");
    // 1. Создаем и настраиваем БД
    OGE::DatabaseManager db;
    db.open_or_create("oge.db");
    db.create_schema();
    db.seed_default_data();

    // 2. Получаем соединение
    sqlite3* conn = db.get_connection();

    // 3. Создаем генератор
    OGE::ProblemType1 generator(conn);

    // 4. Настраиваем
    OGE::ProblemType1::Config config;
    config.allow_removal = true;
    config.word_count_min = 4;
    config.word_count_max = 7;
    config.word_category = "реки";

    // 5. Генерируем задачу
    auto problem = generator.generate(config);

    // 6. Выводим
    cout << problem.problem_text << "\n\n";
    cout << "Ответ: " << problem.correct_answer << "\n";
    cout << "JSON:\n" << problem.to_json() << "\n";

    return 0;
}