// num_system_problem.cpp
#include "num_system_problem.h"
#include <cmath>
#include <algorithm>
#include <regex>

namespace OGE {

    // Инициализация статических членов
    PageSettings NumSystemProblem::page_settings;
    NumSystemConfig NumSystemProblem::default_config;

    std::mt19937& NumSystemProblem::get_random_generator() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }

    NumSystemProblem::NumSystemProblem(const ProblemMeta& meta)
        : ProblemBase(meta),
        source_base(10),
        target_base(10),
        decimal_value(0),
        direction(ConversionDirection::TO_DECIMAL),
        is_valid_problem(false) {
    }

    // Преобразование числа из десятичной в любую СС
    std::string NumSystemProblem::to_base(int number, int base) const {
        if (number == 0) return "0";
        if (base < 2 || base > 36) return std::to_string(number);

        std::string result;
        int num = abs(number);

        while (num > 0) {
            int remainder = num % base;
            result += get_base_symbol(remainder);
            num /= base;
        }

        std::reverse(result.begin(), result.end());

        if (number < 0) {
            result = "-" + result;
        }

        return result;
    }

    // Преобразование числа из любой СС в десятичную
    int NumSystemProblem::from_base(const std::string& number, int base) const {
        if (number.empty()) return 0;

        bool is_negative = (number[0] == '-');
        std::string num_str = is_negative ? number.substr(1) : number;

        int result = 0;
        int power = 0;

        for (auto it = num_str.rbegin(); it != num_str.rend(); ++it) {
            int digit = get_digit_value(*it);
            result += digit * static_cast<int>(std::pow(base, power));
            power++;
        }

        return is_negative ? -result : result;
    }

    // Получение символа для цифры
    std::string NumSystemProblem::get_base_symbol(int digit) const {
        if (digit >= 0 && digit <= 9) {
            return std::to_string(digit);
        }
        else if (digit >= 10 && digit <= 35) {
            return std::string(1, 'A' + (digit - 10));
        }
        return "?";
    }

    // Получение значения цифры из символа
    int NumSystemProblem::get_digit_value(char symbol) const {
        if (symbol >= '0' && symbol <= '9') {
            return symbol - '0';
        }
        else if (symbol >= 'A' && symbol <= 'Z') {
            return 10 + (symbol - 'A');
        }
        else if (symbol >= 'a' && symbol <= 'z') {
            return 10 + (symbol - 'a');
        }
        return 0;
    }

    // Проверка корректности числа для заданной СС
    bool NumSystemProblem::validate_number(const std::string& number, int base) const {
        if (number.empty()) return false;

        std::string num_str = number;
        if (num_str[0] == '-') {
            num_str = num_str.substr(1);
        }

        for (char c : num_str) {
            int digit = get_digit_value(c);
            if (digit >= base || digit < 0) {
                return false;
            }
        }
        return true;
    }

    // Генерация случайного числа в заданной СС
    std::string NumSystemProblem::generate_random_number(int base, int min_val, int max_val) {
        auto& gen = get_random_generator();
        std::uniform_int_distribution<> dis_val(min_val, max_val);

        decimal_value = dis_val(gen);
        return to_base(decimal_value, base);
    }

    // Генерация последовательности цифр
    std::string NumSystemProblem::generate_digit_sequence(int length, int base) {
        auto& gen = get_random_generator();
        std::uniform_int_distribution<> dis_digit(0, base - 1);

        std::string result;
        // Первая цифра не может быть 0
        int first_digit = dis_digit(gen);
        if (first_digit == 0) first_digit = 1;
        result += get_base_symbol(first_digit);

        for (int i = 1; i < length; i++) {
            result += get_base_symbol(dis_digit(gen));
        }

        decimal_value = from_base(result, base);
        return result;
    }

    // Основной метод генерации задачи
    void NumSystemProblem::generate(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        // Получаем настройки из custom_params или используем значения по умолчанию
        NumSystemConfig ns_config = default_config;

        std::string dir_str = config.get_string_param("conversion_direction", "to_decimal");
        if (dir_str == "from_decimal") {
            direction = ConversionDirection::FROM_DECIMAL;
        }
        else {
            direction = ConversionDirection::TO_DECIMAL;
        }

        int min_base = std::stoi(config.get_string_param("min_base", "2"));
        int max_base = std::stoi(config.get_string_param("max_base", "16"));
        int min_value = std::stoi(config.get_string_param("min_value", "10"));
        int max_value = std::stoi(config.get_string_param("max_value", "1000"));

        // Настройка сложности
        switch (config.difficulty) {
        case Difficulty::EASY:
            score = 10;
            min_base = 2; max_base = 8;
            min_value = 10; max_value = 100;
            break;
        case Difficulty::MEDIUM:
            score = 15;
            min_base = 2; max_base = 12;
            min_value = 50; max_value = 500;
            break;
        case Difficulty::HARD:
            score = 20;
            min_base = 2; max_base = 16;
            min_value = 100; max_value = 5000;
            break;
        }

        std::uniform_int_distribution<> dis_base(min_base, max_base);
        source_base = dis_base(gen);
        target_base = 10; // По умолчанию

        if (direction == ConversionDirection::TO_DECIMAL) {
            // Из произвольной СС в десятичную
            std::uniform_int_distribution<> dis_length(2, 4);
            int number_length = dis_length(gen);

            source_number = generate_digit_sequence(number_length, source_base);
            target_number = std::to_string(decimal_value);
            correct_answer = target_number;

            // Формируем текст задачи
            std::stringstream ss;
            ss << "<div class='num-system-problem'>";
            ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
            ss << "<p class='problem-text'>Переведите число <strong>" << source_number
                << "</strong><sub>" << source_base << "</sub> в десятичную систему счисления.</p>";
            ss << "<p class='score'>Баллов: " << score << "</p>";
            ss << "</div>";
            problem_text = ss.str();

            // Решение
            ss.str("");
            ss << "<div class='solution'>";
            ss << "<h4>Решение:</h4>";
            ss << "<p>Для перевода числа из " << source_base << "-й системы счисления в десятичную:</p>";
            ss << "<p><strong>" << source_number << "</strong><sub>" << source_base << "</sub> = ";

            int temp_val = decimal_value;
            for (size_t i = 0; i < source_number.length(); i++) {
                int digit = get_digit_value(source_number[i]);
                int power = source_number.length() - 1 - i;
                ss << digit << " × " << source_base << "<sup>" << power << "</sup>";
                if (i < source_number.length() - 1) ss << " + ";
            }
            ss << " = " << decimal_value << "<sub>10</sub></p>";
            ss << "</div>";
            solution = ss.str();

        }
        else {
            // Из десятичной в произвольную СС
            std::uniform_int_distribution<> dis_target_base(2, max_base);
            target_base = dis_target_base(gen);

            decimal_value = stoi(generate_random_number(10, min_value, max_value));
            source_number = std::to_string(decimal_value);
            target_number = to_base(decimal_value, target_base);
            correct_answer = target_number;

            // Формируем текст задачи
            std::stringstream ss;
            ss << "<div class='num-system-problem'>";
            ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
            ss << "<p class='problem-text'>Переведите число <strong>" << decimal_value
                << "</strong><sub>10</sub> в систему счисления с основанием <strong>"
                << target_base << "</strong>.</p>";
            ss << "<p class='score'>Баллов: " << score << "</p>";
            ss << "</div>";
            problem_text = ss.str();

            // Решение
            ss.str("");
            ss << "<div class='solution'>";
            ss << "<h4>Решение:</h4>";
            ss << "<p>Для перевода числа " << decimal_value << " в систему с основанием "
                << target_base << ":</p>";
            ss << "<p>";

            int quotient = decimal_value;
            std::vector<std::string> steps;

            while (quotient > 0) {
                int remainder = quotient % target_base;
                steps.push_back(get_base_symbol(remainder));
                quotient /= target_base;
            }

            ss << "Последовательно делим на " << target_base << ":<br>";
            int temp = decimal_value;
            while (temp > 0) {
                ss << temp << " ÷ " << target_base << " = " << (temp / target_base)
                    << " (остаток " << get_base_symbol(temp % target_base) << ")<br>";
                temp /= target_base;
            }

            ss << "Записываем остатки в обратном порядке: ";
            for (auto it = steps.rbegin(); it != steps.rend(); ++it) {
                ss << *it;
            }
            ss << "<sub>" << target_base << "</sub></p>";
            ss << "</div>";
            solution = ss.str();
        }

        // Подсказка
        std::stringstream ss;
        ss << "<div class='hint'>";
        ss << "<h4>Подсказка:</h4>";
        ss << "<p>";
        if (direction == ConversionDirection::TO_DECIMAL) {
            ss << "Каждая цифра умножается на основание в степени позиции (справа налево, начиная с 0)";
        }
        else {
            ss << "Последовательно делите число на основание, остатки записывайте справа налево";
        }
        ss << "</p>";
        if (source_base > 10 || target_base > 10) {
            ss << "<p>Цифры: 0-9, затем A=10, B=11, C=12, D=13, E=14, F=15</p>";
        }
        ss << "</div>";
        hint = ss.str();

        unique_id = generate_unique_id();
        generated_at = std::time(nullptr);
        is_valid_problem = true;
    }

    std::string NumSystemProblem::generate_html() const {
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

        // Блок с решением
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

        // Блок с подсказкой
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

    bool NumSystemProblem::check_answer(const std::string& user_answer) const {
        std::string trimmed = user_answer;
        // Удаляем пробелы в начале и конце
        trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r\f\v"));
        trimmed.erase(trimmed.find_last_not_of(" \t\n\r\f\v") + 1);

        // Приводим к верхнему регистру для сравнения
        std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(), ::toupper);
        std::string correct = correct_answer;
        std::transform(correct.begin(), correct.end(), correct.begin(), ::toupper);

        return trimmed == correct;
    }

    void NumSystemProblem::set_page_settings(const PageSettings& settings) {
        page_settings = settings;
    }

    const PageSettings& NumSystemProblem::get_page_settings() const {
        return page_settings;
    }

}