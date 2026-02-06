#include "problem_type2.h"
#include <sqlite3.h>
#include <random>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <cstring>
#include <set>
#include "html_page_generator.h"

using namespace std;
using namespace OGE;

// Вспомогательная функция для SQL запросов
static vector<vector<string>> sql_query(sqlite3* db, const string& sql) {
    vector<vector<string>> results;

    if (!db) return results;

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return results;
    }

    int column_count = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        vector<string> row;
        for (int i = 0; i < column_count; i++) {
            const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            row.push_back(text ? text : "");
        }
        results.push_back(row);
    }

    sqlite3_finalize(stmt);
    return results;
}

// Конструктор
ProblemType2::ProblemType2(sqlite3* connection) : db_conn(connection) {
    reload_cache();
}

// Загрузка шифров из БД
bool ProblemType2::load_ciphers_from_db(const string& category_filter) {
    ciphers_cache.clear();

    string sql = "SELECT cipher_symbol, decoded_char, category, difficulty FROM type2_ciphers";
    if (!category_filter.empty()) {
        sql += " WHERE category = '" + category_filter + "'";
    }
    sql += " ORDER BY difficulty";

    auto results = sql_query(db_conn, sql);

    for (const auto& row : results) {
        if (row.size() >= 4) {
            CipherSymbol symbol;
            symbol.cipher = row[0];
            symbol.decoded = row[1];
            symbol.category = row[2];
            symbol.difficulty = stoi(row[3]);
            ciphers_cache.push_back(symbol);
        }
    }

    return !ciphers_cache.empty();
}

// Загрузка слов из БД (используем те же что в type1)
bool ProblemType2::load_words_from_db(const string& category_filter) {
    words_cache.clear();

    string sql = "SELECT word, length, category FROM type1_words";
    if (!category_filter.empty()) {
        sql += " WHERE category = '" + category_filter + "'";
    }

    auto results = sql_query(db_conn, sql);

    for (const auto& row : results) {
        if (row.size() >= 3) {
            WordItem item;
            item.word = row[0];
            item.length = stoi(row[1]);
            item.category = row[2];
            words_cache.push_back(item);
        }
    }

    return !words_cache.empty();
}

// Загрузка категорий шифров
bool ProblemType2::load_cipher_categories_from_db() {
    cipher_categories_cache.clear();

    string sql = "SELECT DISTINCT category FROM type2_ciphers ORDER BY category";
    auto results = sql_query(db_conn, sql);

    for (const auto& row : results) {
        if (!row.empty()) {
            cipher_categories_cache.push_back(row[0]);
        }
    }

    return !cipher_categories_cache.empty();
}

// Обновление кэша
void ProblemType2::reload_cache() {
    cache_loaded = load_ciphers_from_db() &&
        load_words_from_db() &&
        load_cipher_categories_from_db();
}

// Выбор случайного шифра
const ProblemType2::CipherSymbol& ProblemType2::select_random_cipher() {
    static CipherSymbol empty_symbol{ "", "", "", 0 };

    if (ciphers_cache.empty()) return empty_symbol;

    static default_random_engine rng(random_device{}());
    uniform_int_distribution<size_t> dist(0, ciphers_cache.size() - 1);
    return ciphers_cache[dist(rng)];
}

// Выбор случайного слова
const ProblemType2::WordItem& ProblemType2::select_random_word() {
    static WordItem empty_item{ "", 0, "" };

    if (words_cache.empty()) return empty_item;

    static default_random_engine rng(random_device{}());
    uniform_int_distribution<size_t> dist(0, words_cache.size() - 1);
    return words_cache[dist(rng)];
}

// Выбор случайной категории шифров
string ProblemType2::select_random_cipher_category() {
    if (cipher_categories_cache.empty()) return "rus";

    static default_random_engine rng(random_device{}());
    uniform_int_distribution<size_t> dist(0, cipher_categories_cache.size() - 1);
    return cipher_categories_cache[dist(rng)];
}

