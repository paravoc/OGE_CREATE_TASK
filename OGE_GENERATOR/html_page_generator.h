// html_page_generator.h - генератор HTML для ANSI (Windows-1251)
#pragma once
#ifndef HTML_PAGE_GENERATOR_H
#define HTML_PAGE_GENERATOR_H

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <ctime>

using namespace std;

namespace OGE {

    struct PageConfig {
        string title = "Задачи на кодирование";
        string output_file = "site/задачи.html";
        int total_problems = 0;
        time_t generation_time = 0;
    };

    class HtmlPageGenerator {
    private:
        PageConfig config;

        string format_time(time_t t) const {
            struct tm time_info;
            if (localtime_s(&time_info, &t) != 0) {
                return "Ошибка времени";
            }
            char buf[100];
            strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M", &time_info);
            return string(buf);
        }

        // Экранирование HTML (базовое)
        string escape_html(const string& str) const {
            string result;
            result.reserve(str.length());

            for (char c : str) {
                switch (c) {
                case '&':  result += "&amp;";  break;
                case '<':  result += "&lt;";   break;
                case '>':  result += "&gt;";   break;
                case '"':  result += "&quot;"; break;
                case '\'': result += "&#39;";  break;
                default:   result += c;        break;
                }
            }

            return result;
        }

        // Замена переносов строк на <br>
        string nl2br(const string& str) const {
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

    public:
        HtmlPageGenerator(const PageConfig& cfg = PageConfig()) : config(cfg) {
            if (config.generation_time == 0) {
                config.generation_time = time(nullptr);
            }
        }

        // Генерация страницы в ANSI (Windows-1251)
        string generate_ansi_page(const vector<map<string, string>>& problems) {
            stringstream html;

            string page_id = to_string(time(nullptr));

            html << "<!DOCTYPE html>\n";
            html << "<html lang=\"ru\">\n";
            html << "<head>\n";
            html << "    <title>" << config.title << "</title>\n";
            html << "    <link rel=\"stylesheet\" href=\"web/style.css\">\n";
            html << "</head>\n";
            html << "<body data-page-id=\"" << page_id << "\">\n";
            html << "<body>\n";

            // Заголовок и информация
            html << "    <h1>" << config.title << "</h1>\n";
            html << "    <div class=\"info-bar\">\n";
            html << "        <div class=\"stats\">Всего задач: <span>" << problems.size() << "</span></div>\n";
            html << "        <div class=\"timestamp\">Сгенерировано: " << format_time(config.generation_time) << "</div>\n";
            html << "    </div>\n";

            html << "    <div class=\"progress-container\">\n";
            html << "        <div class=\"progress-bar\"></div>\n";
            html << "    </div>\n";
            html << "    <hr>\n";

            // Задачи
            for (size_t i = 0; i < problems.size(); i++) {
                const auto& problem = problems[i];

                string problem_text = problem.count("problem_text") ? problem.at("problem_text") : "";
                string solution_text = problem.count("solution_explanation") ? problem.at("solution_explanation") : "";
                string correct_answer = problem.count("correct_answer") ? problem.at("correct_answer") : "";

                html << "    <div class=\"problem-container\" data-problem-index=\"" << i << "\">\n";
                html << "        <h2>Задача #" << (i + 1) << "</h2>\n";
                html << "        <div class=\"problem-text\">" << nl2br(escape_html(problem_text)) << "</div>\n";

                html << "        <div class=\"answer-section\">\n";
                html << "            <label class=\"answer-label\">Ваш ответ:</label>\n";
                html << "            <input type=\"text\" class=\"answer-input\" placeholder=\"Введите ваш ответ здесь...\">\n";
                html << "            <div class=\"button-group\">\n";
                html << "                <button class=\"check-btn\">Проверить</button>\n";
                html << "                <button class=\"reset-btn\">Сбросить</button>\n";
                html << "                <button class=\"show-answer-btn\">Показать ответ</button>\n";
                html << "            </div>\n";
                html << "            <div class=\"result-message\"></div>\n";
                html << "        </div>\n";

                html << "        <div class=\"correct-answer\">\n";
                html << "            <span class=\"correct-answer-label\">Правильный ответ:</span> " << escape_html(correct_answer) << "\n";
                html << "        </div>\n";

                html << "    </div>\n";
            }

            html << "    <script src=\"web/scripts.js\"></script>\n";
            html << "</body>\n";
            html << "</html>\n";

            return html.str();
        }

        // Сохранение в файл (ANSI по умолчанию)
        bool save_to_file(const string& content, const string& filename = "") {
            string file = filename.empty() ? config.output_file : filename;

            // Просто открываем файл - он будет сохранен в ANSI
            ofstream out(file);
            if (!out.is_open()) {
                return false;
            }

            out << content;
            out.close();
            return true;
        }

    

        // Генерация и сохранение в ANSI
        bool generate_and_save_ansi(const vector<map<string, string>>& problems) {
            string html = generate_ansi_page(problems);  // или generate_for_ansi
            return save_to_file(html);
        }
    };

} // namespace OGE

#endif // HTML_PAGE_GENERATOR_H