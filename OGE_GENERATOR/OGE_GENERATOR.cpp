#include "database_manager.h"
#include "database_problem1.h"
#include "problem_type1.h"
#include "html_page_generator.h"
#include <iostream>
#include <locale>
#include <vector>
#include <map>

using namespace std;
using namespace OGE;

int main() {
    // Устанавливаем локаль для поддержки русского языка
    setlocale(LC_ALL, "ru");
    cout << "Генератор задач ОГЭ по информатике (Тип 1)\n";
    cout << "==========================================\n\n";

    try {
        // 1. ИНИЦИАЛИЗАЦИЯ БАЗЫ ДАННЫХ
        cout << "[1/7] Инициализация базы данных..." << endl;
        DatabaseManager db;

        // Открываем in-memory базу данных
        if (!db.open_or_create(":memory:")) {
            cerr << "ОШИБКА: Не удалось открыть базу данных" << endl;
            return 1;
        }
        cout << "   ✓ База данных открыта (in-memory)" << endl;

        // 2. ИНИЦИАЛИЗАЦИЯ ДЛЯ ЗАДАЧИ 1
        cout << "[2/7] Инициализация для задачи 1..." << endl;
        DatabaseProblem1 db_problem1(db.get_connection());

        // Создаем таблицы
        if (!db_problem1.create_tables()) {
            cerr << "ОШИБКА: Не удалось создать таблицы для задачи 1" << endl;
            return 1;
        }
        cout << "   ✓ Таблицы созданы" << endl;

        // Заполняем данными
        if (!db_problem1.seed_data()) {
            cerr << "ОШИБКА: Не удалось заполнить данные" << endl;
            return 1;
        }
        cout << "   ✓ Данные заполнены:" << endl;
        cout << "     - Слов в базе: " << db_problem1.get_word_count() << endl;
        cout << "     - Кодировок: " << db_problem1.get_encoding_count() << endl;

        // 3. СОЗДАНИЕ ГЕНЕРАТОРА ЗАДАЧ
        cout << "[3/7] Создание генератора задач..." << endl;
        ProblemType1 generator(db_problem1);

        // Проверяем загрузку кэша
        if (!generator.is_cache_loaded()) {
            cout << "   Перезагружаю кэш данных..." << endl;
            generator.reload_cache();
            if (!generator.is_cache_loaded()) {
                cerr << "ОШИБКА: Не удалось загрузить данные из БД" << endl;
                return 1;
            }
        }
        cout << "   ✓ Генератор готов" << endl;

        // Получаем статистику
        auto stats = generator.get_stats();
        cout << "   Статистика:" << endl;
        cout << "     - Всего слов: " << stats.total_words << endl;
        cout << "     - Всего кодировок: " << stats.total_encodings << endl;
        if (!stats.words_by_category.empty()) {
            cout << "     - Слова по категориям:" << endl;
            for (const auto& [category, count] : stats.words_by_category) {
                cout << "       * " << category << ": " << count << endl;
            }
        }

        // 4. НАСТРОЙКА КОНФИГУРАЦИИ
        cout << "[4/7] Настройка конфигурации..." << endl;
        ProblemType1::Config config;
        config.allow_removal = true;
        config.allow_addition = true;
        config.allow_encoding_change = true;
        config.word_count_min = 7;
        config.word_count_max = 10;
        config.difficulty = 2;

        cout << "   Конфигурация:" << endl;
        cout << "     - Типы задач: ";
        vector<string> types;
        if (config.allow_removal) types.push_back("удаление");
        if (config.allow_addition) types.push_back("добавление");
        if (config.allow_encoding_change) types.push_back("смена кодировки");
        for (size_t i = 0; i < types.size(); i++) {
            if (i > 0) cout << ", ";
            cout << types[i];
        }
        cout << endl;
        cout << "     - Количество слов: " << config.word_count_min
            << "-" << config.word_count_max << endl;
        cout << "     - Сложность: " << config.difficulty << "/3" << endl;

        // 5. ГЕНЕРАЦИЯ ЗАДАЧ
        cout << "[5/7] Генерация задач..." << endl;
        int count_problems = 200;

        vector<map<string, string>> problems_data;
        problems_data.reserve(count_problems);

        // Счетчики типов задач
        int removal_count = 0;
        int addition_count = 0;
        int encoding_change_count = 0;

        for (int i = 0; i < count_problems; i++) {
            try {
                auto problem = generator.generate(config);

                map<string, string> problem_data;
                problem_data["problem_text"] = problem.problem_text;
                problem_data["correct_answer"] = problem.correct_answer;
                problem_data["solution_explanation"] = problem.solution_explanation;

                // Сохраняем метаданные
                problem_data["meta_problem_number"] = to_string(i + 1);
                problem_data["meta_scenario"] = problem.meta.count("scenario") ?
                    problem.meta.at("scenario") : "unknown";

                // Считаем типы задач
                if (problem.meta.count("scenario")) {
                    string scenario = problem.meta.at("scenario");
                    if (scenario == "removal") removal_count++;
                    else if (scenario == "addition") addition_count++;
                    else if (scenario == "encoding_change") encoding_change_count++;
                }

                problems_data.push_back(problem_data);

                // Прогресс
                if ((i + 1) % 20 == 0) {
                    cout << "   " << (i + 1) << "/" << count_problems << " задач сгенерировано" << endl;
                }

            }
            catch (const exception& e) {
                cerr << "   Ошибка при генерации задачи #" << (i + 1) << ": " << e.what() << endl;
            }
        }

        cout << "   ✓ Генерация завершена" << endl;
        cout << "   Статистика типов задач:" << endl;
        cout << "     - Удаление: " << removal_count << endl;
        cout << "     - Добавление: " << addition_count << endl;
        cout << "     - Смена кодировки: " << encoding_change_count << endl;
        cout << "     - Всего сгенерировано: " << problems_data.size() << " задач" << endl;

        if (problems_data.empty()) {
            cerr << "ОШИБКА: Не удалось сгенерировать ни одной задачи" << endl;
            return 1;
        }

        // 6. СОЗДАНИЕ HTML СТРАНИЦЫ
        cout << "[6/7] Создание HTML страницы..." << endl;

        PageConfig page_config;
        page_config.title = "Задачи на кодирование (Тип 1) - ОГЭ по информатике";
        page_config.output_file = "site/задачи.html";
        page_config.total_problems = static_cast<int>(problems_data.size());

        HtmlPageGenerator html_gen(page_config);

        bool success = html_gen.generate_and_save_ansi(problems_data);

        if (!success) {
            cerr << "ОШИБКА: Не удалось создать HTML файл" << endl;

            // Пробуем альтернативное расположение
            cout << "   Пробую альтернативное расположение..." << endl;
            page_config.output_file = "задачи.html";
            HtmlPageGenerator html_gen2(page_config);

            if (!html_gen2.generate_and_save_ansi(problems_data)) {
                cerr << "ОШИБКА: Не удалось сохранить файл" << endl;
                return 1;
            }
        }

        cout << "   ✓ HTML страница создана: " << page_config.output_file << endl;

        // 7. СОЗДАНИЕ ДОПОЛНИТЕЛЬНЫХ ФАЙЛОВ
        cout << "[7/7] Создание дополнительных файлов..." << endl;

        // Создаем файл с простыми задачами (только удаление)
        if (removal_count > 0) {
            vector<map<string, string>> simple_problems;
            ProblemType1::Config simple_config;
            simple_config.allow_removal = true;
            simple_config.allow_addition = false;
            simple_config.allow_encoding_change = false;
            simple_config.word_count_min = 3;
            simple_config.word_count_max = 5;
            simple_config.difficulty = 1;

            for (int i = 0; i < 20 && i < count_problems; i++) {
                try {
                    auto problem = generator.generate(simple_config);

                    map<string, string> problem_data;
                    problem_data["problem_text"] = problem.problem_text;
                    problem_data["correct_answer"] = problem.correct_answer;
                    problem_data["solution_explanation"] = problem.solution_explanation;

                    simple_problems.push_back(problem_data);
                }
                catch (...) {
                    // Пропускаем ошибки
                }
            }

            if (!simple_problems.empty()) {
                PageConfig simple_page_config = page_config;
                simple_page_config.title = "Простые задачи на кодирование (Тип 1)";
                simple_page_config.output_file = "site/простые_задачи.html";
                simple_page_config.total_problems = static_cast<int>(simple_problems.size());

                HtmlPageGenerator simple_gen(simple_page_config);
                if (simple_gen.generate_and_save_ansi(simple_problems)) {
                    cout << "   ✓ Простые задачи: " << simple_problems.size()
                        << " задач -> " << simple_page_config.output_file << endl;
                }
            }
        }

        // Создаем файл со сложными задачами (смена кодировки)
        if (encoding_change_count > 0) {
            vector<map<string, string>> hard_problems;
            ProblemType1::Config hard_config;
            hard_config.allow_removal = false;
            hard_config.allow_addition = false;
            hard_config.allow_encoding_change = true;
            hard_config.word_count_min = 4;
            hard_config.word_count_max = 6;
            hard_config.difficulty = 3;

            for (int i = 0; i < 15 && i < count_problems; i++) {
                try {
                    auto problem = generator.generate(hard_config);

                    map<string, string> problem_data;
                    problem_data["problem_text"] = problem.problem_text;
                    problem_data["correct_answer"] = problem.correct_answer;
                    problem_data["solution_explanation"] = problem.solution_explanation;

                    hard_problems.push_back(problem_data);
                }
                catch (...) {
                    // Пропускаем ошибки
                }
            }

            if (!hard_problems.empty()) {
                PageConfig hard_page_config = page_config;
                hard_page_config.title = "Сложные задачи на кодирование (Тип 1)";
                hard_page_config.output_file = "site/сложные_задачи.html";
                hard_page_config.total_problems = static_cast<int>(hard_problems.size());

                HtmlPageGenerator hard_gen(hard_page_config);
                if (hard_gen.generate_and_save_ansi(hard_problems)) {
                    cout << "   ✓ Сложные задачи: " << hard_problems.size()
                        << " задач -> " << hard_page_config.output_file << endl;
                }
            }
        }

        cout << "\n" << string(50, '=') << endl;
        cout << "✓ ГЕНЕРАЦИЯ ЗАВЕРШЕНА УСПЕШНО!" << endl;
        cout << string(50, '=') << endl;
        cout << "\nСозданные файлы:" << endl;
        cout << "  1. " << page_config.output_file << " - "
            << problems_data.size() << " задач (основной файл)" << endl;
        cout << "  2. site/простые_задачи.html - задачи на удаление (легкие)" << endl;
        cout << "  3. site/сложные_задачи.html - задачи на смену кодировки (сложные)" << endl;
        cout << "\nОткройте файл '" << page_config.output_file
            << "' в любом браузере для просмотра задач." << endl;
        cout << "\nСтатистика базы данных:" << endl;
        cout << "  - Всего слов: " << stats.total_words << endl;
        cout << "  - Всего кодировок: " << stats.total_encodings << endl;
        cout << "  - Категорий слов: " << stats.words_by_category.size() << endl;

    }
    catch (const exception& e) {
        cerr << "\n" << string(50, '=') << endl;
        cerr << "✗ КРИТИЧЕСКАЯ ОШИБКА!" << endl;
        cerr << string(50, '=') << endl;
        cerr << "\nСообщение об ошибке: " << e.what() << endl;
        cerr << "\nВозможные причины:" << endl;
        cerr << "  1. Проблемы с доступом к базе данных" << endl;
        cerr << "  2. Недостаточно данных в БД" << endl;
        cerr << "  3. Проблемы с файловой системой" << endl;
        return 1;
    }

    cout << "\nНажмите Enter для выхода...";
    cin.get();

    return 0;
}