// Фильтрация шифров по категории
vector<ProblemType2::CipherSymbol> ProblemType2::filter_ciphers_by_category(const string& category) const {
    if (category.empty()) return ciphers_cache;

    vector<CipherSymbol> filtered;
    for (const auto& symbol : ciphers_cache) {
        if (symbol.category == category) {
            filtered.push_back(symbol);
        }
    }
    return filtered;
}

// Генерация закодированного сообщения
string ProblemType2::generate_encoded_message(const vector<string>& words,
    const vector<CipherSymbol>& symbols) const {
    stringstream encoded;

    for (const auto& word : words) {
        for (char c : word) {
            char upper_c = toupper(c);
            bool found = false;

            // Ищем символ в таблице шифров
            for (const auto& symbol : symbols) {
                if (symbol.decoded.size() == 1 && symbol.decoded[0] == upper_c) {
                    encoded << symbol.cipher << " ";
                    found = true;
                    break;
                }
            }

            // Если не нашли, используем пробел
            if (!found) {
                encoded << "  ";
            }
        }
        encoded << "   "; // Три пробела между словами
    }

    return encoded.str();
}

// Дешифровка сообщения
string ProblemType2::decode_message(const string& encoded,
    const vector<CipherSymbol>& symbols) const {
    stringstream decoded;
    stringstream token;

    for (char c : encoded) {
        if (c == ' ' || c == '\t') {
            string token_str = token.str();
            if (!token_str.empty()) {
                // Ищем токен в таблице символов
                for (const auto& symbol : symbols) {
                    if (symbol.cipher == token_str) {
                        decoded << symbol.decoded;
                        break;
                    }
                }
                token.str("");
            }
            if (c == ' ' && decoded.str().back() != ' ') {
                // Добавляем пробел только если его нет
                decoded << " ";
            }
        }
        else {
            token << c;
        }
    }

    // Обрабатываем последний токен
    string token_str = token.str();
    if (!token_str.empty()) {
        for (const auto& symbol : symbols) {
            if (symbol.cipher == token_str) {
                decoded << symbol.decoded;
                break;
            }
        }
    }

    return decoded.str();
}

// Шифрование сообщения
string ProblemType2::encode_message(const string& plain,
    const vector<CipherSymbol>& symbols) const {
    stringstream encoded;

    for (char c : plain) {
        char upper_c = toupper(c);
        bool found = false;

        for (const auto& symbol : symbols) {
            if (symbol.decoded.size() == 1 && symbol.decoded[0] == upper_c) {
                encoded << symbol.cipher << " ";
                found = true;
                break;
            }
        }

        if (!found && c != ' ') {
            encoded << "? ";
        }
        else if (c == ' ') {
            encoded << "   "; // Три пробела для разделения слов
        }
    }

    return encoded.str();
}

// Подсчет символов
int ProblemType2::count_character(const string& text, char character) const {
    int count = 0;
    for (char c : text) {
        if (toupper(c) == toupper(character)) {
            count++;
        }
    }
    return count;
}

// Поиск повторяющихся символов
vector<char> ProblemType2::find_repeated_chars(const string& text) const {
    vector<char> repeated;
    map<char, int> char_count;

    for (char c : text) {
        if (isalpha(c) || isdigit(c)) {
            char upper_c = toupper(c);
            char_count[upper_c]++;
        }
    }

    for (const auto& [character, count] : char_count) {
        if (count > 1) {
            repeated.push_back(character);
        }
    }

    sort(repeated.begin(), repeated.end());
    return repeated;
}

// Проверка условия Фано
bool ProblemType2::check_fano_condition(const vector<CipherSymbol>& symbols) const {
    // Условие Фано: ни одно кодовое слово не является началом другого
    for (size_t i = 0; i < symbols.size(); i++) {
        for (size_t j = 0; j < symbols.size(); j++) {
            if (i != j) {
                const string& code1 = symbols[i].cipher;
                const string& code2 = symbols[j].cipher;

                if (code1.empty() || code2.empty()) continue;

                // Проверяем, является ли code1 началом code2
                if (code2.length() >= code1.length() &&
                    code2.substr(0, code1.length()) == code1) {
                    return false;
                }
            }
        }
    }
    return true;
}

