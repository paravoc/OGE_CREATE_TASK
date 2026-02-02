// main.cpp
#include "database_manager.h"
#include "problem_type1.h"
#include <iostream>

using namespace std;
using namespace OGE;

int main() {
    // 1. Создаем менеджер БД
    DatabaseManager db_manager;

    // 2. Открываем/создаем БД
    if (!db_manager.open_or_create("oge_data.db")) {
        cerr << "Не удалось открыть БД" << endl;
        return 1;
    }

    // 3. Создаем таблицы и заполняем данными
    if (!db_manager.create_schema()) {
        cerr << "Не удалось создать схему" << endl;
        return 1;
    }

    if (!db_manager.seed_default_data()) {
        cerr << "Не удалось заполнить данными" << endl;
        return 1;
    }

    // 4. Получаем соединение
    sqlite3* connection = db_manager.get_connection();

    // 5. Создаем генератор задач, передавая соединение
    ProblemType1 generator(connection);

    // 6. Настраиваем
    ProblemType1::Config config;
    config.allow_removal = true;
    config.allow_addition = true;
    config.word_count_min = 4;
    config.word_count_max = 7;
    config.word_category = "реки";

    // 7. Генерируем задачи
    auto problems = generator.generate_batch(3, config);

    for (size_t i = 0; i < problems.size(); i++) {
        cout << "Задача " << (i + 1) << ":\n";
        cout << problems[i].problem_text << "\n\n";
        cout << "Ответ: " << problems[i].correct_answer << "\n";
        cout << "-------------------\n";
    }

    // 8. Получаем статистику
    auto stats = generator.get_stats();
    cout << "Всего слов в БД: " << stats.total_words << endl;
    cout << "Всего кодировок: " << stats.total_encodings << endl;

    // 9. Бэкап БД
    db_manager.backup("oge_data_backup.db");

    return 0;
}