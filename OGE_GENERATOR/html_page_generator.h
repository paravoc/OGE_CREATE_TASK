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

            // Для ANSI НЕ используем charset=UTF-8, вместо этого windows-1251 или ничего
            html << "<!DOCTYPE html>\n";
            html << "<html lang=\"ru\">\n";
            html << "<head>\n";
            html << "    <meta charset=\"windows-1251\">\n";  // ИЛИ просто убрать эту строку
            html << "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\n";
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

                html << "    <h2>Задача #" << (i + 1) << "</h2>\n";
                html << "    <p>" << nl2br(escape_html(problem_text)) << "</p>\n";
                html << "    <p><b>Ответ:</b> " << escape_html(correct_answer) << "</p>\n";
                html << "    <hr>\n";
            }

            // Подвал
            html << "    <p><i>Сгенерировано: " << format_time(config.generation_time) << "</i></p>\n";
            html << "</body>\n";
            html << "</html>\n";

            return html.str();
        }

        // Еще более простая версия - без указания кодировки вообще
        string generate_simple_ansi_page(const vector<map<string, string>>& problems) {
            stringstream html;

            html << "<!DOCTYPE html>\n";
            html << "<html>\n";
            html << "<head>\n";
            html << "    <title>" << config.title << "</title>\n";
            // НИКАКОГО charset - браузер сам определит как ANSI
            html << "</head>\n";
            html << "<body>\n";

            html << "    <h1>" << config.title << "</h1>\n";
            html << "    <p>Всего задач: " << problems.size() << "</p>\n";
            html << "    <hr>\n";

            for (size_t i = 0; i < problems.size(); i++) {
                const auto& problem = problems[i];

                html << "    <h2>Задача #" << (i + 1) << "</h2>\n";

                string problem_text = problem.count("problem_text") ? problem.at("problem_text") : "";
                html << "    <p>" << nl2br(escape_html(problem_text)) << "</p>\n";

                if (problem.count("correct_answer")) {
                    html << "    <p><b>Ответ:</b> " << problem.at("correct_answer") << "</p>\n";
                }

                html << "    <hr>\n";
            }

            html << "</body>\n";
            html << "</html>\n";

            return html.str();
        }

        // Генерация для ANSI без BOM
        string generate_for_ansi(const vector<map<string, string>>& problems) {
            stringstream html;

            // Самый простой HTML без указания кодировки
            html << "<html>\n<head>\n<title>" << config.title << "</title>\n</head>\n<body>\n";
            html << "<h1>" << config.title << "</h1>\n";

            for (size_t i = 0; i < problems.size(); i++) {
                const auto& problem = problems[i];

                html << "<h2>Задача #" << (i + 1) << "</h2>\n";

                // ИСПРАВЛЕНО: добавлены скобки вокруг тернарного оператора
                html << "<p>" << (problem.count("problem_text") ? nl2br(escape_html(problem.at("problem_text"))) : "") << "</p>\n";

                // ИСПРАВЛЕНО: добавлены скобки
                html << "<p><b>Ответ:</b> " << (problem.count("correct_answer") ? escape_html(problem.at("correct_answer")) : "") << "</p>\n";

                html << "<hr>\n";
            }

            html << "</body>\n</html>\n";
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

        // Сохранение специально для ANSI
        bool save_ansi_to_file(const string& content, const string& filename = "") {
            string file = filename.empty() ? config.output_file : filename;

            // Обычное сохранение, НЕ бинарное, НЕ BOM
            ofstream out(file);  // НЕ ios::binary
            if (!out.is_open()) {
                return false;
            }

            out << content;
            out.close();
            return true;
        }

        // Генерация и сохранение в ANSI
        bool generate_and_save_ansi(const vector<map<string, string>>& problems) {
            string html = generate_simple_ansi_page(problems);  // или generate_for_ansi
            return save_ansi_to_file(html);
        }
    };

} // namespace OGE

#endif // HTML_PAGE_GENERATOR_H