// Выбор сценария по сложности
ProblemType2Config::Scenario ProblemType2::select_scenario_by_difficulty(int difficulty) const {
    static default_random_engine rng(random_device{}());

    if (difficulty <= 2) {
        return ProblemType2Config::Scenario::DECODE;
    }
    else if (difficulty == 3) {
        vector<ProblemType2Config::Scenario> options = {
            ProblemType2Config::Scenario::DECODE_AND_COUNT,
            ProblemType2Config::Scenario::REPEATED
        };
        uniform_int_distribution<int> dist(0, options.size() - 1);
        return options[dist(rng)];
    }
    else {
        vector<ProblemType2Config::Scenario> options = {
            ProblemType2Config::Scenario::FANO_OR_NOT,
            ProblemType2Config::Scenario::ENCODE
        };
        uniform_int_distribution<int> dist(0, options.size() - 1);
        return options[dist(rng)];
    }
}

// Выбор случайных слов
vector<string> ProblemType2::select_random_words(int count, const string& category) const {
    vector<string> selected_words;

    // Фильтруем слова по категории если нужно
    vector<WordItem> filtered_words;
    if (category.empty()) {
        filtered_words = words_cache;
    }
    else {
        for (const auto& word : words_cache) {
            if (word.category == category) {
                filtered_words.push_back(word);
            }
        }
    }

    if (filtered_words.empty()) return selected_words;

    static default_random_engine rng(random_device{}());
    uniform_int_distribution<size_t> dist(0, filtered_words.size() - 1);

    // Выбираем случайные слова, избегая повторов
    set<string> used_words;
    while (selected_words.size() < static_cast<size_t>(count) && used_words.size() < filtered_words.size()) {
        size_t idx = dist(rng);
        const string& word = filtered_words[idx].word;

        if (used_words.find(word) == used_words.end()) {
            selected_words.push_back(word);
            used_words.insert(word);
        }
    }

    return selected_words;
}

// Генерация задачи на дешифровку
ProblemType2Result ProblemType2::generate_decode(const ProblemType2Config& config) {
    ProblemType2Result result;

    // Получаем шифры нужной категории
    auto symbols = filter_ciphers_by_category(config.cipher_category);
    if (symbols.empty()) {
        result.problem_text = "Нет доступных шифров для выбранной категории";
        return result;
    }

    // Выбираем случайные слова
    int word_count = config.char_count_min +
        (rand() % (config.char_count_max - config.char_count_min + 1));

    auto words = select_random_words(word_count, config.word_category);
    if (words.empty()) {
        words = select_random_words(word_count, ""); // Берем любые слова
    }

    // Генерируем закодированное сообщение
    string encoded_message = generate_encoded_message(words, symbols);
    string decoded_message = decode_message(encoded_message, symbols);

    // Формируем текст задачи
    stringstream problem_ss;
    problem_ss << "Расшифруйте сообщение, закодированное с помощью следующего кода:\n\n";

    // Показываем таблицу кодирования
    problem_ss << "Таблица кодирования:\n";
    int count = 0;
    for (const auto& symbol : symbols) {
        if (count >= 12) break;
        problem_ss << symbol.cipher << " → " << symbol.decoded << "\n";
        count++;
    }

    problem_ss << "\nЗакодированное сообщение:\n";
    problem_ss << encoded_message << "\n\n";
    problem_ss << "Введите расшифрованный текст в поле ответа.";

    // Формируем объяснение решения
    string solution_text = create_decode_solution(encoded_message, decoded_message, symbols);

    result.problem_text = problem_ss.str();
    result.correct_answer = decoded_message;
    result.solution_explanation = HtmlPageGenerator::nl2br(solution_text);

    // Метаданные
    result.meta["scenario"] = "decode";
    result.meta["cipher_category"] = config.cipher_category;
    result.meta["word_count"] = to_string(word_count);
    result.meta["encoded_length"] = to_string(encoded_message.length());

    return result;
}

