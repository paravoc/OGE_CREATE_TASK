// ip_address_problem.h
#pragma once

#include "problem_base.h"
#include "page_setting.h"
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace OGE {

    enum class IPTaskType {
        RESTORE,        // Восстановить IP из частей
        VALIDATE        // Найти правильный IP среди вариантов
    };

    class IPAddressProblem : public ProblemBase {
    private:
        static PageSettings page_settings;
        static std::mt19937& get_random_generator();

        IPTaskType task_type;

        // Для задачи восстановления
        std::string original_ip;
        std::vector<std::string> ip_parts;
        std::vector<std::pair<char, std::string>> shuffled_parts;
        std::string correct_sequence;

        // Для задачи валидации
        std::vector<std::string> ip_variants;
        int correct_variant_index;

        // Вспомогательные методы
        std::string generate_random_ip();
        bool is_valid_ip(const std::string& ip);
        std::vector<std::string> split_ip(const std::string& ip);
        std::string join_ip(const std::vector<std::string>& parts);

    public:
        explicit IPAddressProblem(const ProblemMeta& meta);

        void generate(const GenerationConfig& config) override;
        std::string generate_html() const override;
        bool check_answer(const std::string& user_answer) const;

        void set_page_settings(const PageSettings& settings);
        const PageSettings& get_page_settings() const;
    };

}