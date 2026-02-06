#include "problem_type1.h"
#include <sqlite3.h>
#include <random>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <cstring>
#include "html_page_generator.h"
#include <functional>
#include <numeric>

using namespace std;
using namespace OGE;

ProblemType1::ProblemType1(DatabaseProblem1& db_problem1_ref)
    : db_problem1(db_problem1_ref) {
    reload_cache();
}

ProblemType1::ProblemType1(sqlite3* connection)
    : db_problem1(connection) {
    reload_cache();
}

bool ProblemType1::load_words_from_db(const string& category_filter) {
    words_cache.clear();

    auto records = db_problem1.get_words(category_filter);

    for (const auto& record : records) {
        WordItem item;
        item.word = record.word;
        item.length = record.length;
        item.category = record.category;
        words_cache.push_back(item);
    }

    return !words_cache.empty();
}

bool ProblemType1::load_encodings_from_db() {
    encodings_cache.clear();

    auto records = db_problem1.get_encodings();

    for (const auto& record : records) {
        EncodingInfo info;
        info.name = record.name;
        info.bits_per_char = record.bits_per_char;
        info.description = record.description;
        encodings_cache.push_back(info);
    }

    return !encodings_cache.empty();
}

bool ProblemType1::load_templates_from_db() {
    prefixes_cache.clear();
    suffixes_cache.clear();
    delimiters_cache.clear();

    auto templates = db_problem1.get_templates();

    for (const auto& template_rec : templates) {
        prefixes_cache.push_back(template_rec.prefix);
        suffixes_cache.push_back(template_rec.suffix);
        delimiters_cache.push_back(template_rec.delimiter);
    }

    return !prefixes_cache.empty();
}

void ProblemType1::reload_cache() {
    cache_loaded = load_words_from_db() &&
        load_encodings_from_db() &&
        load_templates_from_db();
}

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

    if (words_cache.empty()) {
        reload_cache();
        if (words_cache.empty()) return empty_item;
    }

    uniform_int_distribution<size_t> dist(0, words_cache.size() - 1);
    return words_cache[dist(rng)];
}

const ProblemType1::EncodingInfo& ProblemType1::select_random_encoding() {
    static default_random_engine rng(random_device{}());
    static EncodingInfo empty_item{ "", 0, "" };

    if (encodings_cache.empty()) {
        reload_cache();
        if (encodings_cache.empty()) return empty_item;
    }

    uniform_int_distribution<size_t> dist(0, encodings_cache.size() - 1);
    return encodings_cache[dist(rng)];
}

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

int ProblemType1::calculate_size(const string& text, int bits_per_char) const {
    if (text.empty()) return 0;

    if (bits_per_char == 7) {
        return static_cast<int>(ceil(text.length() * 7.0 / 8.0));
    }

    int bytes_per_char = bits_per_char / 8;
    return text.length() * bytes_per_char;
}

