// auth-aware.js - управление контентом в зависимости от авторизации

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
    const menuContent = document.getElementById('menuContent');
    const authButtons = document.getElementById('authButtons');
    
    if (!authButtons) return;
    
    if (authenticated && user) {
        // Авторизован - показываем имя и кнопку выхода
        authButtons.innerHTML = `
            <span class="user-greeting">👤 ${user.username}</span>
            <a href="/profile" class="auth">Профиль</a>
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

// Функция для получения тарифного контента (для неавторизованных)
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
                    <li>✓ Полный доступ ко всем решениям и обучающим материалам</li>
                    <li>✓ Безлимитная генерация вариантов до 100 задач в каждом</li>
                    <li>✓ Хранение всех сгенерированных вариантов</li>
                    <li>✓ Возможность печати уникальных именных вариантов</li>
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

// Функция для получения контента курсов (для авторизованных)
function getCoursesContent(user) {
    const courses = [
        { 
            icon: '💻', 
            title: 'ОГЭ Информатика', 
            desc: 'Подготовка к ОГЭ по информатике',
            progress: 0,
            total: 12,
            link: '/oge/informatics'
        },
        { 
            icon: '📐', 
            title: 'ОГЭ Математика', 
            desc: 'Скоро появится',
            progress: 0,
            total: 0,
            link: '#',
            disabled: true
        },
        { 
            icon: '🔬', 
            title: 'ЕГЭ Физика', 
            desc: 'Скоро появится',
            progress: 0,
            total: 0,
            link: '#',
            disabled: true
        },
        { 
            icon: '🧪', 
            title: 'ЕГЭ Химия', 
            desc: 'Скоро появится',
            progress: 0,
            total: 0,
            link: '#',
            disabled: true
        }
    ];
    
    let coursesHtml = '<div class="courses-container">';
    coursesHtml += '<h2 class="courses-title">📚 Доступные курсы</h2>';
    coursesHtml += '<div class="courses-grid">';
    
    courses.forEach((course, index) => {
        const disabledClass = course.disabled ? 'course-card disabled' : 'course-card';
        const progressPercent = course.total > 0 ? (course.progress / course.total * 100) : 0;
        
        coursesHtml += `
            <a href="${course.link}" class="${disabledClass}" style="--i: ${index + 1}">
                <div class="course-icon">${course.icon}</div>
                <h3 class="course-title">${course.title}</h3>
                <p class="course-description">${course.desc}</p>
                ${!course.disabled ? `
                    <div class="course-progress">
                        <div class="course-progress-fill" style="width: ${progressPercent}%"></div>
                    </div>
                ` : ''}
                <button class="course-btn" ${course.disabled ? 'disabled' : ''}>
                    ${course.disabled ? 'Скоро' : 'Перейти →'}
                </button>
            </a>
        `;
    });
    
    coursesHtml += '</div></div>';
    return coursesHtml;
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
    
    // Ускоряем анимацию для неавторизованных
    if (!authenticated) {
        // Ускоряем появление текста
        const spans = document.querySelectorAll('#animatedText span');
        spans.forEach((span, index) => {
            span.style.animationDuration = '0.3s';
            span.style.animationDelay = `${index * 0.02}s`;
        });
    }
    
    // Показываем соответствующий контент
    if (authenticated && user) {
        // Авторизован - показываем курсы
        dynamicContent.innerHTML = getCoursesContent(user);
        
        // Прячем кнопку быстрого старта
        const quickStartBtn = document.getElementById('quickStartBtn');
        if (quickStartBtn) quickStartBtn.classList.add('hidden');
        
        // Показываем приветствие
        console.log(`👋 С возвращением, ${user.username}!`);
    } else {
        // Не авторизован - показываем тариф
        dynamicContent.innerHTML = getTariffContent();
        
        // Показываем кнопку быстрого старта
        setTimeout(() => {
            const quickStartBtn = document.getElementById('quickStartBtn');
            if (quickStartBtn) quickStartBtn.classList.remove('hidden');
        }, 2000);
        
        // Инициализируем искры для кнопки тарифа
        setTimeout(() => {
            if (typeof initSparkles === 'function') {
                initSparkles('.sparkle-btn', 10);
            }
        }, 500);
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
    
    // Если пользователь авторизован - отправляем на сервер
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