// Генерация задачи на дешифровку и подсчет
ProblemType2Result ProblemType2::generate_decode_and_count(const ProblemType2Config& config) {
    ProblemType2Result result;

    auto symbols = filter_ciphers_by_category(config.cipher_category);
    if (symbols.empty()) {
        result.problem_text = "Нет доступных шифров для выбранной категории";
        return result;
    }

    // Выбираем случайные слова
    int word_count = config.char_count_min +
        (rand() % (config.char_count_max - config.char_count_min + 1));

    auto words = select_random_words(word_count, config.word_category);
    if (words.empty()) {
        words = select_random_words(word_count, "");
    }

    // Генерируем сообщение
    string encoded_message = generate_encoded_message(words, symbols);
    string decoded_message = decode_message(encoded_message, symbols);

    // Выбираем символ для подсчета
    if (decoded_message.empty()) {
        result.problem_text = "Не удалось сгенерировать сообщение";
        return result;
    }

    // Выбираем случайный символ из расшифрованного сообщения
    string valid_chars;
    for (char c : decoded_message) {
        if (isalnum(c)) {
            valid_chars += c;
        }
    }

    if (valid_chars.empty()) {
        valid_chars = "ABCDE";
    }

    char target_char = valid_chars[rand() % valid_chars.size()];
    int char_count = count_character(decoded_message, target_char);

    // Формируем задачу
    stringstream problem_ss;
    problem_ss << "Расшифруйте сообщение и подсчитайте, сколько раз в нём встречается символ '";
    problem_ss << target_char << "':\n\n";

    problem_ss << "Таблица кодирования:\n";
    int count = 0;
    for (const auto& symbol : symbols) {
        if (count >= 10) break;
        problem_ss << symbol.cipher << " → " << symbol.decoded << "\n";
        count++;
    }

    problem_ss << "\nЗакодированное сообщение:\n";
    problem_ss << encoded_message << "\n\n";
    problem_ss << "Введите ответ в формате: <расшифрованный текст> (<количество> раз)";

    // Формируем решение
    stringstream solution_ss;
    solution_ss << "Решение:\n\n";
    solution_ss << "1. Расшифровываем сообщение: " << decoded_message << "\n\n";
    solution_ss << "2. Подсчитываем символ '" << target_char << "':\n";

    int counter = 0;
    for (char c : decoded_message) {
        if (toupper(c) == toupper(target_char)) {
            counter++;
            solution_ss << "   Позиция " << counter << ": '" << c << "'\n";
        }
    }

    solution_ss << "\n3. Всего найдено: " << char_count << " раз\n";
    solution_ss << "\nОтвет: " << decoded_message << " (" << char_count << " раз)";

    result.problem_text = problem_ss.str();
    result.correct_answer = decoded_message + " (" + to_string(char_count) + " раз)";
    result.solution_explanation = HtmlPageGenerator::nl2br(solution_ss.str());

    result.meta["scenario"] = "decode_and_count";
    result.meta["target_char"] = string(1, target_char);
    result.meta["char_count"] = to_string(char_count);

    return result;
}

