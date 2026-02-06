#include "application_manager.h"
#include <fstream>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#undef min
#undef max
#else
#include <cstdlib>
#endif

using namespace std;
using namespace OGE;

ApplicationManager::ApplicationManager()
    : initialized(false), database_ready(false) {
}

bool ApplicationManager::initialize(const AppConfig& cfg) {
    config = cfg;

    try {
        // 1. Инициализация менеджера БД
        db_manager = make_shared<DatabaseManager>();
        if (!db_manager->open_or_create(config.db_path)) {
            last_error = "Не удалось открыть базу данных: " + config.db_path;
            return false;
        }

        // 2. Инициализация для задачи 1
        db_problem1 = make_shared<DatabaseProblem1>(db_manager->get_connection());

        // 3. Настройка базы данных
        if (!setup_database()) {
            last_error = "Не удалось настроить базу данных";
            return false;
        }

        // 4. Инициализация генератора задач
        task_generator = make_unique<TaskGenerator>(db_problem1);
        if (!task_generator->initialize()) {
            last_error = "Не удалось инициализировать генератор задач";
            return false;
        }

        initialized = true;
        database_ready = true;
        return true;

    }
    catch (const exception& e) {
        last_error = string("Ошибка инициализации: ") + e.what();
        return false;
    }
}

void ApplicationManager::set_config(const AppConfig& cfg) {
    config = cfg;
    if (initialized) {
        // Переинициализируем если нужно
        reset();
        initialize(config);
    }
}

bool ApplicationManager::setup_database() {
    if (!db_problem1) return false;

    bool success = true;

    if (config.create_tables) {
        if (!create_database_tables()) {
            success = false;
        }
    }

    if (success && config.seed_data) {
        if (!seed_database()) {
            success = false;
        }
    }

    database_ready = success;
    return success;
}

bool ApplicationManager::create_database_tables() {
    if (!db_problem1) return false;

    cout << "Создание таблиц базы данных..." << endl;
    bool success = db_problem1->create_tables();

    if (success) {
        cout << "✓ Таблицы созданы" << endl;
    }
    else {
        cerr << "✗ Ошибка создания таблиц" << endl;
    }

    return success;
}

bool ApplicationManager::seed_database() {
    if (!db_problem1) return false;

    cout << "Заполнение базы данных..." << endl;
    bool success = db_problem1->seed_data();

    if (success) {
        cout << "✓ Данные заполнены" << endl;
        cout << "  Слов: " << db_problem1->get_word_count() << endl;
        cout << "  Кодировок: " << db_problem1->get_encoding_count() << endl;
    }
    else {
        cerr << "✗ Ошибка заполнения данных" << endl;
    }

    return success;
}


TaskGenerator& ApplicationManager::get_task_generator() {
    return *task_generator;
}

bool ApplicationManager::generate_tasks(const string& scenario_name) {
    if (!initialized || !task_generator) {
        last_error = "Приложение не инициализировано";
        return false;
    }

    string target_scenario = scenario_name.empty() ?
        config.default_scenario : scenario_name;

    cout << "\nГенерация задач для сценария: " << target_scenario << endl;

    auto tasks = task_generator->generate_scenario(target_scenario);
    if (tasks.empty()) {
        last_error = "Не удалось сгенерировать задачи для сценария: " + target_scenario;
        return false;
    }

    cout << "✓ Сгенерировано " << tasks.size() << " задач" << endl;
    return true;
}

bool ApplicationManager::generate_all_tasks() {
    if (!initialized || !task_generator) {
        last_error = "Приложение не инициализировано";
        return false;
    }

    cout << "\nГенерация всех сценариев..." << endl;

    auto tasks = task_generator->generate_all_scenarios();
    if (tasks.empty()) {
        last_error = "Не удалось сгенерировать задачи";
        return false;
    }

    cout << "✓ Всего сгенерировано " << tasks.size() << " задач" << endl;
    return true;
}

bool ApplicationManager::export_to_html(const string& scenario_name,
    const string& output_file) {
    if (!initialized || !task_generator) {
        last_error = "Приложение не инициализировано";
        return false;
    }

    string target_scenario = scenario_name.empty() ?
        config.default_scenario : scenario_name;
    string target_file = output_file.empty() ?
        "site/" + target_scenario + "_задачи.html" : output_file;

    cout << "\nЭкспорт в HTML: " << target_scenario << " -> " << target_file << endl;

    auto tasks_data = task_generator->export_for_html(target_scenario);
    if (tasks_data.empty()) {
        last_error = "Нет данных для экспорта в сценарии: " + target_scenario;
        return false;
    }

    PageConfig page_config;
    page_config.title = "Задачи ОГЭ - " + target_scenario;
    page_config.output_file = target_file;
    page_config.total_problems = static_cast<int>(tasks_data.size());

    html_generator = make_unique<HtmlPageGenerator>(page_config);

    if (!html_generator->generate_and_save_ansi(tasks_data)) {
        last_error = "Не удалось сохранить HTML файл: " + target_file;
        return false;
    }

    cout << "✓ HTML файл создан: " << target_file << endl;

    // Автоматическое открытие в браузере
    if (config.auto_open_browser) {
        open_in_browser(target_file);
    }

    return true;
}

