// decimal_problem.h
#pragma once

#include "problem_base.h"
#include "database_manager.h"
#include "page_setting.h"
#include <random>
#include <sstream>
#include <iomanip>

namespace OGE {

    class DecimalProblem : public ProblemBase, public DatabaseManager {
    private:
        PageSettings page_settings;
        double operand1;
        double operand2;
        char operation;
        int decimal_places;

    public:
        // Конструкторы
        DecimalProblem(const ProblemMeta& meta);
        explicit DecimalProblem(const ProblemMeta& meta, const std::string& db_path);
        ~DecimalProblem() override;

        // Реализация ProblemBase
        void generate(const GenerationConfig& config) override;

        // Реализация DatabaseManager
        bool create_stats_table() override;
        std::string get_table_name() override;
        void record_generation(int problem_num, const std::string& task_type) override;
        void record_solution(int problem_num, bool correct, double time_sec) override;
        std::unordered_map<std::string, int> get_generation_stats() const override;
        int get_total_generated() const override;

        // Методы для работы с PageSettings
        void set_page_settings(const PageSettings& settings);
        const PageSettings& get_page_settings() const;
        std::string generate_html() const;

        // Специфичные методы
        double get_operand1() const { return operand1; }
        double get_operand2() const { return operand2; }
        char get_operation() const { return operation; }
        int get_decimal_places() const { return decimal_places; }
        bool check_answer(const std::string& user_answer) const;
    };

}