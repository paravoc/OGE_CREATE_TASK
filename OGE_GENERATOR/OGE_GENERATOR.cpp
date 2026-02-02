// main.cpp
#include "problem_type1.h"
#include "html_page_generator.h"
#include "database_manager.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace OGE;

int main() {
    setlocale(LC_ALL, "ru");

    // 1. Настройка БД
    DatabaseManager db;
    db.open_or_create("oge.db");
    db.create_schema();
    db.seed_default_data();

    sqlite3* conn = db.get_connection();

    // 2. Создаем генератор задач
    ProblemType1 generator(conn);

    // 3. Конфигурация задач
    ProblemType1::Config config;
    config.allow_removal = true;
    config.word_count_min = 4;
    config.word_count_max = 7;
    config.word_category = "реки";

    // 4. Сколько задач генерировать
    int problems_count = 5;

    // 5. Генерируем HTML задачи
    cout << "Генерация " << problems_count << " задач..." << endl;
    string problems_html = generator.generate_html_problems(problems_count, config);

    // 6. Генерируем JSON данные для JS
    vector<map<string, string>> problems_json =
        generator.generate_problems_json(problems_count, config);

    // 7. Создаем страницу
    PageConfig page_config;
    page_config.title = "Тренировка ОГЭ - " + to_string(problems_count) + " задач";
    page_config.version = "v1.0.0";
    page_config.total_problems = problems_count;
    page_config.output_file = "generated_page.html";
    page_config.css_file = "web/cosmic.css";
    page_config.js_file = "web/cosmic.js";

    HtmlPageGenerator page_generator(page_config);

    // 8. Генерируем и сохраняем
    vector<string> html_problems = { problems_html };
    if (page_generator.generate_and_save(html_problems, problems_json)) {
        cout << "Страница успешно создана: " << page_config.output_file << endl;
        cout << "Всего задач: " << problems_count << endl;
    }
    else {
        cerr << "Ошибка при сохранении страницы" << endl;
    }

    // 9. Также можно сохранить только контент задач
    ofstream content_file("problems_content.html");
    if (content_file.is_open()) {
        content_file << problems_html;
        content_file.close();
        cout << "Контент задач сохранен: problems_content.html" << endl;
    }

    return 0;
}