// Генерация задачи на проверку условия Фано
ProblemType2Result ProblemType2::generate_fano_check(const ProblemType2Config& config) {
    ProblemType2Result result;

    // Получаем случайные символы
    auto all_symbols = filter_ciphers_by_category(config.cipher_category);
    if (all_symbols.size() < 3) {
        result.problem_text = "Недостаточно символов для проверки условия Фано";
        return result;
    }

    static default_random_engine rng(static_cast<unsigned>(time(nullptr)));
    shuffle(all_symbols.begin(), all_symbols.end(), rng);
    int symbol_count = 3 + (rand() % 4);
    if (symbol_count > static_cast<int>(all_symbols.size())) {
        symbol_count = all_symbols.size();
    }

    vector<CipherSymbol> selected_symbols(all_symbols.begin(),
        all_symbols.begin() + symbol_count);

    // Проверяем условие Фано
    bool is_fano = check_fano_condition(selected_symbols);

    // Иногда искусственно нарушаем условие для разнообразия
    if ((rand() % 4) == 0 && is_fano && selected_symbols.size() > 2) {
        // Искусственно создаем нарушение Фано
        selected_symbols[1].cipher = selected_symbols[0].cipher + "X";
        is_fano = false;
    }

    // Формируем задачу
    stringstream problem_ss;
    problem_ss << "Проверьте, удовлетворяет ли данный код условию Фано:\n\n";
    problem_ss << "Условие Фано: ни одно кодовое слово не должно быть началом другого.\n\n";
    problem_ss << "Таблица кодирования:\n";

    for (const auto& symbol : selected_symbols) {
        problem_ss << symbol.cipher << " → " << symbol.decoded << "\n";
    }

    problem_ss << "\nВведите 'ДА', если код удовлетворяет условию Фано, или 'НЕТ', если не удовлетворяет.";

    // Формируем решение
    stringstream solution_ss;
    solution_ss << "Решение:\n\n";
    solution_ss << "1. Проверяем каждую пару кодовых слов:\n";

    bool violation_found = false;
    for (size_t i = 0; i < selected_symbols.size(); i++) {
        for (size_t j = 0; j < selected_symbols.size(); j++) {
            if (i != j) {
                const string& code1 = selected_symbols[i].cipher;
                const string& code2 = selected_symbols[j].cipher;

                if (code2.length() >= code1.length() &&
                    code2.substr(0, code1.length()) == code1) {
                    solution_ss << "   Нарушение: " << code1 << " является началом " << code2 << "\n";
                    violation_found = true;
                }
            }
        }
    }

    if (!violation_found) {
        solution_ss << "   Нарушений не найдено.\n";
    }

    solution_ss << "\n2. Результат проверки: ";
    solution_ss << (is_fano ? "ДА (удовлетворяет)" : "НЕТ (не удовлетворяет)");

    result.problem_text = problem_ss.str();
    result.correct_answer = is_fano ? "ДА" : "НЕТ";
    result.solution_explanation = HtmlPageGenerator::nl2br(solution_ss.str());

    result.meta["scenario"] = "fano_check";
    result.meta["is_fano"] = is_fano ? "true" : "false";
    result.meta["symbol_count"] = to_string(selected_symbols.size());

    return result;
}

// Генерация задачи на шифрование
ProblemType2Result ProblemType2::generate_encode(const ProblemType2Config& config) {
    ProblemType2Result result;

    auto symbols = filter_ciphers_by_category(config.cipher_category);
    if (symbols.empty()) {
        result.problem_text = "Нет доступных шифров для выбранной категории";
        return result;
    }

    // Выбираем слово для шифрования
    auto words = select_random_words(1, config.word_category);
    if (words.empty()) {
        words = select_random_words(1, "");
    }

    if (words.empty()) {
        result.problem_text = "Не удалось выбрать слово для шифрования";
        return result;
    }

    string word_to_encode = words[0];

    // Шифруем слово
    string encoded_message = encode_message(word_to_encode, symbols);

    // Формируем задачу
    stringstream problem_ss;
    problem_ss << "Зашифруйте слово \"" << word_to_encode;
    problem_ss << "\", используя следующую таблицу кодирования:\n\n";

    problem_ss << "Таблица кодирования:\n";
    int count = 0;
    for (const auto& symbol : symbols) {
        if (count >= 12) break;
        problem_ss << symbol.cipher << " → " << symbol.decoded << "\n";
        count++;
    }

    problem_ss << "\nВведите зашифрованное сообщение (разделяйте коды пробелами).";

    // Формируем решение
    string solution_text = create_encode_solution(word_to_encode, encoded_message, symbols);

    result.problem_text = problem_ss.str();
    result.correct_answer = encoded_message;
    result.solution_explanation = HtmlPageGenerator::nl2br(solution_text);

    result.meta["scenario"] = "encode";
    result.meta["original_word"] = word_to_encode;
    result.meta["word_length"] = to_string(word_to_encode.length());

    return result;
}

