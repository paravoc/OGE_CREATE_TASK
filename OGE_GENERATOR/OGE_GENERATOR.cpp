// main.cpp - Простой вариант с HtmlPageGenerator
#include "database_manager.h"
#include "problem_type1.h"
#include "html_page_generator.h"
#include <iostream>

using namespace std;
using namespace OGE;

int main() {
    cout << "Генератор задач на кодирование\n";

    try {
        // 1. Инициализация
        DatabaseManager db;
        db.open_or_create(":memory:");
        db.create_schema();
        db.seed_default_data();

        // 2. Генератор задач
        ProblemType1 generator(db.get_connection());
        generator.reload_cache();

        // 3. Конфигурация
        ProblemType1::Config config;
        config.allow_removal = true;
        config.word_count_min = 3;
        config.word_count_max = 6;

        // 4. Собираем данные задач
        vector<map<string, string>> problems_data;

        for (int i = 0; i < 5; i++) {
            auto problem = generator.generate(config);

            map<string, string> problem_data;
            problem_data["problem_text"] = problem.problem_text;
            problem_data["correct_answer"] = problem.correct_answer;
            problem_data["solution_explanation"] = problem.solution_explanation;

            problems_data.push_back(problem_data);
            cout << "Задача #" << (i + 1) << " создана\n";
        }

        // 5. Используем HtmlPageGenerator для создания HTML
        PageConfig page_config;
        page_config.title = "Задачи на кодирование";
        page_config.total_problems = 5;

        HtmlPageGenerator html_gen(page_config);

        // Создаем простую HTML страницу (без кнопок)
        bool success = html_gen.generate_and_save_plain(problems_data);

        if (success) {
            cout << "Файл 'задачи.html' создан (только текст, без кнопок)\n";
        }
        else {
            cout << "Ошибка сохранения файла\n";
        }

    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}