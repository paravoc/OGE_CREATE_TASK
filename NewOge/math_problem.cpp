// math_problem.cpp
#include "math_problem.h"

namespace OGE {
    std::mt19937 MathProblem::gen(std::random_device{}());
    OGE::PageSettings OGE::MathProblem::page_settings;

    MathProblem::MathProblem(const ProblemMeta& meta)
        : ProblemBase(meta),  operand1(0), operand2(0), operation('+') {

    }

    MathProblem::MathProblem(const ProblemMeta& meta, const std::string& db_path)
        : ProblemBase(meta), operand1(0), operand2(0), operation('+') {

    }

    MathProblem::~MathProblem() {

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

        //record_generation(meta.problem_number, "math");
    }

    std::string MathProblem::generate_html() const {
    std::string html;
    html.reserve(1024);  // Предварительное выделение памяти
    
    // Ручная конкатенация без stringstream
    html += "<div class='problem-wrapper' data-problem-id='";
    html += unique_id;
    html += "'>\n    ";
    html += problem_text;
    html += "\n    <div class='answer-area'>\n        <input type='text' id='answer-";
    html += unique_id;
    html += "' placeholder='Ваш ответ' data-problem-id='";
    html += unique_id;
    html += "'>\n        <button onclick='checkAnswer(\"";
    html += unique_id;
    html += "\", \"";
    html += correct_answer;
    html += "\", ";
    html += std::to_string(score);
    html += ")'>\n            Проверить\n        </button>\n    </div>\n";
    
    if (page_settings.can_view_solutions()) {
        html += "    <div class='solution-wrapper'>\n        ";
        html += solution;
        html += "\n    </div>\n";
    }
    
    if (page_settings.show_hints) {
        html += "    <div class='hint-wrapper'>\n        ";
        html += hint;
        html += "\n    </div>\n";
    }
    
    html += "</div>";
    
    return html;
}

    bool MathProblem::check_answer(const std::string& user_answer) const {
        return user_answer == correct_answer;
    }

    

    void MathProblem::set_page_settings(const PageSettings& settings) {
        page_settings = settings;
    }

    const PageSettings& MathProblem::get_page_settings() const {
        return page_settings;
    }

}