string ProblemType1::modify_text(const string& text,
    const string& target_word,
    const string& delimiter,
    bool is_removal) const {
    if (is_removal) {
        string result = text;

        size_t pos = result.find(target_word);
        while (pos != string::npos) {
            bool left_ok = (pos == 0) ||
                (result[pos - 1] == delimiter[0] || result[pos - 1] == ' ');
            bool right_ok = (pos + target_word.length() == result.length()) ||
                (result[pos + target_word.length()] == delimiter[0] ||
                    result[pos + target_word.length()] == ' ');

            if (left_ok && right_ok) {
                result.erase(pos, target_word.length());

                if (pos > 0 && result.substr(pos - delimiter.length(), delimiter.length()) == delimiter) {
                    result.erase(pos - delimiter.length(), delimiter.length());
                }

                if (pos < result.length() && result.substr(pos, delimiter.length()) == delimiter) {
                    result.erase(pos, delimiter.length());
                }

                break;
            }
            pos = result.find(target_word, pos + 1);
        }

        size_t space_pos;
        while ((space_pos = result.find("  ")) != string::npos) {
            result.erase(space_pos, 1);
        }

        while ((space_pos = result.find(" ,")) != string::npos) {
            result.erase(space_pos, 1);
        }

        while ((space_pos = result.find(",  ")) != string::npos) {
            result.erase(space_pos + 1, 1);
        }

        size_t start = result.find_first_not_of(" \t\n\r");
        size_t end = result.find_last_not_of(" \t\n\r");

        if (start != string::npos && end != string::npos) {
            result = result.substr(start, end - start + 1);
        }

        return result;
    }
    else {
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

ProblemType1::Scenario ProblemType1::select_scenario(const ProblemType1Config& config) const {
    if (config.allow_removal && !config.allow_addition && !config.allow_encoding_change) {
        return Scenario::REMOVAL;
    }
    if (!config.allow_removal && config.allow_addition && !config.allow_encoding_change) {
        return Scenario::ADDITION;
    }
    if (!config.allow_removal && !config.allow_addition && config.allow_encoding_change) {
        return Scenario::ENCODING_CHANGE;
    }

    vector<Scenario> available_scenarios;
    if (config.allow_removal) available_scenarios.push_back(Scenario::REMOVAL);
    if (config.allow_addition) available_scenarios.push_back(Scenario::ADDITION);
    if (config.allow_encoding_change) available_scenarios.push_back(Scenario::ENCODING_CHANGE);

    if (available_scenarios.empty()) {
        return Scenario::REMOVAL;
    }

    static default_random_engine rng(random_device{}());
    uniform_int_distribution<size_t> dist(0, available_scenarios.size() - 1);
    return available_scenarios[dist(rng)];
}

ProblemType1Result ProblemType1::generate_removal(const ProblemType1Config& config) {
    ProblemType1Result result;

    if (encodings_cache.empty()) {
        reload_cache();
        if (encodings_cache.empty()) {
            throw runtime_error("Нет доступных кодировок в БД");
        }
    }

    const auto& encoding = select_random_encoding();
    int bytes_per_char = encoding.bits_per_char / 8;

    static default_random_engine rng(random_device{}());

    const WordItem& main_word_item = select_random_word();
    if (main_word_item.word.empty()) {
        throw runtime_error("Нет доступных слов в БД");
    }

    string main_word = main_word_item.word;
    int main_word_length = main_word_item.length;

    vector<string> selected_words;
    selected_words.push_back(main_word);

    int additional_words = config.word_count_min +
        uniform_int_distribution<>(0, config.word_count_max - config.word_count_min)(rng) - 1;

    // Фильтруем слова по категории если задана
    vector<WordItem> available = words_cache;
    if (!config.word_category.empty()) {
        vector<WordItem> filtered;
        for (const auto& word : available) {
            if (word.category == config.word_category) {
                filtered.push_back(word);
            }
        }
        if (!filtered.empty()) {
            available = filtered;
        }
    }

    shuffle(available.begin(), available.end(), rng);

    for (const auto& word_item : available) {
        if (selected_words.size() >= additional_words + 1) break;
        if (word_item.word == main_word) continue;
        if (word_item.length == main_word_length) continue;

        selected_words.push_back(word_item.word);
    }

    if (prefixes_cache.empty() || suffixes_cache.empty() || delimiters_cache.empty()) {
        reload_cache();
    }

    if (prefixes_cache.empty()) {
        throw runtime_error("Нет доступных шаблонов в БД");
    }

    shuffle(selected_words.begin(), selected_words.end(), rng);

    size_t template_index = uniform_int_distribution<size_t>(0, prefixes_cache.size() - 1)(rng);
    string prefix = prefixes_cache[template_index];
    string suffix = suffixes_cache[template_index];
    string delimiter = delimiters_cache[template_index];

    string original_text = build_text(selected_words, prefix, suffix, delimiter);
    string modified_text = modify_text(original_text, main_word, delimiter, true);

    int original_size = calculate_size(original_text, encoding.bits_per_char);
    int modified_size = calculate_size(modified_text, encoding.bits_per_char);
    int size_diff = original_size - modified_size;

    result.problem_text = format_problem_text(encoding.name, encoding.description,
        original_text, size_diff, true);

    result.correct_answer = main_word;
    result.solution_explanation = create_solution_explanation(original_text, modified_text,
        encoding.name, encoding.bits_per_char,
        size_diff, main_word, true);

    result.meta["encoding"] = encoding.name;
    result.meta["bits_per_char"] = to_string(encoding.bits_per_char);
    result.meta["original_size"] = to_string(original_size);
    result.meta["modified_size"] = to_string(modified_size);
    result.meta["size_difference"] = to_string(size_diff);
    result.meta["scenario"] = "removal";
    result.meta["word_count"] = to_string(selected_words.size());

    return result;
}

ProblemType1Result ProblemType1::generate_addition(const ProblemType1Config& config) {
    ProblemType1Result result;

    if (encodings_cache.empty()) {
        reload_cache();
        if (encodings_cache.empty()) {
            throw runtime_error("Нет доступных кодировок в БД");
        }
    }

    const auto& encoding = select_random_encoding();
    int bytes_per_char = encoding.bits_per_char / 8;

    static default_random_engine rng(random_device{}());

    const WordItem& main_word_item = select_random_word();
    if (main_word_item.word.empty()) {
        throw runtime_error("Нет доступных слов в БД");
    }

    string main_word = main_word_item.word;
    int main_word_length = main_word_item.length;

    vector<string> original_words;
    original_words.push_back(main_word);

    vector<WordItem> available = words_cache;
    if (!config.word_category.empty()) {
        vector<WordItem> filtered;
        for (const auto& word : available) {
            if (word.category == config.word_category) {
                filtered.push_back(word);
            }
        }
        if (!filtered.empty()) {
            available = filtered;
        }
    }

    shuffle(available.begin(), available.end(), rng);

    int original_count = config.word_count_min +
        uniform_int_distribution<>(0, config.word_count_max - config.word_count_min)(rng);

    for (const auto& word_item : available) {
        if (original_words.size() >= original_count) break;
        if (word_item.word == main_word) continue;
        if (word_item.length == main_word_length) continue;

        original_words.push_back(word_item.word);
    }

    if (prefixes_cache.empty() || suffixes_cache.empty() || delimiters_cache.empty()) {
        reload_cache();
    }

    if (prefixes_cache.empty()) {
        throw runtime_error("Нет доступных шаблонов в БД");
    }

    shuffle(original_words.begin(), original_words.end(), rng);

    size_t template_index = uniform_int_distribution<size_t>(0, prefixes_cache.size() - 1)(rng);
    string prefix = prefixes_cache[template_index];
    string suffix = suffixes_cache[template_index];
    string delimiter = delimiters_cache[template_index];

    string original_text = build_text(original_words, prefix, suffix, delimiter);

    int original_size = calculate_size(original_text, encoding.bits_per_char);

    int delimiter_chars = 2;
    int total_added_chars = main_word_length + delimiter_chars;
    int added_size = total_added_chars * bytes_per_char;

    int new_size = original_size + added_size;

    stringstream problem_ss;
    problem_ss << "В кодировке " << encoding.name << " " << encoding.description << ".\n\n";

    problem_ss << "Андрей написал текст (в нем нет лишних пробелов):\n\n";
    problem_ss << "«" << original_text << "».\n\n";

    problem_ss << "Ученик добавил в список название ещё одного элемента. ";
    problem_ss << "Заодно он добавил необходимые запятые и пробелы — ";
    problem_ss << "два пробела не должны идти подряд.\n\n";

    problem_ss << "При этом размер нового предложения в данной кодировке ";
    problem_ss << "оказался на " << added_size << " байт";

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

    stringstream solution_ss;

    solution_ss << "\n1. Исходный текст: «" << original_text << "»\n";
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

    result.problem_text = problem_ss.str();
    result.correct_answer = main_word;
    result.solution_explanation = OGE::HtmlPageGenerator::nl2br(solution_ss.str());

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

ProblemType1Result ProblemType1::generate_encoding_change(const ProblemType1Config& config) {
    ProblemType1Result result;

    static default_random_engine rng(random_device{}());

    if (encodings_cache.size() < 2) {
        reload_cache();
        if (encodings_cache.size() < 2) {
            throw runtime_error("Недостаточно кодировок в БД (нужно минимум 2)");
        }
    }

    vector<EncodingInfo> encodings = encodings_cache;

    vector<pair<EncodingInfo, EncodingInfo>> valid_pairs;

    for (size_t i = 0; i < encodings.size(); i++) {
        for (size_t j = i + 1; j < encodings.size(); j++) {
            if (encodings[i].bits_per_char != encodings[j].bits_per_char) {
                valid_pairs.push_back({ encodings[i], encodings[j] });
            }
        }
    }

    if (valid_pairs.empty()) {
        EncodingInfo source, target;
        source.name = "КОИ-8";
        source.bits_per_char = 8;
        source.description = "кодировка для русского алфавита";

        target.name = "UTF-16";
        target.bits_per_char = 16;
        target.description = "16-битная кодировка Unicode";

        valid_pairs.push_back({ source, target });
    }

    int pair_index = uniform_int_distribution<>(0, (int)valid_pairs.size() - 1)(rng);
    const EncodingInfo& source_encoding = valid_pairs[pair_index].first;
    const EncodingInfo& target_encoding = valid_pairs[pair_index].second;

    bool source_is_smaller = (source_encoding.bits_per_char < target_encoding.bits_per_char);
    const EncodingInfo& smaller_encoding = source_is_smaller ? source_encoding : target_encoding;
    const EncodingInfo& larger_encoding = source_is_smaller ? target_encoding : source_encoding;

    int small_bytes_per_char = smaller_encoding.bits_per_char / 8;
    int large_bytes_per_char = larger_encoding.bits_per_char / 8;
    int bytes_diff = large_bytes_per_char - small_bytes_per_char;

    vector<string> selected_words;
    vector<WordItem> available = words_cache;
    if (!config.word_category.empty()) {
        vector<WordItem> filtered;
        for (const auto& word : available) {
            if (word.category == config.word_category) {
                filtered.push_back(word);
            }
        }
        if (!filtered.empty()) {
            available = filtered;
        }
    }

    shuffle(available.begin(), available.end(), rng);

    int word_count = config.word_count_min +
        uniform_int_distribution<>(0, config.word_count_max - config.word_count_min)(rng);

    for (const auto& word_item : available) {
        if (selected_words.size() >= word_count) break;
        selected_words.push_back(word_item.word);
    }

    if (prefixes_cache.empty() || suffixes_cache.empty() || delimiters_cache.empty()) {
        reload_cache();
    }

    if (prefixes_cache.empty()) {
        throw runtime_error("Нет доступных шаблонов в БД");
    }

    size_t template_idx = uniform_int_distribution<size_t>(0, prefixes_cache.size() - 1)(rng);
    string prefix = prefixes_cache[template_idx];
    string suffix = suffixes_cache[template_idx];
    string delimiter = delimiters_cache[template_idx];

    string text = build_text(selected_words, prefix, suffix, delimiter);
    int text_length = text.length();

    int small_size = calculate_size(text, smaller_encoding.bits_per_char);
    int large_size = calculate_size(text, larger_encoding.bits_per_char);
    int size_diff = large_size - small_size;

    int task_type = uniform_int_distribution<>(1, 4)(rng);

    stringstream problem_ss;
    string correct_answer;
    stringstream solution_ss;

    switch (task_type) {
    case 1: {
        problem_ss << "Текст в кодировке " << smaller_encoding.name
            << " имеет размер " << small_size << " байт.\n\n";

        problem_ss << "Тот же текст в кодировке " << larger_encoding.name
            << " имеет размер " << large_size << " байт.\n\n";

        problem_ss << "Сколько символов в этом тексте?";

        correct_answer = to_string(text_length);

        solution_ss << "1. В кодировке " << smaller_encoding.name
            << ": 1 символ = " << small_bytes_per_char << " байт\n";
        solution_ss << "2. В кодировке " << larger_encoding.name
            << ": 1 символ = " << large_bytes_per_char << " байт\n\n";

        solution_ss << "3. Пусть N - количество символов\n";
        solution_ss << "   Тогда: N * " << small_bytes_per_char << " = " << small_size << "\n";
        solution_ss << "   Или:   N * " << large_bytes_per_char << " = " << large_size << "\n\n";

        solution_ss << "4. Решаем: N = " << small_size << " / " << small_bytes_per_char
            << " = " << text_length << "\n";
        solution_ss << "   Проверка: " << text_length << " * " << large_bytes_per_char
            << " = " << large_size << "\n\n";

        solution_ss << "ОТВЕТ: " << text_length;
        break;
    }

    case 2: {
        problem_ss << "Текст содержит " << text_length << " символов.\n\n";

        problem_ss << "В кодировке " << smaller_encoding.name
            << " его размер составляет " << small_size << " байт.\n\n";

        problem_ss << "Каков будет размер этого текста в кодировке "
            << larger_encoding.name << "?";

        correct_answer = to_string(large_size);

        solution_ss << "1. Текст: " << text_length << " символов\n";
        solution_ss << "2. " << smaller_encoding.name << ": "
            << small_bytes_per_char << " байт/символ\n";
        solution_ss << "3. " << larger_encoding.name << ": "
            << large_bytes_per_char << " байт/символ\n\n";

        solution_ss << "4. Размер в " << larger_encoding.name << ":\n";
        solution_ss << "   " << text_length << " * " << large_bytes_per_char
            << " = " << large_size << " байт\n\n";

        solution_ss << "5. Проверка через разницу:\n";
        solution_ss << "   Разница на 1 символ: " << large_bytes_per_char << " - "
            << small_bytes_per_char << " = " << bytes_diff << " байт\n";
        solution_ss << "   Общая разница: " << text_length << " * " << bytes_diff
            << " = " << size_diff << " байт\n";
        solution_ss << "   Новый размер: " << small_size << " + " << size_diff
            << " = " << large_size << " байт\n\n";

        solution_ss << "ОТВЕТ: " << large_size;
        break;
    }

    case 3: {
        int char_diff = large_bytes_per_char - small_bytes_per_char;

        problem_ss << "Текст перекодировали из " << smaller_encoding.name
            << " в " << larger_encoding.name << ".\n\n";

        problem_ss << "При этом размер текста увеличился на " << size_diff
            << " байт.\n\n";

        problem_ss << "На сколько байт увеличился размер ОДНОГО символа "
            << "при такой перекодировке?";

        correct_answer = to_string(char_diff);

        solution_ss << "1. Разница в размере всего текста: " << size_diff << " байт\n";
        solution_ss << "2. " << smaller_encoding.name << ": "
            << small_bytes_per_char << " байт/символ\n";
        solution_ss << "3. " << larger_encoding.name << ": "
            << large_bytes_per_char << " байт/символ\n\n";

        solution_ss << "4. Разница на 1 символ:\n";
        solution_ss << "   " << large_bytes_per_char << " - " << small_bytes_per_char
            << " = " << char_diff << " байт\n\n";

        solution_ss << "5. Количество символов в тексте:\n";
        solution_ss << "   " << size_diff << " / " << char_diff
            << " = " << text_length << " символов\n\n";

        solution_ss << "ОТВЕТ: " << char_diff;
        break;
    }

    case 4: {
        problem_ss << "Текст перекодировали из " << smaller_encoding.name
            << " в " << larger_encoding.name << ".\n\n";

        problem_ss << "После перекодировки размер текста составляет "
            << large_size << " байт.\n\n";

        problem_ss << "Каков был размер этого текста в исходной кодировке "
            << smaller_encoding.name << "?";

        correct_answer = to_string(small_size);

        solution_ss << "1. Новая кодировка: " << larger_encoding.name
            << " (" << large_bytes_per_char << " байт/символ)\n";
        solution_ss << "2. Исходная кодировка: " << smaller_encoding.name
            << " (" << small_bytes_per_char << " байт/символ)\n";
        solution_ss << "3. Новый размер: " << large_size << " байт\n\n";

        solution_ss << "4. Находим количество символов:\n";
        solution_ss << "   N = " << large_size << " / " << large_bytes_per_char
            << " = " << text_length << " символов\n\n";

        solution_ss << "5. Исходный размер:\n";
        solution_ss << "   " << text_length << " * " << small_bytes_per_char
            << " = " << small_size << " байт\n\n";

        solution_ss << "6. Проверка разницы:\n";
        solution_ss << "   Разница: " << large_size << " - " << small_size
            << " = " << size_diff << " байт\n";
        solution_ss << "   На 1 символ: " << large_bytes_per_char << " - "
            << small_bytes_per_char << " = " << bytes_diff << " байт\n";
        solution_ss << "   Общая: " << text_length << " * " << bytes_diff
            << " = " << size_diff << " байт ✓\n\n";

        solution_ss << "ОТВЕТ: " << small_size;
        break;
    }
    }

    stringstream final_problem;
    final_problem << "В кодировке " << smaller_encoding.name << " "
        << smaller_encoding.description << " ("
        << small_bytes_per_char << " байт на символ).\n\n";

    final_problem << "В кодировке " << larger_encoding.name << " "
        << larger_encoding.description << " ("
        << large_bytes_per_char << " байт на символ).\n\n";

    final_problem << problem_ss.str();

    result.problem_text = final_problem.str();
    result.correct_answer = correct_answer;
    result.solution_explanation = OGE::HtmlPageGenerator::nl2br(solution_ss.str());

    result.meta["encoding_source"] = smaller_encoding.name;
    result.meta["encoding_target"] = larger_encoding.name;
    result.meta["source_bytes_per_char"] = to_string(small_bytes_per_char);
    result.meta["target_bytes_per_char"] = to_string(large_bytes_per_char);
    result.meta["text_length"] = to_string(text_length);
    result.meta["source_size"] = to_string(small_size);
    result.meta["target_size"] = to_string(large_size);
    result.meta["size_difference"] = to_string(size_diff);
    result.meta["scenario"] = "encoding_change";
    result.meta["task_type"] = to_string(task_type);

    return result;
}

ProblemType1Result ProblemType1::generate(const Config& config) {
    if (!cache_loaded) {
        reload_cache();
        if (!cache_loaded) {
            throw runtime_error("Не удалось загрузить данные из БД. Проверьте подключение к БД и наличие данных.");
        }
    }

    // Проверяем, что есть достаточно данных для генерации
    if (words_cache.empty()) {
        reload_cache();
        if (words_cache.empty()) {
            throw runtime_error("Нет слов в базе данных для генерации задачи");
        }
    }

    if (encodings_cache.empty()) {
        reload_cache();
        if (encodings_cache.empty()) {
            throw runtime_error("Нет кодировок в базе данных для генерации задачи");
        }
    }

    Scenario scenario = select_scenario(config);

    try {
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
    catch (const exception& e) {
        // Если произошла ошибка, пробуем перезагрузить кэш и повторить
        reload_cache();

        // Пробуем снова с fallback-сценарием
        try {
            return generate_removal(config);
        }
        catch (const exception& e2) {
            throw runtime_error(string("Не удалось сгенерировать задачу: ") + e2.what());
        }
    }
}

ProblemType1::Stats ProblemType1::get_stats() const {
    Stats stats;
    stats.total_words = static_cast<int>(words_cache.size());
    stats.total_encodings = static_cast<int>(encodings_cache.size());

    for (const auto& word : words_cache) {
        stats.words_by_category[word.category]++;
    }

    return stats;
}

bool ProblemType1::check_answer(const string& user_answer,
    const string& correct_answer) {
    string user_lower = user_answer;
    string correct_lower = correct_answer;

    transform(user_lower.begin(), user_lower.end(), user_lower.begin(), ::tolower);
    transform(correct_lower.begin(), correct_lower.end(), correct_lower.begin(), ::tolower);

    user_lower.erase(remove_if(user_lower.begin(), user_lower.end(), ::isspace), user_lower.end());
    correct_lower.erase(remove_if(correct_lower.begin(), correct_lower.end(), ::isspace), correct_lower.end());

    return user_lower == correct_lower;
}

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

    const string solution_string_n = ss.str();
    string solution_string_br = OGE::HtmlPageGenerator::nl2br(solution_string_n);

    return solution_string_br;
}

string ProblemType1::create_solution_explanation(const string& original_text,
    const string& modified_text,
    const string& encoding_name,
    int bits_per_char,
    int size_diff,
    const string& target_word,
    bool is_removal) {
    stringstream ss;

    ss << "1. Исходный текст: «" << original_text << "»\n";
    ss << "2. Измененный текст: «" << modified_text << "»\n\n";
    ss << "3. Разница в размере: " << size_diff << " байт\n";
    ss << "4. В кодировке " << encoding_name << " 1 символ = "
        << bits_per_char << " бит = " << (bits_per_char / 8.0) << " байт\n\n";

    int removed_chars = 0;
    removed_chars = size_diff * 8 / bits_per_char;

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

    const string solution_string_n = ss.str();
    string solution_string_br = OGE::HtmlPageGenerator::nl2br(solution_string_n);

    return solution_string_br;
}

string ProblemType1::generate_html_problems(int count, const Config& config) {
    stringstream html;

    for (int i = 0; i < count; i++) {
        try {
            auto problem = generate(config);
            html << generate_single_problem_html(problem, i + 1);

            // Добавляем разделитель между задачами
            if (i < count - 1) {
                html << "\n<div class='problem-separator'></div>\n";
            }
        }
        catch (const exception& e) {
            html << "<div class='error-message'>"
                << "Ошибка генерации задачи " << (i + 1) << ": "
                << e.what() << "</div>\n";
        }
    }

    return html.str();
}

string ProblemType1::generate_single_problem_html(const Problem& problem, int problem_number) {
    stringstream html;

    string encoding = "UTF-8";
    string size_diff = "0";

    auto it_encoding = problem.meta.find("encoding");
    if (it_encoding != problem.meta.end()) {
        encoding = it_encoding->second;
    }
    else {
        it_encoding = problem.meta.find("encoding_source");
        if (it_encoding != problem.meta.end()) {
            encoding = it_encoding->second;
        }
    }

    auto it_size = problem.meta.find("size_difference");
    if (it_size != problem.meta.end()) {
        size_diff = it_size->second;
    }

    // Экранирование специальных символов HTML (упрощенное)
    auto escape_html = [](const string& s) -> string {
        string result;
        result.reserve(s.size());
        for (char c : s) {
            switch (c) {
            case '&': result += "&amp;"; break;
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c;
            }
        }
        return result;
        };

    string escaped_text = escape_html(problem.problem_text);
    escaped_text = OGE::HtmlPageGenerator::nl2br(escaped_text);

    string escaped_solution = escape_html(problem.solution_explanation);

    string escaped_answer = escape_html(problem.correct_answer);

    html << R"(<article class="cosmic-problem" data-id=")" << problem_number
        << R"(" data-type="type1" data-scenario=")"
        << (problem.meta.count("scenario") ? problem.meta.at("scenario") : "removal")
        << R"(">
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
                           class="cosmic-input"
                           data-correct-answer=")" << escaped_answer << R"(">
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