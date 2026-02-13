// config.js - Глобальные настройки и состояние
const AppState = {
    stats: {
        total: 0,      // количество отвеченных задач
        correct: 0,    // количество правильных ответов
        totalProblems: 0 // общее количество задач
    }
};

// Конфигурация приложения
const AppConfig = {
    decimalTolerance: (decimalPlaces) => Math.pow(0.1, decimalPlaces) / 2
};

// Делаем доступным глобально
window.AppState = AppState;
window.AppConfig = AppConfig;