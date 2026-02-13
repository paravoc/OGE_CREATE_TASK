// filesystem_problem.cpp
#include "filesystem_problem.h"
#include <algorithm>
#include <sstream>

namespace OGE {

    // Инициализация статических членов
    PageSettings FilesystemProblem::page_settings;

    // Статические списки
    std::vector<std::string> FilesystemProblem::protocols;
    std::vector<std::string> FilesystemProblem::domains;
    std::vector<std::string> FilesystemProblem::folders;
    std::vector<std::string> FilesystemProblem::filenames;
    std::vector<std::string> FilesystemProblem::extensions;
    std::vector<std::string> FilesystemProblem::drives;
    std::vector<std::string> FilesystemProblem::cd_commands;

    std::mt19937& FilesystemProblem::get_random_generator() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }

    void FilesystemProblem::init_static_lists() {
        if (!protocols.empty()) return; // Уже инициализированы

        // Протоколы
        protocols = { "http", "https", "ftp", "sftp", "file", "gopher", "telnet", "ldap",
                      "mailto", "news", "nntp", "irc", "ssh", "telnet", "ws", "wss",
                      "rtsp", "rtmp", "smb", "afp", "webdav", "dav", "svn", "git",
                      "docker", "kubernetes", "mongodb", "mysql", "postgresql", "redis" };

        // Домены
        domains = { "com", "org", "net", "ru", "edu", "gov", "mil", "int", "eu", "рф",
                    "de", "uk", "fr", "jp", "cn", "au", "br", "it", "nl", "es",
                    "info", "biz", "name", "pro", "aero", "coop", "museum", "tv", "me", "cc" };

        // Папки
        folders = { "docs", "files", "images", "css", "js", "assets", "public", "private",
                    "admin", "user", "tmp", "var", "etc", "home", "usr", "bin",
                    "downloads", "uploads", "media", "videos", "audio", "data", "backup",
                    "projects", "work", "school", "photos", "music", "books", "games" };

        // Имена файлов
        filenames = { "index", "main", "script", "style", "app", "config", "settings",
                      "readme", "license", "about", "contact", "page", "post", "article",
                      "image", "photo", "pic", "video", "audio", "song", "movie",
                      "data", "info", "doc", "document", "table", "report", "summary" };

        // Расширения
        extensions = { "txt", "html", "css", "js", "php", "asp", "jsp", "xml", "json",
                       "jpg", "jpeg", "png", "gif", "bmp", "svg", "ico", "mp3", "mp4",
                       "avi", "mov", "pdf", "doc", "docx", "xls", "xlsx", "ppt", "pptx",
                       "zip", "rar", "7z", "exe", "msi", "dll", "so", "sh", "bat" };

        // Диски
        drives = { "C:", "D:", "E:", "F:", "G:", "H:", "X:", "Z:", "A:", "B:" };

        // Команды CD
        cd_commands = { "cd ..", "cd .", "cd /", "cd ~", "cd \\", "cd ..\\..", "cd ..\\",
                        "cd ../..", "cd ../", "cd folder", "cd temp", "cd ..\\folder" };
    }

    FilesystemProblem::FilesystemProblem(const ProblemMeta& meta)
        : ProblemBase(meta), task_type(FSTaskType::URL_PATH) {
        init_static_lists();
    }

    void FilesystemProblem::apply_difficulty_settings(Difficulty difficulty) {
        switch (difficulty) {
        case Difficulty::EASY:
            score = 10;
            break;
        case Difficulty::MEDIUM:
            score = 15;
            break;
        case Difficulty::HARD:
            score = 20;
            break;
        }
    }

    std::string FilesystemProblem::join_vector(const std::vector<std::string>& vec, const std::string& delimiter) {
        std::string result;
        for (size_t i = 0; i < vec.size(); i++) {
            if (i > 0) result += delimiter;
            result += vec[i];
        }
        return result;
    }

    // Генерация задачи на URL