bool ApplicationManager::export_all_to_html(const string& output_file) {
    if (!initialized || !task_generator) {
        last_error = "Приложение не инициализировано";
        return false;
    }

    string target_file = output_file.empty() ? "site/все_задачи.html" : output_file;

    cout << "\nЭкспорт всех задач в HTML: " << target_file << endl;

    auto tasks_data = task_generator->export_all_for_html();
    if (tasks_data.empty()) {
        last_error = "Нет данных для экспорта";
        return false;
    }

    PageConfig page_config;
    page_config.title = "Все задачи ОГЭ по информатике";
    page_config.output_file = target_file;
    page_config.total_problems = static_cast<int>(tasks_data.size());

    html_generator = make_unique<HtmlPageGenerator>(page_config);

    if (!html_generator->generate_and_save_ansi(tasks_data)) {
        last_error = "Не удалось сохранить HTML файл: " + target_file;
        return false;
    }

    cout << "✓ HTML файл создан: " << target_file << endl;

    // Автоматическое открытие в браузере
    if (config.auto_open_browser) {
        open_in_browser(target_file);
    }

    return true;
}

bool ApplicationManager::open_in_browser(const string& filepath) {
    cout << "Открытие файла в браузере: " << filepath << endl;

#ifdef _WIN32
    return open_browser_windows(filepath);
#else
    return open_browser_unix(filepath);
#endif
}

//bool ApplicationManager::open_browser_windows(const string& filepath) {
//    HINSTANCE result = ShellExecuteA(
//        nullptr,
//        "open",
//        filepath.c_str(),
//        nullptr,
//        nullptr,
//        SW_SHOWNORMAL
//    );
//
//    if ((intptr_t)result > 32) {
//        cout << "✓ Файл открыт в браузере" << endl;
//        return true;
//    }
//
//    // Резервный вариант
//    string command = "start \"\" \"" + filepath + "\"";
//    int ret = system(command.c_str());
//    if (ret == 0) {
//        cout << "✓ Файл открыт в браузере (через start)" << endl;
//        return true;
//    }
//
//    cerr << "✗ Не удалось открыть файл в браузере" << endl;
//    return false;
//}


//bool ApplicationManager::open_browser_unix(const string& filepath) {
//    string command;
//
//#ifdef __APPLE__
//    command = "open \"" + filepath + "\"";
//#else
//    command = "xdg-open \"" + filepath + "\"";
//#endif
//
//    int ret = system(command.c_str());
//    if (ret == 0) {
//        cout << "✓ Файл открыт в браузере" << endl;
//        return true;
//    }
//
//    cerr << "✗ Не удалось открыть файл в браузере" << endl;
//    return false;
//}


void ApplicationManager::print_generation_stats(ostream& out) const {
    if (!task_generator) {
        out << "Генератор не инициализирован" << endl;
        return;
    }

    task_generator->print_stats(out);
}

bool ApplicationManager::open_browser_windows(const std::string& filepath) {
    // Простая реализация без Windows API
    std::string command = "start \"\" \"" + filepath + "\"";
    int result = std::system(command.c_str());
    return (result == 0);
}

string ApplicationManager::get_status() const {
    stringstream ss;

    if (!initialized) {
        ss << "Приложение не инициализировано";
        if (!last_error.empty()) {
            ss << " (" << last_error << ")";
        }
        return ss.str();
    }

    ss << "Статус: ";
    ss << (database_ready ? "✓ " : "✗ ");
    ss << "База данных, ";
    ss << (task_generator && task_generator->is_initialized() ? "✓ " : "✗ ");
    ss << "Генератор";

    if (task_generator) {
        auto stats = task_generator->get_stats();
        if (stats.total_generated > 0) {
            ss << ", " << stats.total_generated << " задач сгенерировано";
        }
    }

    return ss.str();
}

void ApplicationManager::reset() {
    db_manager.reset();
    db_problem1.reset();
    task_generator.reset();
    html_generator.reset();
    initialized = false;
    database_ready = false;
    last_error.clear();
}