// Глобальные функции и инициализация
document.addEventListener('DOMContentLoaded', function() {
    initNavigation();
    checkAuthStatus();
    initStartButton();
    initDropdowns();
});

// Инициализация навигации
function initNavigation() {
    // Мобильное меню
    const mobileMenuButton = document.getElementById('mobile-menu-button');
    const closeMobileMenu = document.getElementById('close-mobile-menu');
    const mobileMenu = document.getElementById('mobile-menu');

    if (mobileMenuButton && mobileMenu) {
        mobileMenuButton.addEventListener('click', function() {
            mobileMenu.classList.remove('hidden');
            document.body.style.overflow = 'hidden';
        });
    }

    if (closeMobileMenu && mobileMenu) {
        closeMobileMenu.addEventListener('click', function() {
            mobileMenu.classList.add('hidden');
            document.body.style.overflow = '';
        });
    }

    // Закрытие мобильного меню при клике вне
    window.addEventListener('click', function(e) {
        if (mobileMenu && !mobileMenu.contains(e.target) && 
            mobileMenuButton && !mobileMenuButton.contains(e.target)) {
            mobileMenu.classList.add('hidden');
            document.body.style.overflow = '';
        }
    });
}

// Инициализация выпадающих меню
function initDropdowns() {
    // Продукты меню
    const productsButton = document.querySelector('#products-menu button');
    const productsDropdown = document.getElementById('products-dropdown');

    if (productsButton && productsDropdown) {
        productsButton.addEventListener('click', function(e) {
            e.stopPropagation();
            const isExpanded = productsButton.getAttribute('aria-expanded') === 'true';
            productsButton.setAttribute('aria-expanded', !isExpanded);
            productsDropdown.classList.toggle('hidden');
        });
    }

    // Закрытие всех дропдаунов при клике вне
    window.addEventListener('click', function() {
        document.querySelectorAll('[aria-expanded="true"]').forEach(function(button) {
            button.setAttribute('aria-expanded', 'false');
        });
        document.querySelectorAll('.dropdown-menu, #products-dropdown, #dashboard-dropdown, #user-dropdown').forEach(function(dropdown) {
            dropdown.classList.add('hidden');
        });
    });
}

// Проверка статуса авторизации
async function checkAuthStatus() {
    try {
        const response = await fetch('/api/user/me');
        const data = await response.json();
        
        if (data.authenticated) {
            renderAuthButtons(data.user);
            updateUIForAuth(data.user);
        } else {
            renderGuestButtons();
        }
    } catch (error) {
        console.error('Error checking auth status:', error);
        renderGuestButtons();
    }
}

// Рендер кнопок для авторизованного пользователя
function renderAuthButtons(user) {
    const authButtons = document.getElementById('auth-buttons');
    const mobileAuth = document.getElementById('mobile-auth-buttons');
    const template = document.getElementById('auth-buttons-template');

    if (authButtons && template) {
        const clone = template.content.cloneNode(true);
        const statusSpan = clone.querySelector('#user-status');
        statusSpan.textContent = user.name + ' (' + user.credits + ' кредитов)';
        authButtons.innerHTML = '';
        authButtons.appendChild(clone);
    }

    if (mobileAuth) {
        mobileAuth.innerHTML = 
            '<a href="/dashboard" class="-mx-3 block rounded-lg px-3 py-2 text-base font-semibold text-white hover:bg-white/5">Личный кабинет</a>' +
            '<a href="/profile" class="-mx-3 block rounded-lg px-3 py-2 text-base font-semibold text-white hover:bg-white/5">Профиль</a>' +
            '<a href="/logout" class="-mx-3 block rounded-lg px-3 py-2 text-base font-semibold text-indigo-400 hover:bg-white/5">Выйти</a>';
    }
}

// Рендер кнопок для гостя
function renderGuestButtons() {
    const authButtons = document.getElementById('auth-buttons');
    const mobileAuth = document.getElementById('mobile-auth-buttons');
    const template = document.getElementById('guest-buttons-template');

    if (authButtons && template) {
        const clone = template.content.cloneNode(true);
        authButtons.innerHTML = '';
        authButtons.appendChild(clone);
    }

    if (mobileAuth) {
        mobileAuth.innerHTML = 
            '<a href="/login" class="-mx-3 block rounded-lg px-3 py-2 text-base font-semibold text-white hover:bg-white/5">Войти</a>' +
            '<a href="/register" class="-mx-3 block rounded-lg px-3 py-2 text-base font-semibold text-indigo-400 hover:bg-white/5">Регистрация</a>';
    }
}

// Инициализация кнопки "Начать"
function initStartButton() {
    const startButton = document.getElementById('start-button');
    const stepGuide = document.getElementById('step-guide');

    if (startButton && stepGuide) {
        startButton.addEventListener('click', async function() {
            // Проверяем авторизацию
            try {
                const response = await fetch('/api/user/me');
                const data = await response.json();
                
                if (data.authenticated) {
                    // Если авторизован - показываем инструкцию или редиректим
                    stepGuide.classList.toggle('hidden');
                    if (!stepGuide.classList.contains('hidden')) {
                        stepGuide.scrollIntoView({ behavior: 'smooth' });
                    }
                } else {
                    // Если не авторизован - показываем инструкцию с предложением войти
                    stepGuide.classList.remove('hidden');
                    stepGuide.scrollIntoView({ behavior: 'smooth' });
                    
                    // Подсвечиваем кнопку регистрации
                    setTimeout(function() {
                        const registerBtn = document.querySelector('a[href="/register"]');
                        if (registerBtn) {
                            registerBtn.classList.add('animate-pulse-slow');
                            setTimeout(function() {
                                registerBtn.classList.remove('animate-pulse-slow');
                            }, 3000);
                        }
                    }, 500);
                }
            } catch (error) {
                console.error('Error:', error);
                stepGuide.classList.remove('hidden');
            }
        });
    }
}

// Обновление UI для авторизованного пользователя
function updateUIForAuth(user) {
    console.log('User authenticated:', user);
}

// Форматирование даты
function formatDate(dateString) {
    const date = new Date(dateString);
    const day = date.getDate().toString().padStart(2, '0');
    const month = (date.getMonth() + 1).toString().padStart(2, '0');
    const year = date.getFullYear();
    const hours = date.getHours().toString().padStart(2, '0');
    const minutes = date.getMinutes().toString().padStart(2, '0');
    
    return day + '.' + month + '.' + year + ' ' + hours + ':' + minutes;
}

// Форматирование числа
function formatNumber(number) {
    return number.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ' ');
}