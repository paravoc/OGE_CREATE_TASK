// auth-aware.js - управление контентом в зависимости от авторизации
// В начало файла добавим проверку и подключение CSS
(function() {
    // Подключаем CSS для категорий, если его нет
    if (!document.querySelector('link[href*="categories.css"]')) {
        const link = document.createElement('link');
        link.rel = 'stylesheet';
        link.href = 'css/privet/categories.css';
        document.head.appendChild(link);
    }
})();

// ... остальной код auth-aware.js
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

// Функция для получения контента категорий (для авторизованных)
function getCategoriesContent(user) {
    return `
        <div class="categories-container">
            <h2 class="categories-title">📚 Навигация</h2>
            <div class="categories-grid">
                <!-- Генератор задач -->
                <div class="category-card" data-category="generator">
                    <div class="category-icon">⚡</div>
                    <h3 class="category-name">Генератор задач</h3>
                    <p class="category-desc">Создавайте уникальные варианты под свои нужды</p>
                    <div class="category-expand" id="expand-generator">
                        <div class="subcategory-grid">
                            <div class="subcategory-item" data-topic="informatics">
                                <span class="subcategory-icon">💻</span>
                                <span>Информатика</span>
                            </div>
                            <div class="subcategory-item" data-topic="algebra">
                                <span class="subcategory-icon">📐</span>
                                <span>Алгебра</span>
                            </div>
                            <div class="subcategory-item" data-topic="geometry">
                                <span class="subcategory-icon">📏</span>
                                <span>Геометрия</span>
                            </div>
                            <div class="subcategory-item" data-topic="python">
                                <span class="subcategory-icon">🐍</span>
                                <span>Python</span>
                            </div>
                            <div class="subcategory-item coming-soon">
                                <span class="subcategory-icon">🔮</span>
                                <span>В разработке</span>
                            </div>
                        </div>
                    </div>
                </div>

                <!-- Обучающие материалы -->
                <div class="category-card" data-category="materials">
                    <div class="category-icon">📚</div>
                    <h3 class="category-name">Обучающие материалы</h3>
                    <p class="category-desc">Видеоуроки, шпаргалки, теория</p>
                    <div class="category-expand" id="expand-materials">
                        <div class="subcategory-grid">
                            <div class="subcategory-item" data-material="video">
                                <span class="subcategory-icon">📹</span>
                                <span>Видеоуроки</span>
                            </div>
                            <div class="subcategory-item" data-material="cheatsheets">
                                <span class="subcategory-icon">📝</span>
                                <span>Шпаргалки</span>
                            </div>
                            <div class="subcategory-item" data-material="theory">
                                <span class="subcategory-icon">📖</span>
                                <span>Теория</span>
                            </div>
                            <div class="subcategory-item" data-material="examples">
                                <span class="subcategory-icon">💡</span>
                                <span>Примеры</span>
                            </div>
                        </div>
                    </div>
                </div>

                <!-- Уровни сложности -->
                <div class="category-card" data-category="levels">
                    <div class="category-icon">📊</div>
                    <h3 class="category-name">Уровни сложности</h3>
                    <p class="category-desc">От новичка до эксперта</p>
                    <div class="category-expand" id="expand-levels">
                        <div class="levels-container">
                            <div class="level-item" data-level="beginner">
                                <div class="level-info">
                                    <span class="level-name">Новичок</span>
                                    <span class="level-desc">Базовые задачи</span>
                                </div>
                                <div class="level-progress">
                                    <div class="progress-bar">
                                        <div class="progress-fill" style="width: 30%"></div>
                                    </div>
                                    <span class="progress-text">3/10</span>
                                </div>
                            </div>
                            <div class="level-item" data-level="intermediate">
                                <div class="level-info">
                                    <span class="level-name">Средний</span>
                                    <span class="level-desc">Повышенная сложность</span>
                                </div>
                                <div class="level-progress">
                                    <div class="progress-bar">
                                        <div class="progress-fill" style="width: 15%"></div>
                                    </div>
                                    <span class="progress-text">2/13</span>
                                </div>
                            </div>
                            <div class="level-item" data-level="advanced">
                                <div class="level-info">
                                    <span class="level-name">Эксперт</span>
                                    <span class="level-desc">Сложные задачи</span>
                                </div>
                                <div class="level-progress">
                                    <div class="progress-bar">
                                        <div class="progress-fill" style="width: 5%"></div>
                                    </div>
                                    <span class="progress-text">1/20</span>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>

                <!-- Магазин -->
                <div class="category-card" data-category="shop">
                    <div class="category-icon">🛒</div>
                    <h3 class="category-name">Магазин</h3>
                    <p class="category-desc">Кредиты, премиум, особые возможности</p>
                    <div class="category-expand" id="expand-shop">
                        <div class="shop-items">
                            <div class="shop-item" data-item="credits">
                                <div class="shop-item-icon">💰</div>
                                <div class="shop-item-info">
                                    <span class="shop-item-name">Кредиты</span>
                                    <span class="shop-item-price">100 ₽</span>
                                </div>
                                <button class="shop-item-btn">Купить</button>
                            </div>
                            <div class="shop-item" data-item="premium">
                                <div class="shop-item-icon">⭐</div>
                                <div class="shop-item-info">
                                    <span class="shop-item-name">Премиум</span>
                                    <span class="shop-item-price">600 ₽/мес</span>
                                </div>
                                <button class="shop-item-btn">Оформить</button>
                            </div>
                            <div class="shop-item" data-item="tasks">
                                <div class="shop-item-icon">📋</div>
                                <div class="shop-item-info">
                                    <span class="shop-item-name">Пакеты задач</span>
                                    <span class="shop-item-price">от 50 ₽</span>
                                </div>
                                <button class="shop-item-btn">Выбрать</button>
                            </div>
                        </div>
                    </div>
                </div>
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
        // Авторизован - показываем категории
        dynamicContent.innerHTML = getCategoriesContent(user);
        
        // ВАЖНО: Инициализируем категории ПОСЛЕ добавления в DOM
        if (typeof initCategories === 'function') {
            setTimeout(() => {
                initCategories();
                console.log('✅ Категории инициализированы');
            }, 100);
        }
        
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