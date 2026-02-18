// Инициализация кнопки быстрого старта
function initQuickStart() {
    const quickStartBtn = document.getElementById('quickStartBtn');
    if (quickStartBtn) {
        quickStartBtn.addEventListener('click', quickStartClick);
    }
}

// Функция для показа кнопки
function showQuickStart() {
    const quickStartBtn = document.querySelector('.quick-start-btn');
    if (quickStartBtn) {
        quickStartBtn.classList.remove('hidden');
    }
}

// Функция для скрытия кнопки
function hideQuickStart() {
    const quickStartBtn = document.querySelector('.quick-start-btn');
    if (quickStartBtn) {
        quickStartBtn.classList.add('hidden');
    }
}


// Добавляем стили для анимации
const style = document.createElement('style');
style.textContent = `
    @keyframes fadeOut {
        0% { opacity: 1; transform: translate(-50%, -50%) scale(1); }
        70% { opacity: 1; transform: translate(-50%, -50%) scale(1.1); }
        100% { opacity: 0; transform: translate(-50%, -50%) scale(0.5); }
    }
`;
document.head.appendChild(style);

// Запускаем инициализацию после загрузки страницы
document.addEventListener('DOMContentLoaded', initQuickStart);