// Генерация задачи на повторяющиеся символы
ProblemType2Result ProblemType2::generate_repeated(const ProblemType2Config& config) {
    ProblemType2Result result;

    auto symbols = filter_ciphers_by_category(config.cipher_category);
    if (symbols.empty()) {
        result.problem_text = "Нет доступных шифров для выбранной категории";
        return result;
    }

    // Выбираем слова
    int word_count = config.char_count_min +
        (rand() % (config.char_count_max - config.char_count_min + 1));

    auto words = select_random_words(word_count, config.word_category);
    if (words.empty()) {
        words = select_random_words(word_count, "");
    }

    // Специально добавляем повторяющиеся символы
    set<char> used_chars;
    vector<char> repeated_chars;

    for (const auto& word : words) {
        for (char c : word) {
            char upper_c = toupper(c);
            if (used_chars.count(upper_c)) {
                repeated_chars.push_back(upper_c);
            }
            else {
                used_chars.insert(upper_c);
            }
        }
    }

    // Если нет повторений, добавляем их искусственно
    if (repeated_chars.empty() && !words.empty() && words.size() > 1) {
        char first_char = toupper(words[0][0]);
        repeated_chars.push_back(first_char);
        // Добавляем тот же символ в другое слово
        words[1] = string(1, first_char) + words[1].substr(1);
    }

    // Генерируем сообщение
    string encoded_message = generate_encoded_message(words, symbols);
    string decoded_message = decode_message(encoded_message, symbols);

    // Находим действительно повторяющиеся символы
    vector<char> actual_repeated = find_repeated_chars(decoded_message);

    // Формируем задачу
    stringstream problem_ss;
    problem_ss << "Расшифруйте сообщение и найдите все символы, которые встречаются ";
    problem_ss << "в нём более одного раза:\n\n";

    problem_ss << "Таблица кодирования:\n";
    int count = 0;
    for (const auto& symbol : symbols) {
        if (count >= 10) break;
        problem_ss << symbol.cipher << " → " << symbol.decoded << "\n";
        count++;
    }

    problem_ss << "\nЗакодированное сообщение:\n";
    problem_ss << encoded_message << "\n\n";
    problem_ss << "Введите ответ в формате: <расшифрованный текст> [<повторяющиеся символы>]";

    // Формируем решение
    stringstream solution_ss;
    solution_ss << "Решение:\n\n";
    solution_ss << "1. Расшифровываем сообщение: " << decoded_message << "\n\n";
    solution_ss << "2. Подсчитываем вхождения каждого символа:\n";

    map<char, int> char_count;
    for (char c : decoded_message) {
        if (isalnum(c)) {
            char upper_c = toupper(c);
            char_count[upper_c]++;
        }
    }

    for (const auto& [ch, cnt] : char_count) {
        solution_ss << "   '" << ch << "': " << cnt << " раз\n";
    }

    solution_ss << "\n3. Символы, встречающиеся более одного раза: ";
    if (actual_repeated.empty()) {
        solution_ss << "нет таких символов";
    }
    else {
        for (size_t i = 0; i < actual_repeated.size(); i++) {
            if (i > 0) solution_ss << ", ";
            solution_ss << "'" << actual_repeated[i] << "'";
        }
    }

    // Формируем правильный ответ
    stringstream answer_ss;
    answer_ss << decoded_message << " [";
    for (size_t i = 0; i < actual_repeated.size(); i++) {
        if (i > 0) answer_ss << ", ";
        answer_ss << actual_repeated[i];
    }
    answer_ss << "]";

    result.problem_text = problem_ss.str();
    result.correct_answer = answer_ss.str();
    result.solution_explanation = HtmlPageGenerator::nl2br(solution_ss.str());

    result.meta["scenario"] = "repeated";
    result.meta["repeated_count"] = to_string(actual_repeated.size());

    return result;
}

