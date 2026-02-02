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

        string get_current_time() const {
            time_t now = time(nullptr);
            struct tm time_info;

            // Используем безопасную версию localtime_s
            if (localtime_s(&time_info, &now) != 0) {
                return "Ошибка времени";
            }

            char buf[100];
            strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", &time_info);
            return string(buf);
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

        // ... остальной код остается без изменений ...

        // В методе generate_full_page замените get_current_time() на format_time()

        // Генерация полной HTML страницы
        string generate_full_page(const vector<string>& problems_html,
            const vector<map<string, string>>& problems_json = {}) {
            stringstream html;

            // Начало документа
            html << R"(<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>)" << config.title << R"(</title>
    
    <!-- Шрифты -->
    <link href="https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700;900&family=Exo+2:wght@300;400;600;700;800&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    
    <!-- Стили -->
    <link rel="stylesheet" href=")" << config.css_file << R"(">
    
    <style>
        .loading-screen {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: #0a0a0f;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            z-index: 9999;
            transition: opacity 0.5s ease;
        }
        
        .loading-logo {
            font-family: 'Orbitron', monospace;
            font-size: 3rem;
            margin-bottom: 2rem;
            background: linear-gradient(45deg, #00ccff, #6d44ff);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }
        
        .loading-progress {
            width: 300px;
            height: 4px;
            background: #2d2d5a;
            border-radius: 2px;
            overflow: hidden;
            margin-top: 1rem;
        }
        
        .loading-progress-bar {
            height: 100%;
            background: linear-gradient(45deg, #6d44ff, #00ccff);
            width: 0%;
            transition: width 0.3s ease;
        }
        
        .generation-info {
            position: fixed;
            bottom: 20px;
            right: 20px;
            background: rgba(26, 26, 46, 0.8);
            padding: 0.8rem 1.2rem;
            border-radius: 10px;
            font-size: 0.8rem;
            color: #c7c7ff;
            border: 1px solid #2d2d5a;
            z-index: 100;
        }
    </style>
</head>
<body>
    <!-- Звездное небо -->
    <div class="stars"></div>
    <div class="stars2"></div>
    <div class="stars3"></div>
    <div class="comet"></div>
    <div class="nebula"></div>
    
    <!-- Экран загрузки -->
    <div class="loading-screen" id="loadingScreen">
        <div class="loading-logo">COSMIC ОГЭ</div>
        <div class="loading-progress">
            <div class="loading-progress-bar" id="loadingBar"></div>
        </div>
    </div>
    
    <!-- Информация о генерации -->
    <div class="generation-info">
        <i class="fas fa-code"></i> Сгенерировано: )" << format_time(config.generation_time) << R"(
    </div>
    
    <!-- Основной контейнер -->
    <main class="cosmic-container" id="mainContainer" style="display: none;">)";

            // Шапка
            html << generate_header();

            // Основной контент с задачами
            html << R"(
        <section class="cosmic-main">
            <!-- Панель управления -->
            <aside class="control-panel">
                <div class="panel-header">
                    <h2><i class="fas fa-sliders-h"></i> УПРАВЛЕНИЕ</h2>
                    <div class="panel-line"></div>
                </div>
                
                <div class="controls-grid">
                    <div class="control-item">
                        <div class="control-label">
                            <i class="fas fa-filter"></i>
                            ФИЛЬТРАЦИЯ
                        </div>
                        <div class="cosmic-select">
                            <select id="filterSelect">
                                <option value="all">Все задачи</option>
                                <option value="unsolved">Не решённые</option>
                                <option value="solved">Решённые</option>
                                <option value="incorrect">С ошибками</option>
                            </select>
                            <div class="select-arrow">
                                <i class="fas fa-chevron-down"></i>
                            </div>
                        </div>
                    </div>
                </div>
            </aside>
            
            <!-- Контейнер задач -->
            <div class="problems-container">
                <div class="problems-header">
                    <h2><i class="fas fa-tasks"></i> ЗАДАЧИ</h2>
                    <div class="counter">
                        <div class="counter-number" id="counterNumber">)"
                << config.total_problems << R"(</div>
                        <div class="counter-text" id="counterText">задач не решено</div>
                    </div>
                </div>
                
                <!-- Фильтры -->
                <div class="filter-panel">
                    <button class="filter-btn active" data-filter="all">Все задачи</button>
                    <button class="filter-btn" data-filter="unsolved">Не решённые</button>
                    <button class="filter-btn" data-filter="solved">Решённые</button>
                    <button class="filter-btn" data-filter="incorrect">С ошибками</button>
                </div>
                
                <!-- Задачи -->)";

            // Вставляем задачи
            for (const auto& problem_html : problems_html) {
                html << problem_html;
            }

            // Панель статистики и подвал
            html << R"(
                <!-- Панель сдачи -->
                <div class="submission-panel">
                    <div class="submission-stats">
                        <div class="stat-item">
                            <div class="stat-icon">
                                <i class="fas fa-check-circle"></i>
                            </div>
                            <div class="stat-info">
                                <div class="stat-value" id="statsSolved">0</div>
                                <div class="stat-label">Решено</div>
                            </div>
                        </div>
                        
                        <div class="stat-item">
                            <div class="stat-icon">
                                <i class="fas fa-percentage"></i>
                            </div>
                            <div class="stat-info">
                                <div class="stat-value" id="statsAccuracy">0%</div>
                                <div class="stat-label">Точность</div>
                            </div>
                        </div>
                    </div>
                    
                    <div class="submission-actions">
                        <button class="cosmic-btn cosmic-btn-submit" onclick="problemManager.submitAssignment() >
                <i class = "fas fa-paper-plane">< / i>
                СДАТЬ ЗАДАНИЕ
                < / button>
                < / div>
                < / div>
                < / div>

                <!--Панель статистики-->
                <aside class = "stats-panel">
                <div class = "stats-header">
                <h2><i class = "fas fa-chart-line">< / i> СТАТИСТИКА< / h2>
                <div class = "stats-line">< / div>
                < / div>

                <div class = "stats-content">
                <div class = "stat-item">
                <div class = "stat-icon">
                <i class = "fas fa-hashtag">< / i>
                < / div>
                <div class = "stat-info">
                <div class = "stat-value">)" << config.total_problems << R"(< / div>
                <div class = "stat-label">Всего задач< / div>
                    < / div>
                    < / div>
                    < / div>
                    < / aside>
                    < / section>)";

                // Подвал
                html << generate_footer();

            // JavaScript
            html << R"(
    </main>
    
    <!-- JavaScript -->
    <script src=")" << config.js_file << R"("></script>
    
    <!-- Встроенные данные задач -->)";

            // Генерируем JS данные если есть
            if (!problems_json.empty()) {
                html << generate_js_data(problems_json);
            }

            // Скрипт инициализации
            html << R"(
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            // Плавная загрузка
            setTimeout(() => {
                const loadingBar = document.getElementById('loadingBar');
                loadingBar.style.width = '100%';
                
                setTimeout(() => {
                    document.getElementById('loadingScreen').style.opacity = '0';
                    setTimeout(() => {
                        document.getElementById('loadingScreen').style.display = 'none';
                        document.getElementById('mainContainer').style.display = 'block';
                    }, 500);
                }, 300);
            }, 500);
            
            // Инициализация менеджера задач
            window.problemManager = new CosmicProblemManager();
        });
    </script>
