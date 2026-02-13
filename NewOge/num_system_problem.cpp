// num_system_problem.cpp
#include "num_system_problem.h"
#include <cmath>
#include <algorithm>
#include <regex>
#include <set>
#include <map>

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
        task_type(NumSystemTaskType::CONVERSION),
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

    // Генерация задачи на сравнение чисел в разных СС
    void NumSystemProblem::generate_comparison_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        std::uniform_int_distribution<> dis_base(2, 12);
        std::uniform_int_distribution<> dis_length(2, 3);

        numbers_for_comparison.clear();

        // Генерируем 4-5 чисел в разных системах счисления
        int count = 4 + (gen() % 2);

        // Выбираем критерий: 0 - максимум, 1 - минимум, 2 - ближайшее к числу
        int criteria = gen() % 3;
        std::vector<std::pair<int, std::pair<std::string, int>>> decimal_values;

        for (int i = 0; i < count; i++) {
            int base = dis_base(gen);
            int length = dis_length(gen);
            std::string number = generate_digit_sequence(length, base);
            numbers_for_comparison.push_back({ number, base });
            decimal_values.push_back({ decimal_value, {number, base} });
        }

        // Сортируем по десятичному значению
        std::sort(decimal_values.begin(), decimal_values.end());

        std::stringstream ss;
        ss << "<div class='num-system-comparison'>";
        ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";

        if (criteria == 0) {
            // Найти максимальное
            auto max_val = decimal_values.back();
            comparison_result = max_val.second.first;
            comparison_criteria = "максимальное";
            ss << "<p class='problem-text'>Даны числа в различных системах счисления. Найдите <strong>НАИБОЛЬШЕЕ</strong> из них.</p>";
        }
        else if (criteria == 1) {
            // Найти минимальное
            auto min_val = decimal_values.front();
            comparison_result = min_val.second.first;
            comparison_criteria = "минимальное";
            ss << "<p class='problem-text'>Даны числа в различных системах счисления. Найдите <strong>НАИМЕНЬШЕЕ</strong> из них.</p>";
        }
        else {
            // Найти ближайшее к заданному числу
            std::uniform_int_distribution<> dis_target(20, 200);
            int target_value = dis_target(gen);

            // Находим число, ближайшее к target_value
            int min_diff = std::abs(decimal_values[0].first - target_value);
            int best_index = 0;
            bool unique = false;

            // Проверяем, чтобы не было двух одинаково близких
            for (size_t i = 1; i < decimal_values.size(); i++) {
                int diff = std::abs(decimal_values[i].first - target_value);
                if (diff < min_diff) {
                    min_diff = diff;
                    best_index = i;
                    unique = true;
                }
                else if (diff == min_diff) {
                    unique = false;
                }
            }

            // Если нашли неуникальное, генерируем заново (упрощенно)
            if (!unique) {
                // Просто берем максимальное
                best_index = decimal_values.size() - 1;
            }

            comparison_result = decimal_values[best_index].second.first
;
            comparison_criteria = "ближайшее к " + std::to_string(target_value);
            ss << "<p class='problem-text'>Даны числа в различных системах счисления. Найдите число, <strong>НАИБОЛЕЕ БЛИЗКОЕ</strong> к значению "
                << target_value << "<sub>10</sub>.</p>";
        }

        ss << "<div class='numbers-list'>";
        for (const auto& num : numbers_for_comparison) {
            ss << "<p><strong>" << num.first << "</strong><sub>" << num.second << "</sub></p>";
        }
        ss << "</div>";
        ss << "<p class='score'>Баллов: " << score << "</p>";
        ss << "</div>";

        problem_text = ss.str();

        // Решение
        ss.str("");
        ss << "<div class='solution'>";
        ss << "<h4>Решение:</h4>";
        ss << "<p>Переведем все числа в десятичную систему:</p><ul>";

        for (const auto& num : numbers_for_comparison) {
            int dec_val = from_base(num.first, num.second);
            ss << "<li>" << num.first << "<sub>" << num.second << "</sub> = " << dec_val << "<sub>10</sub></li>";
        }

        ss << "</ul>";
        ss << "<p><strong>Ответ:</strong> " << comparison_criteria << " число: ";
        size_t underscore_pos = comparison_result.find('_');
        std::string result_num = comparison_result.substr(0, underscore_pos);
        std::string result_base = comparison_result.substr(underscore_pos + 1);
        ss << result_num << "<sub>" << result_base << "</sub></p>";
        ss << "</div>";

        solution = ss.str();
        correct_answer = comparison_result;
    }

    // Генерация уравнения X + число = число
    void NumSystemProblem::generate_equation_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        std::uniform_int_distribution<> dis_base(2, 10);
        std::uniform_int_distribution<> dis_value(10, 100);
        std::uniform_int_distribution<> dis_op(0, 1); // 0 - сложение, 1 - вычитание

        equation_base = dis_base(gen);

        // Генерируем два числа в одной СС
        int val1 = dis_value(gen);
        int val2 = dis_value(gen);

        equation_operation = (dis_op(gen) == 0) ? '+' : '-';

        if (equation_operation == '+') {
            // X + val1 = val2
            equation_term1 = val1;
            equation_term2 = val2;
            equation_x = val2 - val1;
        }
        else {
            // X - val1 = val2
            equation_term1 = val1;
            equation_term2 = val2;
            equation_x = val2 + val1;
        }

        std::string num1_str = to_base(equation_term1, equation_base);
        std::string num2_str = to_base(equation_term2, equation_base);

        std::stringstream ss;
        ss << "<div class='num-system-equation'>";
        ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        ss << "<p class='problem-text'>Решите уравнение: ";
        ss << "<strong>X " << equation_operation << " " << num1_str << "<sub>" << equation_base << "</sub> = "
            << num2_str << "<sub>" << equation_base << "</sub></strong></p>";
        ss << "<p>Ответ запишите в <strong>" << equation_base << "</strong>-й системе счисления.</p>";
        ss << "<p class='score'>Баллов: " << score << "</p>";
        ss << "</div>";

        problem_text = ss.str();

        // Решение
        ss.str("");
        ss << "<div class='solution'>";
        ss << "<h4>Решение:</h4>";
        ss << "<p>1. Переведем числа в десятичную систему:</p>";
        ss << "<p>" << num1_str << "<sub>" << equation_base << "</sub> = " << equation_term1 << "<sub>10</sub></p>";
        ss << "<p>" << num2_str << "<sub>" << equation_base << "</sub> = " << equation_term2 << "<sub>10</sub></p>";
        ss << "<p>2. Решаем уравнение в десятичной системе:</p>";

        if (equation_operation == '+') {
            ss << "<p>X + " << equation_term1 << " = " << equation_term2 << "</p>";
            ss << "<p>X = " << equation_term2 << " - " << equation_term1 << " = " << equation_x << "</p>";
        }
        else {
            ss << "<p>X - " << equation_term1 << " = " << equation_term2 << "</p>";
            ss << "<p>X = " << equation_term2 << " + " << equation_term1 << " = " << equation_x << "</p>";
        }

        ss << "<p>3. Переводим ответ обратно в " << equation_base << "-ю систему:</p>";
        ss << "<p>" << equation_x << "<sub>10</sub> = <strong>" << to_base(equation_x, equation_base)
            << "</strong><sub>" << equation_base << "</sub></p>";
        ss << "</div>";

        solution = ss.str();
        correct_answer = to_base(equation_x, equation_base);
    }

    // Генерация арифметики в одинаковой системе счисления
    void NumSystemProblem::generate_arithmetic_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        std::uniform_int_distribution<> dis_base(2, 9); // Исключаем 10
        std::uniform_int_distribution<> dis_op(0, 2); // 0-сложение, 1-вычитание, 2-умножение
        std::uniform_int_distribution<> dis_val(5, 50);

        arithmetic_base = dis_base(gen);

        int val1 = dis_val(gen);
        int val2 = dis_val(gen);

        if (arithmetic_base <= 2) {
            val1 %= 8;
            val2 %= 8;
        }

        arithmetic_op1 = val1;
        arithmetic_op2 = val2;
        arithmetic_operation = dis_op(gen);

        switch (arithmetic_operation) {
        case 0:
            arithmetic_operation = '+';
            arithmetic_result = val1 + val2;
            break;
        case 1:
            arithmetic_operation = '-';
            // Чтобы не было отрицательных
            if (val1 < val2) std::swap(val1, val2);
            arithmetic_op1 = val1;
            arithmetic_op2 = val2;
            arithmetic_result = val1 - val2;
            break;
        case 2:
            arithmetic_operation = '*';
            arithmetic_result = val1 * val2;
            break;
        }

        arithmetic_op1_str = to_base(arithmetic_op1, arithmetic_base);
        arithmetic_op2_str = to_base(arithmetic_op2, arithmetic_base);
        arithmetic_result_str = to_base(arithmetic_result, arithmetic_base);

        std::stringstream ss;
        ss << "<div class='num-system-arithmetic'>";
        ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        ss << "<p class='problem-text'>Выполните действие в системе счисления с основанием <strong>"
            << arithmetic_base << "</strong>:</p>";
        ss << "<p class='arithmetic-example'><strong>"
            << arithmetic_op1_str << "<sub>" << arithmetic_base << "</sub> "
            << arithmetic_operation << " "
            << arithmetic_op2_str << "<sub>" << arithmetic_base << "</sub> = ?</strong></p>";
        ss << "<p class='score'>Баллов: " << score << "</p>";
        ss << "</div>";

        problem_text = ss.str();

        // Решение
        ss.str("");
        ss << "<div class='solution'>";
        ss << "<h4>Решение:</h4>";
        ss << "<p>1. Переведем числа в десятичную систему для проверки:</p>";
        ss << "<p>" << arithmetic_op1_str << "<sub>" << arithmetic_base << "</sub> = "
            << arithmetic_op1 << "<sub>10</sub></p>";
        ss << "<p>" << arithmetic_op2_str << "<sub>" << arithmetic_base << "</sub> = "
            << arithmetic_op2 << "<sub>10</sub></p>";
        ss << "<p>2. Выполняем действие в десятичной системе:</p>";
        ss << "<p>" << arithmetic_op1 << " " << arithmetic_operation << " " << arithmetic_op2
            << " = " << arithmetic_result << "<sub>10</sub></p>";
        ss << "<p>3. Переводим результат обратно в " << arithmetic_base << "-ю систему:</p>";
        ss << "<p>" << arithmetic_result << "<sub>10</sub> = <strong>"
            << arithmetic_result_str << "</strong><sub>" << arithmetic_base << "</sub></p>";
        ss << "</div>";

        solution = ss.str();
        correct_answer = arithmetic_result_str;
    }

    // Генерация задачи на поиск монотонного ряда
    void NumSystemProblem::generate_monotonic_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        sequences.clear();

        std::uniform_int_distribution<> dis_base(2, 12);
        std::uniform_int_distribution<> dis_seq_length(4, 6);

        // Генерируем 5 рядов
        for (int seq = 0; seq < 5; seq++) {
            std::vector<std::pair<std::string, int>> sequence;
            int base = dis_base(gen);
            int length = dis_seq_length(gen);

            bool increasing = (gen() % 2 == 0);

            // Генерируем монотонный ряд
            int current = 10 + (gen() % 50);
            std::uniform_int_distribution<> dis_step(3, 15);

            for (int i = 0; i < length; i++) {
                sequence.push_back({ to_base(current, base), base });

                if (increasing) {
                    current += dis_step(gen);
                }
                else {
                    current -= dis_step(gen);
                    if (current < 1) current = 1;
                }
            }

            sequences.push_back(sequence);
        }

        // Делаем один ряд немонотонным
        int non_monotonic_index = gen() % 5;
        auto& non_mono_seq = sequences[non_monotonic_index];
        int swap_pos1 = gen() % non_mono_seq.size();
        int swap_pos2 = gen() % non_mono_seq.size();
        if (swap_pos1 != swap_pos2) {
            std::swap(non_mono_seq[swap_pos1], non_mono_seq[swap_pos2]);
        }

        // Создаем монотонный ряд (остальные делаем монотонными)
        monotonic_sequence_index = gen() % 5;
        // Убеждаемся, что выбранный ряд монотонный
        int base = dis_base(gen);
        int length = dis_seq_length(gen);
        bool increasing = (gen() % 2 == 0);

        std::vector<std::pair<std::string, int>> monotonic_seq;
        int current = 10 + (gen() % 50);
        std::uniform_int_distribution<> dis_step(3, 15);

        for (int i = 0; i < length; i++) {
            monotonic_seq.push_back({ to_base(current, base), base });
            if (increasing) {
                current += dis_step(gen);
            }
            else {
                current -= dis_step(gen);
            }
        }

        sequences[monotonic_sequence_index] = monotonic_seq;
        monotonic_increasing = increasing;

        std::stringstream ss;
        ss << "<div class='num-system-monotonic'>";
        ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        ss << "<p class='problem-text'>Даньі 5 рядов чисел в различных системах счисления. ";
        ss << "Определите номер ряда, который является <strong>МОНОТОННЫМ</strong> ";
        ss << "(строго возрастающим или строго убывающим).</p>";

        ss << "<div class='sequences-list'>";
        for (size_t i = 0; i < sequences.size(); i++) {
            ss << "<p><strong>Ряд " << (i + 1) << ":</strong> ";
            for (size_t j = 0; j < sequences[i].size(); j++) {
                ss << sequences[i][j].first << "<sub>" << sequences[i][j].second << "</sub>";
                if (j < sequences[i].size() - 1) ss << ", ";
            }
            ss << "</p>";
        }
        ss << "</div>";

        ss << "<p class='score'>Баллов: " << score << "</p>";
        ss << "</div>";

        problem_text = ss.str();

        // Решение
        ss.str("");
        ss << "<div class='solution'>";
        ss << "<h4>Решение:</h4>";
        ss << "<p>Переведем все числа в десятичную систему и проверим монотонность:</p>";

        for (size_t i = 0; i < sequences.size(); i++) {
            ss << "<p><strong>Ряд " << (i + 1) << ":</strong> ";
            std::vector<int> dec_values;
            for (const auto& num : sequences[i]) {
                int val = from_base(num.first, num.second);
                dec_values.push_back(val);
                ss << val << " ";
            }

            bool is_increasing = true;
            bool is_decreasing = true;
            for (size_t j = 1; j < dec_values.size(); j++) {
                if (dec_values[j] <= dec_values[j - 1]) is_increasing = false;
                if (dec_values[j] >= dec_values[j - 1]) is_decreasing = false;
            }

            if (is_increasing) {
                ss << "- строго возрастающий";
            }
            else if (is_decreasing) {
                ss << "- строго убывающий";
            }
            else {
                ss << "- не монотонный";
            }
            ss << "</p>";
        }

        ss << "<p><strong>Ответ:</strong> Ряд " << (monotonic_sequence_index + 1) << "</p>";
        ss << "</div>";

        solution = ss.str();
        correct_answer = std::to_string(monotonic_sequence_index + 1);
    }

    // Основной метод генерации задачи
    void NumSystemProblem::generate(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        // Получаем тип задачи
        std::string task_type_str = config.get_string_param("task_type", "conversion");

        if (task_type_str == "comparison") {
            task_type = NumSystemTaskType::COMPARISON;
        }
        else if (task_type_str == "equation") {
            task_type = NumSystemTaskType::EQUATION;
        }
        else if (task_type_str == "arithmetic") {
            task_type = NumSystemTaskType::ARITHMETIC;
        }
        else if (task_type_str == "monotonic") {
            task_type = NumSystemTaskType::MONOTONIC_SEQUENCE;
        }
        else {
            task_type = NumSystemTaskType::CONVERSION;
        }

        // Настройка сложности
        switch (config.difficulty) {
        case Difficulty::EASY:
            min_base = 2;
            max_base = 8;
            min_value = 10;
            max_value = 100;
            score = 10;
            
            break;
        case Difficulty::MEDIUM:
            min_base = 2;
            max_base = 16;
            min_value = 1000;
            max_value = 10000;
            score = 15;
            break;
        case Difficulty::HARD:
            min_base = 2;
            max_base = 36;
            min_value = 10000;
            max_value = 100000;
            score = 20;
            break;
        }

        // Генерируем задачу в зависимости от типа
        switch (task_type) {
        case NumSystemTaskType::COMPARISON:
            generate_comparison_task(config);
            break;
        case NumSystemTaskType::EQUATION:
            generate_equation_task(config);
            break;
        case NumSystemTaskType::ARITHMETIC:
            generate_arithmetic_task(config);
            break;
        case NumSystemTaskType::MONOTONIC_SEQUENCE:
            generate_monotonic_task(config);
            break;
        case NumSystemTaskType::CONVERSION:
        default:
            // Старая логика конвертации
            std::string dir_str = config.get_string_param("conversion_direction", "to_decimal");
            if (dir_str == "from_decimal") {
                direction = ConversionDirection::FROM_DECIMAL;
            }
            else {
                direction = ConversionDirection::TO_DECIMAL;
            }


            std::uniform_int_distribution<> dis_base(min_base, max_base);
            source_base = dis_base(gen);
            target_base = 10;

            if (direction == ConversionDirection::TO_DECIMAL) {
                std::uniform_int_distribution<> dis_length(2, 4);
                int number_length = dis_length(gen);
                source_number = generate_digit_sequence(number_length, source_base);
                target_number = std::to_string(decimal_value);
                correct_answer = target_number;

                std::stringstream ss;
                ss << "<div class='num-system-problem'>";
                ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
                ss << "<p class='problem-text'>Переведите число <strong>" << source_number
                    << "</strong><sub>" << source_base << "</sub> в десятичную систему счисления.</p>";
                ss << "<p class='score'>Баллов: " << score << "</p>";
                ss << "</div>";
                problem_text = ss.str();

                ss.str("");
                ss << "<div class='solution'>";
                ss << "<h4>Решение:</h4>";
                ss << "<p>Для перевода числа из " << source_base << "-й системы счисления в десятичную:</p>";
                ss << "<p><strong>" << source_number << "</strong><sub>" << source_base << "</sub> = ";

                for (size_t i = 0; i < source_number.length(); i++) {
                    int digit = get_digit_value(source_number[i]);
                    int power = source_number.length() - 1 - i;
                    ss << digit << " * " << source_base << "<sup>" << power << "</sup>";
                    if (i < source_number.length() - 1) ss << " + ";
                }
                ss << " = " << decimal_value << "<sub>10</sub></p>";
                ss << "</div>";
                solution = ss.str();
            }
            else {
                std::uniform_int_distribution<> dis_target_base(2, max_base);
                target_base = dis_target_base(gen);

                decimal_value = stoi(generate_random_number(10, min_value, max_value));
                source_number = std::to_string(decimal_value);
                target_number = to_base(decimal_value, target_base);
                correct_answer = target_number;

                std::stringstream ss;
                ss << "<div class='num-system-problem'>";
                ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
                ss << "<p class='problem-text'>Переведите число <strong>" << decimal_value
                    << "</strong><sub>10</sub> в систему счисления с основанием <strong>"
                    << target_base << "</strong>.</p>";
                ss << "<p class='score'>Баллов: " << score << "</p>";
                ss << "</div>";
                problem_text = ss.str();

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
            break;
        }

        // Подсказка
        std::stringstream ss;
        ss << "<div class='hint'>";
        ss << "<h4>Подсказка:</h4>";
        ss << "<p>";

        switch (task_type) {
        case NumSystemTaskType::CONVERSION:
            if (direction == ConversionDirection::TO_DECIMAL) {
                ss << "Каждая цифра умножается на основание в степени позиции (справа налево, начиная с 0)";
            }
            else {
                ss << "Последовательно делите число на основание, остатки записывайте справа налево";
            }
            break;
        case NumSystemTaskType::COMPARISON:
            ss << "Переведите все числа в десятичную систему и сравните их значения";
            break;
        case NumSystemTaskType::EQUATION:
            ss << "Переведите числа в десятичную систему, решите уравнение, затем переведите ответ обратно";
            break;
        case NumSystemTaskType::ARITHMETIC:
            ss << "Можно перевести в десятичную систему, выполнить действие и перевести обратно";
            break;
        case NumSystemTaskType::MONOTONIC_SEQUENCE:
            ss << "Переведите числа в десятичную систему и проверьте, строго возрастают они или строго убывают";
            break;
        }

        ss << "</p>";
        if (source_base > 10 || target_base > 10 || task_type != NumSystemTaskType::CONVERSION) {
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