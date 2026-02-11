// decimal_problem.cpp
#include "decimal_problem.h"
#include <cmath>

namespace OGE {

    // Конструкторы
    DecimalProblem::DecimalProblem(const ProblemMeta& meta)
        : ProblemBase(meta), DatabaseManager(),
        operand1(0.0), operand2(0.0), operation('+'), decimal_places(1) {
        open_or_create();
        create_stats_table();
    }

    DecimalProblem::DecimalProblem(const ProblemMeta& meta, const std::string& db_path)
        : ProblemBase(meta), DatabaseManager(db_path),
        operand1(0.0), operand2(0.0), operation('+'), decimal_places(1) {
        open_or_create();
        create_stats_table();
    }

    DecimalProblem::~DecimalProblem() {
        close();
    }

    // Генерация задачи с десятичными дробями
    void DecimalProblem::generate(const GenerationConfig& config) {
        std::random_device rd;
        std::mt19937 gen(rd());

        // Настройка сложности
        int max_num;
        switch (config.difficulty) {
        case Difficulty::EASY:
            max_num = 10;
            decimal_places = 1;
            score = 10;
            break;
        case Difficulty::MEDIUM:
            max_num = 50;
            decimal_places = 2;
            score = 15;
            break;
        case Difficulty::HARD:
            max_num = 100;
            decimal_places = 3;
            score = 20;
            break;
        default:
            max_num = 30;
            decimal_places = 2;
            score = 15;
        }

        std::uniform_int_distribution<> dis_num(1, max_num);
        std::uniform_int_distribution<> dis_dec(0, 9);
        std::uniform_int_distribution<> dis_op(0, 3);

        // Генерация десятичных чисел
        int int_part1 = dis_num(gen);
        int dec_part1 = dis_dec(gen);
        operand1 = int_part1 + static_cast<double>(dec_part1) / 10.0;

        int int_part2 = dis_num(gen);
        int dec_part2 = dis_dec(gen);
        operand2 = int_part2 + static_cast<double>(dec_part2) / 10.0;

        // Округление до нужного количества знаков
        double multiplier = std::pow(10.0, decimal_places);
        operand1 = std::round(operand1 * multiplier) / multiplier;
        operand2 = std::round(operand2 * multiplier) / multiplier;

        int op = dis_op(gen);
        std::stringstream ss;
        ss << std::fixed << std::setprecision(decimal_places);

        switch (op) {
        case 0:
            operation = '+';
            correct_answer = std::to_string(operand1 + operand2);
            break;
        case 1:
            operation = '-';
            correct_answer = std::to_string(operand1 - operand2);
            break;
        case 2:
            operation = '*';
            correct_answer = std::to_string(operand1 * operand2);
            break;
        case 3:
            operation = '/';
            if (operand2 == 0) operand2 = 0.1;
            correct_answer = std::to_string(operand1 / operand2);
            break;
        }

        // Округление ответа
        double answer = std::stod(correct_answer);
        answer = std::round(answer * multiplier) / multiplier;
        correct_answer = std::to_string(answer);
        correct_answer = correct_answer.substr(0, correct_answer.find('.') + decimal_places + 1);

        // Текст задачи
        ss.str("");
        ss << "<div class='decimal-problem'>";
        ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        ss << "<p class='problem-text'>Решите пример с десятичными дробями: <br>";
        ss << "<strong>" << std::fixed << std::setprecision(decimal_places)
            << operand1 << " " << operation << " " << operand2 << " = ?</strong></p>";
        ss << "<p class='score'>Баллов: " << score << "</p>";
        ss << "<p class='decimal-hint'>Ответ округлите до " << decimal_places << " знаков после запятой</p>";
        ss << "</div>";

        problem_text = ss.str();

        // Решение
        ss.str("");
        ss << "<div class='solution'>";
        ss << "<h4>Решение:</h4>";
        ss << "<p>" << std::fixed << std::setprecision(decimal_places)
            << operand1 << " " << operation << " " << operand2 << " = " << correct_answer << "</p>";
        ss << "</div>";
        solution = ss.str();

        // Подсказка
        ss.str("");
        ss << "<div class='hint'>";
        ss << "<h4>Подсказка:</h4><p>";
        if (operation == '+') ss << "Сложите числа, сохраняя десятичные разряды";
        else if (operation == '-') ss << "Вычтите числа, сохраняя десятичные разряды";
        else if (operation == '*') ss << "Умножьте числа и округлите результат";
        else if (operation == '/') ss << "Разделите числа и округлите результат";
        ss << "</p></div>";
        hint = ss.str();

        unique_id = generate_unique_id();
        generated_at = std::time(nullptr);

        record_generation(meta.problem_number, "decimal");
    }

