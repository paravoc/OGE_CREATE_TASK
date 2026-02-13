// main.cpp
#include "html_generator.h"
#include "num_system_problem.h"
#include "filesystem_problem.h"
#include <chrono>
#include <iostream>
#include <memory>

using namespace OGE;
using namespace std;
using namespace chrono;

int main() {
    auto start = high_resolution_clock::now();
    setlocale(LC_ALL, "Russian");

    // НАСТРОЙКИ
    PageSettings settings;
    settings.solution_access = AccessLevel::FULL_ACCESS;
    settings.show_hints = true;
    settings.show_progress_bar = true;
    settings.show_score = true;

    // СОЗДАЕМ ГЕНЕРАТОР
    HtmlGenerator generator(settings);

    // ===== ЗАДАЧИ =====
    // Системы счисления
    vector<pair<string, Difficulty>> ns_tasks = {
        {"conversion", Difficulty::EASY},
        {"conversion", Difficulty::MEDIUM},
        {"conversion", Difficulty::HARD},
        {"comparison", Difficulty::EASY},
        {"comparison", Difficulty::MEDIUM},
        {"comparison", Difficulty::HARD},
        {"equation", Difficulty::EASY},
        {"equation", Difficulty::MEDIUM},
        {"equation", Difficulty::HARD}
    };

    int num = 1;
    for (auto& task : ns_tasks) {
        ProblemMeta meta;
        meta.type_id = "numsys";
        meta.display_name = "Системы счисления";
        meta.problem_number = num++;
        meta.default_score = 15;
        meta.is_active = true;

        auto problem = make_unique<NumSystemProblem>(meta);
        problem->set_page_settings(settings);

        GenerationConfig config;
        config.difficulty = task.second;
        config.custom_params["task_type"] = task.first;
        problem->generate(config);

        generator.add_problem(move(problem));
    }

    // Файловая система
    vector<pair<string, Difficulty>> fs_tasks = {
        {"url", Difficulty::EASY},
        {"url", Difficulty::MEDIUM},
        {"url", Difficulty::HARD},
        {"filepath", Difficulty::EASY},
        {"filepath", Difficulty::MEDIUM},
        {"filepath", Difficulty::HARD},
        {"cd", Difficulty::EASY},
        {"cd", Difficulty::MEDIUM},
        {"cd", Difficulty::HARD}
    };

    for (auto& task : fs_tasks) {
        ProblemMeta meta;
        meta.type_id = "filesystem";
        meta.display_name = "Файловая система";
        meta.problem_number = num++;
        meta.default_score = 15;
        meta.is_active = true;

        auto problem = make_unique<FilesystemProblem>(meta);
        problem->set_page_settings(settings);

        GenerationConfig config;
        config.difficulty = task.second;
        config.custom_params["task_type"] = task.first;
        problem->generate(config);

        generator.add_problem(move(problem));
    }

    // ГЕНЕРАЦИЯ И СОХРАНЕНИЕ
    string filename = "web/variant_oge.html";
    generator.generate_full_page();

    if (generator.save_to_file(filename)) {
        cout << "OK: " << filename << endl;
    }

    auto end = high_resolution_clock::now();
    cout << "Time: " << duration_cast<milliseconds>(end - start).count() << "ms" << endl;
    cout << "Tasks: " << generator.get_problems_count() << endl;

    return 0;
}