// math_problem.cpp
#include "math_problem.h"

namespace OGE {
    std::mt19937 MathProblem::gen(std::random_device{}());
    OGE::PageSettings OGE::MathProblem::page_settings;

    MathProblem::MathProblem(const ProblemMeta& meta)
        : ProblemBase(meta), DatabaseManager(), operand1(0), operand2(0), operation('+') {
        open_or_create();
        create_stats_table();
    }

    MathProblem::MathProblem(const ProblemMeta& meta, const std::string& db_path)
        : ProblemBase(meta), DatabaseManager(db_path), operand1(0), operand2(0), operation('+') {
        open_or_create();
        create_stats_table();
    }

    MathProblem::~MathProblem() {
        close();
    }

    void MathProblem::generate(const GenerationConfig& config) {

        int max_num = 20;
        switch (config.difficulty) {
        case Difficulty::EASY: max_num = 10; score = 5; break;
        case Difficulty::MEDIUM: max_num = 50; score = 10; break;
        case Difficulty::HARD: max_num = 100; score = 15; break;
        default: max_num = 30; score = 10; break;
        }

        std::uniform_int_distribution<> dis_num(1, max_num);
        std::uniform_int_distribution<> dis_op(0, 3);

        operand1 = dis_num(gen);
        operand2 = dis_num(gen);

        int op = dis_op(gen);
        switch (op) {
        case 0: operation = '+';
            correct_answer = std::to_string(operand1 + operand2);
            break;
        case 1: operation = '-';
            correct_answer = std::to_string(operand1 - operand2);
            break;
        case 2: operation = '*';
            correct_answer = std::to_string(operand1 * operand2);
            break;
        case 3: operation = '/';
            operand2 = operand2 == 0 ? 1 : operand2;
            operand1 = operand1 - (operand1 % operand2);
            correct_answer = std::to_string(operand1 / operand2);
            break;
        }

        std::stringstream ss;
        ss << "<div class='math-problem'>";
        ss << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        ss << "<p class='problem-text'>Решите пример: <strong>"
            << operand1 << " " << operation << " " << operand2 << " = ?</strong></p>";
        ss << "<p class='score'>Баллов: " << score << "</p>";
        ss << "</div>";

        problem_text = ss.str();

        ss.str("");
        ss << "<div class='solution'>";
        ss << "<h4>Решение:</h4>";
        ss << "<p>" << operand1 << " " << operation << " " << operand2 << " = " << correct_answer << "</p>";
        ss << "</div>";
        solution = ss.str();

        ss.str("");
        ss << "<div class='hint'>";
        ss << "<h4>Подсказка:</h4>";
        ss << "<p>";
        if (operation == '+') ss << "Сложите два числа";
        else if (operation == '-') ss << "Вычтите из первого числа второе";
        else if (operation == '*') ss << "Умножьте числа";
        else if (operation == '/') ss << "Разделите первое число на второе";
        ss << "</p>";
        ss << "</div>";
        hint = ss.str();

        unique_id = generate_unique_id();
        generated_at = std::time(nullptr);

        record_generation(meta.problem_number, "math");
    }

    std::string MathProblem::generate_html() const {
        std::stringstream html;

        html << R"(
        <div class='problem-wrapper' data-problem-id=')" << unique_id << R"('>
            
            )" << problem_text << R"(
            
            <div class='answer-area'>
                <input type='text' id='answer-)" << unique_id << R"(' 
                       placeholder='Ваш ответ' 
                       data-problem-id=')" << unique_id << R"('>
                <button onclick='checkAnswer(")" << unique_id << R"(", ")"
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

    bool MathProblem::check_answer(const std::string& user_answer) const {
        return user_answer == correct_answer;
    }

    bool MathProblem::create_stats_table() {
        std::string sql =
            "CREATE TABLE IF NOT EXISTS math_generation_stats ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "problem_num INTEGER,"
            "task_type TEXT,"
            "generated_at DATETIME DEFAULT CURRENT_TIMESTAMP"
            ");";

        sql +=
            "CREATE TABLE IF NOT EXISTS math_solution_stats ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "problem_num INTEGER,"
            "correct BOOLEAN,"
            "time_seconds REAL,"
            "solved_at DATETIME DEFAULT CURRENT_TIMESTAMP"
            ");";

        return execute_sql(sql);
    }

    std::string MathProblem::get_table_name() {
        return "math_problems";
    }

    void MathProblem::record_generation(int problem_num, const std::string& task_type) {
        std::string sql = "INSERT INTO math_generation_stats (problem_num, task_type) VALUES ("
            + std::to_string(problem_num) + ", '" + task_type + "');";
        execute_sql(sql);
    }

    void MathProblem::record_solution(int problem_num, bool correct, double time_sec) {
        std::string sql = "INSERT INTO math_solution_stats (problem_num, correct, time_seconds) VALUES ("
            + std::to_string(problem_num) + ", "
            + (correct ? "1" : "0") + ", "
            + std::to_string(time_sec) + ");";
        execute_sql(sql);
    }

    std::unordered_map<std::string, int> MathProblem::get_generation_stats() const {
        std::unordered_map<std::string, int> stats;
        auto result = query("SELECT task_type, COUNT(*) FROM math_generation_stats GROUP BY task_type;");

        for (const auto& row : result) {
            if (row.size() >= 2) {
                stats[row[0]] = std::stoi(row[1]);
            }
        }
        return stats;
    }

    int MathProblem::get_total_generated() const {
        auto result = query("SELECT COUNT(*) FROM math_generation_stats;");
        if (!result.empty() && !result[0].empty()) {
            return std::stoi(result[0][0]);
        }
        return 0;
    }

    void MathProblem::set_page_settings(const PageSettings& settings) {
        page_settings = settings;
    }

    const PageSettings& MathProblem::get_page_settings() const {
        return page_settings;
    }

}