// Основной метод генерации
ProblemType2Result ProblemType2::generate(const Config& config) {
    if (!cache_loaded) {
        reload_cache();
        if (!cache_loaded) {
            throw runtime_error("Не удалось загрузить данные из БД");
        }
    }

    // Определяем сценарий
    ProblemType2Config::Scenario scenario = config.scenario;
    if (scenario == ProblemType2Config::Scenario::DECODE) {
        scenario = select_scenario_by_difficulty(config.difficulty);
    }

    // Генерируем задачу в зависимости от сценария
    switch (scenario) {
    case ProblemType2Config::Scenario::DECODE:
        return generate_decode(config);

    case ProblemType2Config::Scenario::DECODE_AND_COUNT:
        return generate_decode_and_count(config);

    case ProblemType2Config::Scenario::REPEATED:
        return generate_repeated(config);

    case ProblemType2Config::Scenario::FANO_OR_NOT:
        return generate_fano_check(config);

    case ProblemType2Config::Scenario::ENCODE:
        return generate_encode(config);

    default:
        return generate_decode(config);
    }
}

// Проверка ответа
bool ProblemType2::check_answer(const string& user_answer,
    const string& correct_answer) {
    string user_lower = user_answer;
    string correct_lower = correct_answer;

    transform(user_lower.begin(), user_lower.end(), user_lower.begin(), ::toupper);
    transform(correct_lower.begin(), correct_lower.end(), correct_lower.begin(), ::toupper);

    // Удаляем лишние пробелы
    auto remove_extra_spaces = [](const string& s) -> string {
        string result;
        bool last_was_space = false;
        for (char c : s) {
            if (isspace(c)) {
                if (!last_was_space) {
                    result += ' ';
                    last_was_space = true;
                }
            }
            else {
                result += c;
                last_was_space = false;
            }
        }

        // Удаляем пробелы в начале и конце
        size_t start = result.find_first_not_of(" ");
        size_t end = result.find_last_not_of(" ");
        if (start == string::npos || end == string::npos) {
            return "";
        }
        return result.substr(start, end - start + 1);
        };

    user_lower = remove_extra_spaces(user_lower);
    correct_lower = remove_extra_spaces(correct_lower);

    return user_lower == correct_lower;
}

// Получение статистики
ProblemType2::Stats ProblemType2::get_stats() const {
    Stats stats;
    stats.total_ciphers = static_cast<int>(ciphers_cache.size());
    stats.total_words = static_cast<int>(words_cache.size());

    for (const auto& symbol : ciphers_cache) {
        stats.ciphers_by_category[symbol.category]++;
    }

    return stats;
}

// Генерация HTML для нескольких задач
string ProblemType2::generate_html_problems(int count, const Config& config) {
    stringstream html;

    for (int i = 0; i < count; i++) {
        auto problem = generate(config);
        html << generate_single_problem_html(problem, i + 1);
    }

    return html.str();
}

