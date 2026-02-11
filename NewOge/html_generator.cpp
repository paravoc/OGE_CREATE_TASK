// html_generator.cpp
#include "html_generator.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace OGE {

    // Конструктор
    HtmlGenerator::HtmlGenerator(const PageSettings& settings)
        : page_settings(settings) {
    }

    // Генерация стилей
    string HtmlGenerator::generate_styles() const {
        stringstream css;

        css << R"(
        <style>
            * {
                margin: 0;
                padding: 0;
                box-sizing: border-box;
            }
            
            body {
                font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
                line-height: 1.6;
                padding: 20px;
                background-color: )"
            << (page_settings.dark_mode ? "#1a1a1a" : "#f5f5f5") << R"(;
                color: )"
            << (page_settings.dark_mode ? "#ffffff" : "#333333") << R"(;
                transition: all 0.3s ease;
            }
            
            .container {
                max-width: 1200px;
                margin: 0 auto;
                padding: 20px;
            }
            
            .header {
                text-align: center;
                padding: 20px;
                margin-bottom: 30px;
                background: )"
            << (page_settings.dark_mode ? "#2d2d2d" : "#ffffff") << R"(;
                border-radius: 10px;
                box-shadow: 0 2px 5px rgba(0,0,0,0.1);
            }
            
            .problems-grid {
                display: grid;
                grid-template-columns: repeat(auto-fill, minmax(350px, 1fr));
                gap: 20px;
                margin-top: 20px;
            }
            
            .problem-wrapper {
                background: )"
            << (page_settings.dark_mode ? "#2d2d2d" : "#ffffff") << R"(;
                border-radius: 10px;
                padding: 20px;
                box-shadow: 0 2px 5px rgba(0,0,0,0.1);
                transition: transform 0.3s ease;
            }
            
            .problem-wrapper:hover {
                transform: translateY(-5px);
                box-shadow: 0 5px 15px rgba(0,0,0,0.2);
            }
            
            .answer-area {
                margin-top: 20px;
                padding-top: 20px;
                border-top: 1px solid #ddd;
            }
            
            input[type="text"] {
                width: calc(100% - 110px);
                padding: 10px;
                border: 1px solid #ddd;
                border-radius: 5px;
                font-size: 16px;
                margin-right: 10px;
                background: )"
            << (page_settings.dark_mode ? "#3d3d3d" : "#ffffff") << R"(;
                color: )"
            << (page_settings.dark_mode ? "#ffffff" : "#333333") << R"(;
            }
            
            button {
                padding: 10px 20px;
                background: #4CAF50;
                color: white;
                border: none;
                border-radius: 5px;
                cursor: pointer;
                font-size: 16px;
                transition: background 0.3s ease;
            }
            
            button:hover {
                background: #45a049;
            }
            
            .score {
                display: inline-block;
                padding: 5px 10px;
                background: #ff9800;
                color: white;
                border-radius: 3px;
                font-weight: bold;
                margin-top: 10px;
            }
            
            .progress {
                margin-top: 20px;
                height: 20px;
                background: #ddd;
                border-radius: 10px;
                overflow: hidden;
            }
            
            progress {
                width: 100%;
                height: 100%;
                border-radius: 10px;
            }
            
            progress::-webkit-progress-value {
                background: #4CAF50;
                border-radius: 10px;
            }
            
            .solution-wrapper, .hint-wrapper {
                margin-top: 20px;
                padding: 15px;
                background: )"
            << (page_settings.dark_mode ? "#3d3d3d" : "#f9f9f9") << R"(;
                border-left: 4px solid #2196F3;
                border-radius: 5px;
            }
            
            .timer {
                position: sticky;
                top: 20px;
                right: 20px;
                background: )"
            << (page_settings.dark_mode ? "#2d2d2d" : "#ffffff") << R"(;
                padding: 15px;
                border-radius: 10px;
                box-shadow: 0 2px 5px rgba(0,0,0,0.1);
                margin-bottom: 20px;
                text-align: center;
                font-size: 20px;
                font-weight: bold;
            }
            
            @media (max-width: 768px) {
                .problems-grid {
                    grid-template-columns: 1fr;
                }
                
                .container {
                    padding: 10px;
                }
            }
        </style>
        )";

        return css.str();
    }

    // Генерация скриптов
    string HtmlGenerator::generate_scripts() const {
        stringstream js;

        js << R"(
        <script>
            // Глобальная статистика
            let stats = {
                total: 0,
                correct: 0,
                incorrect: 0,
                startTime: Date.now()
            };
            
            // Функция проверки ответа
            window.checkAnswer = function(problemId, correctAnswer, score) {
                const input = document.getElementById('answer-' + problemId);
                const userAnswer = input.value.trim();
                
                fetch('/api/check-answer', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({
                        problem_id: problemId,
                        user_answer: userAnswer,
                        correct_answer: correctAnswer,
                        score: score
                    })
                })
                .then(response => response.json())
                .then(data => {
                    if (data.correct) {
                        alert('✅ Правильно! +' + score + ' баллов');
                        input.style.borderColor = '#4CAF50';
                        stats.correct++;
                    } else {
                        alert('❌ Неправильно. Попробуйте еще раз');
                        input.style.borderColor = '#f44336';
                        stats.incorrect++;
                    }
                    stats.total++;
                    
                    if ()" << (page_settings.record_statistics ? "true" : "false") << R"() {
                        console.log('Statistics:', stats);
                    }
                })
                .catch(error => {
                    console.error('Error:', error);
                    alert('Ошибка при проверке ответа');
                });
            };
            
            // Таймер
            if ()" << (page_settings.timer_mode != TimerMode::NO_TIMER ? "true" : "false") << R"() {
                let timeLeft = )" << page_settings.timer_seconds << R"(;
                const timerElement = document.createElement('div');
                timerElement.className = 'timer';
                timerElement.id = 'timer';
                document.querySelector('.container').prepend(timerElement);
                
                const timerInterval = setInterval(() => {
                    timeLeft--;
                    const minutes = Math.floor(timeLeft / 60);
                    const seconds = timeLeft % 60;
                    timerElement.textContent = `Осталось времени: ${minutes}:${seconds.toString().padStart(2, '0')}`;
                    
                    if (timeLeft <= 0) {
                        clearInterval(timerInterval);
                        alert('Время вышло!');
                        location.reload();
                    }
                }, 1000);
            }
            
            // Сохранение прогресса
            window.saveProgress = function() {
                const progress = {
                    stats: stats,
                    answers: {},
                    timestamp: Date.now()
                };
                
                document.querySelectorAll('input[type="text"]').forEach((input, index) => {
                    progress.answers[input.id] = input.value;
                });
                
                localStorage.setItem('exam_progress', JSON.stringify(progress));
                alert('Прогресс сохранен!');
            };
            
            // Загрузка прогресса
            window.loadProgress = function() {
                const saved = localStorage.getItem('exam_progress');
                if (saved) {
                    const progress = JSON.parse(saved);
                    Object.entries(progress.answers).forEach(([id, value]) => {
                        const input = document.getElementById(id);
                        if (input) input.value = value;
                    });
                    alert('Прогресс загружен!');
                }
            };
            
            // Темная тема
            window.toggleTheme = function() {
                document.body.classList.toggle('dark-mode');
            };
        </script>
        )";

        return js.str();
    }

    // Генерация заголовка
    string HtmlGenerator::generate_header() const {
        stringstream header;

        std::time_t now = std::time(nullptr);
        std::tm timeinfo;
        localtime_s(&timeinfo, &now);

        header << R"(
        <!DOCTYPE html>
        <html lang="ru">
        <head>
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <meta name="generator" content="OGE HTML Generator">
            <title>Вариант ОГЭ</title>
        )" << generate_styles() << R"(
        </head>
        <body>
            <div class="container">
                <div class="header">
                    <h1>Тренировочный вариант ОГЭ</h1>
                    <p>Сгенерировано: )" << put_time(&timeinfo, "%d.%m.%Y %H:%M:%S") << R"(</p>
                    )";

        if (page_settings.show_score) {
            header << "<p class='score'>Максимальный балл: " << page_settings.passing_score << "</p>";
        }

        if (page_settings.show_progress_bar) {
            header << R"(
                    <div class="progress">
                        <progress id="main-progress" value="0" max="100"></progress>
                    </div>
            )";
        }

        header << R"(
                </div>
                <div class="problems-grid">
        )";

        return header.str();
    }

    // Генерация подвала
    string HtmlGenerator::generate_footer() const {
        stringstream footer;

        footer << R"(
                </div>
        )";

        if (page_settings.allow_retry) {
            footer << R"(
                <div style="text-align: center; margin-top: 30px; padding: 20px;">
                    <button onclick="saveProgress() " > 💾 Сохранить прогресс< / button>
                <button onclick = "loadProgress() ">📂 Загрузить прогресс< / button>
                <button onclick = "location.reload() ">🔄 Новый вариант< / button>
                < / div>
                )";
        }

        footer << R"(
            </div>
        )" << generate_scripts() << R"(
        </body>
        </html>
        )";

        return footer.str();
    }

    // Генерация списка задач
    string HtmlGenerator::generate_problem_list() const {
        stringstream content;

        for (const auto& problem : all_problems) {
            if (problem) {
                content << problem->generate_html() << "\n";
            }
        }

        return content.str();
    }

    // Генерация всех задач
    void HtmlGenerator::generate_all_problems() {
        all_problems.clear();

        for (const auto& [problem_type, count] : page_settings.problem_numbers) {
            for (int i = 0; i < count; i++) {
                unique_ptr<ProblemBase> problem;

                ProblemMeta meta;
                meta.type_id = problem_type + "_" + to_string(i + 1);
                meta.display_name = problem_type;
                meta.problem_number = i + 1;
                meta.default_score = 10;
                meta.is_active = true;

                // Динамическое создание задач через фабрику
                // Здесь можно добавить другие типы задач

                if (problem) {
                    GenerationConfig config;
                    config.difficulty = Difficulty::MEDIUM;
                    problem->generate(config);
                    all_problems.push_back(std::move(problem));
                }
            }
        }
    }

    // Добавление конкретной задачи
    void HtmlGenerator::add_problem(unique_ptr<ProblemBase> problem) {
        if (problem) {
            all_problems.push_back(std::move(problem));
        }
    }

    // Генерация полной HTML страницы
    string HtmlGenerator::generate_full_page() {
        stringstream full_page;
        full_page << generate_header();
        full_page << generate_problem_list();
        full_page << generate_footer();

        html_page = full_page.str();
        return html_page;
    }

    // Генерация только контента
    string HtmlGenerator::generate_content_only() {
        stringstream content;
        content << generate_problem_list();
        return content.str();
    }

    // Очистка всех задач
    void HtmlGenerator::clear_problems() {
        all_problems.clear();
    }

    // Сохранение в файл
    bool HtmlGenerator::save_to_file(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        file << html_page;
        return file.good();
    }

}