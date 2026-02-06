#include "task_generator.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace std;
using namespace OGE;

// Реализация GeneratedTask
GeneratedTask::GeneratedTask(int task_id, const string& scenario)
    : id(task_id), scenario_name(scenario) {
}

// Реализация TaskGenerator
TaskGenerator::TaskGenerator()
    : db_problem1(nullptr), generator(nullptr) {
    
}

TaskGenerator::TaskGenerator(shared_ptr<DatabaseProblem1> db)
    : db_problem1(db) {
    if (db_problem1) {
        generator = make_shared<ProblemType1>(*db_problem1);
    }
   
}

bool TaskGenerator::initialize() {
    if (!db_problem1) {
        return false;
    }

    // Проверяем генератор
    if (!generator) {
        generator = make_shared<ProblemType1>(*db_problem1);
    }

    // Проверяем загрузку кэша
    if (!generator->is_cache_loaded()) {
        generator->reload_cache();
        if (!generator->is_cache_loaded()) {
            cerr << "Ошибка: Не удалось загрузить кэш данных" << endl;
            return false;
        }
    }

    return true;
}

void TaskGenerator::reset_stats() {
    stats.total_generated = 0;
    stats.successful_generations = 0;
    stats.failed_generations = 0;
    stats.by_scenario.clear();
    stats.by_type.clear();
}

bool TaskGenerator::is_initialized() const {
    return generator && generator->is_cache_loaded();
}

vector<GeneratedTask> TaskGenerator::generate_scenario(const string& scenario_name) {
    vector<GeneratedTask> tasks;

    if (!is_initialized()) {
        cerr << "Ошибка: Генератор не инициализирован" << endl;
        return tasks;
    }

    if (!scenario_manager.has_scenario(scenario_name)) {
        cerr << "Ошибка: Сценарий '" << scenario_name << "' не найден" << endl;
        return tasks;
    }

    const auto& scenario = scenario_manager.get_all_scenarios().at(scenario_name);
    if (!scenario.enabled) {
        cout << "Сценарий '" << scenario_name << "' отключен, пропускаю" << endl;
        return tasks;
    }

    cout << "Генерация сценария '" << scenario_name << "' ("
        << scenario.problem_count << " задач)..." << endl;

    int success_count = 0;
    int fail_count = 0;

    for (int i = 0; i < scenario.problem_count; i++) {
        try {
            auto problem = generator->generate(scenario.config);

            GeneratedTask task(i + 1, scenario_name);
            task.problem_text = problem.problem_text;
            task.correct_answer = problem.correct_answer;
            task.solution_explanation = problem.solution_explanation;
            task.meta = problem.meta;

            tasks.push_back(task);
            success_count++;

            // Прогресс
            if ((i + 1) % 20 == 0) {
                cout << "  " << (i + 1) << "/" << scenario.problem_count
                    << " задач сгенерировано" << endl;
            }

        }
        catch (const exception& e) {
            cerr << "  Ошибка генерации задачи #" << (i + 1)
                << ": " << e.what() << endl;
            fail_count++;
        }
    }

    // Сохраняем в кэш
    generated_tasks[scenario_name] = tasks;

    // Обновляем статистику
    stats.total_generated += tasks.size();
    stats.successful_generations += success_count;
    stats.failed_generations += fail_count;
    stats.by_scenario[scenario_name] = tasks.size();

    // Считаем типы задач
    for (const auto& task : tasks) {
        if (task.meta.count("scenario")) {
            string type = task.meta.at("scenario");
            stats.by_type[type]++;
        }
    }

    cout << "  ✓ Сгенерировано: " << success_count << " задач, "
        << "ошибок: " << fail_count << endl;

    return tasks;
}

vector<GeneratedTask> TaskGenerator::generate_all_scenarios() {
    vector<GeneratedTask> all_tasks;

    if (!is_initialized()) {
        return all_tasks;
    }

    auto scenario_names = scenario_manager.get_scenario_names();

    cout << "Генерация всех сценариев (" << scenario_names.size()
        << " сценариев)..." << endl;

    int total_scenarios = 0;
    int total_problems = 0;

    for (const auto& name : scenario_names) {
        auto tasks = generate_scenario(name);
        all_tasks.insert(all_tasks.end(), tasks.begin(), tasks.end());

        if (!tasks.empty()) {
            total_scenarios++;
            total_problems += tasks.size();
        }
    }

    cout << "✓ Всего сгенерировано: " << total_problems
        << " задач в " << total_scenarios << " сценариях" << endl;

    return all_tasks;
}

const vector<GeneratedTask>& TaskGenerator::get_tasks(const string& scenario_name) const {
    static const vector<GeneratedTask> empty;
    auto it = generated_tasks.find(scenario_name);
    if (it != generated_tasks.end()) {
        return it->second;
    }
    return empty;
}

vector<GeneratedTask> TaskGenerator::get_all_tasks() const {
    vector<GeneratedTask> all_tasks;
    for (const auto& [name, tasks] : generated_tasks) {
        all_tasks.insert(all_tasks.end(), tasks.begin(), tasks.end());
    }
    return all_tasks;
}

