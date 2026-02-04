#include "html_page_generator.h"
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace OGE;

// Конструктор с инициализацией времени генерации
HtmlPageGenerator::HtmlPageGenerator(const PageConfig& cfg) : config(cfg) {
    if (config.generation_time == 0) {
        config.generation_time = time(nullptr);
    }
}

// Форматирование времени в читаемый вид
string HtmlPageGenerator::format_time(time_t t) const {
    struct tm time_info;
    if (localtime_s(&time_info, &t) != 0) {
        return "Ошибка времени";
    }
    char buf[100];
    strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M", &time_info);
    return string(buf);
}

// Замена переносов строк на HTML-тег <br>
string HtmlPageGenerator::nl2br(const string& str) {
    string result;
    result.reserve(str.length() * 2);

    for (char c : str) {
        if (c == '\n') {
            result += "<br>";
        }
        else {
            result += c;
        }
    }

    return result;
}

string HtmlPageGenerator::generate_ansi_page(const vector<map<string, string>>& problems) {
    stringstream html;
    string page_id = to_string(time(nullptr));


    html << R"(<!DOCTYPE html>
<html lang="ru">
<head>
    <title>)" << config.title << R"(</title>
    <link rel="stylesheet" href="web/style.css">
</head>
<body data-page-id=")" << page_id << R"(">
    <h1>)" << config.title << R"(</h1>
    <div class="info-bar">
        <div class="stats">Всего задач: <span>)" << problems.size() << R"(</span></div>
        <div class="timestamp">Сгенерировано: )" << format_time(config.generation_time) << R"(</div>
    </div>
    <div class="progress-container">
        <div class="progress-bar"></div>
    </div>
    <hr>)";

    // Генерация блока для каждой задачи
    for (size_t i = 0; i < problems.size(); i++) {
        const auto& problem = problems[i];

        string problem_text = problem.count("problem_text") ? problem.at("problem_text") : "";
        string solution_text = problem.count("solution_explanation") ? problem.at("solution_explanation") : "";
        string correct_answer = problem.count("correct_answer") ? problem.at("correct_answer") : "";

        html << R"(
    <div class="problem-container" data-problem-index=")" << i << R"(">
        <h2>Задача #)" << (i + 1) << R"(</h2>
        <div class="problem-text">)" << nl2br(problem_text) << R"(</div>
        <div class="answer-section">
            <label class="answer-label">Ваш ответ:</label>
            <input type="text" class="answer-input" placeholder="Введите ваш ответ здесь...">
            <div class="button-group">
                <button class="check-btn">Проверить</button>
                <button class="reset-btn">Сбросить</button>
                <button class="show-answer-btn">Показать ответ</button>
            </div>
            <div class="result-message"></div>
        </div>
        <div class="correct-answer">
            <span class="correct-answer-label">Правильный ответ:</span> )" << correct_answer << R"(
            <br>
            <span class="correct-answer-label">Решение: </span>
            )" << solution_text <<R"(
        </div>
    </div>)";
    }

    html << R"(
    <script src="web/scripts.js"></script>
</body>
</html>)";

    return html.str();
}

bool HtmlPageGenerator::save_to_file(const string& content, const string& filename) {
    string file = filename.empty() ? config.output_file : filename;
    ofstream out(file);

    if (!out.is_open()) {
        return false;
    }

    out << content;
    out.close();
    return true;
}

bool HtmlPageGenerator::generate_and_save_ansi(const vector<map<string, string>>& problems) {
    string html = generate_ansi_page(problems);
    return save_to_file(html);
}