// Генерация HTML для одной задачи
string ProblemType2::generate_single_problem_html(const Problem& problem, int problem_number) {
    stringstream html;

    string scenario = "unknown";
    string cipher_category = "unknown";

    auto it_scenario = problem.meta.find("scenario");
    if (it_scenario != problem.meta.end()) {
        scenario = it_scenario->second;
    }

    auto it_category = problem.meta.find("cipher_category");
    if (it_category != problem.meta.end()) {
        cipher_category = it_category->second;
    }

    string escaped_text = problem.problem_text;
    string escaped_solution = problem.solution_explanation;
    string escaped_answer = problem.correct_answer;

    html << R"(<article class="cosmic-problem" data-id=")" << problem_number
        << R"(" data-type="type2" data-scenario=")" << scenario << R"(">
        <div class="problem-header">
            <div class="problem-id">
                <span class="id-number">#)" << problem_number << R"(</span>
                <span class="id-type">ТИП 2</span>
            </div>
            <div class="problem-meta">
                <span class="meta-item">
                    <i class="fas fa-key"></i>
                    )" << cipher_category << R"(
                </span>
                <span class="meta-item">
                    <i class="fas fa-brain"></i>
                    )" << scenario << R"(
                </span>
                <span class="meta-item">
                    <i class="fas fa-clock"></i>
                    7 мин
                </span>
            </div>
            <button class="problem-expand">
                <i class="fas fa-chevron-down"></i>
            </button>
        </div>
        
        <div class="problem-content" style="max-height: 0; opacity: 0; overflow: hidden;">
            <div class="problem-text">
                )" << escaped_text << R"(
            </div>
            
            <div class="problem-actions">
                <div class="answer-field">
                    <input type="text" 
                           placeholder="Введите ваш ответ..." 
                           class="cosmic-input">
                    <button class="cosmic-btn cosmic-btn-check">
                        <i class="fas fa-check"></i>
                        ПРОВЕРИТЬ
                    </button>
                </div>
                
                <div class="solution-controls">
                    <button class="show-solution-btn">
                        <i class="fas fa-eye"></i>
                        ПОКАЗАТЬ РЕШЕНИЕ
                    </button>
                    <button class="hide-solution-btn" style="display: none;">
                        <i class="fas fa-eye-slash"></i>
                        СКРЫТЬ РЕШЕНИЕ
                    </button>
                </div>
            </div>
            
            <div class="problem-solution hidden">
                <div class="solution-header">
                    <h3><i class="fas fa-cogs"></i> РЕШЕНИЕ</h3>
                </div>
                <div class="solution-content">
                    )" << escaped_solution << R"(
                    <p class="answer-final">ОТВЕТ: <strong>)"
        << escaped_answer << R"(</strong></p>
                </div>
            </div>
        </div>
        
        <div class="problem-footer">
            <div class="status-indicator">
                <div class="status-dot"></div>
                <span>Не решено</span>
            </div>
            <div class="problem-stats">
                <span><i class="fas fa-code"></i> Шифрование</span>
            </div>
        </div>
    </article>)";

    return html.str();
}

// Статические методы

string ProblemType2::format_cipher_table(const vector<CipherSymbol>& symbols, int max_rows) {
    stringstream ss;
    int count = 0;

    for (const auto& symbol : symbols) {
        if (count >= max_rows) break;
        ss << symbol.cipher << " → " << symbol.decoded << "\n";
        count++;
    }

    return ss.str();
}

string ProblemType2::create_decode_solution(const string& encoded,
    const string& decoded,
    const vector<CipherSymbol>& symbols) {
    stringstream ss;
    ss << "Решение:\n\n";
    ss << "1. Разбиваем закодированное сообщение на отдельные коды:\n";

    // Разбиваем на токены
    vector<string> tokens;
    string token;
    for (char c : encoded) {
        if (c == ' ') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }
        else {
            token += c;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }

    for (size_t i = 0; i < tokens.size(); i++) {
        if (i > 0) ss << " ";
        ss << tokens[i];
    }
    ss << "\n\n";

    ss << "2. Заменяем каждый код соответствующим символом:\n";
    for (size_t i = 0; i < tokens.size(); i++) {
        for (const auto& symbol : symbols) {
            if (symbol.cipher == tokens[i]) {
                ss << "   " << tokens[i] << " → " << symbol.decoded << "\n";
                break;
            }
        }
    }

    ss << "\n3. Получаем текст: " << decoded;

    return ss.str();
}

string ProblemType2::create_encode_solution(const string& plain,
    const string& encoded,
    const vector<CipherSymbol>& symbols) {
    stringstream ss;
    ss << "Решение:\n\n";
    ss << "1. Буквы слова \"" << plain << "\":\n";

    for (char c : plain) {
        char upper_c = toupper(c);
        bool found = false;

        for (const auto& symbol : symbols) {
            if (symbol.decoded.size() == 1 && symbol.decoded[0] == upper_c) {
                ss << "   " << c << " → " << symbol.cipher << "\n";
                found = true;
                break;
            }
        }

        if (!found) {
            ss << "   " << c << " → (не найден в таблице)\n";
        }
    }

    ss << "\n2. Зашифрованное сообщение: " << encoded;

    return ss.str();
}