</body>
</html>)";

            return html.str();
        }

        // Генерация только шапки
        string generate_header() {
            stringstream html;

            html << R"(
        <header class="cosmic-header">
            <div class="title-wrapper">
                <h1 class="main-title">
                    <span class="title-word title-word-1">COSMIC</span>
                    <span class="title-word title-word-2">ОГЭ</span>
                    <span class="title-word title-word-3">ГЕНЕРАТОР</span>
                </h1>
                <div class="title-sub">
                    <span class="sub-text">СИСТЕМА ТРЕНИРОВКИ</span>
                    <span class="version">)" << config.version << R"(</span>
                    <div class="pulse-dot"></div>
                </div>
            </div>
            
            <div class="header-actions">
                <button class="cosmic-btn cosmic-btn-primary" onclick="problemManager.submitAssignment() >
                <i class = "fas fa-rocket">< / i>
                <span>ПРОВЕРИТЬ РЕШЕНИЯ< / span>
                < / button>
                <button class = "cosmic-btn cosmic-btn-secondary" onclick = "problemManager.showDetailedStats()>
                <i class = "fas fa-chart-pie">< / i>
                <span>СТАТИСТИКА< / span>
                < / button>
                < / div>
                < / header>)";

                return html.str();
        }

        // Генерация подвала
        string generate_footer() {
            stringstream html;

            html << R"(
        <footer class="cosmic-footer">
            <div class="footer-grid">
                <div class="footer-section">
                    <h3><i class="fas fa-info-circle"></i> О СИСТЕМЕ</h3>
                    <p>Сгенерировано C++ генератором задач ОГЭ. Всего задач: )"
                << config.total_problems << R"(</p>
                    <div class="system-status">
                        <i class="fas fa-circle"></i>
                        <span>СИСТЕМА АКТИВНА</span>
                    </div>
                </div>
                
                <div class="footer-section">
                    <h3><i class="fas fa-cogs"></i> ТЕХНОЛОГИИ</h3>
                    <ul>
                        <li>C++ генерация задач</li>
                        <li>SQLite база данных</li>
                        <li>JavaScript интерфейс</li>
                        <li>HTML5 & CSS3</li>
                    </ul>
                </div>
                
                <div class="footer-section">
                    <h3><i class="fas fa-code"></i> СГЕНЕРИРОВАНО</h3>
                    <p>)" << format_time(config.generation_time) << R"(</p>
                    <p>Версия: )" << config.version << R"(</p>
                </div>
            </div>
            
            <div class="footer-bottom">
                <div class="scanline"></div>
                <p>&copy; 2024 Cosmic OГЭ Generator</p>
                <div class="terminal-text">Система загружена. Готов к работе...</div>
            </div>
        </footer>)";

            return html.str();
        }

        // Генерация JS данных
        string generate_js_data(const vector<map<string, string>>& problems_json) {
            stringstream js;

            js << "\n<script>\n";
            js << "// Данные задач, сгенерированные C++\n";
            js << "const problemsData = [\n";

            for (size_t i = 0; i < problems_json.size(); i++) {
                js << "  {\n";
                js << "    \"id\": \"" << (i + 1) << "\",\n";
                js << "    \"type\": \"1\",\n";

                // Добавляем все поля из JSON
                for (const auto& [key, value] : problems_json[i]) {
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