// page_settings.h
#pragma once

#include <string>
#include <unordered_map>
#include <ctime>

namespace OGE {

    // Уровни доступа к решениям
    enum class AccessLevel {
        NO_ACCESS,      // 0 - нет доступа к решениям (пусть в этом случае статистика будет генерироваться в виде зашифрованного архива с html страницей внутри)
        AFTER_ANSWER,   // 2 - решение после ответа
        FULL_ACCESS,    // 3 - полный доступ
    };

    // Режим таймера
    enum class TimerMode {
        NO_TIMER,       // без таймера
        WITH_TIMER,     // общее время за вариант
        COUNT_DOWN      // ограниченое время
    };

    // Настройки страницы варианта
    struct PageSettings {
        std::string teacher_password_hash;   // Хеш пароля учителя
        std::string student_password_hash;   // Хеш пароля студента (если нужно)

        // Доступ к решениям
        AccessLevel solution_access = AccessLevel::NO_ACCESS;
        bool show_hints = false;             // Показывать подсказки
        bool show_after_answer = false;      // Показывать после ответа
        bool allow_retry = true;             // Разрешать перерешивание

        // Таймер
        TimerMode timer_mode = TimerMode::NO_TIMER;
        int timer_seconds = 0;               // Общее время в секундах


        // Задачи
        std::vector<int> problem_numbers;    // Какие задачи включены (1-16)
        int tasks_per_variant = 10;          // Сколько задач в варианте
        bool random_order = true;            // Случайный порядок

        // Статистика и оценка
        bool record_statistics = true;       // Записывать статистику
        bool auto_grade = false;             // Автоматическая оценка
        int passing_score = 0;               // Проходной балл

        // Внешний вид
        bool show_progress_bar = true;
        bool show_score = true;
        bool dark_mode = false;

        // Защита
        bool encrypt_solutions = false;      // Шифровать решения
        std::string encryption_key;          // Ключ шифрования
        bool require_password_for_results = false; // Требовать пароль для результатов

        // Время и дата
        std::time_t creation_time;           // Время создания
        // Методы проверки
        bool is_expired() const;
        bool check_password(const std::string& password) const;
        bool can_view_solutions() const;
        bool can_view_hints() const;

    private:
        static std::string hash_password(const std::string& password);
    };

    class SettingsManager {
    public:
        // Текущие настройки
        void set_title(const std::string& title);
        void set_problems(const std::vector<int>& problems);
        void set_timer(TimerMode mode, int seconds);
        void set_access_level(AccessLevel level);
        void set_teacher_password(const std::string& password);

        // Получение настроек
        const PageSettings& get_settings() const;

        // Проверки
        bool verify_password(const std::string& password) const;
        bool is_teacher_mode() const;

        // Сохранение/загрузка
        bool save_to_file(const std::string& filename) const;
        bool load_from_file(const std::string& filename);

        // Генерация HTML с настройками
        std::string generate_html_config() const;

    private:
        PageSettings current_settings;
    };

} 