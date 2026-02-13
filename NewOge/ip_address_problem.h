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
        RESTORE_IPV4,           // Восстановить IPv4 из частей (с точками)
        VALIDATE_IPV4,          // Найти правильный IPv4 среди вариантов
        SAME_NETWORK,           // Определить, какие IP в одной сети
        VALIDATE_IPV6,          // Проверить корректность IPv6
        SUBNET_MASK,            // Задачи на маску подсети
        IP_CLASS,               // Определить класс IP-адреса
        BROADCAST_ADDRESS       // Найти broadcast-адрес
    };

    class IPAddressProblem : public ProblemBase {
    private:
        static PageSettings page_settings;
        static std::mt19937& get_random_generator();

        IPTaskType task_type;

        // Для задачи восстановления IPv4
        std::string original_ip;
        std::vector<std::string> ip_parts_with_dots;
        std::vector<std::pair<char, std::string>> shuffled_parts;
        std::string correct_sequence;

        // Для задачи валидации IPv4
        std::vector<std::string> ip_variants;
        int correct_variant_index;

        // Для задачи "в одной ли сети"
        std::vector<std::string> network_ips;
        std::string network_address;
        std::string subnet_mask;
        std::vector<int> same_network_indices;
        std::string network_answer;

        // Для IPv6
        std::vector<std::string> ipv6_variants;
        int correct_ipv6_index;
        std::vector<std::string> ipv6_errors;

        // Для маски подсети
        std::string mask_problem_ip;
        std::string mask_problem_mask;
        std::string mask_problem_result;
        std::vector<std::string> mask_options;
        int mask_correct_index;

        // Для класса IP
        std::string class_ip;
        std::string class_answer;
        std::vector<std::string> class_options;

        // Для broadcast
        std::string broadcast_ip;
        std::string broadcast_mask;
        std::string broadcast_address;

        // Вспомогательные методы
        std::string generate_random_ipv4();
        std::string generate_random_ipv6();
        std::string int_to_ipv6(int parts[8]);
        bool is_valid_ipv4(const std::string& ip);
        bool is_valid_ipv6(const std::string& ip);
        std::vector<std::string> split_ipv4(const std::string& ip);
        std::string join_ipv4(const std::vector<std::string>& parts);
        int ipv4_to_int(const std::string& ip);
        std::string int_to_ipv4(int num);
        std::string apply_subnet_mask(const std::string& ip, const std::string& mask);
        std::string get_network_address(const std::string& ip, const std::string& mask);
        std::string get_broadcast_address(const std::string& ip, const std::string& mask);
        char get_ip_class(const std::string& ip);
        void apply_difficulty_settings(Difficulty difficulty);

        // Методы генерации задач
        void generate_restore_ipv4_task(const GenerationConfig& config);
        void generate_validate_ipv4_task(const GenerationConfig& config);
        void generate_same_network_task(const GenerationConfig& config);
        void generate_validate_ipv6_task(const GenerationConfig& config);
        void generate_subnet_mask_task(const GenerationConfig& config);
        void generate_ip_class_task(const GenerationConfig& config);
        void generate_broadcast_task(const GenerationConfig& config);

    public:
        explicit IPAddressProblem(const ProblemMeta& meta);

        void generate(const GenerationConfig& config) override;
        std::string generate_html() const override;
        bool check_answer(const std::string& user_answer) const;

        void set_page_settings(const PageSettings& settings);
        const PageSettings& get_page_settings() const;
    };

}