// Генерация задачи на URL
// Генерация задачи на URL
    void FilesystemProblem::generate_url_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        url_parts.clear();

        // Выбираем случайные элементы
        std::uniform_int_distribution<> dis_protocol(0, protocols.size() - 1);
        std::uniform_int_distribution<> dis_domain(0, domains.size() - 1);
        std::uniform_int_distribution<> dis_folder(0, folders.size() - 1);
        std::uniform_int_distribution<> dis_file(0, filenames.size() - 1);
        std::uniform_int_distribution<> dis_ext(0, extensions.size() - 1);

        // Количество папок зависит от сложности
        int folder_count = 1;
        switch (config.difficulty) {
        case Difficulty::EASY: folder_count = 1; break;
        case Difficulty::MEDIUM: folder_count = 2; break;
        case Difficulty::HARD: folder_count = 3; break;
        }

        std::string protocol = protocols[dis_protocol(gen)];
        std::string domain_name = "example";
        std::string tld = domains[dis_domain(gen)];
        std::string domain_full = domain_name + "." + tld;

        std::vector<std::string> selected_folders;
        for (int i = 0; i < folder_count; i++) {
            selected_folders.push_back(folders[dis_folder(gen)]);
        }

        std::string file = filenames[dis_file(gen)];
        std::string ext = extensions[dis_ext(gen)];
        std::string filename_full = file + "." + ext;

        // Собираем правильную последовательность для ответа
        std::vector<std::string> correct_parts;
        correct_parts.push_back(protocol + "://");  // протокол
        correct_parts.push_back(domain_full + "/"); // домен
        for (const auto& folder : selected_folders) {
            correct_parts.push_back(folder + "/");  // папки
        }
        correct_parts.push_back(filename_full);      // файл

        // Создаем перемешанные части с временными метками
        std::vector<std::string> shuffled_parts = correct_parts;
        std::shuffle(shuffled_parts.begin(), shuffled_parts.end(), gen);

        // Присваиваем буквы А, Б, В, Г, Д... в порядке перемешанного списка
        std::vector<std::pair<char, std::string>> parts_with_letters;
        char letter = 'А';
        for (const auto& part : shuffled_parts) {
            parts_with_letters.push_back({ letter++, part });
        }

        // Формируем описание задачи
        std::stringstream question;
        question << "<div class='filesystem-url'>";
        question << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        question << "<p class='problem-text'>На сервере <strong>" << domain_full
            << "</strong> по протоколу <strong>" << protocol
            << "</strong> в папке ";

        // Описываем путь
        if (selected_folders.size() == 1) {
            question << "<strong>" << selected_folders[0] << "</strong> ";
        }
        else {
            question << "<strong>";
            for (size_t i = 0; i < selected_folders.size(); i++) {
                question << selected_folders[i];
                if (i < selected_folders.size() - 1) question << " -> ";
            }
            question << "</strong> ";
        }

        question << "находится файл <strong>" << filename_full
            << "</strong>. Восстановите правильную последовательность частей адреса.</p>";

        question << "<div class='parts-list'>";
        question << "<p>Части адреса:</p>";

        // Выводим части с их случайными буквами
        for (const auto& part : parts_with_letters) {
            question << "<p><strong>" << part.first << ")</strong> " << part.second << "</p>";
        }

        question << "</div>";
        question << "<p>В ответе укажите последовательность букв (например, АБВГД).</p>";
        question << "<p class='score'>Баллов: " << score << "</p>";
        question << "</div>";

        problem_text = question.str();

        // Находим правильную последовательность букв
        std::string correct_sequence;
        for (const auto& correct_part : correct_parts) {
            // Ищем букву, соответствующую этой части
            for (const auto& part : parts_with_letters) {
                if (part.second == correct_part) {
                    correct_sequence += part.first;
                    break;
                }
            }
        }

        correct_answer = correct_sequence;

        // Решение
        std::stringstream solution_ss;
        solution_ss << "<div class='solution'>";
        solution_ss << "<h4>Решение:</h4>";
        solution_ss << "<p>Правильный порядок URL:</p>";
        solution_ss << "<p><strong>";

        // Показываем правильный URL
        for (const auto& part : correct_parts) {
            solution_ss << part;
        }

        solution_ss << "</strong></p>";
        solution_ss << "<p>Последовательность букв: <strong>" << correct_sequence << "</strong></p>";
        solution_ss << "</div>";

        solution = solution_ss.str();
    }    // Генерация задачи на путь к файлу на компьютере
