// page_settings.cpp
#include "page_setting.h"
#include <functional>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>

namespace OGE {

    // ============= PageSettings Implementation =============

    bool PageSettings::is_expired() const {
        if (timer_mode == TimerMode::NO_TIMER || timer_seconds <= 0) {
            return false;
        }

        std::time_t current_time = std::time(nullptr);
        return (current_time - creation_time) > timer_seconds;
    }

    bool PageSettings::check_password(const std::string& password) const {
        if (password.empty()) return false;
        return hash_password(password) == teacher_password_hash;
    }

    bool PageSettings::can_view_solutions() const {
        return solution_access != AccessLevel::NO_ACCESS;
    }

    bool PageSettings::can_view_hints() const {
        return show_hints;
    }

    std::string PageSettings::hash_password(const std::string& password) {
        // Простое хеширование для демонстрации
        std::hash<std::string> hasher;
        size_t hash = hasher(password);
        std::stringstream ss;
        ss << std::hex << hash;
        return ss.str();
    }

    // ============= SettingsManager Implementation =============

    void SettingsManager::set_title(const std::string& title) {
        // Можно добавить поле title в PageSettings
        // current_settings.title = title;
    }

    void SettingsManager::set_problems(const std::vector<int>& problems) {
        current_settings.problem_numbers.clear();
        for (int problem_id : problems) {
            current_settings.problem_numbers[std::to_string(problem_id)] = 1;
        }
    }

    void SettingsManager::set_timer(TimerMode mode, int seconds) {
        current_settings.timer_mode = mode;
        current_settings.timer_seconds = seconds;
        current_settings.creation_time = std::time(nullptr);
    }

    void SettingsManager::set_access_level(AccessLevel level) {
        current_settings.solution_access = level;
    }

    void SettingsManager::set_teacher_password(const std::string& password) {
        current_settings.teacher_password_hash = PageSettings::hash_password(password);
    }

    const PageSettings& SettingsManager::get_settings() const {
        return current_settings;
    }

    bool SettingsManager::verify_password(const std::string& password) const {
        return current_settings.check_password(password);
    }

    bool SettingsManager::is_teacher_mode() const {
        return !current_settings.teacher_password_hash.empty();
    }

    bool SettingsManager::save_to_file(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) return false;

        file << generate_html_config();
        return file.good();
    }

    bool SettingsManager::load_from_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        // Простая загрузка - в реальности нужен парсер
        std::string line;
        while (std::getline(file, line)) {
            // Здесь должна быть логика парсинга
        }

        return true;
    }

    std::string SettingsManager::generate_html_config() const {
        std::stringstream html;

        html << R"(
        <div id="page-settings" style="display:none;" 
             data-solution-access=")" << static_cast<int>(current_settings.solution_access) << R"("
             data-show-hints=")" << current_settings.show_hints << R"("
             data-show-after-answer=")" << current_settings.show_after_answer << R"("
             data-allow-retry=")" << current_settings.allow_retry << R"("
             data-timer-mode=")" << static_cast<int>(current_settings.timer_mode) << R"("
             data-timer-seconds=")" << current_settings.timer_seconds << R"("
             data-record-statistics=")" << current_settings.record_statistics << R"("
             data-auto-grade=")" << current_settings.auto_grade << R"("
             data-passing-score=")" << current_settings.passing_score << R"("
             data-show-progress-bar=")" << current_settings.show_progress_bar << R"("
             data-show-score=")" << current_settings.show_score << R"("
             data-dark-mode=")" << current_settings.dark_mode << R"("
             data-encrypt-solutions=")" << current_settings.encrypt_solutions << R"("
             data-require-password-for-results=")" << current_settings.require_password_for_results << R"("
             data-creation-time=")" << current_settings.creation_time << R"(">
        )";

        // Добавляем информацию о задачах
        html << "<div id='problem-list'>";
        for (const auto& [problem_type, count] : current_settings.problem_numbers) {
            html << "<span data-problem-type='" << problem_type
                << "' data-count='" << count << "'></span>";
        }
        html << "</div>";

        html << "</div>";

        return html.str();
    }

}