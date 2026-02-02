// problem_type1.h
#ifndef PROBLEM_TYPE1_H
#define PROBLEM_TYPE1_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <pqxx/pqxx>

using namespace std;

namespace OGE {

    class ProblemType1 {
    private:
        shared_ptr<pqxx::connection> db_conn;

        // Простая структура данных
        struct WordItem {
            string word;
            int length;
            string category; // для фильтрации если нужно
        };

        struct EncodingInfo {
            string name;
            int bits_per_char;
            string description;
        };

        vector<WordItem> words_cache;
        vector<EncodingInfo> encodings_cache;
        vector<string> prefixes_cache;
        vector<string> suffixes_cache;
        vector<string> delimiters_cache;

        // Загрузка из БД
        void load_words_from_db(const string& category_filter = "");
        void load_encodings_from_db();
        void load_templates_from_db();

        // Вспомогательные методы
        string select_random(const vector<string>& items);
        const WordItem& select_random_word();
        const EncodingInfo& select_random_encoding();

        string build_text(const vector<string>& words,
            const string& prefix,
            const string& suffix,
            const string& delimiter) const;

        int calculate_size(const string& text, int bits_per_char) const;
        string modify_text(const string& text,
            const string& target_word,
            const string& delimiter,
            bool is_removal) const;

    public:
        // Конфигурация
        struct Config {
            bool allow_removal = true;
            bool allow_addition = false;
            bool allow_encoding_change = false;
            int word_count_min = 3;
            int word_count_max = 6;
            string word_category = ""; // если пусто - все категории
            int difficulty = 2; // 1-легко, 2-средне, 3-сложно
        };

        // Результат
        struct Problem {
            string problem_text;
            string correct_answer;
            string solution_explanation;
            map<string, string> meta; // размеры, кодировка и т.д.

            string to_json() const;
            string to_html() const;
        };

        // Конструктор
        ProblemType1(shared_ptr<pqxx::connection> conn);

        // Основные методы
        Problem generate(const Config& config);
        vector<Problem> generate_batch(int count, const Config& config);

        // Статистика
        struct Stats {
            int total_words;
            int total_encodings;
            map<string, int> words_by_category;
        };

        Stats get_stats() const;

        // Валидация
        static bool check_answer(const string& user_answer,
            const string& correct_answer);
    };

} // namespace OGE

#endif // PROBLEM_TYPE1_H