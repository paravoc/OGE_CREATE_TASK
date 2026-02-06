#pragma once
#ifndef SCENARIO_CONFIG_H
#define SCENARIO_CONFIG_H

#include <string>
#include <vector>
#include <map>
#include "problem_type1.h"

namespace OGE {

    // Настройки одного сценария генерации
    struct GenerationScenario {
        std::string name;                    // Название сценария
        std::string description;             // Описание
        ProblemType1::Config config;         // Конфигурация задачи
        int problem_count;                   // Количество задач
        std::string output_file;             // Выходной файл
        bool enabled;                        // Включен ли сценарий

        GenerationScenario(const std::string& n = "",
            const std::string& desc = "",
            int count = 0,
            const std::string& file = "",
            bool en = true);

        // Статические фабричные методы для стандартных сценариев
        static GenerationScenario create_default();
        static GenerationScenario create_easy();
        static GenerationScenario create_medium();
        static GenerationScenario create_hard();
        static GenerationScenario create_mixed();
        static GenerationScenario create_exam_simulation();
    };

    // Менеджер конфигураций сценариев
    class ScenarioManager {
    private:
        std::map<std::string, GenerationScenario> scenarios;
        std::string active_scenario;

    public:
        ScenarioManager();

        // Управление сценариями
        void add_scenario(const GenerationScenario& scenario);
        void remove_scenario(const std::string& name);
        bool has_scenario(const std::string& name) const;

        // Выбор сценария
        void set_active_scenario(const std::string& name);
        GenerationScenario& get_active_scenario();
        const GenerationScenario& get_active_scenario() const;
        std::string get_active_scenario_name() const { return active_scenario; }

        // Получение списка сценариев
        std::vector<std::string> get_scenario_names() const;
        const std::map<std::string, GenerationScenario>& get_all_scenarios() const;

        // Статистика
        int get_total_problem_count() const;
        std::map<std::string, int> get_scenario_stats() const;

        // Сохранение/загрузка
        bool save_to_file(const std::string& filename) const;
        bool load_from_file(const std::string& filename);

        // Создание стандартных сценариев
        void create_default_scenarios();
    };

} // namespace OGE

#endif // SCENARIO_CONFIG_H