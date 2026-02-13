// main.cpp
#include "html_generator.h"
#include "filesystem_problem.h"
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
    // 1. НАСТРОЙКИ СТРАНИЦЫ - ТОЛЬКО ФАЙЛОВАЯ СИСТЕМА
    // =========================================
    PageSettings settings;

    // Только задачи на файловую систему
    settings.problem_numbers["filesystem"] = 9;     // 9 задач на файловую систему

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
    cout << "   ФАЙЛОВАЯ СИСТЕМА - ГЕНЕРАЦИЯ" << endl;
    cout << "==================================" << endl;
    cout << "Начинаем генерацию..." << endl << endl;

    // =========================================
    // 4. ЗАДАЧИ НА ФАЙЛОВУЮ СИСТЕМУ
    // =========================================
    cout << "📁 Задачи на файловую систему:" << endl;

    // Типы задач для файловой системы
    vector<string> task_types = {
        "url",          // URL адреса
        "filepath",     // Пути к файлам
        "cd"            // Команды cd
    };

    vector<string> task_names = {
        "URL адреса",
        "Пути к файлам",
        "Команды cd"
    };

    vector<Difficulty> difficulties = {
        Difficulty::EASY,
        Difficulty::MEDIUM,
        Difficulty::HARD
    };

    // Генерируем по 3 задачи каждого типа (по одной на каждый уровень сложности)
    int problem_counter = 0;
    for (int type_idx = 0; type_idx < task_types.size(); type_idx++) {
        for (int diff_idx = 0; diff_idx < 3; diff_idx++) {
            ProblemMeta meta;
            meta.type_id = "filesystem";
            meta.display_name = task_names[type_idx];
            meta.problem_number = ++problem_counter;

            // Баллы в зависимости от сложности
            switch (difficulties[diff_idx]) {
            case Difficulty::EASY: meta.default_score = 10; break;
            case Difficulty::MEDIUM: meta.default_score = 15; break;
            case Difficulty::HARD: meta.default_score = 20; break;
            }

            meta.is_active = true;

            auto problem = make_unique<FilesystemProblem>(meta);
            problem->set_page_settings(settings);

            GenerationConfig fs_config = base_config;
            fs_config.difficulty = difficulties[diff_idx];
            fs_config.custom_params["task_type"] = task_types[type_idx];

            problem->generate(fs_config);
            generator.add_problem(move(problem));

            string difficulty_str;
            switch (difficulties[diff_idx]) {
            case Difficulty::EASY: difficulty_str = "легкая"; break;
            case Difficulty::MEDIUM: difficulty_str = "средняя"; break;
            case Difficulty::HARD: difficulty_str = "сложная"; break;
            }

            cout << "  + Задача #" << problem_counter << ": "
                << task_names[type_idx] << " (" << difficulty_str << ")" << endl;
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