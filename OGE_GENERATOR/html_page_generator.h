// html_page_generator.h - простой генератор HTML (только текст)
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
        string output_file = "задачи.html";
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

        // Экранирование HTML
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

        // Генерация ТОЛЬКО текста задачи (без кнопок и CSS)
        string generate_plain_problem_text(const string& problem_text,
            const string& solution_text,
            const string& correct_answer,
            int problem_number) {
            stringstream html;

            html << "<h2>Задача #" << problem_number << "</h2>\n";
            html << "<p>" << nl2br(escape_html(problem_text)) << "</p>\n";
            html << "<h3>Решение:</h3>\n";
            html << "<p>" << nl2br(escape_html(solution_text)) << "</p>\n";
            html << "<p><b>Ответ: " << escape_html(correct_answer) << "</b></p>\n";
            html << "<hr>\n";

            return html.str();
        }

        // Генерация полной страницы (простой текст)
        string generate_plain_page(const vector<map<string, string>>& problems) {
            stringstream html;

            html << "<!DOCTYPE html>\n";
            html << "<html lang=\"ru\">\n";
            html << "<head>\n";
            html << "    <meta charset=\"UTF-8\">\n";
            html << "    <title>" << config.title << "</title>\n";
            html << "</head>\n";
            html << "<body>\n";

            // Заголовок
            html << "    <h1>" << config.title << "</h1>\n";
            html << "    <p>Всего задач: " << problems.size() << "</p>\n";
            html << "    <hr>\n";

            // Задачи
            for (size_t i = 0; i < problems.size(); i++) {
                const auto& problem = problems[i];

                string problem_text = problem.count("problem_text") ? problem.at("problem_text") : "";
                string solution_text = problem.count("solution_explanation") ? problem.at("solution_explanation") : "";
                string correct_answer = problem.count("correct_answer") ? problem.at("correct_answer") : "";

                html << generate_plain_problem_text(problem_text, solution_text,
                    correct_answer, i + 1);
            }

            // Подвал
            html << "    <p><i>Сгенерировано: " << format_time(config.generation_time) << "</i></p>\n";
            html << "</body>\n";
            html << "</html>\n";

            return html.str();
        }

        // Альтернативный метод - проще
        string generate_simple_page(const vector<map<string, string>>& problems) {
            stringstream html;

            html << "<!DOCTYPE html>\n<html lang=\"ru\">\n<head>\n";
            html << "<meta charset=\"UTF-8\">\n";
            html << "<title>" << config.title << "</title>\n";
            html << "</head>\n<body>\n";
            html << "<h1>" << config.title << "</h1>\n";

            for (size_t i = 0; i < problems.size(); i++) {
                const auto& problem = problems[i];

                html << "<h2>Задача #" << (i + 1) << "</h2>\n";

                // Условие
                if (problem.count("problem_text")) {
                    string text = problem.at("problem_text");
                    html << "<p>" << nl2br(escape_html(text)) << "</p>\n";
                }

                // Ответ
                if (problem.count("correct_answer")) {
                    html << "<p><b>Ответ:</b> " << escape_html(problem.at("correct_answer")) << "</p>\n";
                }

                html << "<hr>\n";
            }

            html << "</body>\n</html>\n";
            return html.str();
        }

        // Сохранение в файл
        bool save_to_file(const string& content, const string& filename = "") {
            string file = filename.empty() ? config.output_file : filename;

            ofstream out(file);
            if (!out.is_open()) {
                return false;
            }

            out << content;
            out.close();
            return true;
        }

        // Генерация и сохранение
        bool generate_and_save_plain(const vector<map<string, string>>& problems) {
            string html = generate_plain_page(problems);
            return save_to_file(html);
        }
    };

} // namespace OGE

#endif // HTML_PAGE_GENERATOR_H