    // DatabaseManager implementation
    bool DecimalProblem::create_stats_table() {
        std::string sql =
            "CREATE TABLE IF NOT EXISTS decimal_generation_stats ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "problem_num INTEGER,"
            "task_type TEXT,"
            "decimal_places INTEGER,"
            "generated_at DATETIME DEFAULT CURRENT_TIMESTAMP"
            ");";

        sql +=
            "CREATE TABLE IF NOT EXISTS decimal_solution_stats ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "problem_num INTEGER,"
            "correct BOOLEAN,"
            "time_seconds REAL,"
            "user_answer TEXT,"
            "solved_at DATETIME DEFAULT CURRENT_TIMESTAMP"
            ");";

        return execute_sql(sql);
    }

    std::string DecimalProblem::get_table_name() {
        return "decimal_problems";
    }

    void DecimalProblem::record_generation(int problem_num, const std::string& task_type) {
        std::string sql = "INSERT INTO decimal_generation_stats (problem_num, task_type, decimal_places) VALUES ("
            + std::to_string(problem_num) + ", '"
            + task_type + "', "
            + std::to_string(decimal_places) + ");";
        execute_sql(sql);
    }

    void DecimalProblem::record_solution(int problem_num, bool correct, double time_sec) {
        std::string sql = "INSERT INTO decimal_solution_stats (problem_num, correct, time_seconds) VALUES ("
            + std::to_string(problem_num) + ", "
            + (correct ? "1" : "0") + ", "
            + std::to_string(time_sec) + ");";
        execute_sql(sql);
    }

    std::unordered_map<std::string, int> DecimalProblem::get_generation_stats() const {
        std::unordered_map<std::string, int> stats;
        auto result = query("SELECT task_type, COUNT(*) FROM decimal_generation_stats GROUP BY task_type;");

        for (const auto& row : result) {
            if (row.size() >= 2) {
                stats[row[0]] = std::stoi(row[1]);
            }
        }
        return stats;
    }

    int DecimalProblem::get_total_generated() const {
        auto result = query("SELECT COUNT(*) FROM decimal_generation_stats;");
        if (!result.empty() && !result[0].empty()) {
            return std::stoi(result[0][0]);
        }
        return 0;
    }

    // PageSettings methods
    void DecimalProblem::set_page_settings(const PageSettings& settings) {
        page_settings = settings;
    }

    const PageSettings& DecimalProblem::get_page_settings() const {
        return page_settings;
    }

    std::string DecimalProblem::generate_html() const {
        std::stringstream html;

        html << R"(
        <div class='problem-wrapper decimal-wrapper' data-problem-id=')" << unique_id << R"('>
            
            )" << problem_text << R"(
            
            <div class='answer-area'>
                <input type='text' id='answer-)" << unique_id << R"(' 
                       placeholder='Введите десятичную дробь' 
                       data-problem-id=')" << unique_id << R"('>
                <button onclick='checkDecimalAnswer(")" << unique_id << R"(", ")"
            << correct_answer << R"(", )" << score << R"()'>
                    Проверить
                </button>
            </div>
        )";

        if (page_settings.can_view_solutions()) {
            html << R"(
            <div class='solution-wrapper'>
                )" << solution << R"(
            </div>
            )";
        }

        if (page_settings.show_hints) {
            html << R"(
            <div class='hint-wrapper'>
                )" << hint << R"(
            </div>
            )";
        }

        html << R"(
        </div>
        )";

        return html.str();
    }

    bool DecimalProblem::check_answer(const std::string& user_answer) const {
        try {
            double user_val = std::stod(user_answer);
            double correct_val = std::stod(correct_answer);
            double diff = std::abs(user_val - correct_val);
            double tolerance = std::pow(0.1, decimal_places) / 2.0;
            return diff <= tolerance;
        }
        catch (...) {
            return false;
        }
    }

} // namespace OGE