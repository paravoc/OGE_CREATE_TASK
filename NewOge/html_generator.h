// html_generator.h
#pragma once

#include "page_setting.h"
#include "problem_base.h"
#include "math_problem.h"
#include "decimal_problem.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

using namespace std;

namespace OGE {

    class HtmlGenerator {
    private:
        PageSettings page_settings;
        vector<unique_ptr<ProblemBase>> all_problems;
        string html_page;

        // Приватные методы
        string generate_header() const;
        string generate_footer() const;
        string generate_problem_list() const;
        string generate_scripts() const;
        string generate_styles() const;

    public:
        // Конструкторы
        explicit HtmlGenerator(const PageSettings& settings);

        // Деструктор
        ~HtmlGenerator() = default;

        // Генерация всех задач
        void generate_all_problems();

        // Добавление конкретной задачи
        void add_problem(unique_ptr<ProblemBase> problem);

        // Генерация полной HTML страницы
        string generate_full_page();

        // Генерация только контента (без header/footer)
        string generate_content_only();

        // Получение сгенерированной страницы
        const string& get_html_page() const { return html_page; }

        // Очистка всех задач
        void clear_problems();

        // Сохранение в файл
        bool save_to_file(const string& filename) const;

        // Статистика по сгенерированным задачам
        int get_problems_count() const { return all_problems.size(); }
        const vector<unique_ptr<ProblemBase>>& get_all_problems() const { return all_problems; }
    };

}