// Генерация задачи на путь к файлу на компьютере
// Генерация задачи на путь к файлу на компьютере
    void FilesystemProblem::generate_filepath_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        path_parts.clear();

        std::uniform_int_distribution<> dis_drive(0, drives.size() - 1);
        std::uniform_int_distribution<> dis_folder(0, folders.size() - 1);
        std::uniform_int_distribution<> dis_file(0, filenames.size() - 1);
        std::uniform_int_distribution<> dis_ext(0, extensions.size() - 1);

        // Количество папок зависит от сложности
        int folder_count = 2;
        switch (config.difficulty) {
        case Difficulty::EASY: folder_count = 2; break;
        case Difficulty::MEDIUM: folder_count = 3; break;
        case Difficulty::HARD: folder_count = 4; break;
        }

        std::string drive = drives[dis_drive(gen)];

        std::vector<std::string> selected_folders;
        for (int i = 0; i < folder_count; i++) {
            selected_folders.push_back(folders[dis_folder(gen)]);
        }

        std::string file = filenames[dis_file(gen)];
        std::string ext = extensions[dis_ext(gen)];
        std::string filename_full = file + "." + ext;

        // Собираем правильную последовательность для ответа
        std::vector<std::string> correct_parts;
        correct_parts.push_back(drive + "\\");  // диск
        for (const auto& folder : selected_folders) {
            correct_parts.push_back(folder + "\\");  // папки
        }
        correct_parts.push_back(filename_full);      // файл

        // Создаем перемешанные части
        std::vector<std::string> shuffled_parts = correct_parts;
        std::shuffle(shuffled_parts.begin(), shuffled_parts.end(), gen);

        // Присваиваем буквы А, Б, В, Г, Д... в порядке перемешанного списка
        std::vector<std::pair<char, std::string>> parts_with_letters;
        char letter = 'А';
        for (const auto& part : shuffled_parts) {
            parts_with_letters.push_back({ letter++, part });
        }

        // Формируем описание задачи
        std::stringstream question;
        question << "<div class='filesystem-path'>";
        question << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        question << "<p class='problem-text'>На диске <strong>" << drive
            << "</strong> в папке ";

        // Описываем путь
        if (selected_folders.size() == 1) {
            question << "<strong>" << selected_folders[0] << "</strong> ";
        }
        else {
            question << "<strong>";
            for (size_t i = 0; i < selected_folders.size(); i++) {
                question << selected_folders[i];
                if (i < selected_folders.size() - 1) question << " -> ";
            }
            question << "</strong> ";
        }

        question << "лежит файл <strong>" << filename_full
            << "</strong>. Восстановите правильную последовательность частей пути.</p>";

        question << "<div class='parts-list'>";
        question << "<p>Части пути:</p>";

        // Выводим части с их случайными буквами
        for (const auto& part : parts_with_letters) {
            question << "<p><strong>" << part.first << ")</strong> " << part.second << "</p>";
        }

        question << "</div>";
        question << "<p>В ответе укажите последовательность букв (например, АБВГД).</p>";
        question << "<p class='score'>Баллов: " << score << "</p>";
        question << "</div>";

        problem_text = question.str();

        // Находим правильную последовательность букв
        std::string correct_sequence;
        for (const auto& correct_part : correct_parts) {
            // Ищем букву, соответствующую этой части
            for (const auto& part : parts_with_letters) {
                if (part.second == correct_part) {
                    correct_sequence += part.first;
                    break;
                }
            }
        }

        correct_answer = correct_sequence;

        // Решение
        std::stringstream solution_ss;
        solution_ss << "<div class='solution'>";
        solution_ss << "<h4>Решение:</h4>";
        solution_ss << "<p>Правильный порядок пути:</p>";
        solution_ss << "<p><strong>";

        // Показываем правильный путь
        for (const auto& part : correct_parts) {
            solution_ss << part;
        }

        solution_ss << "</strong></p>";
        solution_ss << "<p>Последовательность букв: <strong>" << correct_sequence << "</strong></p>";
        solution_ss << "</div>";

        solution = solution_ss.str();
    }    // Генерация задачи на команды CD
