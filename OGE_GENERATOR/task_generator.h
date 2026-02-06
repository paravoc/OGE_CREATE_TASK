#pragma once
#ifndef TASK_GENERATOR_H
#define TASK_GENERATOR_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include "database_problem1.h"
#include "problem_type1.h"
#include "scenario_config.h"

namespace OGE {

    // Результат генерации одной задачи
    struct GeneratedTask {
        int id;
        std::string problem_text;
        std::string correct_answer;
        std::string solution_explanation;
        std::map<std::string, std::string> meta;
        std::string scenario_name;

        GeneratedTask(int task_id = 0,
            const std::string& scenario = "");
    };

    // Менеджер генерации задач
    class TaskGenerator {
    private:
        std::shared_ptr<DatabaseProblem1> db_problem1;
        std::shared_ptr<ProblemType1> generator;
        ScenarioManager scenario_manager;

        // Кэш сгенерированных задач
        std::map<std::string, std::vector<GeneratedTask>> generated_tasks;

        // Статистика
        struct GenerationStats {
            int total_generated;
            int successful_generations;
            int failed_generations;
            std::map<std::string, int> by_scenario;
            std::map<std::string, int> by_type;

            GenerationStats() : total_generated(0), successful_generations(0), failed_generations(0) {}
        } stats;

    public:
        TaskGenerator();
        explicit TaskGenerator(std::shared_ptr<DatabaseProblem1> db);

        // Инициализация
        bool initialize();
        bool is_initialized() const;

        // Генерация задач
        std::vector<GeneratedTask> generate_scenario(const std::string& scenario_name);
        std::vector<GeneratedTask> generate_all_scenarios();

        // Управление сценариями
        ScenarioManager& get_scenario_manager() { return scenario_manager; }
        const ScenarioManager& get_scenario_manager() const { return scenario_manager; }

        // Получение сгенерированных задач
        const std::vector<GeneratedTask>& get_tasks(const std::string& scenario_name) const;
        std::vector<GeneratedTask> get_all_tasks() const;

        // Экспорт
        std::vector<std::map<std::string, std::string>>
            export_for_html(const std::string& scenario_name) const;
        std::vector<std::map<std::string, std::string>>
            export_all_for_html() const;

        // Статистика
        const GenerationStats& get_stats() const { return stats; }
        void print_stats(std::ostream& out = std::cout) const;
        void reset_stats();

        // Сохранение/загрузка
        bool save_tasks(const std::string& filename) const;
        bool load_tasks(const std::string& filename);

        // Очистка
        void clear_cache();
        void clear_scenario_cache(const std::string& scenario_name);
    };

} // namespace OGE

#endif // TASK_GENERATOR_H