vector<map<string, string>> TaskGenerator::export_for_html(const string& scenario_name) const {
    vector<map<string, string>> result;

    const auto& tasks = get_tasks(scenario_name);
    for (const auto& task : tasks) {
        map<string, string> task_data;
        task_data["problem_text"] = task.problem_text;
        task_data["correct_answer"] = task.correct_answer;
        task_data["solution_explanation"] = task.solution_explanation;
        task_data["meta_problem_number"] = to_string(task.id);
        task_data["meta_scenario"] = task.scenario_name;

        // Добавляем все метаданные
        for (const auto& [key, value] : task.meta) {
            task_data["meta_" + key] = value;
        }

        result.push_back(task_data);
    }

    return result;
}

vector<map<string, string>> TaskGenerator::export_all_for_html() const {
    vector<map<string, string>> result;

    for (const auto& [name, tasks] : generated_tasks) {
        auto scenario_tasks = export_for_html(name);
        result.insert(result.end(), scenario_tasks.begin(), scenario_tasks.end());
    }

    return result;
}

void TaskGenerator::print_stats(ostream& out) const {
    out << "\n📊 Статистика генерации:" << endl;
    out << "  Всего сгенерировано задач: " << stats.total_generated << endl;
    out << "  Успешных генераций: " << stats.successful_generations << endl;
    out << "  Неудачных генераций: " << stats.failed_generations << endl;

    if (!stats.by_scenario.empty()) {
        out << "\n  По сценариям:" << endl;
        for (const auto& [scenario, count] : stats.by_scenario) {
            out << "    - " << scenario << ": " << count << " задач" << endl;
        }
    }

    if (!stats.by_type.empty()) {
        out << "\n  По типам задач:" << endl;
        for (const auto& [type, count] : stats.by_type) {
            out << "    - " << type << ": " << count << endl;
        }
    }

    // Эффективность
    if (stats.total_generated > 0) {
        double efficiency = (double)stats.successful_generations /
            (stats.successful_generations + stats.failed_generations) * 100.0;
        out << "\n  Эффективность: " << fixed << setprecision(1)
            << efficiency << "%" << endl;
    }
}



bool TaskGenerator::save_tasks(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) return false;

    for (const auto& [scenario_name, tasks] : generated_tasks) {
        file << "[" << scenario_name << "]\n";
        file << "count=" << tasks.size() << "\n";

        for (const auto& task : tasks) {
            file << "task=" << task.id << "\n";
            file << "problem_text=" << task.problem_text << "\n";
            file << "correct_answer=" << task.correct_answer << "\n";
            file << "solution=" << task.solution_explanation << "\n";

            // Метаданные
            for (const auto& [key, value] : task.meta) {
                file << "meta_" << key << "=" << value << "\n";
            }

            file << "---\n";
        }
        file << "\n";
    }

    file.close();
    return true;
}

bool TaskGenerator::load_tasks(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    generated_tasks.clear();

    string line;
    string current_scenario;
    vector<GeneratedTask> current_tasks;
    GeneratedTask current_task;
    bool in_task = false;

    while (getline(file, line)) {
        // Пропускаем пустые строки
        if (line.empty()) continue;

        // Новая секция
        if (line[0] == '[' && line.back() == ']') {
            // Сохраняем предыдущую секцию
            if (!current_scenario.empty() && !current_tasks.empty()) {
                generated_tasks[current_scenario] = current_tasks;
            }

            current_scenario = line.substr(1, line.length() - 2);
            current_tasks.clear();
            continue;
        }

        // Разделитель задач
        if (line == "---") {
            if (in_task && current_task.id > 0) {
                current_tasks.push_back(current_task);
            }
            current_task = GeneratedTask();
            in_task = false;
            continue;
        }

        // Параметры
        size_t eq_pos = line.find('=');
        if (eq_pos != string::npos) {
            string key = line.substr(0, eq_pos);
            string value = line.substr(eq_pos + 1);

            in_task = true;

            if (key == "task") current_task.id = stoi(value);
            else if (key == "problem_text") current_task.problem_text = value;
            else if (key == "correct_answer") current_task.correct_answer = value;
            else if (key == "solution") current_task.solution_explanation = value;
            else if (key == "scenario") current_task.scenario_name = value;
            else if (key.find("meta_") == 0) {
                string meta_key = key.substr(5);
                current_task.meta[meta_key] = value;
            }
        }
    }

    // Сохраняем последнюю секцию
    if (!current_scenario.empty() && !current_tasks.empty()) {
        generated_tasks[current_scenario] = current_tasks;
    }

    file.close();
    return true;
}

void TaskGenerator::clear_cache() {
    generated_tasks.clear();
    reset_stats();
}

void TaskGenerator::clear_scenario_cache(const string& scenario_name) {
    generated_tasks.erase(scenario_name);
    stats.by_scenario.erase(scenario_name);
}