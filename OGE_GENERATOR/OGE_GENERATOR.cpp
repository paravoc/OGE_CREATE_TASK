// main.cpp - Генератор 5 задач ОГЭ
#include "database_manager.h"
#include "problem_type1.h"
#include "html_page_generator.h"

#include <iostream>
#include <vector>
#include <string>
#include <memory>

using namespace std;
using namespace OGE;

int main() {
    setlocale(LC_ALL, "ru");
    cout << "========================================\n";
    cout << "КОСМИЧЕСКИЙ ГЕНЕРАТОР ЗАДАЧ ОГЭ\n";
    cout << "Генерация 5 задач...\n";
    cout << "========================================\n\n";

    try {
        // 1. Инициализация базы данных
        cout << "[1/4] Инициализация базы данных... ";
        DatabaseManager db_manager;

        // Используем базу в памяти для простоты
        if (!db_manager.open_or_create(":memory:")) {
            cerr << "\nОшибка: Не удалось создать базу данных\n";
            return 1;
        }

        // Создаем схему и заполняем данными
        if (!db_manager.create_schema()) {
            cerr << "\nОшибка: Не удалось создать схему БД\n";
            return 1;
        }

        if (!db_manager.seed_default_data()) {
            cerr << "\nОшибка: Не удалось заполнить БД данными\n";
            return 1;
        }
        cout << "Готово ✓\n";

        // 2. Создание генератора задач
        cout << "[2/4] Создание генератора задач... ";
        ProblemType1 problem_generator(db_manager.get_connection());

        if (!problem_generator.is_cache_loaded()) {
            problem_generator.reload_cache();
        }
        cout << "Готово ✓\n";

        // 3. Генерация 5 задач
        cout << "[3/4] Генерация 5 задач... ";
        ProblemType1::Config config;
        config.allow_removal = true;
        config.allow_addition = false;
        config.allow_encoding_change = false;
        config.word_count_min = 3;
        config.word_count_max = 6;
        config.word_category = "";

        // Генерируем HTML для задач
        string problems_html = problem_generator.generate_html_problems(5, config);

        // Генерируем JSON данные для JavaScript
        vector<map<string, string>> problems_json =
            problem_generator.generate_problems_json(5, config);
        cout << "Готово ✓\n";

        // 4. Создание HTML страницы
        cout << "[4/4] Создание HTML страницы... ";

        PageConfig page_config;
        page_config.title = "Космический Генератор Задач ОГЭ";
        page_config.version = "v1.0.0";
        page_config.output_file = "oge_tasks.html";
        page_config.css_file = "web/cosmic.css";      // Ваш CSS файл
        page_config.js_file = "web/cosmic.js";        // Ваш JS файл
        page_config.total_problems = 5;
        page_config.generation_time = time(nullptr);

        HtmlPageGenerator html_generator(page_config);

        // Разбиваем HTML задач на отдельные блоки
        vector<string> problem_html_blocks;
        // Простая разбивка - каждая задача в отдельном div
        size_t pos = 0;
        string html = problems_html;

        // Ищем начало каждой задачи
        while ((pos = html.find("<article class=\"cosmic-problem\"", pos)) != string::npos) {
            // Находим конец этой статьи
            size_t end_pos = html.find("</article>", pos);
            if (end_pos == string::npos) break;

            end_pos += 10; // Длина </article>
            problem_html_blocks.push_back(html.substr(pos, end_pos - pos));
            pos = end_pos;
        }

        // Если не удалось разбить, используем всю строку
        if (problem_html_blocks.empty()) {
            problem_html_blocks.push_back(problems_html);
        }

        // Генерируем и сохраняем полную страницу
        if (html_generator.generate_and_save(problem_html_blocks, problems_json)) {
            cout << "Готово ✓\n\n";

            cout << "========================================\n";
            cout << "ВСЕ ЗАДАЧИ УСПЕШНО СОЗДАНЫ!\n\n";
            cout << "Файлы:\n";
            cout << "  1. " << page_config.output_file << " - HTML страница с задачами\n";
            cout << "  2. " << page_config.css_file << " - Стили (ваш файл)\n";
            cout << "  3. " << page_config.js_file << " - JavaScript (ваш файл)\n\n";

            cout << "Откройте файл " << page_config.output_file
                << " в браузере для просмотра задач.\n";
            cout << "========================================\n";

            // Предлагаем открыть в браузере
#ifdef _WIN32
            cout << "\nОткрыть в браузере? (y/n): ";
            char choice;
            cin >> choice;
            if (choice == 'y' || choice == 'Y') {
                string command = "start " + page_config.output_file;
                system(command.c_str());
            }
#elif __linux__
            cout << "\nОткрыть в браузере? (y/n): ";
            char choice;
            cin >> choice;
            if (choice == 'y' || choice == 'Y') {
                string command = "xdg-open " + page_config.output_file;
                system(command.c_str());
            }
#endif

            return 0;
        }
        else {
            cerr << "\nОшибка: Не удалось сохранить HTML файл\n";
            return 1;
        }
    }
    catch (const exception& e) {
        cerr << "\nКРИТИЧЕСКАЯ ОШИБКА: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cerr << "\nНеизвестная ошибка при выполнении программы\n";
        return 1;
    }
}