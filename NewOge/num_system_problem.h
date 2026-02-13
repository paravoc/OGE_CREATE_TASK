// num_system_problem.h
#pragma once

#include "problem_base.h"
#include "page_setting.h"
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace OGE {

    enum class ConversionDirection {
        TO_DECIMAL,     // Из любой СС в десятичную
        FROM_DECIMAL    // Из десятичной в любую СС
    };

    enum class NumSystemTaskType {
        CONVERSION,          // Простой перевод
        COMPARISON,          // Сравнение чисел в разных СС
        EQUATION,           // Уравнение X + число = число
        ARITHMETIC,         // Арифметика в одной СС
        MONOTONIC_SEQUENCE  // Поиск монотонного ряда
    };

    struct NumSystemConfig {
        
    };

    class NumSystemProblem : public ProblemBase {
    private:
        static PageSettings page_settings;
        static std::mt19937& get_random_generator();

        ConversionDirection direction = ConversionDirection::TO_DECIMAL;
        NumSystemTaskType task_type = NumSystemTaskType::CONVERSION;
        int min_base = 2;
        int max_base = 16;
        int min_value = 10;
        int max_value = 10000;

        // Основные параметры задачи
        int source_base;
        int target_base;
        int decimal_value;
        std::string source_number;
        std::string target_number;
        NumSystemConfig system_config;
        bool is_valid_problem;

        // Для задачи сравнения
        std::vector<std::pair<std::string, int>> numbers_for_comparison;
        std::string comparison_result;
        std::string comparison_criteria;

        // Для уравнения
        int equation_x;
        int equation_term1;
        int equation_term2;
        int equation_base;
        char equation_operation;

        // Для арифметики
        int arithmetic_op1;
        int arithmetic_op2;
        int arithmetic_result;
        int arithmetic_base;
        char arithmetic_operation;
        std::string arithmetic_op1_str;
        std::string arithmetic_op2_str;
        std::string arithmetic_result_str;

        // Для монотонного ряда
        std::vector<std::vector<std::pair<std::string, int>>> sequences;
        int monotonic_sequence_index;
        bool monotonic_increasing;

        // Вспомогательные методы
        std::string to_base(int number, int base) const;
        int from_base(const std::string& number, int base) const;
        std::string get_base_symbol(int digit) const;
        int get_digit_value(char symbol) const;
        bool validate_number(const std::string& number, int base) const;
        std::string generate_random_number(int base, int min_val, int max_val);
        std::string generate_digit_sequence(int length, int base);

        // Новые методы для генерации задач
        void generate_comparison_task(const GenerationConfig& config);
        void generate_equation_task(const GenerationConfig& config);
        void generate_arithmetic_task(const GenerationConfig& config);
        void generate_monotonic_task(const GenerationConfig& config);

    public:
        // Конструкторы
        explicit NumSystemProblem(const ProblemMeta& meta);
        ~NumSystemProblem() override = default;

        // Реализация виртуальных методов
        void generate(const GenerationConfig& config) override;
        std::string generate_html() const override;

        // Проверка ответа
        bool check_answer(const std::string& user_answer) const;

        // Управление настройками страницы
        void set_page_settings(const PageSettings& settings);
        const PageSettings& get_page_settings() const;

        // Геттеры
        int get_source_base() const { return source_base; }
        int get_target_base() const { return target_base; }
        std::string get_source_number() const { return source_number; }
        std::string get_target_number() const { return target_number; }
        bool is_valid() const { return is_valid_problem; }
        ConversionDirection get_direction() const { return direction; }
        NumSystemTaskType get_task_type() const { return task_type; }

        // Конфигурация по умолчанию
        static NumSystemConfig default_config;
    };

}