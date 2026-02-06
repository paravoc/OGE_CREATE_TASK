#include "database_manager.h"
#include "database_problem1.h"
#include "problem_type1.h"
#include "html_page_generator.h"
#include <iostream>
#include <locale>
#include <vector>
#include <map>
#include <string>

using namespace std;
using namespace OGE;

int main() {
    // Устанавливаем локаль
    setlocale(LC_ALL, "ru");

    cout << "Генератор задач ОГЭ по информатике" << endl;
    cout << "==================================" << endl;

    try {
        // 1. ИНИЦИАЛИЗАЦИЯ БАЗЫ ДАННЫХ
        cout << "\n1. Инициализация базы данных..." << endl;
        DatabaseManager db;

        if (!db.open_or_create(":memory:")) {
            cerr << "Ошибка: Не удалось открыть базу данных" << endl;
            return 1;
        }
        cout << "   База данных открыта" << endl;

        // 2. НАСТРОЙКА ДЛЯ ЗАДАЧИ 1
        cout << "2. Настройка задачи 1..." << endl;
        DatabaseProblem1 db_problem1(db.get_connection());

        if (!db_problem1.create_tables()) {
            cerr << "Ошибка: Не удалось создать таблицы" << endl;
            return 1;
        }
        cout << "   Таблицы созданы" << endl;

        if (!db_problem1.seed_data()) {
            cerr << "Ошибка: Не удалось заполнить данные" << endl;
            return 1;
        }
        cout << "   Данные заполнены" << endl;

        // 3. СОЗДАНИЕ ГЕНЕРАТОРА
        cout << "3. Создание генератора задач..." << endl;
        ProblemType1 generator(db_problem1);

        if (!generator.is_cache_loaded()) {
            generator.reload_cache();
            if (!generator.is_cache_loaded()) {
                cerr << "Ошибка: Не удалось загрузить данные из БД" << endl;
                return 1;
            }
        }
        cout << "   Генератор готов" << endl;

        // 4. КОНФИГУРАЦИЯ
        ProblemType1::Config config;
        config.allow_removal = true;
        config.allow_addition = true;
        config.allow_encoding_change = true;
        config.word_count_min = 7;
        config.word_count_max = 102;
        config.difficulty = 2;

        // 5. ГЕНЕРАЦИЯ ЗАДАЧ
        cout << "4. Генерация задач..." << endl;
        int count_problems = 5; // Меньше для теста
        vector<map<string, string>> problems_data;

        for (int i = 0; i < count_problems; i++) {
            try {
                auto problem = generator.generate(config);

                map<string, string> problem_data;
                problem_data["problem_text"] = problem.problem_text;
                problem_data["correct_answer"] = problem.correct_answer;
                problem_data["solution_explanation"] = problem.solution_explanation;

                problems_data.push_back(problem_data);

                if ((i + 1) % 10 == 0) {
                    cout << "   " << (i + 1) << " задач сгенерировано" << endl;
                }

            }
            catch (const exception& e) {
                cerr << "   Ошибка задачи #" << (i + 1) << ": " << e.what() << endl;
            }
        }

        if (problems_data.empty()) {
            cerr << "Ошибка: Не удалось сгенерировать задачи" << endl;
            return 1;
        }

        cout << "   Всего сгенерировано: " << problems_data.size() << " задач" << endl;

        // 6. СОЗДАНИЕ HTML
        cout << "5. Создание HTML файла..." << endl;

        PageConfig page_config;
        page_config.title = "Задачи на кодирование - ОГЭ по информатике";
        page_config.output_file = "site/задачи_оге.html";
        page_config.total_problems = static_cast<int>(problems_data.size());

        HtmlPageGenerator html_gen(page_config);

        if (!html_gen.generate_and_save_ansi(problems_data)) {
            // Попробуем другой путь
            page_config.output_file = "site/zadachi.html";
            HtmlPageGenerator html_gen2(page_config);

            if (!html_gen2.generate_and_save_ansi(problems_data)) {
                cerr << "Ошибка: Не удалось создать HTML файл" << endl;
                return 1;
            }
        }

        cout << "   HTML файл создан: " << page_config.output_file << endl;

        // 7. ЗАВЕРШЕНИЕ
        cout << "\n==================================" << endl;
        cout << "ГЕНЕРАЦИЯ ЗАВЕРШЕНА!" << endl;
        cout << "==================================" << endl;
        cout << "\nФайл: " << page_config.output_file << endl;
        cout << "Задач: " << problems_data.size() << endl;
        cout << "\nОткройте файл в браузере для просмотра." << endl;

    }
    catch (const exception& e) {
        cerr << "\nОШИБКА: " << e.what() << endl;
        cerr << "\nНажмите Enter для выхода...";
        cin.get();
        return 1;
    }

    cout << "\nНажмите Enter для выхода...";
    cin.get();

    return 0;
}