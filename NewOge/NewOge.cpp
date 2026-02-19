// main.cpp
#include "html_generator.h"
#include "ip_address_problem.h"
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

    // ===== IP-АДРЕСА: 7 ТИПОВ ПО 3 УРОВНЯ СЛОЖНОСТИ =====
    vector<pair<string, Difficulty>> ip_tasks = {
        // 1. Восстановление IPv4
        {"restore", Difficulty::EASY},
        {"restore", Difficulty::MEDIUM},
        {"restore", Difficulty::HARD},

        // 2. Валидация IPv4
        {"validate", Difficulty::EASY},
        {"validate", Difficulty::MEDIUM},
        {"validate", Difficulty::HARD},

        // 3. В одной ли сети
        {"same_network", Difficulty::EASY},
        {"same_network", Difficulty::MEDIUM},
        {"same_network", Difficulty::HARD},

        // 4. Валидация IPv6
        {"ipv6", Difficulty::EASY},
        {"ipv6", Difficulty::MEDIUM},
        {"ipv6", Difficulty::HARD},

        // 5. Маска подсети
        {"subnet_mask", Difficulty::EASY},
        {"subnet_mask", Difficulty::MEDIUM},
        {"subnet_mask", Difficulty::HARD},

        // 6. Класс IP-адреса
        {"ip_class", Difficulty::EASY},
        {"ip_class", Difficulty::MEDIUM},
        {"ip_class", Difficulty::HARD},

        // 7. Broadcast-адрес
        {"broadcast", Difficulty::EASY},
        {"broadcast", Difficulty::MEDIUM},
        {"broadcast", Difficulty::HARD},

        {"invert_bits", Difficulty::EASY},
        {"invert_bits", Difficulty::MEDIUM},
        {"invert_bits", Difficulty::HARD}
    };

    int num = 1;
    for (auto& task : ip_tasks) {
        ProblemMeta meta;
        meta.type_id = "ipaddress";
        meta.display_name = "IP-адреса";
        meta.problem_number = num++;
        meta.default_score = 15;
        meta.is_active = true;

        auto problem = make_unique<IPAddressProblem>(meta);
        problem->set_page_settings(settings);

        GenerationConfig config;
        config.difficulty = task.second;
        config.custom_params["task_type"] = task.first;
        problem->generate(config);

        generator.add_problem(move(problem));
    }

    // ГЕНЕРАЦИЯ И СОХРАНЕНИЕ
    string filename = "C:/Users/smidr/source/repos/NewOge/NewOge/web/oge_tasks.html";
    generator.generate_full_page();

    if (generator.save_to_file(filename)) {
        cout << "OK: " << filename << endl;
    }

    auto end = high_resolution_clock::now();
    cout << "Time: " << duration_cast<milliseconds>(end - start).count() << "ms" << endl;
    cout << "Tasks: " << generator.get_problems_count() << endl;

    return 0;
}