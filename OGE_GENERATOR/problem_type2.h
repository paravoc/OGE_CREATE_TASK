#pragma once
#ifndef PROBLEM_TYPE2_H
#define PROBLEM_TYPE2_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

struct sqlite3;

using namespace std;

namespace OGE {

    struct ProblemType2Config {
        int char_count_min = 3;
        int char_count_max = 6;
        string word_category = "";
        int difficulty = 2;
        string cipher_category = "rus"; // rus, eng, dig

        enum class Scenario {
            DECODE,           // Расшифровать сообщение
            DECODE_AND_COUNT, // Расшифровать и посчитать символы
            REPEATED,         // Найти повторяющиеся символы
            FANO_OR_NOT,      // Проверка условия Фано
            ENCODE            // Зашифровать сообщение
        };

        Scenario scenario = Scenario::DECODE;
    };

    struct ProblemType2Result {
        string problem_text;
        string correct_answer;
        string solution_explanation;
        map<string, string> meta;
    };

    class ProblemType2 {
    private:
        sqlite3* db_conn = nullptr;

        // Структуры для шифров
        struct CipherSymbol {
            string cipher;
            string decoded;
            string category;
            int difficulty;
        };

        // Структура для слов (используем те же что в type1)
        struct WordItem {
            string word;
            int length;
            string category;
        };

        vector<CipherSymbol> ciphers_cache;
        vector<WordItem> words_cache;
        vector<string> cipher_categories_cache;

        bool cache_loaded = false;

        // Загрузка из БД
        bool load_ciphers_from_db(const string& category_filter = "");
        bool load_words_from_db(const string& category_filter = "");
        bool load_cipher_categories_from_db();

        // Выбор случайных элементов
        const CipherSymbol& select_random_cipher();
        const WordItem& select_random_word();
        string select_random_cipher_category();

        // Фильтрация
        vector<CipherSymbol> filter_ciphers_by_category(const string& category) const;

        // Генерация текстов
        string generate_encoded_message(const vector<string>& words,
            const vector<CipherSymbol>& symbols) const;
        string generate_problem_text(ProblemType2Config::Scenario scenario,
            const string& encoded_message,
            const vector<CipherSymbol>& symbols,
            const string& additional_info = "") const;

        // Алгоритмы шифрования
        string decode_message(const string& encoded,
            const vector<CipherSymbol>& symbols) const;
        string encode_message(const string& plain,
            const vector<CipherSymbol>& symbols) const;
        int count_character(const string& text, char character) const;
        vector<char> find_repeated_chars(const string& text) const;
        bool check_fano_condition(const vector<CipherSymbol>& symbols) const;

        // Методы генерации для разных сценариев
        ProblemType2Result generate_decode(const ProblemType2Config& config);
        ProblemType2Result generate_decode_and_count(const ProblemType2Config& config);
        ProblemType2Result generate_repeated(const ProblemType2Config& config);
        ProblemType2Result generate_fano_check(const ProblemType2Config& config);
        ProblemType2Result generate_encode(const ProblemType2Config& config);

        // Вспомогательные методы
        ProblemType2Config::Scenario select_scenario_by_difficulty(int difficulty) const;
        vector<string> select_random_words(int count, const string& category = "") const;

    public:
        using Config = ProblemType2Config;
        using Problem = ProblemType2Result;

        // Конструктор принимает уже открытое соединение SQLite
        ProblemType2(sqlite3* connection);

        // Основные методы
        Problem generate(const Config& config);

        // Управление кэшем
        void reload_cache();
        bool is_cache_loaded() const { return cache_loaded; }

        // Генерация HTML контента
        string generate_html_problems(int count, const Config& config);
        string generate_single_problem_html(const Problem& problem, int problem_number = 1);

        // Статистика
        struct Stats {
            int total_ciphers;
            int total_words;
            map<string, int> ciphers_by_category;
        };

        Stats get_stats() const;

        // Статические утилиты
        static bool check_answer(const string& user_answer,
            const string& correct_answer);

        // Вспомогательные статические методы
        static string format_cipher_table(const vector<CipherSymbol>& symbols,
            int max_rows = 10);

        static string create_decode_solution(const string& encoded,
            const string& decoded,
            const vector<CipherSymbol>& symbols);

        static string create_encode_solution(const string& plain,
            const string& encoded,
            const vector<CipherSymbol>& symbols);
    };

} // namespace OGE

#endif // PROBLEM_TYPE2_H