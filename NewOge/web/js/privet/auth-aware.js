// auth-aware.js - управление контентом в зависимости от авторизации

(function() {
    // Подключаем CSS для категорий, если его нет
    if (!document.querySelector('link[href*="categories.css"]')) {
        const link = document.createElement('link');
        link.rel = 'stylesheet';
        link.href = 'css/privet/categories.css';
        document.head.appendChild(link);
    }
})();

// Функция для проверки авторизации
async function checkAuth() {
    try {
        const response = await fetch('/api/user/me');
        if (response.ok) {
            const userData = await response.json();
            return { authenticated: true, user: userData };
        }
    } catch (error) {
        console.error('Ошибка проверки авторизации:', error);
    }
    return { authenticated: false, user: null };
}

// Функция для обновления меню
function updateMenu(authenticated, user) {
    const authButtons = document.getElementById('authButtons');
    
    if (!authButtons) return;
    
    if (authenticated && user) {
        // Авторизован - показываем имя и ссылку на dashboard
        authButtons.innerHTML = `
            <span class="user-greeting">👤 ${user.username}</span>
            <a href="/dashboard" class="auth">Личный кабинет</a>
            <a href="/logout" class="auth logout">Выйти</a>
        `;
    } else {
        // Не авторизован - показываем вход и регистрацию
        authButtons.innerHTML = `
            <a href="/login" class="auth" id="loginBtn">Войти</a>
            <a href="/register" class="auth" id="registerBtn">Регистрация</a>
        `;
    }
}

// Функция для получения контента для авторизованных
function getAuthorizedContent(user) {
    return `
        <div class="categories-container">
            <h2 class="categories-title">С возвращением, ${user.username}!</h2>
            <p class="categories-subtitle">Продолжите подготовку</p>
            <div class="categories-grid">
                <!-- Генератор задач -->
                <div class="category-card" onclick="location.href='/generate'">
                    <div class="category-icon">⚡</div>
                    <h3 class="category-name">Генератор</h3>
                    <p class="category-desc">Создать новый вариант</p>
                </div>
                
                <!-- Мои результаты -->
                <div class="category-card" onclick="location.href='/dashboard'">
                    <div class="category-icon">📊</div>
                    <h3 class="category-name">Статистика</h3>
                    <p class="category-desc">Мои результаты</p>
                </div>
                
                <!-- Задачи -->
                <div class="category-card" onclick="location.href='/tasks'">
                    <div class="category-icon">📋</div>
                    <h3 class="category-name">Задачи</h3>
                    <p class="category-desc">Все типы заданий</p>
                </div>
            </div>
        </div>
    `;
}

// Функция для получения контента для неавторизованных (тариф)
function getTariffContent() {
    return `
        <div class="tarif-container">
            <div class="mascot-wrapper">
                <img src="/images/money1.png" alt="Маскот" class="mascot-image">
                <div class="wave-container">
                    <div class="wave-line"></div>
                    <div class="wave-line"></div>
                    <div class="wave-line"></div>
                    <div class="wave-line"></div>
                    <div class="wave-line"></div>
                    <div class="circle-wave"></div>
                    <div class="circle-wave"></div>
                    <div class="circle-wave"></div>
                </div>
            </div>
            <div class="tarif-card floating">
                <div class="tarif-header">
                    <h2>ПОДГОТОВКА ЗА<br><span class="highlight">72 ЧАСА</span></h2>
                </div>
                <ul class="tarif-features">
                    <li>✓ Полный доступ ко всем решениям</li>
                    <li>✓ Безлимитная генерация вариантов</li>
                    <li>✓ Хранение всех вариантов</li>
                    <li>✓ Печать именных вариантов</li>
                </ul>
                <div class="tarif-price">
                    <span class="old-price">1000 ₽</span>
                    <span class="new-price">600 ₽</span>
                    <span class="period">/месяц</span>
                </div>
                <button class="tarif-btn sparkle-btn" onclick="location.href='/upgrade'">
                    <span class="btn-text">ВЫБРАТЬ ТАРИФ</span>
                </button>
            </div>
        </div>
    `;
}

// Основная функция инициализации
async function initAuthAware() {
    // Проверяем авторизацию
    const { authenticated, user } = await checkAuth();
    
    // Обновляем меню
    updateMenu(authenticated, user);
    
    // Получаем контейнер для динамического контента
    const dynamicContent = document.getElementById('dynamicContent');
    if (!dynamicContent) return;
    
    // Показываем соответствующий контент
    if (authenticated && user) {
        // Авторизован - показываем приветствие и ссылки
        dynamicContent.innerHTML = getAuthorizedContent(user);
        
        // Прячем кнопку быстрого старта
        const quickStartBtn = document.getElementById('quickStartBtn');
        if (quickStartBtn) quickStartBtn.classList.add('hidden');
    } else {
        // Не авторизован - показываем тариф
        dynamicContent.innerHTML = getTariffContent();
        
        // Показываем кнопку быстрого старта
        setTimeout(() => {
            const quickStartBtn = document.getElementById('quickStartBtn');
            if (quickStartBtn) quickStartBtn.classList.remove('hidden');
        }, 2000);
    }
    
    // Добавляем класс visible для анимации
    setTimeout(() => {
        dynamicContent.classList.add('visible');
    }, 100);
}

// Запускаем после загрузки DOM
document.addEventListener('DOMContentLoaded', () => {
    initAuthAware();
});

// Функции для куки-баннера
window.acceptCookies = function() {
    document.getElementById('cookieConsentBanner').style.display = 'none';
    localStorage.setItem('cookieConsent', 'true');
    
    fetch('/api/user/me')
        .then(res => res.json())
        .then(user => {
            if (user && user.id) {
                fetch(`/api/cookie-consent?user_id=${user.id}`, { method: 'POST' });
            }
        })
        .catch(() => {});
};

window.showCookieInfo = function() {
    document.getElementById('cookieInfoModal').style.display = 'flex';
};

window.closeCookieModal = function() {
    document.getElementById('cookieInfoModal').style.display = 'none';
};

// Показываем баннер при первом посещении
if (!localStorage.getItem('cookieConsent')) {
    setTimeout(() => {
        document.getElementById('cookieConsentBanner').style.display = 'block';
    }, 1000);
}