// math_problem.h
#pragma once

#include "problem_base.h"
#include "database_manager.h"
#include "page_setting.h"
#include <random>
#include <sstream>

namespace OGE {

    class MathProblem : public ProblemBase {
    private:
        static PageSettings page_settings;
        static std::mt19937& get_random_generator();
        int operand1;
        int operand2;
        char operation;
        

    public:
        // Конструкторы
        MathProblem(const ProblemMeta& meta);
        explicit MathProblem(const ProblemMeta& meta, const std::string& db_path);
        ~MathProblem() override;

        // Реализация ProblemBase
        void generate(const GenerationConfig& config) override;
        std::string generate_html() const override;
        bool check_answer(const std::string& user_answer) const;


        // Методы для работы с PageSettings
        void set_page_settings(const PageSettings& settings);
        const PageSettings& get_page_settings() const;

        // Специфичные методы
        int get_operand1() const { return operand1; }
        int get_operand2() const { return operand2; }
        char get_operation() const { return operation; }
    };

}