#include "scenario_config.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
using namespace OGE;

// Реализация GenerationScenario
GenerationScenario::GenerationScenario(const string& n,
    const string& desc,
    int count,
    const string& file,
    bool en)
    : name(n), description(desc), problem_count(count),
    output_file(file), enabled(en) {
}

GenerationScenario GenerationScenario::create_default() {
    GenerationScenario scenario("default", "Стандартный сценарий", 200);
    scenario.config.allow_removal = true;
    scenario.config.allow_addition = true;
    scenario.config.allow_encoding_change = true;
    scenario.config.word_count_min = 7;
    scenario.config.word_count_max = 10;
    scenario.config.difficulty = 2;
    scenario.output_file = "site/задачи.html";
    return scenario;
}

GenerationScenario GenerationScenario::create_easy() {
    GenerationScenario scenario("easy", "Простые задачи", 50);
    scenario.config.allow_removal = true;
    scenario.config.allow_addition = false;
    scenario.config.allow_encoding_change = false;
    scenario.config.word_count_min = 3;
    scenario.config.word_count_max = 5;
    scenario.config.difficulty = 1;
    scenario.output_file = "site/простые_задачи.html";
    return scenario;
}

GenerationScenario GenerationScenario::create_medium() {
    GenerationScenario scenario("medium", "Средней сложности", 100);
    scenario.config.allow_removal = true;
    scenario.config.allow_addition = true;
    scenario.config.allow_encoding_change = false;
    scenario.config.word_count_min = 5;
    scenario.config.word_count_max = 8;
    scenario.config.difficulty = 2;
    scenario.output_file = "site/средние_задачи.html";
    return scenario;
}

GenerationScenario GenerationScenario::create_hard() {
    GenerationScenario scenario("hard", "Сложные задачи", 50);
    scenario.config.allow_removal = false;
    scenario.config.allow_addition = false;
    scenario.config.allow_encoding_change = true;
    scenario.config.word_count_min = 4;
    scenario.config.word_count_max = 6;
    scenario.config.difficulty = 3;
    scenario.output_file = "site/сложные_задачи.html";
    return scenario;
}

GenerationScenario GenerationScenario::create_mixed() {
    GenerationScenario scenario("mixed", "Смешанный набор", 150);
    scenario.config.allow_removal = true;
    scenario.config.allow_addition = true;
    scenario.config.allow_encoding_change = true;
    scenario.config.word_count_min = 4;
    scenario.config.word_count_max = 7;
    scenario.config.difficulty = 2;
    scenario.output_file = "site/смешанные_задачи.html";
    return scenario;
}

GenerationScenario GenerationScenario::create_exam_simulation() {
    GenerationScenario scenario("exam", "Имитация экзамена", 15);
    scenario.config.allow_removal = true;
    scenario.config.allow_addition = true;
    scenario.config.allow_encoding_change = true;
    scenario.config.word_count_min = 5;
    scenario.config.word_count_max = 9;
    scenario.config.difficulty = 3;
    scenario.output_file = "site/экзамен.html";
    return scenario;
}

// Реализация ScenarioManager
ScenarioManager::ScenarioManager() : active_scenario("default") {
    create_default_scenarios();
}

void ScenarioManager::add_scenario(const GenerationScenario& scenario) {
    if (!scenario.name.empty()) {
        scenarios[scenario.name] = scenario;
    }
}

void ScenarioManager::remove_scenario(const string& name) {
    scenarios.erase(name);
    if (active_scenario == name && !scenarios.empty()) {
        active_scenario = scenarios.begin()->first;
    }
}

bool ScenarioManager::has_scenario(const string& name) const {
    return scenarios.find(name) != scenarios.end();
}

void ScenarioManager::set_active_scenario(const string& name) {
    if (has_scenario(name)) {
        active_scenario = name;
    }
}

GenerationScenario& ScenarioManager::get_active_scenario() {
    return scenarios[active_scenario];
}

