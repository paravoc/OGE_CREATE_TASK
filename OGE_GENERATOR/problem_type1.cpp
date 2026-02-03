#include "problem_type1.h"
#include <sqlite3.h>
#include <random>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <cstring>

using namespace std;
using namespace OGE;

// Вспомогательная функция для выполнения SQL запроса
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
ProblemType1::ProblemType1(sqlite3* connection) : db_conn(connection) {
    // Инициализируем кэш
    reload_cache();
}

// Загрузка слов из БД
bool ProblemType1::load_words_from_db(const string& category_filter) {
    words_cache.clear();

    string sql = "SELECT word, length, category FROM problem1_words";
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

// Загрузка кодировок из БД
bool ProblemType1::load_encodings_from_db() {
    encodings_cache.clear();

    string sql = "SELECT name, bits_per_char, description FROM problem1_encodings";
    auto results = sql_query(db_conn, sql);

    for (const auto& row : results) {
        if (row.size() >= 3) {
            EncodingInfo info;
            info.name = row[0];
            info.bits_per_char = stoi(row[1]);
            info.description = row[2];
            encodings_cache.push_back(info);
        }
    }

    return !encodings_cache.empty();
}

// Загрузка шаблонов из БД
bool ProblemType1::load_templates_from_db() {
    prefixes_cache.clear();
    suffixes_cache.clear();
    delimiters_cache.clear();

    string sql = "SELECT prefix, suffix, delimiter FROM problem1_text_templates";
    auto results = sql_query(db_conn, sql);

    for (const auto& row : results) {
        if (row.size() >= 3) {
            prefixes_cache.push_back(row[0]);
            suffixes_cache.push_back(row[1]);
            delimiters_cache.push_back(row[2]);
        }
    }

    return !prefixes_cache.empty();
}

// Перезагрузка всего кэша
void ProblemType1::reload_cache() {
    cache_loaded = load_words_from_db() &&
        load_encodings_from_db() &&
        load_templates_from_db();
}

// Выбор случайного элемента
const string& ProblemType1::select_random(const vector<string>& items) {
    static default_random_engine rng(random_device{}());
    static string empty_string;

    if (items.empty()) return empty_string;

    uniform_int_distribution<size_t> dist(0, items.size() - 1);
    return items[dist(rng)];
}

const ProblemType1::WordItem& ProblemType1::select_random_word() {
    static default_random_engine rng(random_device{}());
    static WordItem empty_item{ "", 0, "" };

    if (words_cache.empty()) return empty_item;

    uniform_int_distribution<size_t> dist(0, words_cache.size() - 1);
    return words_cache[dist(rng)];
}

const ProblemType1::EncodingInfo& ProblemType1::select_random_encoding() {
    static default_random_engine rng(random_device{}());
    static EncodingInfo empty_item{ "", 0, "" };

    if (encodings_cache.empty()) return empty_item;

    uniform_int_distribution<size_t> dist(0, encodings_cache.size() - 1);
    return encodings_cache[dist(rng)];
}

// Построение текста из слов
string ProblemType1::build_text(const vector<string>& words,
    const string& prefix,
    const string& suffix,
    const string& delimiter) const {
    if (words.empty()) return "";

    stringstream ss;
    if (!prefix.empty()) {
        ss << prefix;
    }

    for (size_t i = 0; i < words.size(); i++) {
        ss << words[i];
        if (i < words.size() - 1) {
            ss << delimiter;
        }
    }

    if (!suffix.empty()) {
        ss << " " << suffix;
    }

    return ss.str();
}

// Расчет размера текста в байтах
int ProblemType1::calculate_size(const string& text, int bits_per_char) const {
    if (text.empty()) return 0;

    // Для 7-битного ASCII нужен специальный расчет
    if (bits_per_char == 7) {
        return static_cast<int>(ceil(text.length() * 7.0 / 8.0));
    }

    // Для 8, 16, 32 бит - просто делим на 8
    int bytes_per_char = bits_per_char / 8;
    return text.length() * bytes_per_char;
}

// Модификация текста (удаление или добавление слова)
string ProblemType1::modify_text(const string& text,
    const string& target_word,
    const string& delimiter,
    bool is_removal) const {
    if (is_removal) {
        // Удаление слова
        string result = text;

        // Ищем слово с учетом разделителей
        size_t pos = result.find(target_word);
        while (pos != string::npos) {
            // Проверяем границы слова
            bool left_ok = (pos == 0) ||
                (result[pos - 1] == delimiter[0] || result[pos - 1] == ' ');
            bool right_ok = (pos + target_word.length() == result.length()) ||
                (result[pos + target_word.length()] == delimiter[0] ||
                    result[pos + target_word.length()] == ' ');

            if (left_ok && right_ok) {
                // Удаляем слово
                result.erase(pos, target_word.length());

                // Удаляем разделитель перед словом если есть
                if (pos > 0 && result.substr(pos - delimiter.length(), delimiter.length()) == delimiter) {
                    result.erase(pos - delimiter.length(), delimiter.length());
                }

                // Удаляем разделитель после слова если есть
                if (pos < result.length() && result.substr(pos, delimiter.length()) == delimiter) {
                    result.erase(pos, delimiter.length());
                }

                break;
            }
            pos = result.find(target_word, pos + 1);
        }

        // Чистка лишних пробелов
        size_t space_pos;
        while ((space_pos = result.find("  ")) != string::npos) {
            result.erase(space_pos, 1);
        }

        // Убираем пробелы перед запятыми
        while ((space_pos = result.find(" ,")) != string::npos) {
            result.erase(space_pos, 1);
        }

        // Убираем пробелы после запятых если их много
        while ((space_pos = result.find(",  ")) != string::npos) {
            result.erase(space_pos + 1, 1);
        }

        // Обрезаем пробелы по краям
        size_t start = result.find_first_not_of(" \t\n\r");
        size_t end = result.find_last_not_of(" \t\n\r");

        if (start != string::npos && end != string::npos) {
            result = result.substr(start, end - start + 1);
        }

        return result;
    }
    else {
        // Добавление слова (просто добавляем в конец с разделителем)
        if (text.empty()) return target_word;

        string result = text;
        if (result.back() == delimiter[0]) {
            result += " " + target_word;
        }
        else {
            result += delimiter + target_word;
        }
        return result;
    }
}

// Выбор сценария на основе конфигурации
ProblemType1::Scenario ProblemType1::select_scenario(const ProblemType1Config& config) const {
    // Если только один сценарий доступен - возвращаем его
    if (config.allow_removal && !config.allow_addition && !config.allow_encoding_change) {
        return Scenario::REMOVAL;
    }
    if (!config.allow_removal && config.allow_addition && !config.allow_encoding_change) {
        return Scenario::ADDITION;
    }
    if (!config.allow_removal && !config.allow_addition && config.allow_encoding_change) {
        return Scenario::ENCODING_CHANGE;
    }

    // Случайный выбор из доступных сценариев
    vector<Scenario> available_scenarios;
    if (config.allow_removal) available_scenarios.push_back(Scenario::REMOVAL);
    if (config.allow_addition) available_scenarios.push_back(Scenario::ADDITION);
    if (config.allow_encoding_change) available_scenarios.push_back(Scenario::ENCODING_CHANGE);

    if (available_scenarios.empty()) {
        return Scenario::REMOVAL; // По умолчанию
    }

    static default_random_engine rng(random_device{}());
    uniform_int_distribution<size_t> dist(0, available_scenarios.size() - 1);
    return available_scenarios[dist(rng)];
}


ProblemType1Result ProblemType1::generate_removal(const ProblemType1Config& config) {
    ProblemType1Result result;

    // 1. Выбираем кодировку
    const auto& encoding = select_random_encoding();
    int bytes_per_char = encoding.bits_per_char / 8;

    static default_random_engine rng(random_device{}());

    // 2. Выбираем главное слово
    const WordItem& main_word_item = select_random_word();
    string main_word = main_word_item.word;
    int main_word_length = main_word_item.length;

    // 3. Выбираем другие слова (без такой же длины)
    vector<string> selected_words;
    selected_words.push_back(main_word);

    int additional_words = config.word_count_min +
        uniform_int_distribution<>(0, config.word_count_max - config.word_count_min)(rng) - 1;

    // Берем слова с разными длинами
    vector<WordItem> available = words_cache;
    shuffle(available.begin(), available.end(), rng);

    for (const auto& word_item : available) {
        if (selected_words.size() >= additional_words + 1) break;
        if (word_item.word == main_word) continue;
        if (word_item.length == main_word_length) continue; // ПРОПУСКАЕМ слова такой же длины!

        selected_words.push_back(word_item.word);
    }

    // 4. Шаблон и текст
    size_t template_index = uniform_int_distribution<size_t>(0, prefixes_cache.size() - 1)(rng);
    string prefix = prefixes_cache[template_index];
    string suffix = suffixes_cache[template_index];
    string delimiter = delimiters_cache[template_index];

    string original_text = build_text(selected_words, prefix, suffix, delimiter);
    string modified_text = modify_text(original_text, main_word, delimiter, true);

    // 5. Расчет разницы
    int original_size = calculate_size(original_text, encoding.bits_per_char);
    int modified_size = calculate_size(modified_text, encoding.bits_per_char);
    int size_diff = original_size - modified_size;

    // 6. Формируем результат
    result.problem_text = format_problem_text(encoding.name, encoding.description,
        original_text, size_diff, true);

    result.correct_answer = main_word;
    result.solution_explanation = create_solution_explanation(original_text, modified_text,
        encoding.name, encoding.bits_per_char,
        size_diff, main_word, true);

    // 7. Метаданные
    result.meta["encoding"] = encoding.name;
    result.meta["bits_per_char"] = to_string(encoding.bits_per_char);
    result.meta["original_size"] = to_string(original_size);
    result.meta["modified_size"] = to_string(modified_size);
    result.meta["size_difference"] = to_string(size_diff);
    result.meta["scenario"] = "removal";
    result.meta["word_count"] = to_string(selected_words.size());

    return result;
}
// Генерация задачи на добавление слова
// Генерация задачи на добавление слова
ProblemType1Result ProblemType1::generate_addition(const ProblemType1Config& config) {
    ProblemType1Result result;

    // 1. Выбираем кодировку
    const auto& encoding = select_random_encoding();
    int bytes_per_char = encoding.bits_per_char / 8;

    static default_random_engine rng(random_device{}());

    // 2. Выбираем главное слово (которое будет добавлено)
    const WordItem& main_word_item = select_random_word();
    string main_word = main_word_item.word;
    int main_word_length = main_word_item.length;

    // 3. Выбираем другие слова для ИСХОДНОГО текста
    vector<string> original_words; // Слова в исходном тексте (БЕЗ добавленного)
    original_words.push_back(main_word);

    // Берем слова с разными длинами (не равными длине добавляемого слова)
    vector<WordItem> available = words_cache;
    shuffle(available.begin(), available.end(), rng);

    int original_count = config.word_count_min +
        uniform_int_distribution<>(0, config.word_count_max - config.word_count_min)(rng);

    for (const auto& word_item : available) {
        if (original_words.size() >= original_count) break;
        if (word_item.word == main_word) continue; // Не добавляем наше слово
        if (word_item.length == main_word_length) continue; // Пропускаем слова такой же длины!

        original_words.push_back(word_item.word);
    }

    // 4. Выбираем шаблон
    size_t template_index = uniform_int_distribution<size_t>(0, prefixes_cache.size() - 1)(rng);
    string prefix = prefixes_cache[template_index];
    string suffix = suffixes_cache[template_index];
    string delimiter = delimiters_cache[template_index];

    // 5. Строим ИСХОДНЫЙ текст (без добавленного слова)
    string original_text = build_text(original_words, prefix, suffix, delimiter);

    // 6. Рассчитываем размер исходного текста
    int original_size = calculate_size(original_text, encoding.bits_per_char);

    // 7. Выбираем количество разделителей для добавленного слова
    int delimiter_chars = 2; // 1 или 2 символа
    int total_added_chars = main_word_length + delimiter_chars;
    int added_size = total_added_chars * bytes_per_char;

    // 8. Рассчитываем новый размер
    int new_size = original_size + added_size;

    // 9. Формируем текст задачи
    stringstream problem_ss;
    problem_ss << "В кодировке " << encoding.name << " " << encoding.description << ".\n\n";

    problem_ss << "Андрей написал текст (в нем нет лишних пробелов):\n\n";
    problem_ss << "«" << original_text << "».\n\n";  // ← ПОКАЗЫВАЕМ ИСХОДНЫЙ ТЕКСТ

    problem_ss << "Ученик добавил в список название ещё одного элемента. ";
    problem_ss << "Заодно он добавил необходимые запятые и пробелы — ";
    problem_ss << "два пробела не должны идти подряд.\n\n";

    problem_ss << "При этом размер нового предложения в данной кодировке ";
    problem_ss << "оказался на " << added_size << " байт";

    // Правильное склонение
    if (added_size % 10 == 1 && added_size % 100 != 11) {
        problem_ss << " больше";
    }
    else if (added_size % 10 >= 2 && added_size % 10 <= 4 &&
        (added_size % 100 < 10 || added_size % 100 >= 20)) {
        problem_ss << "а больше";
    }
    else {
        problem_ss << "ов больше";
    }

    problem_ss << ", чем размер исходного предложения.\n\n";
    problem_ss << "Напишите в ответе добавленное название.";

    // 10. Формируем объяснение решения
    stringstream solution_ss;
    solution_ss << "РЕШЕНИЕ:\n\n";
    solution_ss << "1. Исходный текст: «" << original_text << "»\n";
    solution_ss << "2. Разница в размере: " << new_size << " - " << original_size
        << " = " << added_size << " байт\n";
    solution_ss << "3. В кодировке " << encoding.name << " 1 символ = "
        << bytes_per_char << " байт\n";
    solution_ss << "4. Добавлено символов: " << added_size << " / " << bytes_per_char
        << " = " << total_added_chars << " символов\n\n";

    solution_ss << "5. Из " << total_added_chars << " символов:\n";
    solution_ss << "   - Слово: " << main_word_length << " символов\n";
    solution_ss << "   - Разделители: " << delimiter_chars << " символа\n\n";

    solution_ss << "6. В исходном тексте слова имеют длины:\n";
    for (const auto& word : original_words) {
        solution_ss << "   - " << word << ": " << word.length() << " символов\n";
    }
    solution_ss << "\n";

    solution_ss << "7. Добавленное слово должно иметь длину " << main_word_length << " символов\n";
    solution_ss << "   (в исходном тексте нет слов такой длины)\n\n";

    solution_ss << "ОТВЕТ: " << main_word;

    // 11. Заполняем результат
    result.problem_text = problem_ss.str();
    result.correct_answer = main_word;
    result.solution_explanation = solution_ss.str();

    // 12. Метаданные
    result.meta["encoding"] = encoding.name;
    result.meta["original_size"] = to_string(original_size);
    result.meta["new_size"] = to_string(new_size);
    result.meta["size_difference"] = to_string(added_size);
    result.meta["added_word_length"] = to_string(main_word_length);
    result.meta["delimiter_chars"] = to_string(delimiter_chars);
    result.meta["scenario"] = "addition";
    result.meta["original_word_count"] = to_string(original_words.size());

    return result;
}
// Генерация задачи на изменение кодировки
ProblemType1Result ProblemType1::generate_encoding_change(const ProblemType1Config& config) {
    // Пока не реализовано
    ProblemType1Result result;
    result.problem_text = "Задача на изменение кодировки (в разработке)";
    result.correct_answer = "0";
    return result;
}

// Основной метод генерации
ProblemType1Result ProblemType1::generate(const Config& config) {
    if (!cache_loaded) {
        reload_cache();
        if (!cache_loaded) {
            throw runtime_error("Не удалось загрузить данные из БД");
        }
    }

    Scenario scenario = select_scenario(config);

    switch (scenario) {
    case Scenario::REMOVAL:
        return generate_removal(config);
    case Scenario::ADDITION:
        return generate_addition(config);
    case Scenario::ENCODING_CHANGE:
        return generate_encoding_change(config);
    default:
        return generate_removal(config);
    }
}

// Генерация пакета задач
vector<ProblemType1::Problem> ProblemType1::generate_batch(int count, const Config& config) {
    vector<Problem> problems;

    for (int i = 0; i < count; i++) {
        problems.push_back(generate(config));
    }

    return problems;
}

// Получение статистики
ProblemType1::Stats ProblemType1::get_stats() const {
    Stats stats;
    stats.total_words = static_cast<int>(words_cache.size());
    stats.total_encodings = static_cast<int>(encodings_cache.size());

    // Группируем слова по категориям
    for (const auto& word : words_cache) {
        stats.words_by_category[word.category]++;
    }

    return stats;
}

// Проверка ответа
bool ProblemType1::check_answer(const string& user_answer,
    const string& correct_answer) {
    string user_lower = user_answer;
    string correct_lower = correct_answer;

    // Приводим к нижнему регистру
    transform(user_lower.begin(), user_lower.end(), user_lower.begin(), ::tolower);
    transform(correct_lower.begin(), correct_lower.end(), correct_lower.begin(), ::tolower);

    // Убираем лишние пробелы
    user_lower.erase(remove_if(user_lower.begin(), user_lower.end(), ::isspace), user_lower.end());
    correct_lower.erase(remove_if(correct_lower.begin(), correct_lower.end(), ::isspace), correct_lower.end());

    return user_lower == correct_lower;
}

// Вычисление разницы в размере
int ProblemType1::compute_size_difference(const string& text1,
    const string& text2,
    int bits_per_char) {
    int size1 = 0;
    int size2 = 0;

    if (bits_per_char == 7) {
        size1 = static_cast<int>(ceil(text1.length() * 7.0 / 8.0));
        size2 = static_cast<int>(ceil(text2.length() * 7.0 / 8.0));
    }
    else {
        int bytes_per_char = bits_per_char / 8;
        size1 = text1.length() * bytes_per_char;
        size2 = text2.length() * bytes_per_char;
    }

    return size1 - size2;
}

// Форматирование текста задачи
string ProblemType1::format_problem_text(const string& encoding_name,
    const string& encoding_desc,
    const string& original_text,
    int size_diff,
    bool is_removal) {
    stringstream ss;

    ss << "В кодировке " << encoding_name << " " << encoding_desc << ".\n\n";
    ss << "Андрей написал текст (в нем нет лишних пробелов):\n\n";
    ss << "«" << original_text << "».\n\n";

    if (is_removal) {
        ss << "Ученик вычеркнул из списка название одного из элементов. ";
    }
    else {
        ss << "Ученик добавил в список название ещё одного элемента. ";
    }

    ss << "Заодно он вычеркнул ставшие лишними запятые и пробелы — ";
    ss << "два пробела не должны идти подряд.\n\n";

    ss << "При этом размер нового предложения в данной кодировке ";
    ss << "оказался на " << size_diff << " байт";

    // Правильное склонение
    if (size_diff % 10 == 1 && size_diff % 100 != 11) {
        ss << " меньше";
    }
    else if (size_diff % 10 >= 2 && size_diff % 10 <= 4 &&
        (size_diff % 100 < 10 || size_diff % 100 >= 20)) {
        ss << "а меньше";
    }
    else {
        ss << "ов меньше";
    }

    ss << ", чем размер исходного предложения.\n\n";

    if (is_removal) {
        ss << "Напишите в ответе вычеркнутое название.";
    }
    else {
        ss << "Напишите в ответе добавленное название.";
    }

    return ss.str();
}

// Создание объяснения решения
string ProblemType1::create_solution_explanation(const string& original_text,
    const string& modified_text,
    const string& encoding_name,
    int bits_per_char,
    int size_diff,
    const string& target_word,
    bool is_removal) {
    stringstream ss;

    ss << "РЕШЕНИЕ:\n\n";
    ss << "1. Исходный текст: «" << original_text << "»\n";
    ss << "2. Измененный текст: «" << modified_text << "»\n\n";
    ss << "3. Разница в размере: " << size_diff << " байт\n";
    ss << "4. В кодировке " << encoding_name << " 1 символ = "
        << bits_per_char << " бит = " << (bits_per_char / 8.0) << " байт\n\n";

    int removed_chars = 0;
    if (bits_per_char == 7) {
        removed_chars = static_cast<int>(round(size_diff * 8.0 / 7.0));
    }
    else {
        removed_chars = size_diff * 8 / bits_per_char;
    }

    ss << "5. Удалено символов: " << size_diff << " байт / ("
        << bits_per_char << " бит/8) = " << removed_chars << " символов\n\n";

    if (is_removal) {
        ss << "6. Слово \"" << target_word << "\" содержит "
            << target_word.length() << " символов\n";

        int extra_chars = removed_chars - target_word.length();
        if (extra_chars > 0) {
            ss << "7. Также удалено " << extra_chars
                << " лишних знаков препинания/пробелов\n";
        }

        ss << "\nОТВЕТ: " << target_word;
    }

    return ss.str();
}

// Методы ProblemType1Result
string ProblemType1Result::to_json() const {
    stringstream ss;
    ss << "{\n";
    ss << "  \"problem_text\": \"" << problem_text << "\",\n";
    ss << "  \"correct_answer\": \"" << correct_answer << "\",\n";
    ss << "  \"solution_explanation\": \"" << solution_explanation << "\",\n";
    ss << "  \"meta\": {\n";

    size_t i = 0;
    for (const auto& [key, value] : meta) {
        ss << "    \"" << key << "\": \"" << value << "\"";
        if (++i < meta.size()) ss << ",";
        ss << "\n";
    }

    ss << "  }\n";
    ss << "}";
    return ss.str();
}

string ProblemType1Result::to_html() const {
    stringstream ss;
    ss << "<div class=\"problem type1\">\n";
    ss << "  <div class=\"problem-text\">\n";
    ss << "    <p>" << problem_text << "</p>\n";
    ss << "  </div>\n";
    ss << "  <div class=\"solution\" style=\"display: none;\">\n";
    ss << "    <p><strong>Решение:</strong><br>\n";
    ss << "    " << solution_explanation << "</p>\n";
    ss << "  </div>\n";
    ss << "  <button class=\"show-solution\">Показать решение</button>\n";
    ss << "</div>";
    return ss.str();
}

string ProblemType1::escape_html(const string& text) const {
    string result;
    result.reserve(text.length());

    for (char c : text) {
        switch (c) {
        case '&':  result += "&amp;";  break;
        case '<':  result += "&lt;";   break;
        case '>':  result += "&gt;";   break;
        case '"':  result += "&quot;"; break;
        case '\'': result += "&#39;";  break;
        case '\n': result += "<br>";   break;
        default:   result += c;        break;
        }
    }

    return result;
}

// Обернуть текст в параграфы
string ProblemType1::wrap_paragraphs(const string& text) const {
    stringstream input(text);
    stringstream output;
    string line;

    while (getline(input, line, '\n')) {
        if (!line.empty()) {
            output << "<p>" << escape_html(line) << "</p>\n";
        }
    }

    return output.str();
}

// Генерация HTML для нескольких задач
string ProblemType1::generate_html_problems(int count, const Config& config) {
    stringstream html;

    for (int i = 0; i < count; i++) {
        auto problem = generate(config);
        html << generate_single_problem_html(problem, i + 1);
    }

    return html.str();
}

// Генерация HTML для одной задачи
string ProblemType1::generate_single_problem_html(const Problem& problem, int problem_number) {
    stringstream html;

    // Получаем метаданные
    string encoding = "UTF-8";
    string size_diff = "0";

    auto it_encoding = problem.meta.find("encoding");
    if (it_encoding != problem.meta.end()) {
        encoding = it_encoding->second;
    }

    auto it_size = problem.meta.find("size_difference");
    if (it_size != problem.meta.end()) {
        size_diff = it_size->second;
    }

    // Экранируем текст
    string escaped_text = escape_html(problem.problem_text);
    string escaped_solution = wrap_paragraphs(problem.solution_explanation);
    string escaped_answer = escape_html(problem.correct_answer);

    // Генерируем HTML
    html << R"(<article class="cosmic-problem" data-id=")" << problem_number
        << R"(" data-type="type1">
        <div class="problem-header">
            <div class="problem-id">
                <span class="id-number">#)" << problem_number << R"(</span>
                <span class="id-type">ТИП 1</span>
            </div>
            <div class="problem-meta">
                <span class="meta-item">
                    <i class="fas fa-microchip"></i>
                    )" << encoding << R"(
                </span>
                <span class="meta-item">
                    <i class="fas fa-brain"></i>
                    Задача на кодирование
                </span>
                <span class="meta-item">
                    <i class="fas fa-clock"></i>
                    5 мин
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
                <span><i class="fas fa-database"></i> Разница: )" << size_diff
        << R"( байт</span>
            </div>
        </div>
    </article>)";

    return html.str();
}

// Генерация JSON данных для JavaScript
vector<map<string, string>> ProblemType1::generate_problems_json(int count, const Config& config) {
    vector<map<string, string>> problems;

    for (int i = 0; i < count; i++) {
        auto problem = generate(config);
        map<string, string> problem_json;

        problem_json["id"] = to_string(i + 1);
        problem_json["type"] = "1";
        problem_json["problem_text"] = problem.problem_text;
        problem_json["correct_answer"] = problem.correct_answer;
        problem_json["solution_explanation"] = problem.solution_explanation;

        // Добавляем все метаданные
        for (const auto& [key, value] : problem.meta) {
            problem_json[key] = value;
        }

        problems.push_back(problem_json);
    }

    return problems;
}
