// ip_address_problem.cpp
#include "ip_address_problem.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <cmath>

namespace OGE {

    PageSettings IPAddressProblem::page_settings;

    std::mt19937& IPAddressProblem::get_random_generator() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }

    IPAddressProblem::IPAddressProblem(const ProblemMeta& meta)
        : ProblemBase(meta), task_type(IPTaskType::RESTORE_IPV4) {
    }

    void IPAddressProblem::apply_difficulty_settings(Difficulty difficulty) {
        switch (difficulty) {
        case Difficulty::EASY: score = 10; break;
        case Difficulty::MEDIUM: score = 15; break;
        case Difficulty::HARD: score = 20; break;
        }
    }

    std::string IPAddressProblem::generate_random_ipv4() {
        auto& gen = get_random_generator();

        std::uniform_int_distribution<> dis_octet(0, 255);

        std::stringstream ss;
        ss << dis_octet(gen) << "."
            << dis_octet(gen) << "."
            << dis_octet(gen) << "."
            << dis_octet(gen);

        return ss.str();
    }

    std::string IPAddressProblem::generate_random_ipv6() {
        auto& gen = get_random_generator();

        std::uniform_int_distribution<> dis_hex(0, 65535); // 16-bit

        std::stringstream ss;
        for (int i = 0; i < 8; i++) {
            if (i > 0) ss << ":";
            ss << std::hex << dis_hex(gen);
        }

        return ss.str();
    }

    bool IPAddressProblem::is_valid_ipv4(const std::string& ip) {
        std::regex ip_pattern(R"(^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$)");
        std::smatch matches;

        if (!std::regex_match(ip, matches, ip_pattern)) {
            return false;
        }

        for (int i = 1; i <= 4; i++) {
            int octet = std::stoi(matches[i].str());
            if (octet < 0 || octet > 255) {
                return false;
            }
        }

        return true;
    }

    bool IPAddressProblem::is_valid_ipv6(const std::string& ip) {
        // Упрощенная проверка IPv6
        std::regex ipv6_pattern(R"(^([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}$)");
        std::regex ipv6_short(R"(^([0-9a-fA-F]{1,4}:){1,7}:([0-9a-fA-F]{1,4}:){0,6}[0-9a-fA-F]{1,4}$)");

        return std::regex_match(ip, ipv6_pattern) || std::regex_match(ip, ipv6_short);
    }

    std::vector<std::string> IPAddressProblem::split_ipv4(const std::string& ip) {
        std::vector<std::string> parts;
        std::stringstream ss(ip);
        std::string part;

        while (std::getline(ss, part, '.')) {
            parts.push_back(part);
        }

        return parts;
    }

    std::string IPAddressProblem::join_ipv4(const std::vector<std::string>& parts) {
        std::stringstream ss;
        for (size_t i = 0; i < parts.size(); i++) {
            if (i > 0) ss << ".";
            ss << parts[i];
        }
        return ss.str();
    }

    int IPAddressProblem::ipv4_to_int(const std::string& ip) {
        auto parts = split_ipv4(ip);
        return (std::stoi(parts[0]) << 24) |
            (std::stoi(parts[1]) << 16) |
            (std::stoi(parts[2]) << 8) |
            std::stoi(parts[3]);
    }

    std::string IPAddressProblem::int_to_ipv4(int num) {
        std::stringstream ss;
        ss << ((num >> 24) & 0xFF) << "."
            << ((num >> 16) & 0xFF) << "."
            << ((num >> 8) & 0xFF) << "."
            << (num & 0xFF);
        return ss.str();
    }

    std::string IPAddressProblem::apply_subnet_mask(const std::string& ip, const std::string& mask) {
        int ip_int = ipv4_to_int(ip);
        int mask_int = ipv4_to_int(mask);
        return int_to_ipv4(ip_int & mask_int);
    }

    std::string IPAddressProblem::get_network_address(const std::string& ip, const std::string& mask) {
        return apply_subnet_mask(ip, mask);
    }

    std::string IPAddressProblem::get_broadcast_address(const std::string& ip, const std::string& mask) {
        int ip_int = ipv4_to_int(ip);
        int mask_int = ipv4_to_int(mask);
        int broadcast_int = ip_int | (~mask_int);
        return int_to_ipv4(broadcast_int);
    }

    char IPAddressProblem::get_ip_class(const std::string& ip) {
        int first_octet = std::stoi(split_ipv4(ip)[0]);

        if (first_octet >= 1 && first_octet <= 126) return 'A';
        if (first_octet >= 128 && first_octet <= 191) return 'B';
        if (first_octet >= 192 && first_octet <= 223) return 'C';
        if (first_octet >= 224 && first_octet <= 239) return 'D';
        if (first_octet >= 240 && first_octet <= 255) return 'E';
        return '?';
    }

    // ============== ЗАДАЧА 1: ВОССТАНОВЛЕНИЕ IP С ТОЧКАМИ ==============
    void IPAddressProblem::generate_restore_ipv4_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        // Генерируем 4 случайных числа от 0 до 255
        std::uniform_int_distribution<> dis_octet(0, 255);
        int o1 = dis_octet(gen);
        int o2 = dis_octet(gen);
        int o3 = dis_octet(gen);
        int o4 = dis_octet(gen);

        // Сохраняем оригинальный IP для решения
        std::stringstream ip_ss;
        ip_ss << o1 << "." << o2 << "." << o3 << "." << o4;
        original_ip = ip_ss.str();

        // Создаем 4 части в формате как на фото:
        // .33  | 3.232 | 3.20 | 23
        std::vector<std::string> parts = {
            "." + std::to_string(o1),                          // .175
            std::to_string(o2).substr(0, 1) + "." + std::to_string(o3), // 3.144 (берем первую цифру от o2)
            std::to_string(o3).substr(0, 1) + "." + std::to_string(o4).substr(0, 2), // 1.44 (первая цифра o3 + первые две o4)
            std::to_string(o4)                                   // 141
        };

        // Корректируем, чтобы все части были уникальными и в правильном формате
        // Часть 1: .XX (всегда 3 символа: точка + 2 цифры)
        if (o1 < 10) {
            parts[0] = ".0" + std::to_string(o1);
        }
        else if (o1 < 100) {
            parts[0] = "." + std::to_string(o1);
        }
        else {
            parts[0] = "." + std::to_string(o1); // .175
        }

        // Часть 2: X.YYY (цифра.три цифры)
        std::string o2_str = std::to_string(o2);
        std::string o3_str = std::to_string(o3);
        parts[1] = o2_str.substr(0, 1) + "." + o3_str;

        // Часть 3: X.YY (цифра.две цифры)
        std::string o3_first = o3_str.substr(0, 1);
        std::string o4_str = std::to_string(o4);
        std::string o4_first_two = o4_str.substr(0, std::min(2, (int)o4_str.length()));
        parts[2] = o3_first + "." + o4_first_two;

        // Часть 4: XXX (три цифры или меньше)
        parts[3] = o4_str;

        // Перемешиваем части
        std::shuffle(parts.begin(), parts.end(), gen);

        // Присваиваем буквы А, Б, В, Г
        shuffled_parts.clear();
        char letter = 'А';
        for (const auto& part : parts) {
            shuffled_parts.push_back({ letter++, part });
        }

        // Находим правильную последовательность букв
        std::vector<std::string> correct_order = {
            "." + std::to_string(o1),
            std::to_string(o2).substr(0, 1) + "." + std::to_string(o3),
            std::to_string(o3).substr(0, 1) + "." + std::to_string(o4).substr(0, std::min(2, (int)std::to_string(o4).length())),
            std::to_string(o4)
        };

        correct_sequence.clear();
        for (const auto& target : correct_order) {
            for (const auto& part : shuffled_parts) {
                if (part.second == target) {
                    correct_sequence += part.first;
                    break;
                }
            }
        }

        // Формируем текст задачи
        std::stringstream ss;
        ss << "<div class='ip-restore'>";
        ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        ss << "<p class='problem-text'>Восстановите IP-адрес. В ответе укажите последовательность букв, обозначающих фрагменты, в порядке, соответствующем IP-адресу.</p>";

        ss << "<table border='1' cellpadding='10' style='border-collapse: collapse; margin: 15px 0; font-size: 18px;'>";
        ss << "<tr>";
        for (const auto& part : shuffled_parts) {
            ss << "<td align='center' width='80'><strong>" << part.first << "</strong><br>" << part.second << "</td>";
        }
        ss << "</tr>";
        ss << "</table>";

        ss << "<p class='score'>Баллов: " << score << "</p>";
        ss << "</div>";

        problem_text = ss.str();
        correct_answer = correct_sequence;

        // Решение
        std::stringstream sol;
        sol << "<div class='solution'>";
        sol << "<h4>Решение:</h4>";
        sol << "<p>Правильный IP-адрес: <strong>" << original_ip << "</strong></p>";
        sol << "<table border='1' cellpadding='5' style='border-collapse: collapse; margin-top: 10px;'>";
        sol << "<tr><th>Позиция</th><th>Часть</th><th>Буква</th></tr>";

        std::vector<std::string> display_order = {
            std::to_string(o1),
            std::to_string(o2),
            std::to_string(o3),
            std::to_string(o4)
        };

        for (size_t i = 0; i < correct_order.size(); i++) {
            char found_letter = '?';
            for (const auto& part : shuffled_parts) {
                if (part.second == correct_order[i]) {
                    found_letter = part.first;
                    break;
                }
            }
            sol << "<tr><td>" << (i + 1) << " (" << display_order[i] << ")</td><td>"
                << correct_order[i] << "</td><td><strong>" << found_letter << "</strong></td></tr>";
        }

        sol << "</table>";
        sol << "<p><strong>Ответ:</strong> " << correct_sequence << "</p>";
        sol << "</div>";

        solution = sol.str();
    }
    // ============== ЗАДАЧА 2: ВАЛИДАЦИЯ IPv4 ==============
    void IPAddressProblem::generate_validate_ipv4_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        // Генерируем 5 вариантов IPv4
        ip_variants.clear();

        // Создаем один правильный IP
        std::string correct_ip = generate_random_ipv4();

        // Выбираем случайную позицию для правильного варианта
        std::uniform_int_distribution<> dis_pos(0, 4);
        correct_variant_index = dis_pos(gen);

        // Заполняем все 5 позиций
        for (int i = 0; i < 5; i++) {
            if (i == correct_variant_index) {
                ip_variants.push_back(correct_ip);
            }
            else {
                // Генерируем неправильный IP
                std::uniform_int_distribution<> dis_error(0, 3);
                int error_type = dis_error(gen);

                std::string bad_ip;
                switch (error_type) {
                case 0: // Слишком много октетов
                    bad_ip = generate_random_ipv4() + "." + std::to_string(gen() % 256);
                    break;
                case 1: // Октет > 255
                {
                    auto parts = split_ipv4(generate_random_ipv4());
                    parts[gen() % 4] = std::to_string(256 + (gen() % 100));
                    bad_ip = join_ipv4(parts);
                }
                break;
                case 2: // Октет < 0
                {
                    auto parts = split_ipv4(generate_random_ipv4());
                    parts[gen() % 4] = "-" + std::to_string(gen() % 50);
                    bad_ip = join_ipv4(parts);
                }
                break;
                case 3: // Пропущен октет
                {
                    auto parts = split_ipv4(generate_random_ipv4());
                    parts.erase(parts.begin() + (gen() % 4));
                    bad_ip = join_ipv4(parts);
                }
                break;
                }
                ip_variants.push_back(bad_ip);
            }
        }

        // Формируем текст задачи
        std::stringstream ss;
        ss << "<div class='ip-validate'>";
        ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        ss << "<p class='problem-text'>Даны 5 IP-адресов. Найдите номер строки с корректным IP-адресом:</p>";
        ss << "<div class='ip-list' style='font-family: monospace; font-size: 16px;'>";

        for (size_t i = 0; i < ip_variants.size(); i++) {
            ss << "<p><strong>" << (i + 1) << ".</strong> " << ip_variants[i] << "</p>";
        }

        ss << "</div>";
        ss << "<p class='score'>Баллов: " << score << "</p>";
        ss << "</div>";

        problem_text = ss.str();
        correct_answer = std::to_string(correct_variant_index + 1);

        // Решение
        std::stringstream sol;
        sol << "<div class='solution'>";
        sol << "<h4>Решение:</h4>";
        sol << "<p>Правильный IP-адрес должен состоять из 4 чисел от 0 до 255, разделенных точками.</p>";
        sol << "<ul>";
        for (size_t i = 0; i < ip_variants.size(); i++) {
            sol << "<li>" << (i + 1) << ". " << ip_variants[i] << " — ";
            if (is_valid_ipv4(ip_variants[i])) {
                sol << "<strong>корректный</strong>";
            }
            else {
                sol << "некорректный";
            }
            sol << "</li>";
        }
        sol << "</ul>";
        sol << "<p><strong>Ответ:</strong> " << correct_answer << "</p>";
        sol << "</div>";

        solution = sol.str();
    }

    // ============== ЗАДАЧА 3: В ОДНОЙ ЛИ СЕТИ ==============
    void IPAddressProblem::generate_same_network_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        // Генерируем маску подсети
        std::uniform_int_distribution<> dis_mask(16, 28);
        int mask_bits = dis_mask(gen);

        int mask_int = 0xFFFFFFFF << (32 - mask_bits);
        subnet_mask = int_to_ipv4(mask_int);

        // Генерируем базовый адрес сети
        std::uniform_int_distribution<> dis_octet(1, 254);
        int base_ip_int = (dis_octet(gen) << 24) | (dis_octet(gen) << 16) | (dis_octet(gen) << 8) | 0;
        base_ip_int &= mask_int; // Приводим к адресу сети

        network_address = int_to_ipv4(base_ip_int);

        // Генерируем 5 IP-адресов, некоторые в той же сети, некоторые нет
        std::vector<int> ip_ints;
        same_network_indices.clear();

        for (int i = 0; i < 5; i++) {
            std::uniform_int_distribution<> dis_same(0, 1);
            bool same_network = (i < 3) ? true : (dis_same(gen) == 1); // Первые 3 точно в сети для гарантии

            int new_ip;
            if (same_network) {
                // В той же сети - варьируем только последний октет
                new_ip = base_ip_int | (dis_octet(gen) & 0xFF);
                same_network_indices.push_back(i + 1);
            }
            else {
                // В другой сети - меняем другой октет
                new_ip = base_ip_int ^ (1 << (gen() % 24));
            }

            ip_ints.push_back(new_ip);
        }

        // Преобразуем в строки
        network_ips.clear();
        for (int ip_int : ip_ints) {
            network_ips.push_back(int_to_ipv4(ip_int));
        }

        // Формируем ответ (номера IP в одной сети)
        std::stringstream ans;
        for (size_t i = 0; i < same_network_indices.size(); i++) {
            if (i > 0) ans << ", ";
            ans << same_network_indices[i];
        }
        network_answer = ans.str();

        // Формируем текст задачи
        std::stringstream ss;
        ss << "<div class='ip-network'>";
        ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        ss << "<p class='problem-text'>Даны IP-адреса. Определите, какие из них находятся в одной сети.</p>";
        ss << "<p><strong>Маска подсети:</strong> " << subnet_mask << "</p>";
        ss << "<div class='ip-list' style='font-family: monospace; font-size: 16px;'>";

        for (size_t i = 0; i < network_ips.size(); i++) {
            ss << "<p><strong>" << (i + 1) << ".</strong> " << network_ips[i] << "</p>";
        }

        ss << "</div>";
        ss << "<p>В ответе укажите номера IP через запятую (например: 1,3,4).</p>";
        ss << "<p class='score'>Баллов: " << score << "</p>";
        ss << "</div>";

        problem_text = ss.str();
        correct_answer = network_answer;

        // Решение
        std::stringstream sol;
        sol << "<div class='solution'>";
        sol << "<h4>Решение:</h4>";
        sol << "<p>Маска подсети: " << subnet_mask << " (битов: " << mask_bits << ")</p>";
        sol << "<p>Адрес сети: " << network_address << "</p>";
        sol << "<ul>";

        for (size_t i = 0; i < network_ips.size(); i++) {
            std::string network = get_network_address(network_ips[i], subnet_mask);
            sol << "<li>" << (i + 1) << ". " << network_ips[i] << " → сеть " << network;
            if (network == network_address) {
                sol << " <strong>(в одной сети)</strong>";
            }
            sol << "</li>";
        }

        sol << "</ul>";
        sol << "<p><strong>Ответ:</strong> " << network_answer << "</p>";
        sol << "</div>";

        solution = sol.str();
    }

    // ============== ЗАДАЧА 4: ВАЛИДАЦИЯ IPv6 ==============
    void IPAddressProblem::generate_validate_ipv6_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        ipv6_variants.clear();
        ipv6_errors.clear();

        // Генерируем 5 вариантов IPv6
        std::uniform_int_distribution<> dis_pos(0, 4);
        correct_ipv6_index = dis_pos(gen);

        for (int i = 0; i < 5; i++) {
            if (i == correct_ipv6_index) {
                // Правильный IPv6
                std::string correct = generate_random_ipv6();
                ipv6_variants.push_back(correct);
                ipv6_errors.push_back("корректный");
            }
            else {
                // Неправильный IPv6
                std::uniform_int_distribution<> dis_error(0, 3);
                int error_type = dis_error(gen);

                std::string bad_ip;
                switch (error_type) {
                case 0: // Слишком много групп
                {
                    std::string base = generate_random_ipv6();
                    bad_ip = base + ":1234";
                }
                break;
                case 1: // Слишком мало групп
                {
                    auto parts = split_ipv4(generate_random_ipv4()); // не лучший способ
                    bad_ip = "2001:db8::1"; // упрощенно
                }
                break;
                case 2: // Неверные символы
                    bad_ip = "2001:db8::g123";
                    break;
                case 3: // Пустая группа
                    bad_ip = "2001::db8:::1";
                    break;
                }
                ipv6_variants.push_back(bad_ip);
                ipv6_errors.push_back("некорректный");
            }
        }

        // Формируем текст задачи
        std::stringstream ss;
        ss << "<div class='ipv6-validate'>";
        ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        ss << "<p class='problem-text'>Даны IPv6-адреса. Найдите номер строки с корректным адресом:</p>";
        ss << "<div class='ip-list' style='font-family: monospace; font-size: 14px;'>";

        for (size_t i = 0; i < ipv6_variants.size(); i++) {
            ss << "<p><strong>" << (i + 1) << ".</strong> " << ipv6_variants[i] << "</p>";
        }

        ss << "</div>";
        ss << "<p class='score'>Баллов: " << score << "</p>";
        ss << "</div>";

        problem_text = ss.str();
        correct_answer = std::to_string(correct_ipv6_index + 1);

        // Решение
        std::stringstream sol;
        sol << "<div class='solution'>";
        sol << "<h4>Решение:</h4>";
        sol << "<p>IPv6-адрес состоит из 8 групп по 4 шестнадцатеричные цифры, разделенных двоеточиями.</p>";
        sol << "<ul>";
        for (size_t i = 0; i < ipv6_variants.size(); i++) {
            sol << "<li>" << (i + 1) << ". " << ipv6_variants[i] << " — ";
            if (i == correct_ipv6_index) {
                sol << "<strong>корректный</strong>";
            }
            else {
                sol << "некорректный (" << ipv6_errors[i] << ")";
            }
            sol << "</li>";
        }
        sol << "</ul>";
        sol << "<p><strong>Ответ:</strong> " << correct_answer << "</p>";
        sol << "</div>";

        solution = sol.str();
    }

    // ============== ЗАДАЧА 5: МАСКА ПОДСЕТИ ==============
    void IPAddressProblem::generate_subnet_mask_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        // Генерируем IP и маску
        mask_problem_ip = generate_random_ipv4();

        std::uniform_int_distribution<> dis_mask(8, 30);
        int mask_bits = dis_mask(gen);
        int mask_int = 0xFFFFFFFF << (32 - mask_bits);
        mask_problem_mask = int_to_ipv4(mask_int);

        // Вычисляем результат (адрес сети или broadcast в зависимости от типа)
        std::uniform_int_distribution<> dis_type(0, 1);
        bool ask_network = (dis_type(gen) == 0);

        if (ask_network) {
            mask_problem_result = get_network_address(mask_problem_ip, mask_problem_mask);
        }
        else {
            mask_problem_result = get_broadcast_address(mask_problem_ip, mask_problem_mask);
        }

        // Генерируем варианты ответов
        mask_options.clear();
        std::vector<std::string> wrong_options;

        // Добавляем правильный ответ
        mask_options.push_back(mask_problem_result);

        // Добавляем неправильные варианты
        for (int i = 0; i < 4; i++) {
            auto parts = split_ipv4(mask_problem_ip);
            parts[gen() % 4] = std::to_string(gen() % 300);
            mask_options.push_back(join_ipv4(parts));
        }

        // Перемешиваем
        std::shuffle(mask_options.begin(), mask_options.end(), gen);

        // Находим индекс правильного ответа
        for (size_t i = 0; i < mask_options.size(); i++) {
            if (mask_options[i] == mask_problem_result) {
                mask_correct_index = i;
                break;
            }
        }

        // Формируем текст задачи
        std::stringstream ss;
        ss << "<div class='ip-mask'>";
        ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        ss << "<p class='problem-text'>Для IP-адреса <strong>" << mask_problem_ip
            << "</strong> с маской <strong>" << mask_problem_mask << "</strong> ";

        if (ask_network) {
            ss << "определите адрес сети.";
        }
        else {
            ss << "определите broadcast-адрес.";
        }

        ss << "</p>";
        ss << "<div class='options-list' style='margin: 15px 0;'>";

        char opt_letter = 'А';
        for (const auto& opt : mask_options) {
            ss << "<p><strong>" << opt_letter++ << ".</strong> " << opt << "</p>";
        }

        ss << "</div>";
        ss << "<p>В ответе укажите букву выбранного варианта.</p>";
        ss << "<p class='score'>Баллов: " << score << "</p>";
        ss << "</div>";

        problem_text = ss.str();
        correct_answer = std::string(1, 'А' + mask_correct_index);

        // Решение
        std::stringstream sol;
        sol << "<div class='solution'>";
        sol << "<h4>Решение:</h4>";
        sol << "<p>IP: " << mask_problem_ip << "</p>";
        sol << "<p>Маска: " << mask_problem_mask << "</p>";

        if (ask_network) {
            sol << "<p>Адрес сети = IP & Маска = " << mask_problem_result << "</p>";
        }
        else {
            sol << "<p>Broadcast = IP | ~Маска = " << mask_problem_result << "</p>";
        }

        sol << "<p><strong>Ответ:</strong> " << correct_answer << "</p>";
        sol << "</div>";

        solution = sol.str();
    }

    // ============== ЗАДАЧА 6: КЛАСС IP-АДРЕСА ==============
    void IPAddressProblem::generate_ip_class_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        // Генерируем IP
        class_ip = generate_random_ipv4();
        char ip_class = get_ip_class(class_ip);

        // Определяем ответ
        switch (ip_class) {
        case 'A': class_answer = "Класс A (1.0.0.0 - 126.255.255.255)"; break;
        case 'B': class_answer = "Класс B (128.0.0.0 - 191.255.255.255)"; break;
        case 'C': class_answer = "Класс C (192.0.0.0 - 223.255.255.255)"; break;
        case 'D': class_answer = "Класс D (224.0.0.0 - 239.255.255.255) - мультикаст"; break;
        case 'E': class_answer = "Класс E (240.0.0.0 - 255.255.255.255) - зарезервировано"; break;
        default: class_answer = "Неопределен";
        }

        // Генерируем варианты
        std::vector<std::string> all_classes = {
            "Класс A (1.0.0.0 - 126.255.255.255)",
            "Класс B (128.0.0.0 - 191.255.255.255)",
            "Класс C (192.0.0.0 - 223.255.255.255)",
            "Класс D (224.0.0.0 - 239.255.255.255) - мультикаст",
            "Класс E (240.0.0.0 - 255.255.255.255) - зарезервировано"
        };

        class_options = all_classes;
        std::shuffle(class_options.begin(), class_options.end(), gen);

        // Находим индекс правильного
        int correct_idx = 0;
        for (size_t i = 0; i < class_options.size(); i++) {
            if (class_options[i] == class_answer) {
                correct_idx = i;
                break;
            }
        }

        // Формируем текст задачи
        std::stringstream ss;
        ss << "<div class='ip-class'>";
        ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        ss << "<p class='problem-text'>Определите класс IP-адреса <strong>" << class_ip << "</strong>.</p>";
        ss << "<div class='options-list' style='margin: 15px 0;'>";

        char opt_letter = 'А';
        for (const auto& opt : class_options) {
            ss << "<p><strong>" << opt_letter++ << ".</strong> " << opt << "</p>";
        }

        ss << "</div>";
        ss << "<p>В ответе укажите букву выбранного варианта.</p>";
        ss << "<p class='score'>Баллов: " << score << "</p>";
        ss << "</div>";

        problem_text = ss.str();
        correct_answer = std::string(1, 'А' + correct_idx);

        // Решение
        std::stringstream sol;
        sol << "<div class='solution'>";
        sol << "<h4>Решение:</h4>";
        sol << "<p>IP: " << class_ip << "</p>";
        sol << "<p>Первый октет: " << split_ipv4(class_ip)[0] << "</p>";
        sol << "<p>Это соответствует: <strong>" << class_answer << "</strong></p>";
        sol << "<p><strong>Ответ:</strong> " << correct_answer << "</p>";
        sol << "</div>";

        solution = sol.str();
    }

    // ============== ЗАДАЧА 7: BROADCAST-АДРЕС ==============
    void IPAddressProblem::generate_broadcast_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        // Генерируем IP и маску
        broadcast_ip = generate_random_ipv4();

        std::uniform_int_distribution<> dis_mask(16, 28);
        int mask_bits = dis_mask(gen);
        int mask_int = 0xFFFFFFFF << (32 - mask_bits);
        broadcast_mask = int_to_ipv4(mask_int);

        // Вычисляем broadcast
        broadcast_address = get_broadcast_address(broadcast_ip, broadcast_mask);

        // Формируем текст задачи
        std::stringstream ss;
        ss << "<div class='ip-broadcast'>";
        ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        ss << "<p class='problem-text'>Для IP-адреса <strong>" << broadcast_ip
            << "</strong> с маской <strong>" << broadcast_mask
            << "</strong> определите broadcast-адрес.</p>";
        ss << "<p>Введите ответ в формате IPv4.</p>";
        ss << "<p class='score'>Баллов: " << score << "</p>";
        ss << "</div>";

        problem_text = ss.str();
        correct_answer = broadcast_address;

        // Решение
        std::stringstream sol;
        sol << "<div class='solution'>";
        sol << "<h4>Решение:</h4>";
        sol << "<p>IP: " << broadcast_ip << "</p>";
        sol << "<p>Маска: " << broadcast_mask << "</p>";

        int ip_int = ipv4_to_int(broadcast_ip);
        mask_int = ipv4_to_int(broadcast_mask);
        int broadcast_int = ip_int | (~mask_int);

        sol << "<p>Broadcast = IP | ~Маска = " << broadcast_address << "</p>";
        sol << "<p><strong>Ответ:</strong> " << broadcast_address << "</p>";
        sol << "</div>";

        solution = sol.str();
    }

    // ============== MAIN GENERATE METHOD ==============
    void IPAddressProblem::generate(const GenerationConfig& config) {
        apply_difficulty_settings(config.difficulty);

        std::string task_type_str = config.get_string_param("task_type", "restore");

        if (task_type_str == "restore") {
            task_type = IPTaskType::RESTORE_IPV4;
            generate_restore_ipv4_task(config);
        }
        else if (task_type_str == "validate") {
            task_type = IPTaskType::VALIDATE_IPV4;
            generate_validate_ipv4_task(config);
        }
        else if (task_type_str == "same_network") {
            task_type = IPTaskType::SAME_NETWORK;
            generate_same_network_task(config);
        }
        else if (task_type_str == "ipv6") {
            task_type = IPTaskType::VALIDATE_IPV6;
            generate_validate_ipv6_task(config);
        }
        else if (task_type_str == "subnet_mask") {
            task_type = IPTaskType::SUBNET_MASK;
            generate_subnet_mask_task(config);
        }
        else if (task_type_str == "ip_class") {
            task_type = IPTaskType::IP_CLASS;
            generate_ip_class_task(config);
        }
        else if (task_type_str == "broadcast") {
            task_type = IPTaskType::BROADCAST_ADDRESS;
            generate_broadcast_task(config);
        }

        // Подсказка
        std::stringstream hint_ss;
        hint_ss << "<div class='hint'>";
        hint_ss << "<h4>Подсказка:</h4><p>";

        switch (task_type) {
        case IPTaskType::RESTORE_IPV4:
            hint_ss << "IP-адрес состоит из 4 чисел от 0 до 255, разделенных точками";
            break;
        case IPTaskType::VALIDATE_IPV4:
            hint_ss << "IP-адрес должен содержать 4 числа от 0 до 255";
            break;
        case IPTaskType::SAME_NETWORK:
            hint_ss << "Адрес сети получается применением маски к IP (побитовое И)";
            break;
        case IPTaskType::VALIDATE_IPV6:
            hint_ss << "IPv6 состоит из 8 групп по 4 шестнадцатеричные цифры";
            break;
        case IPTaskType::SUBNET_MASK:
            hint_ss << "Адрес сети = IP & Маска, Broadcast = IP | ~Маска";
            break;
        case IPTaskType::IP_CLASS:
            hint_ss << "Класс A: 1-126, B: 128-191, C: 192-223, D: 224-239, E: 240-255";
            break;
        case IPTaskType::BROADCAST_ADDRESS:
            hint_ss << "Broadcast-адрес получается установкой всех битов хоста в 1";
            break;
        }

        hint_ss << "</p></div>";
        hint = hint_ss.str();

        unique_id = generate_unique_id();
        generated_at = std::time(nullptr);
    }

    std::string IPAddressProblem::generate_html() const {
        std::stringstream html;

        html << "<div class='problem-wrapper' data-problem-id='" << unique_id << "'>\n";
        html << "    " << problem_text << "\n";
        html << "    <div class='answer-area'>\n";
        html << "        <input type='text' id='" << unique_id << "'\n";
        html << "               placeholder='Ваш ответ' \n";
        html << "               data-correct-answer='" << correct_answer << "'\n";
        html << "               data-score='" << score << "'\n";
        html << "               data-answered='false'>\n";
        html << "        <button onclick='checkAnswer(\"" << unique_id << "\")'>\n";
        html << "            Проверить\n";
        html << "        </button>\n";
        html << "    </div>\n";

        if (get_page_settings().can_view_solutions()) {
            html << "    <div class='solution-section'>\n";
            html << "        <button class='solution-toggle-btn' onclick='SolutionManager.toggleSolution(\"";
            html << unique_id << "\")'>\n";
            html << "            📚 Показать решение\n";
            html << "        </button>\n";
            html << "        <div id='solution-" << unique_id << "' class='solution-content' style='display: none;'>\n";
            html << "            " << solution << "\n";
            html << "        </div>\n";
            html << "    </div>\n";
        }

        if (get_page_settings().show_hints) {
            html << "    <div class='hint-section'>\n";
            html << "        <details>\n";
            html << "            <summary>💡 Подсказка</summary>\n";
            html << "            " << hint << "\n";
            html << "        </details>\n";
            html << "    </div>\n";
        }

        html << "</div>\n";

        return html.str();
    }

    bool IPAddressProblem::check_answer(const std::string& user_answer) const {
        std::string trimmed = user_answer;
        trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r\f\v"));
        trimmed.erase(trimmed.find_last_not_of(" \t\n\r\f\v") + 1);

        // Для задач с буквами приводим к верхнему регистру
        if (task_type == IPTaskType::RESTORE_IPV4 ||
            task_type == IPTaskType::SUBNET_MASK ||
            task_type == IPTaskType::IP_CLASS) {
            std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(), ::toupper);
        }

        std::string correct = correct_answer;
        std::transform(correct.begin(), correct.end(), correct.begin(), ::toupper);

        return trimmed == correct;
    }

    void IPAddressProblem::set_page_settings(const PageSettings& settings) {
        page_settings = settings;
    }

    const PageSettings& IPAddressProblem::get_page_settings() const {
        return page_settings;
    }

}