const GenerationScenario& ScenarioManager::get_active_scenario() const {
    static GenerationScenario empty_scenario;
    auto it = scenarios.find(active_scenario);
    if (it != scenarios.end()) {
        return it->second;
    }
    return empty_scenario;
}

vector<string> ScenarioManager::get_scenario_names() const {
    vector<string> names;
    for (const auto& [name, scenario] : scenarios) {
        if (scenario.enabled) {
            names.push_back(name);
        }
    }
    return names;
}

const map<string, GenerationScenario>& ScenarioManager::get_all_scenarios() const {
    return scenarios;
}

int ScenarioManager::get_total_problem_count() const {
    int total = 0;
    for (const auto& [name, scenario] : scenarios) {
        if (scenario.enabled) {
            total += scenario.problem_count;
        }
    }
    return total;
}

map<string, int> ScenarioManager::get_scenario_stats() const {
    map<string, int> stats;
    for (const auto& [name, scenario] : scenarios) {
        if (scenario.enabled) {
            stats[name] = scenario.problem_count;
        }
    }
    return stats;
}

bool ScenarioManager::save_to_file(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) return false;

    for (const auto& [name, scenario] : scenarios) {
        file << "[" << name << "]\n";
        file << "description=" << scenario.description << "\n";
        file << "problem_count=" << scenario.problem_count << "\n";
        file << "output_file=" << scenario.output_file << "\n";
        file << "enabled=" << (scenario.enabled ? "1" : "0") << "\n";
        file << "allow_removal=" << (scenario.config.allow_removal ? "1" : "0") << "\n";
        file << "allow_addition=" << (scenario.config.allow_addition ? "1" : "0") << "\n";
        file << "allow_encoding_change=" << (scenario.config.allow_encoding_change ? "1" : "0") << "\n";
        file << "word_count_min=" << scenario.config.word_count_min << "\n";
        file << "word_count_max=" << scenario.config.word_count_max << "\n";
        file << "difficulty=" << scenario.config.difficulty << "\n";
        file << "\n";
    }

    file.close();
    return true;
}

bool ScenarioManager::load_from_file(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    scenarios.clear();
    string line;
    GenerationScenario current;
    string current_section;

    while (getline(file, line)) {
        // Пропускаем комментарии и пустые строки
        if (line.empty() || line[0] == '#') continue;

        // Новая секция
        if (line[0] == '[' && line.back() == ']') {
            // Сохраняем предыдущую секцию
            if (!current_section.empty() && !current.name.empty()) {
                scenarios[current_section] = current;
            }

            // Начинаем новую
            current_section = line.substr(1, line.length() - 2);
            current = GenerationScenario();
            current.name = current_section;
            continue;
        }

        // Параметр
        size_t eq_pos = line.find('=');
        if (eq_pos != string::npos) {
            string key = line.substr(0, eq_pos);
            string value = line.substr(eq_pos + 1);

            if (key == "description") current.description = value;
            else if (key == "problem_count") current.problem_count = stoi(value);
            else if (key == "output_file") current.output_file = value;
            else if (key == "enabled") current.enabled = (value == "1");
            else if (key == "allow_removal") current.config.allow_removal = (value == "1");
            else if (key == "allow_addition") current.config.allow_addition = (value == "1");
            else if (key == "allow_encoding_change") current.config.allow_encoding_change = (value == "1");
            else if (key == "word_count_min") current.config.word_count_min = stoi(value);
            else if (key == "word_count_max") current.config.word_count_max = stoi(value);
            else if (key == "difficulty") current.config.difficulty = stoi(value);
        }
    }

    // Сохраняем последнюю секцию
    if (!current_section.empty() && !current.name.empty()) {
        scenarios[current_section] = current;
    }

    file.close();
    return true;
}

void ScenarioManager::create_default_scenarios() {
    add_scenario(GenerationScenario::create_default());
    add_scenario(GenerationScenario::create_easy());
    add_scenario(GenerationScenario::create_medium());
    add_scenario(GenerationScenario::create_hard());
    add_scenario(GenerationScenario::create_mixed());
    add_scenario(GenerationScenario::create_exam_simulation());
}