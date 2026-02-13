// main.cpp
#include "html_generator.h"
#include "num_system_problem.h"
#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <memory>

using namespace OGE;
using namespace std;
using namespace chrono;

int main() {
    auto start = chrono::high_resolution_clock::now();
    setlocale(LC_ALL, "Russian");

    // =========================================
    // 1. НАСТРОЙКИ СТРАНИЦЫ - ТОЛЬКО СИСТЕМЫ СЧИСЛЕНИЯ
    // =========================================
    PageSettings settings;

    // Только задачи на системы счисления
    settings.problem_numbers["numsys"] = 10;     // 10 задач на системы счисления

    // Минимальные настройки интерфейса
    settings.solution_access = AccessLevel::FULL_ACCESS;
    settings.show_hints = true;
    settings.show_progress_bar = true;
    settings.dark_mode = false;
    settings.show_score = true;

    // =========================================
    // 2. НАСТРОЙКИ СЛОЖНОСТИ
    // =========================================
    GenerationConfig base_config;
    base_config.difficulty = Difficulty::MEDIUM;

    // =========================================
    // 3. СОЗДАЕМ ГЕНЕРАТОР HTML
    // =========================================
    HtmlGenerator generator(settings);

    cout << "==================================" << endl;
    cout << "   СИСТЕМЫ СЧИСЛЕНИЯ - ГЕНЕРАЦИЯ" << endl;
    cout << "==================================" << endl;
    cout << "Начинаем генерацию..." << endl << endl;

    // =========================================
    // 4. ЗАДАЧИ НА СИСТЕМЫ СЧИСЛЕНИЯ
    // =========================================
    cout << "💻 Задачи на системы счисления:" << endl;

    // Типы задач для систем счисления
    vector<string> task_types = {
        "conversion",    // Простой перевод
        "comparison",    // Сравнение чисел
        "equation",      // Уравнение
        "arithmetic",    // Арифметика в СС
        "monotonic"      // Монотонный ряд
    };

    vector<string> task_names = {
        "Перевод чисел",
        "Сравнение чисел",
        "Уравнение",
        "Арифметика",
        "Монотонный ряд"
    };

    // Генерируем по 2 задачи каждого типа
    int problem_counter = 0;
    for (int type_idx = 0; type_idx < task_types.size(); type_idx++) {
        for (int variant = 0; variant < 2; variant++) {
            ProblemMeta meta;
            meta.type_id = "numsys";
            meta.display_name = task_names[type_idx];
            meta.problem_number = ++problem_counter;
            meta.default_score = 15;
            meta.is_active = true;

            auto problem = make_unique<NumSystemProblem>(meta);
            problem->set_page_settings(settings);

            GenerationConfig ns_config = base_config;
            ns_config.custom_params["task_type"] = task_types[type_idx];

            // Минимальные настройки для каждого типа
            if (task_types[type_idx] == "conversion") {
                if (variant == 0) {
                    ns_config.custom_params["conversion_direction"] = "to_decimal";
                }
                else {
                    ns_config.custom_params["conversion_direction"] = "from_decimal";
                }
            }
            else if (task_types[type_idx] == "comparison") {
                if (variant == 0) {
                    ns_config.custom_params["comparison_criteria"] = "maximum";
                }
                else {
                    ns_config.custom_params["comparison_criteria"] = "minimum";
                }
            }
            else if (task_types[type_idx] == "arithmetic") {
                if (variant == 0) {
                    ns_config.custom_params["arithmetic_operation"] = "addition";
                }
                else {
                    ns_config.custom_params["arithmetic_operation"] = "subtraction";
                }
            }

            problem->generate(ns_config);
            generator.add_problem(move(problem));

            cout << "  + Задача #" << problem_counter << ": "
                << task_names[type_idx] << endl;
        }
    }
    cout << endl;

    // =========================================
    // 5. ГЕНЕРАЦИЯ HTML СТРАНИЦЫ
    // =========================================
    cout << "📄 Создание HTML страницы..." << endl;
    string html = generator.generate_full_page();
    cout << "   ✓ HTML сгенерирован" << endl;

    // =========================================
    // 6. СОХРАНЕНИЕ В ФАЙЛ
    // =========================================
    string filename = "web/variant_oge.html";
    if (generator.save_to_file(filename)) {
        cout << "   ✓ Файл сохранен: " << filename << endl;
    }
    else {
        cout << "   ❌ Ошибка сохранения файла!" << endl;
    }
    cout << endl;

    // =========================================
    // 7. ВРЕМЯ ВЫПОЛНЕНИЯ
    // =========================================
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    cout << "==================================" << endl;
    cout << "   ГОТОВО!" << endl;
    cout << "==================================" << endl;
    cout << "⏱️  Время: " << duration.count() << " мс" << endl;
    cout << "✅ Всего задач: " << generator.get_problems_count() << endl;
    cout << "==================================" << endl;

    return 0;
}