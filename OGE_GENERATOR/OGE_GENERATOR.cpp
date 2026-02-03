#include "database_manager.h"
#include "problem_type1.h"
#include "html_page_generator.h"
#include <iostream>

int main() {
    std::setlocale(LC_ALL, "ru");
    std::cout << "Генератор задач на кодирование (ANSI)\n";

    try {
        // 1. Инициализация
        OGE::DatabaseManager db;
        db.open_or_create(":memory:");
        db.create_schema();
        db.seed_default_data();

        // 2. Генератор задач
        OGE::ProblemType1 generator(db.get_connection());
        generator.reload_cache();

        // 3. Конфигурация
        OGE::ProblemType1::Config config;
        config.allow_removal = true;
        config.word_count_min = 3;
        config.word_count_max = 6;

        // 4. Собираем данные задач
        std::vector<std::map<std::string, std::string>> problems_data;

        for (int i = 0; i < 100; i++) {
            auto problem = generator.generate(config);

            std::map<std::string, std::string> problem_data;
            problem_data["problem_text"] = problem.problem_text;
            problem_data["correct_answer"] = problem.correct_answer;
            problem_data["solution_explanation"] = problem.solution_explanation;

            problems_data.push_back(problem_data);
            std::cout << "Задача #" << (i + 1) << " создана\n";
        }

        // 5. Используем HtmlPageGenerator для создания ANSI HTML
        OGE::PageConfig page_config;
        page_config.title = "Задачи на кодирование";
        page_config.total_problems = 100;

        OGE::HtmlPageGenerator html_gen(page_config);

        // Создаем ANSI HTML (без кнопок)
        bool success = html_gen.generate_and_save_ansi(problems_data);

        if (success) {
            std::cout << "Файл '" << page_config.output_file << "' создан в кодировке ANSI\n";
        }
        else {
            std::cout << "Ошибка сохранения файла\n";
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}