// main.cpp
#include "math_problem.h"
#include "decimal_problem.h"
#include "html_generator.h"
#include<chrono>
#include <iostream>
#include"num_system_problem.h"

using namespace OGE;
using namespace std;
using namespace chrono;


int main() {
    auto start = chrono::high_resolution_clock::now();
    setlocale(LC_ALL, "Russian");

    // 1. Настройки страницы
    PageSettings settings;

    // 2. Выбираем какие задачи и сколько
    settings.problem_numbers["math"] = 7;      // 3 математические задачи
    settings.problem_numbers["decimal"] = 2;   // 2 задачи с десятичными дробями
    /*settings.timer_mode = TimerMode::COUNT_DOWN;
    settings.timer_seconds = 100;*/
    settings.solution_access = AccessLevel::FULL_ACCESS;
    settings.show_hints = true;
    settings.show_progress_bar = true;
    settings.dark_mode = false;





    // 3. Настройки сложности
    GenerationConfig config;
    config.difficulty = Difficulty::MEDIUM;    // EASY, MEDIUM, HARD

    // 4. Создаем генератор HTML
    HtmlGenerator generator(settings);

    // 5. Создаем и генерируем задачи
    cout << "Генерация задач..." << endl;

    settings.problem_numbers["numsys"] = 3;    // 3 задачи на системы счисления

    // 2. После задач с десятичными дробями добавьте генерацию задач на СС:

    // Задачи на системы счисления
    for (int i = 0; i < settings.problem_numbers["numsys"]; i++) {
        ProblemMeta meta;
        meta.type_id = "numsys";
        meta.display_name = "Системы счисления";
        meta.problem_number = i + 1;
        meta.default_score = 15;
        meta.is_active = true;

        auto problem = make_unique<NumSystemProblem>(meta);
        problem->set_page_settings(settings);

        // Можно настроить тип конверсии через custom_params
        GenerationConfig config;
        config.difficulty = Difficulty::MEDIUM;

        // Чередуем типы задач
        if (i % 2 == 0) {
            config.custom_params["conversion_direction"] = "to_decimal";  // в десятичную
        }
        else {
            config.custom_params["conversion_direction"] = "from_decimal"; // из десятичной
        }

        problem->generate(config);
        generator.add_problem(move(problem));
    }


    // 6. Генерируем HTML страницу
    cout << "\nСоздание HTML страницы..." << endl;
    string html = generator.generate_full_page();

    // 7. Сохраняем в файл
    string filename = "web/variant_oge.html";
    generator.save_to_file(filename);

    cout << "Готово! Страница сохранена в файл: " << filename << endl;
    cout << "Всего задач: " << generator.get_problems_count() << endl;


    // ФИНИШ ТАЙМЕРА
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    auto duration_us = duration_cast<microseconds>(end - start);
    auto duration_sec = duration_cast<seconds>(end - start);

    cout << "⏱️  Время выполнения:" << endl;
    cout << "   • " << duration.count() << " мс" << endl;
    cout << "   • " << duration_us.count() << " мкс" << endl;
    cout << "   • " << duration_sec.count() << " сек" << endl;
    cout << "==================================" << endl;
    return 0;
}