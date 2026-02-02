#ifndef PROBLEM_TYPE1_H
#define PROBLEM_TYPE1_H

#include <string>
#include <vector>
#include <map>
#include <memory>

struct sqlite3;

using namespace std;

namespace OGE {

    struct ProblemType1Config {
        bool allow_removal = true;
        bool allow_addition = false;
        bool allow_encoding_change = false;
        int word_count_min = 3;
        int word_count_max = 6;
        string word_category = "";
        int difficulty = 2;
    };

    struct ProblemType1Result {
        string problem_text;
        string correct_answer;
        string solution_explanation;
        map<string, string> meta;

        string to_json() const;
        string to_html() const;
    };

    class ProblemType1 {
    private:
        // Принимаем уже готовое соединение
        sqlite3* db_conn = nullptr;

        struct WordItem {
            string word;
            int length;
            string category;
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

        bool cache_loaded = false;

        // Загрузка из БД через переданное соединение
        bool load_words_from_db(const string& category_filter = "");
        bool load_encodings_from_db();
        bool load_templates_from_db();

        const string& select_random(const vector<string>& items);
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

        enum class Scenario {
            REMOVAL,
            ADDITION,
            ENCODING_CHANGE
        };

        Scenario select_scenario(const ProblemType1Config& config) const;

        ProblemType1Result generate_removal(const ProblemType1Config& config);
        ProblemType1Result generate_addition(const ProblemType1Config& config);
        ProblemType1Result generate_encoding_change(const ProblemType1Config& config);

    public:
        // Используем объявленные выше структуры
        using Config = ProblemType1Config;
        using Problem = ProblemType1Result;

        // Конструктор принимает уже открытое соединение SQLite
        ProblemType1(sqlite3* connection);

        // Основные методы
        Problem generate(const Config& config);
        vector<Problem> generate_batch(int count, const Config& config);

        // Управление кэшем
        void reload_cache();
        bool is_cache_loaded() const { return cache_loaded; }

        // Статистика
        struct Stats {
            int total_words;
            int total_encodings;
            map<string, int> words_by_category;
        };

        Stats get_stats() const;

        // Статические утилиты
        static bool check_answer(const string& user_answer,
            const string& correct_answer);

        static int compute_size_difference(const string& text1,
            const string& text2,
            int bits_per_char);

        static string format_problem_text(const string& encoding_name,
            const string& encoding_desc,
            const string& original_text,
            int size_diff,
            bool is_removal = true);

        static string create_solution_explanation(const string& original_text,
            const string& modified_text,
            const string& encoding_name,
            int bits_per_char,
            int size_diff,
            const string& target_word,
            bool is_removal = true);
    };

} // namespace OGE

#endif // PROBLEM_TYPE1_H