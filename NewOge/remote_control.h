// secure_teacher_control.h
#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <thread>


namespace OGE {

    // Структура для безопасного хранения пароля
    struct SecurePassword {
        std::string salt;      // Соль для каждого пользователя
        std::string hash;      // Argon2id хеш
        int iterations;        // Количество итераций

        bool verify(const std::string& input_password) const;
        static SecurePassword create(const std::string& password);
    };

    // Уроковая сессия с защитой
    class LessonSession {
    private:
        std::string session_id_;           // Уникальный ID сессии
        std::string session_key_;          // 32-байтный ключ сессии
        std::chrono::system_clock::time_point created_at_;
        std::chrono::system_clock::time_point expires_at_;

        // Белый список MAC-адресов (загружается из конфига)
        std::unordered_set<std::string> approved_macs_;

        // Подключенные в этой сессии
        std::unordered_set<std::string> connected_macs_;

    public:
        LessonSession(const std::string& teacher_id);

        // Генерация challenge для компьютера
        std::string generate_challenge(const std::string& mac) const;

        // Проверка ответа на challenge
        bool verify_response(const std::string& mac,
            const std::string& response) const;

        // Подключение компьютера
        bool connect_computer(const std::string& mac,
            const std::string& challenge_response);

        // Проверка, подключен ли компьютер
        bool is_connected(const std::string& mac) const;

        // Загрузка белого списка
        bool load_whitelist(const std::string& classroom_config);

        // Получение сессионного ключа для шифрования
        const std::string& get_session_key() const { return session_key_; }

        // Валидация сессии
        bool is_valid() const;
    };

    // Защищенное сетевое соединение
    class SecureNetwork {
    private:
        std::string local_ip_;
        std::string local_mac_;

    public:
        SecureNetwork();

        // Сканирование сети с проверкой на спуфинг
        struct NetworkComputer {
            std::string mac_address;
            std::string ip_address;
            std::string hostname;
            int latency_ms;           // Для проверки на спуфинг
            bool is_local;            // Локальный компьютер
        };

        std::vector<NetworkComputer> scan_secure(int timeout_ms = 2000);

        // Отправка файла с шифрованием
        bool send_encrypted_file(const std::string& ip,
            const std::string& session_key,
            const std::string& filename,
            const std::string& content);

        // Прием файла с проверкой
        std::pair<bool, std::string> receive_encrypted_file(
            const std::string& expected_hash);

        // Генерация уникального хеша файла
        static std::string generate_file_hash(
            const std::string& mac,
            const std::string& session_id,
            const std::string& content_hash,
            const std::string& timestamp);
    };

    // Главный контроллер учителя
    class TeacherController {
    private:
        // Учетные данные учителя
        SecurePassword teacher_password_;
        std::string teacher_id_;

        // Текущая сессия
        std::unique_ptr<LessonSession> current_session_;

        // Сетевой модуль
        SecureNetwork network_;

        // Настройки путей
        std::string local_results_path_;
        std::string remote_results_path_;

        // Лог безопасности
        class SecurityLogger {
        private:
            std::string log_path_;
            std::string session_id_;

        public:
            SecurityLogger(const std::string& session_id);
            void log_event(const std::string& action,
                const std::string& mac,
                bool success,
                const std::string& details = "");
            std::string get_session_log() const;
        };

        std::unique_ptr<SecurityLogger> security_log_;

        // Кэш отправленных заданий
        struct SentTask {
            std::string file_hash;
            std::string mac_address;
            std::chrono::system_clock::time_point sent_at;
            bool acknowledged;
        };

        std::unordered_map<std::string, SentTask> sent_tasks_; // mac -> task

    public:
        TeacherController();

        // === Аутентификация учителя ===
        bool login(const std::string& teacher_id, const std::string& password);
        bool is_authenticated() const;
        void logout();

        // === Управление сессиями ===
        bool create_session(const std::string& classroom_config);
        bool end_current_session();
        const LessonSession* get_current_session() const;

        // === Работа с компьютерами ===

        // Получить список доступных компьютеров
        std::vector<SecureNetwork::NetworkComputer> discover_computers();

        // Подключить компьютер к сессии
        bool connect_computer(const std::string& mac_address);

        // Отключить компьютер
        bool disconnect_computer(const std::string& mac_address);

        // Получить список подключенных
        std::vector<std::string> get_connected_computers() const;

        // === Генерация и отправка заданий ===

        // Создать защищенный вариант задания
        struct SecureVariant {
            std::string content;           // HTML контент
            std::string content_hash;      // SHA256 контента
            std::string encrypted_content; // Зашифрованное содержимое
            std::string file_hash;         // Уникальный хеш файла
        };

        SecureVariant create_secure_variant(const std::string& html_content);

        // Отправить вариант всем подключенным компьютерам
        bool distribute_variant(const SecureVariant& variant);

        // Отправить вариант конкретному компьютеру
        bool send_to_computer(const std::string& mac_address,
            const SecureVariant& variant);

        // === Сбор результатов ===

        // Начать мониторинг результатов
        void start_results_monitor(int check_interval_ms = 3000);
        void stop_results_monitor();

        // Проверить наличие новых результатов
        struct ReceivedResult {
            std::string file_hash;      // Хеш исходного задания
            std::string mac_address;    // От какого компьютера
            std::string content;        // Расшифрованное содержимое
            std::chrono::system_clock::time_point received_at;
            bool verified;              // Проверена подпись
        };

        std::vector<ReceivedResult> check_for_results();

        // Получить результат по хешу
        std::string get_result_by_hash(const std::string& file_hash) const;

        // === Настройки путей ===
        void set_paths(const std::string& local_path, const std::string& remote_path);

        // === Безопасное логирование ===
        std::string get_security_log() const;

        // === Утилиты ===
        static std::string get_local_mac();
        static std::string get_local_ip();

    private:
        // Внутренние методы
        bool validate_mac(const std::string& mac) const;
        bool setup_remote_directory(const std::string& mac);
        bool verify_file_integrity(const std::string& filepath,
            const std::string& expected_hash);

        // Мониторинг
        bool monitoring_active_;
        std::thread monitor_thread_;
        void monitor_results_loop(int interval_ms);
    };

}