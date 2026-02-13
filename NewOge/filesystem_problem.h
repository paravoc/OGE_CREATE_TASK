#pragma once

#include "problem_base.h"
#include "page_setting.h"
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace OGE {

    enum class FSTaskType {
        URL_PATH,        // Составление URL пути
        FILE_PATH,       // Путь к файлу на компьютере
        CD_COMMANDS      // Навигация по папкам
    };

    class FilesystemProblem : public ProblemBase {
    private:
        static PageSettings page_settings;
        static std::mt19937& get_random_generator();

        // Статические списки для генерации
        static std::vector<std::string> protocols;
        static std::vector<std::string> domains;
        static std::vector<std::string> folders;
        static std::vector<std::string> filenames;
        static std::vector<std::string> extensions;
        static std::vector<std::string> drives;
        static std::vector<std::string> cd_commands;

        // Тип задачи
        FSTaskType task_type;

        // Параметры для URL пути
        std::vector<std::string> url_parts;
        std::string url_result;

        // Параметры для файлового пути
        std::vector<std::string> path_parts;
        std::string filepath_result;

        // Параметры для CD команд
        std::vector<std::string> current_path;
        std::vector<std::string> commands;
        std::string cd_result;
        std::string initial_path_str;

        // Инициализация статических списков
        static void init_static_lists();

        // Вспомогательные методы
        std::string join_vector(const std::vector<std::string>& vec, const std::string& delimiter);
        void apply_difficulty_settings(Difficulty difficulty);

        // Методы генерации задач
        void generate_url_task(const GenerationConfig& config);
        void generate_filepath_task(const GenerationConfig& config);
        void generate_cd_task(const GenerationConfig& config);

    public:
        explicit FilesystemProblem(const ProblemMeta& meta);

        void generate(const GenerationConfig& config) override;
        std::string generate_html() const override;
        bool check_answer(const std::string& user_answer) const;

        void set_page_settings(const PageSettings& settings);
        const PageSettings& get_page_settings() const;
    };

}