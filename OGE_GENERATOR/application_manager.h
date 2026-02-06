#pragma once
#ifndef APPLICATION_MANAGER_H
#define APPLICATION_MANAGER_H

#include <memory>
#include <string>
#include <iostream>
#include "database_manager.h"
#include "task_generator.h"
#include "html_page_generator.h"

namespace OGE {

    class ApplicationManager {
    private:
        std::shared_ptr<DatabaseManager> db_manager;
        std::shared_ptr<DatabaseProblem1> db_problem1;
        std::unique_ptr<TaskGenerator> task_generator;
        std::unique_ptr<HtmlPageGenerator> html_generator;

        // Состояние приложения
        bool initialized;
        bool database_ready;
        std::string last_error;

    public:
        // Настройки приложения (сделаем public)
        struct AppConfig {
            std::string db_path = ":memory:";
            bool create_tables = true;
            bool seed_data = true;
            bool auto_open_browser = true;
            std::string default_scenario = "default";
        };

        ApplicationManager();

        // Инициализация
        bool initialize(const AppConfig& cfg = AppConfig());
        bool is_initialized() const { return initialized; }
        bool is_database_ready() const { return database_ready; }

        // Конфигурация
        AppConfig& get_config() { return config; }
        const AppConfig& get_config() const { return config; }
        void set_config(const AppConfig& cfg);

        // Управление базой данных
        bool setup_database();
        bool create_database_tables();
        bool seed_database();
        OGE::DatabaseManager::DatabaseInfo get_database_info() const;

        // Управление генерацией
        TaskGenerator& get_task_generator();
        bool generate_tasks(const std::string& scenario_name = "");
        bool generate_all_tasks();

        // Экспорт
        bool export_to_html(const std::string& scenario_name = "",
            const std::string& output_file = "");
        bool export_all_to_html(const std::string& output_file = "");

        // Открытие в браузере
        bool open_in_browser(const std::string& filepath);

        // Статистика
        void print_database_stats(std::ostream& out = std::cout) const;
        void print_generation_stats(std::ostream& out = std::cout) const;
        void print_all_stats(std::ostream& out = std::cout) const;

        // Состояние
        std::string get_status() const;
        std::string get_last_error() const { return last_error; }
        void clear_error() { last_error.clear(); }

        // Сброс
        void reset();

    private:
        AppConfig config;

        bool open_browser_windows(const std::string& filepath);
        bool open_browser_unix(const std::string& filepath);
    };

} // namespace OGE

#endif // APPLICATION_MANAGER_H