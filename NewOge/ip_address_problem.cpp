// ip_address_problem.cpp
#include "ip_address_problem.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace OGE {

    PageSettings IPAddressProblem::page_settings;

    std::mt19937& IPAddressProblem::get_random_generator() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }

    IPAddressProblem::IPAddressProblem(const ProblemMeta& meta)
        : ProblemBase(meta), task_type(IPTaskType::RESTORE) {
    }

    std::string IPAddressProblem::generate_random_ip() {
        auto& gen = get_random_generator();

        std::uniform_int_distribution<> dis_octet(0, 255);

        std::stringstream ss;
        ss << dis_octet(gen) << "."
            << dis_octet(gen) << "."
            << dis_octet(gen) << "."
            << dis_octet(gen);

        return ss.str();
    }

    bool IPAddressProblem::is_valid_ip(const std::string& ip) {
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

    std::vector<std::string> IPAddressProblem::split_ip(const std::string& ip) {
        std::vector<std::string> parts;
        std::stringstream ss(ip);
        std::string part;

        while (std::getline(ss, part, '.')) {
            parts.push_back(part);
        }

        return parts;
    }

    std::string IPAddressProblem::join_ip(const std::vector<std::string>& parts) {
        std::stringstream ss;
        for (size_t i = 0; i < parts.size(); i++) {
            if (i > 0) ss << ".";
            ss << parts[i];
        }
        return ss.str();
    }

    void IPAddressProblem::generate(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        // Определяем тип задачи
        std::string task_type_str = config.get_string_param("task_type", "restore");

        if (task_type_str == "validate") {
            task_type = IPTaskType::VALIDATE;
            score = 15;

            // Генерируем 5 вариантов IP
            ip_variants.clear();

            // Создаем один правильный IP
            std::string correct_ip = generate_random_ip();

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
                    std::uniform_int_distribution<> dis_error(0, 2);
                    int error_type = dis_error(gen);

                    std::string bad_ip;
                    switch (error_type) {
                    case 0: // Слишком много октетов
                        bad_ip = generate_random_ip() + "." + std::to_string(gen() % 256);
                        break;
                    case 1: // Октет > 255
                    {
                        auto parts = split_ip(generate_random_ip());
                        parts[gen() % 4] = std::to_string(256 + (gen() % 100));
                        bad_ip = join_ip(parts);
                    }
                    break;
                    case 2: // Октет < 0
                    {
                        auto parts = split_ip(generate_random_ip());
                        parts[gen() % 4] = "-" + std::to_string(gen() % 50);
                        bad_ip = join_ip(parts);
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
            ss << "<div class='ip-list'>";

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
                if (is_valid_ip(ip_variants[i])) {
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
        else {
            // Задача на восстановление IP
            task_type = IPTaskType::RESTORE;
            score = 10;

            // Генерируем случайный IP
            original_ip = generate_random_ip();

            // Разбиваем на части по октетам
            std::vector<std::string> octets = split_ip(original_ip);

            // Создаем 4 части для перемешивания
            std::vector<std::string> parts_to_shuffle;
            for (const auto& octet : octets) {
                parts_to_shuffle.push_back(octet);
            }

            // Перемешиваем части
            std::shuffle(parts_to_shuffle.begin(), parts_to_shuffle.end(), gen);

            // Присваиваем буквы А, Б, В, Г
            shuffled_parts.clear();
            char letter = 'А';
            for (const auto& part : parts_to_shuffle) {
                shuffled_parts.push_back({ letter++, part });
            }

            // Находим правильную последовательность букв
            correct_sequence.clear();
            for (const auto& octet : octets) {
                for (const auto& part : shuffled_parts) {
                    if (part.second == octet) {
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
            ss << "<table border='1' cellpadding='8' style='border-collapse: collapse; margin: 10px 0;'>";
            ss << "<tr>";

            for (const auto& part : shuffled_parts) {
                ss << "<td align='center'><strong>" << part.first << "</strong><br>" << part.second << "</td>";
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
            sol << "<p>Последовательность букв: <strong>" << correct_sequence << "</strong></p>";
            sol << "</div>";

            solution = sol.str();
        }

        // Подсказка
        std::stringstream hint_ss;
        hint_ss << "<div class='hint'>";
        hint_ss << "<h4>Подсказка:</h4><p>";

        if (task_type == IPTaskType::RESTORE) {
            hint_ss << "IP-адрес состоит из 4 чисел от 0 до 255, разделенных точками";
        }
        else {
            hint_ss << "IP-адрес должен содержать 4 числа от 0 до 255";
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

        // Приводим к верхнему регистру для букв
        std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(), ::toupper);
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