// Генерация задачи на команды CD
    void FilesystemProblem::generate_cd_task(const GenerationConfig& config) {
        auto& gen = get_random_generator();

        commands.clear();
        current_path.clear();

        std::uniform_int_distribution<> dis_folder(0, folders.size() - 1);
        std::uniform_int_distribution<> dis_drive(0, drives.size() - 1);

        // Начальный путь зависит от сложности
        int initial_depth = 2;
        switch (config.difficulty) {
        case Difficulty::EASY: initial_depth = 2; break;
        case Difficulty::MEDIUM: initial_depth = 3; break;
        case Difficulty::HARD: initial_depth = 4; break;
        }

        std::string drive = drives[dis_drive(gen)];

        // Создаем начальный путь
        current_path.push_back(drive);
        std::vector<std::string> initial_folders;

        for (int i = 0; i < initial_depth; i++) {
            std::string folder = folders[dis_folder(gen)];
            current_path.push_back(folder);
            initial_folders.push_back(folder);
        }

        // Сохраняем копию начального пути для решения
        std::vector<std::string> temp_path = current_path;

        // Формируем строку начального пути для отображения
        initial_path_str = drive + "\\";
        for (size_t i = 0; i < initial_folders.size(); i++) {
            initial_path_str += initial_folders[i];
            if (i < initial_folders.size() - 1) initial_path_str += "\\";
        }
        initial_path_str += ">";

        // Количество команд зависит от сложности
        int cmd_count = 3;
        switch (config.difficulty) {
        case Difficulty::EASY: cmd_count = 2; break;
        case Difficulty::MEDIUM: cmd_count = 3; break;
        case Difficulty::HARD: cmd_count = 4; break;
        }

        std::vector<std::string> command_descriptions;
        commands.clear();

        // Генерируем команды и сразу применяем их к current_path
        for (int i = 0; i < cmd_count; i++) {
            std::uniform_int_distribution<> dis_cmd(0, 3); // 4 типа команд

            int cmd_type = dis_cmd(gen);
            std::string cmd;
            std::string description;

            switch (cmd_type) {
            case 0: // cd ..
                cmd = "cd ..";
                description = "перейти на уровень вверх";
                if (current_path.size() > 1) {
                    current_path.pop_back();
                }
                break;

            case 1: // cd folder
            {
                std::string new_folder = folders[dis_folder(gen)];
                cmd = "cd " + new_folder;
                description = "перейти в папку " + new_folder;
                if (current_path.size() < 7) { // Не уходим слишком глубоко
                    current_path.push_back(new_folder);
                }
            }
            break;

            case 2: // cd \
                                cmd = "cd \\";
                description = "перейти в корень диска";
                current_path.resize(1); // Только диск
                break;

            case 3: // cd ..
                cmd = "cd ..";
                description = "перейти на уровень вверх";
                if (current_path.size() > 1) {
                    current_path.pop_back();
                }
                break;
            }

            commands.push_back(cmd);
            command_descriptions.push_back(description);
        }

        // Результат - где мы оказались после всех команд
        std::string result;
        if (current_path.size() == 1) {
            result = current_path[0]; // Название диска (например, "G:")
        }
        else {
            result = current_path.back(); // Название последней папки
        }

        cd_result = result;

        // Формируем текст задачи
        std::stringstream question;
        question << "<div class='filesystem-cd'>";
        question << "<h3>Задача #" << meta.problem_number << ": " << meta.display_name << "</h3>";
        question << "<p class='problem-text'>Мы находимся в папке: <strong>"
            << initial_path_str << "</strong></p>";
        question << "<p>Выполните последовательно команды:</p>";
        question << "<div class='commands-list'>";

        for (size_t i = 0; i < commands.size(); i++) {
            question << "<p><strong>" << (i + 1) << ".</strong> " << commands[i]
                << " (" << command_descriptions[i] << ")</p>";
        }

        question << "</div>";
        question << "<p>Укажите название папки (или том), в которой вы окажетесь.</p>";
        question << "<p class='score'>Баллов: " << score << "</p>";
        question << "</div>";

        problem_text = question.str();
        correct_answer = cd_result;

        // Решение
        std::stringstream solution_ss;
        solution_ss << "<div class='solution'>";
        solution_ss << "<h4>Решение:</h4>";
        solution_ss << "<p>Начальный путь: " << initial_path_str << "</p>";
        solution_ss << "<p>Выполняем команды:</p><ul>";

        // Используем temp_path для отображения решения
        std::vector<std::string> solution_path = temp_path;

        for (size_t i = 0; i < commands.size(); i++) {
            solution_ss << "<li>" << commands[i] << " — ";

            if (commands[i] == "cd ..") {
                if (solution_path.size() > 1) {
                    solution_path.pop_back();
                    solution_ss << "поднимаемся в " << solution_path.back();
                }
                else {
                    solution_ss << "остаемся в корне " << solution_path[0];
                }
            }
            else if (commands[i].substr(0, 3) == "cd " && commands[i] != "cd .." && commands[i] != "cd \\") {
                std::string folder = commands[i].substr(3);
                solution_path.push_back(folder);
                solution_ss << "переходим в " << folder;
            }
            else if (commands[i] == "cd \\") {
                solution_path.resize(1);
                solution_ss << "переходим в корень " << solution_path[0];
            }

            // Показываем текущий путь после команды
            solution_ss << " (текущий путь: ";
            for (size_t j = 0; j < solution_path.size(); j++) {
                solution_ss << solution_path[j];
                if (j < solution_path.size() - 1) solution_ss << "\\";
            }
            solution_ss << ">)";

            solution_ss << "</li>";
        }

        solution_ss << "</ul>";
        solution_ss << "<p><strong>Ответ:</strong> " << cd_result << "</p>";
        solution_ss << "</div>";

        solution = solution_ss.str();
    }
    void FilesystemProblem::generate(const GenerationConfig& config) {
        apply_difficulty_settings(config.difficulty);

        std::string task_type_str = config.get_string_param("task_type", "url");

        if (task_type_str == "filepath") {
            task_type = FSTaskType::FILE_PATH;
            generate_filepath_task(config);
        }
        else if (task_type_str == "cd") {
            task_type = FSTaskType::CD_COMMANDS;
            generate_cd_task(config);
        }
        else {
            task_type = FSTaskType::URL_PATH;
            generate_url_task(config);
        }

        // Подсказка
        std::stringstream ss;
        ss << "<div class='hint'>";
        ss << "<h4>Подсказка:</h4><p>";

        switch (task_type) {
        case FSTaskType::URL_PATH:
            ss << "URL начинается с протокола (http://), затем домен, затем путь к файлу";
            break;
        case FSTaskType::FILE_PATH:
            ss << "Путь в Windows начинается с диска (C:\\\\), затем папки, затем файл";
            break;
        case FSTaskType::CD_COMMANDS:
            ss << "cd .. - на уровень выше, cd \\ - в корень, cd ~ - домой";
            break;
        }

        ss << "</p></div>";
        hint = ss.str();

        unique_id = generate_unique_id();
        generated_at = std::time(nullptr);
    }

    std::string FilesystemProblem::generate_html() const {
        std::stringstream html;

        html << "<div class='problem-wrapper' data-problem-id='" << unique_id << "'>\n";
        html << "    " << problem_text << "\n";
        html << "    <div class='answer-area'>\n";
        html << "        <input type='text' id='" << unique_id << "'\n";
        html << "               placeholder='Ваш ответ' \n";
        html << "               data-correct-answer='" << correct_answer << "'\n";
        html << "               data-score='" << score << "'\n";
        html << "               data-answered='false'>\n";
        html << "        <button onclick='checkAnswer(\"" << unique_id << "\")'>\n";
        html << "            Проверить\n";
        html << "        </button>\n";
        html << "    </div>\n";

        if (get_page_settings().can_view_solutions()) {
            html << "    <div class='solution-section'>\n";
            html << "        <button class='solution-toggle-btn' onclick='SolutionManager.toggleSolution(\"";
            html << unique_id << "\")'>\n";
            html << "            📚 Показать решение\n";
            html << "        </button>\n";
            html << "        <div id='solution-" << unique_id << "' class='solution-content' style='display: none;'>\n";
            html << "            " << solution << "\n";
            html << "        </div>\n";
            html << "    </div>\n";
        }

        if (get_page_settings().show_hints) {
            html << "    <div class='hint-section'>\n";
            html << "        <details>\n";
            html << "            <summary>💡 Подсказка</summary>\n";
            html << "            " << hint << "\n";
            html << "        </details>\n";
            html << "    </div>\n";
        }

        html << "</div>\n";

        return html.str();
    }

    bool FilesystemProblem::check_answer(const std::string& user_answer) const {
        std::string trimmed = user_answer;
        trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r\f\v"));
        trimmed.erase(trimmed.find_last_not_of(" \t\n\r\f\v") + 1);

        // Приводим к нижнему регистру для сравнения
        std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(), ::tolower);
        std::string correct = correct_answer;
        std::transform(correct.begin(), correct.end(), correct.begin(), ::tolower);

        return trimmed == correct;
    }

    void FilesystemProblem::set_page_settings(const PageSettings& settings) {
        page_settings = settings;
    }

    const PageSettings& FilesystemProblem::get_page_settings() const {
        return page_settings;
    }

}