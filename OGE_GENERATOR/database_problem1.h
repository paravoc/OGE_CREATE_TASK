#pragma once
#ifndef DATABASE_PROBLEM1_H
#define DATABASE_PROBLEM1_H

#include <string>
#include <vector>
#include <memory>

// Forward declaration
struct sqlite3;

namespace OGE {

    class DatabaseProblem1 {
    private:
        sqlite3* db = nullptr;

    public:
        DatabaseProblem1() = default;
        explicit DatabaseProblem1(sqlite3* database) : db(database) {}

        void set_connection(sqlite3* database) { db = database; }

        // Структуры данных
        struct WordRecord {
            std::string word;
            int length;
            std::string category;
        };

        struct EncodingRecord {
            std::string name;
            int bits_per_char;
            std::string description;
        };

        // Создание таблиц
        bool create_tables();

        // Заполнение данными
        bool seed_data();

        // CRUD операции
        std::vector<WordRecord> get_words(const std::string& category = "");
        std::vector<EncodingRecord> get_encodings();
        bool add_word(const std::string& word, const std::string& category);
        bool add_encoding(const std::string& name, int bits, const std::string& desc);

        // Шаблоны текста
        struct TextTemplate {
            std::string prefix;
            std::string suffix;
            std::string delimiter;
        };

        std::vector<TextTemplate> get_templates();
        bool add_template(const std::string& prefix, const std::string& suffix,
            const std::string& delimiter);

        // Статистика
        int get_word_count(const std::string& category = "");
        int get_encoding_count();

        // Очистка данных (для тестов)
        bool clear_data();

        // Проверка существования таблиц
        bool tables_exist() const;
    };

} // namespace OGE

#endif // DATABASE_PROBLEM1_H