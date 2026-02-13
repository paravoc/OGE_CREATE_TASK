// num_system_problem.h
#pragma once

#include "problem_base.h"
#include "page_setting.h"
#include <random>
#include <sstream>
#include <string>

namespace OGE {

    enum class ConversionDirection {
        TO_DECIMAL,     // Из любой СС в десятичную
        FROM_DECIMAL    // Из десятичной в любую СС
    };

    struct NumSystemConfig {
        ConversionDirection direction = ConversionDirection::TO_DECIMAL;
        int min_base = 2;
        int max_base = 16;
        int min_value = 10;
        int max_value = 10000;
    };

    class NumSystemProblem : public ProblemBase {
    private:
        static PageSettings page_settings;
        static std::mt19937& get_random_generator();

        // Основные параметры задачи
        int source_base;          // Исходная система счисления
        int target_base;          // Целевая система счисления
        int decimal_value;        // Значение в десятичной системе
        std::string source_number; // Число в исходной СС
        std::string target_number; // Число в целевой СС
        ConversionDirection direction;
        bool is_valid_problem;    // Флаг корректности генерации

        // Вспомогательные методы
        std::string to_base(int number, int base) const;
        int from_base(const std::string& number, int base) const;
        std::string get_base_symbol(int digit) const;
        int get_digit_value(char symbol) const;
        bool validate_number(const std::string& number, int base) const;
        std::string generate_random_number(int base, int min_val, int max_val);
        std::string generate_digit_sequence(int length, int base);

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

        // Конфигурация по умолчанию
        static NumSystemConfig default_config;
    };

}