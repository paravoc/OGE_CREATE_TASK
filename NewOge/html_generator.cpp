// html_generator.cpp
#include "html_generator.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace OGE {

    // Конструктор
    HtmlGenerator::HtmlGenerator(const PageSettings& settings)
        : page_settings(settings) {
    }

    string HtmlGenerator::generate_styles() const {
        stringstream css;
        css << R"(
        <link rel="stylesheet" href="css/base.css">
        <link rel="stylesheet" href="css/layout.css">
        <link rel="stylesheet" href="css/problem-card.css">
        <link rel="stylesheet" href="css/forms.css">
        <link rel="stylesheet" href="css/progress.css">
        <link rel="stylesheet" href="css/feedback.css">
    

        <style>
            /* Динамические стили, зависящие от настроек */
            body {
                background-color: )" << (page_settings.dark_mode ? "#1a1a1a" : "#f5f5f5") << R"(;
                color: )" << (page_settings.dark_mode ? "#ffffff" : "#333333") << R"(;
            }
            
            .header {
                background: )" << (page_settings.dark_mode ? "#2d2d2d" : "#ffffff") << R"(;
            }
            
            .problem-wrapper {
                background: )" << (page_settings.dark_mode ? "#2d2d2d" : "#ffffff") << R"(;
            }
            
            input[type="text"] {
                background: )" << (page_settings.dark_mode ? "#3d3d3d" : "#ffffff") << R"(;
                color: )" << (page_settings.dark_mode ? "#ffffff" : "#333333") << R"(;
            }
            
            .solution-wrapper, .hint-wrapper {
                background: )" << (page_settings.dark_mode ? "#3d3d3d" : "#f9f9f9") << R"(;
            }
        </style>
    )";
        return css.str();
    }

    // Генерация скриптов - только подключение внешнего JS
    string HtmlGenerator::generate_scripts() const {
        stringstream js;
        js << R"(
        <!-- Подключаем внешний JavaScript -->
                <!-- JavaScript - ВАЖЕН ПОРЯДОК! -->
                <script src="js/config.js"></script>
                <script src="js/utils.js"></script>
                <script src="js/progress.js"></script>
                <script src="js/checkers.js"></script>
                <script src="js/handlers.js"></script>
                <script src="js/init.js"></script>
    )";
        return js.str();
    }

    // Генерация заголовка
   // Генерация заголовка
   // Генерация заголовка
    string HtmlGenerator::generate_header() const {
        stringstream header;

        std::time_t now = std::time(nullptr);
        std::tm timeinfo;
        localtime_s(&timeinfo, &now);

        // Подсчитываем общее количество задач
        int total_problems = 0;
        for (const auto& [problem_type, count] : page_settings.problem_numbers) {
            total_problems += count;
        }

        header << R"(
    <!DOCTYPE html>
    <html lang="ru">
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <meta name="generator" content="OGE HTML Generator">
        <title>Тренировочный вариант ОГЭ</title>
    )" << generate_styles() << R"(
    </head>
    <body>
        <div class="container">
            <div class="header">
                <h1>Тренировочный вариант ОГЭ</h1>
                <p>Сгенерировано: )" << put_time(&timeinfo, "%d.%m.%Y %H:%M:%S") << R"(</p>
                )";

        if (page_settings.show_score) {
            header << "<p class='score'>Максимальный балл: " << page_settings.passing_score << "</p>";
        }

        if (page_settings.show_progress_bar) {
            header << R"(
                <div class="progress-container">
                    <div class="progress-label">Прогресс выполнения:</div>
                    <div class="progress">
                        <progress id="main-progress" value="0" max="100"></progress>
                    </div>
                    <div class="progress-stats">
                        <span id="correct-count">0</span>/<span id="total-count">)"
                << total_problems << R"(</span> решено
                    </div>
                </div>
        )";
        }

        header << R"(
            </div>
            <div class="problems-grid">
    )";

        return header.str();
    }
    // Генерация подвала
    string HtmlGenerator::generate_footer() const {
        stringstream footer;

        footer << R"(
                </div>
        )";

        // Удалены кнопки сохранения/загрузки прогресса
        // Удален таймер

        footer << R"(
            </div>
        )" << generate_scripts() << R"(
        </body>
        </html>
        )";

        return footer.str();
    }

    // Генерация списка задач
    string HtmlGenerator::generate_problem_list() const {
        stringstream content;

        for (const auto& problem : all_problems) {
            if (problem) {
                content << problem->generate_html() << "\n";
            }
        }

        return content.str();
    }

    // Генерация всех задач
    void HtmlGenerator::generate_all_problems() {
        all_problems.clear();

        for (const auto& [problem_type, count] : page_settings.problem_numbers) {
            for (int i = 0; i < count; i++) {
                unique_ptr<ProblemBase> problem;

                ProblemMeta meta;
                meta.type_id = problem_type + "_" + to_string(i + 1);
                meta.display_name = problem_type;
                meta.problem_number = i + 1;
                meta.default_score = 10;
                meta.is_active = true;

                // Динамическое создание задач через фабрику
                // Здесь можно добавить другие типы задач

                if (problem) {
                    GenerationConfig config;
                    config.difficulty = Difficulty::MEDIUM;
                    problem->generate(config);
                    all_problems.push_back(std::move(problem));
                }
            }
        }
    }

    // Добавление конкретной задачи
    void HtmlGenerator::add_problem(unique_ptr<ProblemBase> problem) {
        if (problem) {
            all_problems.emplace_back(std::move(problem));
        }
    }

    // Генерация полной HTML страницы
    string HtmlGenerator::generate_full_page() {
        stringstream full_page;
        full_page << generate_header();
        full_page << generate_problem_list();
        full_page << generate_footer();

        html_page = full_page.str();
        return html_page;
    }

    // Генерация только контента
    string HtmlGenerator::generate_content_only() {
        stringstream content;
        content << generate_problem_list();
        return content.str();
    }

    // Очистка всех задач
    void HtmlGenerator::clear_problems() {
        all_problems.clear();
    }

    // Сохранение в файл
    bool HtmlGenerator::save_to_file(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        file << html_page;
        return file.good();
    }

}