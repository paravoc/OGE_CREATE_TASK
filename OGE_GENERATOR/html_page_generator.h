// html_page_generator.h - генератор HTML для ANSI (Windows-1251)
#pragma once
#ifndef HTML_PAGE_GENERATOR_H
#define HTML_PAGE_GENERATOR_H

#include <string>
#include <vector>
#include <map>
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

        string format_time(time_t t) const;
        

    public:
        static string nl2br(const string& str);
        HtmlPageGenerator(const PageConfig& cfg = PageConfig());

        // Генерация страницы в ANSI (Windows-1251)
        string generate_ansi_page(const vector<map<string, string>>& problems);

        // Сохранение в файл (ANSI по умолчанию)
        bool save_to_file(const string& content, const string& filename = "");

        // Генерация и сохранение в ANSI
        bool generate_and_save_ansi(const vector<map<string, string>>& problems);
    };

} // namespace OGE

#endif // HTML_PAGE_GENERATOR_H