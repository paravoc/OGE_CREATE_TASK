// html_page_generator.h - исправленная версия
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
        string title = "Космический Генератор Задач ОГЭ";
        string version = "v1.0.0";
        string output_file = "generated_page.html";
        string css_file = "cosmic.css";
        string js_file = "cosmic.js";
        int total_problems = 0;
        time_t generation_time = 0;
    };

    class HtmlPageGenerator {
    private:
        PageConfig config;

        // Вспомогательные функции
        string escape_js_string(const string& str) const {
            string result;
            result.reserve(str.length());

            for (char c : str) {
                switch (c) {
                case '\\': result += "\\\\"; break;
                case '\"': result += "\\\""; break;
                case '\'': result += "\\'"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
                }
            }

            return result;
        }

        string format_time(time_t t) const {
            struct tm time_info;

            // Используем безопасную версию localtime_s
            if (localtime_s(&time_info, &t) != 0) {
                return "Ошибка времени";
            }

            char buf[100];
            strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", &time_info);
            return string(buf);
        }

    public:
        HtmlPageGenerator(const PageConfig& cfg = PageConfig()) : config(cfg) {
            if (config.generation_time == 0) {
                config.generation_time = time(nullptr);
            }
        }

        // Генерация полной HTML страницы
        string generate_full_page(const vector<string>& problems_html,
            const vector<map<string, string>>& problems_json = {}) {
            stringstream html;

            // Начало документа
            html << "<!DOCTYPE html>\n";
            html << "<html lang=\"ru\">\n";
            html << "<head>\n";
            html << "    <meta charset=\"UTF-8\">\n";
            html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
            html << "    <title>" << config.title << "</title>\n";
            html << "    \n";
            html << "    <!-- Шрифты -->\n";
            html << "    <link href=\"https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700;900&family=Exo+2:wght@300;400;600;700;800&display=swap\" rel=\"stylesheet\">\n";
            html << "    <link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css\">\n";
            html << "    \n";
            html << "    <!-- Стили -->\n";
            html << "    <link rel=\"stylesheet\" href=\"" << config.css_file << "\">\n";
            html << "    \n";
            html << "    <style>\n";
            html << "        .loading-screen {\n";
            html << "            position: fixed;\n";
            html << "            top: 0;\n";
            html << "            left: 0;\n";
            html << "            width: 100%;\n";
            html << "            height: 100%;\n";
            html << "            background: #0a0a0f;\n";
            html << "            display: flex;\n";
            html << "            flex-direction: column;\n";
            html << "            align-items: center;\n";
            html << "            justify-content: center;\n";
            html << "            z-index: 9999;\n";
            html << "            transition: opacity 0.5s ease;\n";
            html << "        }\n";
            html << "        \n";
            html << "        .loading-logo {\n";
            html << "            font-family: 'Orbitron', monospace;\n";
            html << "            font-size: 3rem;\n";
            html << "            margin-bottom: 2rem;\n";
            html << "            background: linear-gradient(45deg, #00ccff, #6d44ff);\n";
            html << "            -webkit-background-clip: text;\n";
            html << "            -webkit-text-fill-color: transparent;\n";
            html << "        }\n";
            html << "        \n";
            html << "        .loading-progress {\n";
            html << "            width: 300px;\n";
            html << "            height: 4px;\n";
            html << "            background: #2d2d5a;\n";
            html << "            border-radius: 2px;\n";
            html << "            overflow: hidden;\n";
            html << "            margin-top: 1rem;\n";
            html << "        }\n";
            html << "        \n";
            html << "        .loading-progress-bar {\n";
            html << "            height: 100%;\n";
            html << "            background: linear-gradient(45deg, #6d44ff, #00ccff);\n";
            html << "            width: 0%;\n";
            html << "            transition: width 0.3s ease;\n";
            html << "        }\n";
            html << "        \n";
            html << "        .generation-info {\n";
            html << "            position: fixed;\n";
            html << "            bottom: 20px;\n";
            html << "            right: 20px;\n";
            html << "            background: rgba(26, 26, 46, 0.8);\n";
            html << "            padding: 0.8rem 1.2rem;\n";
            html << "            border-radius: 10px;\n";
            html << "            font-size: 0.8rem;\n";
            html << "            color: #c7c7ff;\n";
            html << "            border: 1px solid #2d2d5a;\n";
            html << "            z-index: 100;\n";
            html << "        }\n";
            html << "    </style>\n";
            html << "</head>\n";
            html << "<body>\n";
            html << "    <!-- Звездное небо -->\n";
            html << "    <div class=\"stars\"></div>\n";
            html << "    <div class=\"stars2\"></div>\n";
            html << "    <div class=\"stars3\"></div>\n";
            html << "    <div class=\"comet\"></div>\n";
            html << "    <div class=\"nebula\"></div>\n";
            html << "    \n";
            html << "    <!-- Экран загрузки -->\n";
            html << "    <div class=\"loading-screen\" id=\"loadingScreen\">\n";
            html << "        <div class=\"loading-logo\">COSMIC ОГЭ</div>\n";
            html << "        <div class=\"loading-progress\">\n";
            html << "            <div class=\"loading-progress-bar\" id=\"loadingBar\"></div>\n";
            html << "        </div>\n";
            html << "    </div>\n";
            html << "    \n";
            html << "    <!-- Информация о генерации -->\n";
            html << "    <div class=\"generation-info\">\n";
            html << "        <i class=\"fas fa-code\"></i> Сгенерировано: " << format_time(config.generation_time) << "\n";
            html << "    </div>\n";
            html << "    \n";
            html << "    <!-- Основной контейнер -->\n";
            html << "    <main class=\"cosmic-container\" id=\"mainContainer\" style=\"display: none;\">\n";

            // Шапка
            html << generate_header();

            // Основной контент с задачами
            html << "        <section class=\"cosmic-main\">\n";
            html << "            <!-- Панель управления -->\n";
            html << "            <aside class=\"control-panel\">\n";
            html << "                <div class=\"panel-header\">\n";
            html << "                    <h2><i class=\"fas fa-sliders-h\"></i> УПРАВЛЕНИЕ</h2>\n";
            html << "                    <div class=\"panel-line\"></div>\n";
            html << "                </div>\n";
            html << "                \n";
            html << "                <div class=\"controls-grid\">\n";
            html << "                    <div class=\"control-item\">\n";
            html << "                        <div class=\"control-label\">\n";
            html << "                            <i class=\"fas fa-filter\"></i>\n";
            html << "                            ФИЛЬТРАЦИЯ\n";
            html << "                        </div>\n";
            html << "                        <div class=\"cosmic-select\">\n";
            html << "                            <select id=\"filterSelect\">\n";
            html << "                                <option value=\"all\">Все задачи</option>\n";
            html << "                                <option value=\"unsolved\">Не решённые</option>\n";
            html << "                                <option value=\"solved\">Решённые</option>\n";
            html << "                                <option value=\"incorrect\">С ошибками</option>\n";
            html << "                            </select>\n";
            html << "                            <div class=\"select-arrow\">\n";
            html << "                                <i class=\"fas fa-chevron-down\"></i>\n";
            html << "                            </div>\n";
            html << "                        </div>\n";
            html << "                    </div>\n";
            html << "                </div>\n";
            html << "            </aside>\n";
            html << "            \n";
            html << "            <!-- Контейнер задач -->\n";
            html << "            <div class=\"problems-container\">\n";
            html << "                <div class=\"problems-header\">\n";
            html << "                    <h2><i class=\"fas fa-tasks\"></i> ЗАДАЧИ</h2>\n";
            html << "                    <div class=\"counter\">\n";
            html << "                        <div class=\"counter-number\" id=\"counterNumber\">"
                << config.total_problems << "</div>\n";
            html << "                        <div class=\"counter-text\" id=\"counterText\">задач не решено</div>\n";
            html << "                    </div>\n";
            html << "                </div>\n";
            html << "                \n";
            html << "                <!-- Фильтры -->\n";
            html << "                <div class=\"filter-panel\">\n";
            html << "                    <button class=\"filter-btn active\" data-filter=\"all\">Все задачи</button>\n";
            html << "                    <button class=\"filter-btn\" data-filter=\"unsolved\">Не решённые</button>\n";
            html << "                    <button class=\"filter-btn\" data-filter=\"solved\">Решённые</button>\n";
            html << "                    <button class=\"filter-btn\" data-filter=\"incorrect\">С ошибками</button>\n";
            html << "                </div>\n";
            html << "                \n";
            html << "                <!-- Задачи -->\n";

            // Вставляем задачи
            for (const auto& problem_html : problems_html) {
                html << problem_html;
            }

            // Панель сдачи
            html << "                <!-- Панель сдачи -->\n";
            html << "                <div class=\"submission-panel\">\n";
            html << "                    <div class=\"submission-stats\">\n";
            html << "                        <div class=\"stat-item\">\n";
            html << "                            <div class=\"stat-icon\">\n";
            html << "                                <i class=\"fas fa-check-circle\"></i>\n";
            html << "                            </div>\n";
            html << "                            <div class=\"stat-info\">\n";
            html << "                                <div class=\"stat-value\" id=\"statsSolved\">0</div>\n";
            html << "                                <div class=\"stat-label\">Решено</div>\n";
            html << "                            </div>\n";
            html << "                        </div>\n";
            html << "                        \n";
            html << "                        <div class=\"stat-item\">\n";
            html << "                            <div class=\"stat-icon\">\n";
            html << "                                <i class=\"fas fa-percentage\"></i>\n";
            html << "                            </div>\n";
            html << "                            <div class=\"stat-info\">\n";
            html << "                                <div class=\"stat-value\" id=\"statsAccuracy\">0%</div>\n";
            html << "                                <div class=\"stat-label\">Точность</div>\n";
            html << "                            </div>\n";
            html << "                        </div>\n";
            html << "                    </div>\n";
            html << "                    \n";
            html << "                    <div class=\"submission-actions\">\n";
            html << "                        <button class=\"cosmic-btn cosmic-btn-submit\" onclick=\"problemManager.submitAssignment()\">\n";
            html << "                            <i class=\"fas fa-paper-plane\"></i>\n";
            html << "                            СДАТЬ ЗАДАНИЕ\n";
            html << "                        </button>\n";
            html << "                    </div>\n";
            html << "                </div>\n";
            html << "            </div>\n\n";

            html << "            <!-- Панель статистики -->\n";
            html << "            <aside class=\"stats-panel\">\n";
            html << "                <div class=\"stats-header\">\n";
            html << "                    <h2><i class=\"fas fa-chart-line\"></i> СТАТИСТИКА</h2>\n";
            html << "                    <div class=\"stats-line\"></div>\n";
            html << "                </div>\n\n";
            html << "                <div class=\"stats-content\">\n";
            html << "                    <div class=\"stat-item\">\n";
            html << "                        <div class=\"stat-icon\">\n";
            html << "                            <i class=\"fas fa-hashtag\"></i>\n";
            html << "                        </div>\n";
            html << "                        <div class=\"stat-info\">\n";
            html << "                            <div class=\"stat-value\">" << config.total_problems << "</div>\n";
            html << "                            <div class=\"stat-label\">Всего задач</div>\n";
            html << "                        </div>\n";
            html << "                    </div>\n";
            html << "                </div>\n";
            html << "            </aside>\n";
            html << "        </section>\n";

            // Подвал
            html << generate_footer();

            // JavaScript
            html << "    </main>\n";
            html << "    \n";
            html << "    <!-- JavaScript -->\n";
            html << "    <script src=\"" << config.js_file << "\"></script>\n";
            html << "    \n";
            html << "    <!-- Встроенные данные задач -->\n";

            // Генерация JS данных если есть
            if (!problems_json.empty()) {
                html << generate_js_data(problems_json);
            }

            // Скрипт инициализации
            html << "    <script>\n";
            html << "        document.addEventListener('DOMContentLoaded', function() {\n";
            html << "            // Плавная загрузка\n";
            html << "            setTimeout(() => {\n";
            html << "                const loadingBar = document.getElementById('loadingBar');\n";
            html << "                loadingBar.style.width = '100%';\n";
            html << "                \n";
            html << "                setTimeout(() => {\n";
            html << "                    document.getElementById('loadingScreen').style.opacity = '0';\n";
            html << "                    setTimeout(() => {\n";
            html << "                        document.getElementById('loadingScreen').style.display = 'none';\n";
            html << "                        document.getElementById('mainContainer').style.display = 'block';\n";
            html << "                    }, 500);\n";
            html << "                }, 300);\n";
            html << "            }, 500);\n";
            html << "            \n";
            html << "            // Инициализация менеджера задач\n";
            html << "            window.problemManager = new CosmicProblemManager();\n";
            html << "        });\n";
            html << "    </script>\n";
            html << "</body>\n";
            html << "</html>\n";

            return html.str();
        }

        // Генерация только шапки
        string generate_header() {
            stringstream html;

            html << "        <header class=\"cosmic-header\">\n";
            html << "            <div class=\"title-wrapper\">\n";
            html << "                <h1 class=\"main-title\">\n";
            html << "                    <span class=\"title-word title-word-1\">COSMIC</span>\n";
            html << "                    <span class=\"title-word title-word-2\">ОГЭ</span>\n";
            html << "                    <span class=\"title-word title-word-3\">ГЕНЕРАТОР</span>\n";
            html << "                </h1>\n";
            html << "                <div class=\"title-sub\">\n";
            html << "                    <span class=\"sub-text\">СИСТЕМА ТРЕНИРОВКИ</span>\n";
            html << "                    <span class=\"version\">" << config.version << "</span>\n";
            html << "                    <div class=\"pulse-dot\"></div>\n";
            html << "                </div>\n";
            html << "            </div>\n";
            html << "            \n";
            html << "            <div class=\"header-actions\">\n";
            html << "                <button class=\"cosmic-btn cosmic-btn-primary\" onclick=\"problemManager.submitAssignment()\">\n";
            html << "                    <i class=\"fas fa-rocket\"></i>\n";
            html << "                    <span>ПРОВЕРИТЬ РЕШЕНИЯ</span>\n";
            html << "                </button>\n";
            html << "                <button class=\"cosmic-btn cosmic-btn-secondary\" onclick=\"problemManager.showDetailedStats()\">\n";
            html << "                    <i class=\"fas fa-chart-pie\"></i>\n";
            html << "                    <span>СТАТИСТИКА</span>\n";
            html << "                </button>\n";
            html << "            </div>\n";
            html << "        </header>\n";

            return html.str();
        }

        // Генерация подвала
        string generate_footer() {
            stringstream html;

            html << "        <footer class=\"cosmic-footer\">\n";
            html << "            <div class=\"footer-grid\">\n";
            html << "                <div class=\"footer-section\">\n";
            html << "                    <h3><i class=\"fas fa-info-circle\"></i> О СИСТЕМЕ</h3>\n";
            html << "                    <p>Сгенерировано C++ генератором задач ОГЭ. Всего задач: "
                << config.total_problems << "</p>\n";
            html << "                    <div class=\"system-status\">\n";
            html << "                        <i class=\"fas fa-circle\"></i>\n";
            html << "                        <span>СИСТЕМА АКТИВНА</span>\n";
            html << "                    </div>\n";
            html << "                </div>\n";
            html << "                \n";
            html << "                <div class=\"footer-section\">\n";
            html << "                    <h3><i class=\"fas fa-cogs\"></i> ТЕХНОЛОГИИ</h3>\n";
            html << "                    <ul>\n";
            html << "                        <li>C++ генерация задач</li>\n";
            html << "                        <li>SQLite база данных</li>\n";
            html << "                        <li>JavaScript интерфейс</li>\n";
            html << "                        <li>HTML5 & CSS3</li>\n";
            html << "                    </ul>\n";
            html << "                </div>\n";
            html << "                \n";
            html << "                <div class=\"footer-section\">\n";
            html << "                    <h3><i class=\"fas fa-code\"></i> СГЕНЕРИРОВАНО</h3>\n";
            html << "                    <p>" << format_time(config.generation_time) << "</p>\n";
            html << "                    <p>Версия: " << config.version << "</p>\n";
            html << "                </div>\n";
            html << "            </div>\n";
            html << "            \n";
            html << "            <div class=\"footer-bottom\">\n";
            html << "                <div class=\"scanline\"></div>\n";
            html << "                <p>&copy; 2024 Cosmic OГЭ Generator</p>\n";
            html << "                <div class=\"terminal-text\">Система загружена. Готов к работе...</div>\n";
            html << "            </div>\n";
            html << "        </footer>\n";

            return html.str();
        }

        // Генерация JS данных
        string generate_js_data(const vector<map<string, string>>& problems_json) {
            stringstream js;

            js << "<script>\n";
            js << "// Данные задач, сгенерированные C++\n";
            js << "const problemsData = [\n";

            for (size_t i = 0; i < problems_json.size(); i++) {
                js << "  {\n";

                // Устанавливаем базовые поля
                js << "    \"id\": \"" << (i + 1) << "\",\n";
                js << "    \"type\": \"1\",\n";

                // Добавляем все поля из JSON (кроме дубликатов id)
                bool has_own_id = false;
                bool has_own_type = false;

                for (const auto& [key, value] : problems_json[i]) {
                    if (key == "id") has_own_id = true;
                    if (key == "type") has_own_type = true;
                }

                for (const auto& [key, value] : problems_json[i]) {
                    // Пропускаем дублирующиеся поля
                    if (key == "id" && has_own_id) continue;
                    if (key == "type" && has_own_type) continue;

                    js << "    \"" << key << "\": \"" << escape_js_string(value) << "\",\n";
                }

                js << "    \"solved\": false,\n";
                js << "    \"userAnswer\": \"\",\n";
                js << "    \"answerChecked\": false,\n";
                js << "    \"difficulty\": 2\n";
                js << "  }";
                if (i < problems_json.size() - 1) js << ",";
                js << "\n";
            }

            js << "];\n\n";
            js << "// Автозагрузка данных при инициализации\n";
            js << "window.addEventListener('load', function() {\n";
            js << "  setTimeout(() => {\n";
            js << "    if (window.problemManager && problemsData.length > 0) {\n";
            js << "      window.problemManager.loadProblemsFromServer(problemsData);\n";
            js << "    }\n";
            js << "  }, 1000);\n";
            js << "});\n";
            js << "</script>\n";

            return js.str();
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
        bool generate_and_save(const vector<string>& problems_html,
            const vector<map<string, string>>& problems_json = {}) {
            string html = generate_full_page(problems_html, problems_json);
            return save_to_file(html);
        }
    };

} // namespace OGE

#endif // HTML_PAGE_GENERATOR_H