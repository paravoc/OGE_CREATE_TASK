// teacher_control.h
#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <chrono>

namespace OGE {

    // Подключенный компьютер в сети
    struct NetworkComputer {
        std::string mac_address;         // "00:1A:2B:3C:4D:5E"
        std::string ip_address;          // "192.168.1.101"
        std::string computer_name;       // Имя в сети
        bool is_connected = false;       // Подключен к сессии

        // Генерация уникального хеша файла
        std::string generate_file_hash(const std::string& html_content,
            const std::string& teacher_password) const;
    };

    // Учительская сессия
    struct TeacherSession {
        std::string teacher_login;       // Логин учителя
        std::string session_password;    // Пароль сессии (хеш)
        std::chrono::system_clock::time_point created_at;

        // Подключенные компьютеры по MAC
        std::unordered_set<std::string> connected_macs;

        // Папки
        std::string local_save_path;     // Локальная папка учителя
        std::string remote_save_path;    // Папка на ученических компах

        bool is_active = false;
    };

    // Главный контроллер для учителя
    class TeacherController {
    public:
        TeacherController();

        // === Авторизация учителя ===
        bool teacher_login(const std::string& login, const std::string& password);
        bool is_teacher_logged_in() const;

        // === Обнаружение компьютеров в сети ===
        std::vector<NetworkComputer> discover_network_computers();

        // === Подключение по MAC ===
        bool connect_by_mac(const std::string& mac_address);
        bool disconnect_by_mac(const std::string& mac_address);
        std::vector<NetworkComputer> get_connected_computers() const;

        // === Генерация и отправка заданий ===

        // Сгенерировать вариант для всех подключенных компьютеров
        bool generate_and_send_variant(const std::string& html_content);

        // Получить хешированное имя файла для конкретного компьютера
        std::string get_file_hash_for_computer(const std::string& mac_address,
            const std::string& html_content) const;

        // === Сбор результатов ===

        // Мониторинг папки с результатами
        void start_monitoring_results(int check_interval_ms = 5000);
        void stop_monitoring_results();

        // Проверить новые результаты
        std::vector<std::string> check_for_new_results();

        // Получить результат по хешу файла
        std::string get_result_by_hash(const std::string& file_hash) const;

        // === Настройки путей ===
        void set_local_save_path(const std::string& path);
        void set_remote_save_path(const std::string& path);

        const std::string& get_local_save_path() const;
        const std::string& get_remote_save_path() const;

        // === Вспомогательные методы ===

        // Получить MAC текущего компьютера (учительского)
        static std::string get_local_mac();

        // Получить локальный IP
        static std::string get_local_ip();

        // Хеширование для защиты
        static std::string hash_filename(const std::string& ip,
            const std::string& mac,
            const std::string& html_hash,
            const std::string& password_salt);

    private:
        TeacherSession current_session_;
        std::vector<NetworkComputer> network_computers_;

        // Кэш сгенерированных файлов
        std::unordered_map<std::string, std::string> file_hash_cache_; // mac -> hash

        // Результаты
        std::unordered_map<std::string, std::string> collected_results_; // hash -> content

        // Внутренние методы
        bool send_to_computer(const std::string& mac_address,
            const std::string& file_hash,
            const std::string& content);

        bool setup_remote_folder(const std::string& mac_address);

        // Мониторинг
        bool monitoring_active_ = false;
        std::chrono::system_clock::time_point last_check_time_;

        // Сетевое взаимодействие
        bool network_scan_complete_ = false;
    };

} // namespace OGE