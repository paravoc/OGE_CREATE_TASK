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