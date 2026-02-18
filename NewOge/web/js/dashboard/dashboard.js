// dashboard.js - исправленная версия с загрузкой данных с сервера

// Получаем ID пользователя из URL
const userId = window.USER_ID;

// Если нет ID - перенаправляем на вход
if (!userId) {
    window.location.href = '/login';
}

// Элементы для заполнения данными
const elements = {
    userFullName: document.getElementById('userFullName'),
    userNickname: document.getElementById('userNickname'),
    userRegisteredDate: document.getElementById('userRegisteredDate'),
    userStatus: document.getElementById('userStatus'),
    premiumIndicator: document.getElementById('premiumIndicator'),
    tasksSolved: document.getElementById('tasksSolved'),
    generationsLeft: document.getElementById('generationsLeft'),
    userFirstName: document.getElementById('userFirstName'),
    welcomeSubtitle: document.getElementById('welcomeSubtitle'),
    informaticsProgress: document.getElementById('informaticsProgress')
};

// Загружаем данные пользователя с сервера
async function loadUserData() {
    try {
        const response = await fetch(`/api/user?user_id=${userId}`);
        const userData = await response.json();
        
        // Обновляем интерфейс
        updateUI(userData);
    } catch (error) {
        console.error('Ошибка загрузки данных:', error);
    }
}

// Обновление интерфейса
function updateUI(user) {
    // Основная информация
    if (elements.userFullName) {
        elements.userFullName.textContent = user.full_name || user.username;
    }
    
    if (elements.userNickname) {
        elements.userNickname.textContent = '@' + user.username;
    }
    
    if (elements.userRegisteredDate) {
        const date = new Date(user.registered_at * 1000);
        elements.userRegisteredDate.textContent = `📅 С нами с ${date.toLocaleDateString()}`;
    }
    
    // Статус и премиум
    if (user.is_premium) {
        if (elements.userStatus) {
            elements.userStatus.className = 'user-status premium-status';
            elements.userStatus.innerHTML = '⭐ PREMIUM';
        }
        if (elements.premiumIndicator) {
            elements.premiumIndicator.style.display = 'flex';
        }
    } else {
        if (elements.userStatus) {
            elements.userStatus.className = 'user-status';
            elements.userStatus.textContent = 'FREE';
        }
    }
    
    // Статистика
    if (elements.tasksSolved) {
        elements.tasksSolved.textContent = user.tasks_solved || 0;
    }
    
    if (elements.generationsLeft) {
        const maxGen = user.is_premium ? 1000 : 3;
        const left = maxGen - (user.generations_today || 0);
        elements.generationsLeft.textContent = `${left}/${maxGen}`;
    }
    
    // Приветствие
    if (elements.userFirstName) {
        const firstName = (user.full_name || user.username).split(' ')[0];
        elements.userFirstName.textContent = firstName;
    }
    
    if (elements.welcomeSubtitle) {
        const maxGen = user.is_premium ? 1000 : 3;
        const left = maxGen - (user.generations_today || 0);
        elements.welcomeSubtitle.textContent = `Продолжим подготовку? У вас ${left} попыток генерации на сегодня`;
    }
    
    // Прогресс по информатике (пример)
    if (elements.informaticsProgress) {
        // Здесь можно загрузить реальный прогресс из БД
        elements.informaticsProgress.textContent = '0/12 тем';
    }
    
    // Обновляем ссылки с user_id
    document.querySelectorAll('a[href*="/generate"]').forEach(link => {
        link.href = `/generate?user_id=${userId}`;
    });
}

// Загружаем данные при загрузке страницы
document.addEventListener('DOMContentLoaded', loadUserData);

// Элементы модального окна
const logoutBtn = document.getElementById('logoutBtn');
const logoutModal = document.getElementById('logoutModal');
const cancelLogout = document.getElementById('cancelLogout');
const confirmLogout = document.getElementById('confirmLogout');
const modalOverlay = document.querySelector('.modal-overlay');

// Премиум индикатор
const premiumIndicator = document.querySelector('.premium-indicator');

// Открытие модального окна
if (logoutBtn) {
    logoutBtn.addEventListener('click', (e) => {
        e.preventDefault();
        logoutModal.classList.add('active');
        document.body.style.overflow = 'hidden';
    });
}

// Закрытие модального окна
function closeModal() {
    logoutModal.classList.remove('active');
    document.body.style.overflow = '';
}

if (cancelLogout) cancelLogout.addEventListener('click', closeModal);
if (modalOverlay) modalOverlay.addEventListener('click', closeModal);

// Подтверждение выхода
if (confirmLogout) {
    confirmLogout.addEventListener('click', () => {
        confirmLogout.style.transform = 'scale(0.95)';
        setTimeout(() => {
            window.location.href = '/';
        }, 300);
    });
}

// Закрытие по Escape
document.addEventListener('keydown', (e) => {
    if (e.key === 'Escape' && logoutModal?.classList.contains('active')) {
        closeModal();
    }
});

// Обработка клика на премиум индикатор
if (premiumIndicator) {
    premiumIndicator.addEventListener('click', function(e) {
        e.preventDefault();
        e.stopPropagation();
        
        this.style.transform = 'scale(0.95)';
        setTimeout(() => {
            this.style.transform = '';
        }, 200);
        
        // Проверяем реальный премиум статус через API
        fetch(`/api/user?user_id=${userId}`)
            .then(res => res.json())
            .then(user => {
                if (user.is_premium) {
                    showPremiumInfo();
                } else {
                    window.location.href = `/upgrade?user_id=${userId}`;
                }
            });
    });
}

// Функция для показа информации о премиум
function showPremiumInfo() {
    const premiumModal = document.createElement('div');
    premiumModal.className = 'logout-modal active';
    premiumModal.innerHTML = `
        <div class="modal-overlay"></div>
        <div class="modal-content" style="background: linear-gradient(135deg, #1a1e2a, #2a1e3a);">
            <div class="modal-image">
                <img src="images/деньги1.png" alt="Премиум" class="angry-mascot" style="filter: drop-shadow(0 0 30px #ffd700);">
            </div>
            <h3 class="modal-title" style="color: #ffd700;">✨ PREMIUM АКТИВЕН</h3>
            <p class="modal-text">У вас есть доступ ко всем материалам:</p>
            <ul style="color: white; text-align: left; margin-bottom: 2rem; list-style: none;">
                <li style="margin-bottom: 0.5rem;">✓ Все видео-уроки</li>
                <li style="margin-bottom: 0.5rem;">✓ Подробные решения</li>
                <li style="margin-bottom: 0.5rem;">✓ Безлимитная генерация</li>
                <li style="margin-bottom: 0.5rem;">✓ Приоритетная поддержка</li>
            </ul>
            <div class="modal-buttons">
                <button class="modal-btn confirm" id="closePremiumModal" style="background: linear-gradient(135deg, #ffd700, #ffb347); color: #000;">Отлично!</button>
            </div>
        </div>
    `;
    
    document.body.appendChild(premiumModal);
    document.body.style.overflow = 'hidden';
    
    const closeBtn = document.getElementById('closePremiumModal');
    if (closeBtn) {
        closeBtn.addEventListener('click', () => {
            premiumModal.remove();
            document.body.style.overflow = '';
        });
    }
    
    premiumModal.querySelector('.modal-overlay')?.addEventListener('click', () => {
        premiumModal.remove();
        document.body.style.overflow = '';
    });
}

// Клик по заблокированным материалам
document.querySelectorAll('.premium-locked').forEach(card => {
    card.addEventListener('click', function() {
        this.style.transform = 'scale(1.02)';
        setTimeout(() => {
            this.style.transform = '';
        }, 200);
        
        fetch(`/api/user?user_id=${userId}`)
            .then(res => res.json())
            .then(user => {
                if (user.is_premium) {
                    alert('✨ У вас уже есть премиум! Наслаждайтесь материалами.');
                } else {
                    window.location.href = `/upgrade?user_id=${userId}`;
                }
            });
    });
});

// Переход в категорию
document.querySelectorAll('.category-card:not(.coming-soon) .category-btn').forEach(btn => {
    btn.addEventListener('click', function(e) {
        e.stopPropagation();
        const card = this.closest('.category-card');
        const category = card?.dataset.category;
        
        this.style.transform = 'scale(0.95)';
        setTimeout(() => {
            this.style.transform = '';
            if (category) {
                window.location.href = `/category?cat=${category}&user_id=${userId}`;
            }
        }, 300);
    });
});

// Обновление времени до сброса
function updateTimer() {
    const timerElement = document.querySelector('.stat-mini-item:last-child .stat-mini-value');
    if (!timerElement) return;
    
    const now = new Date();
    const midnight = new Date(now);
    midnight.setHours(24, 0, 0, 0);
    
    const diffMs = midnight - now;
    const diffHours = Math.floor(diffMs / (1000 * 60 * 60));
    const diffMinutes = Math.floor((diffMs % (1000 * 60 * 60)) / (1000 * 60));
    
    timerElement.textContent = `${diffHours}ч ${diffMinutes}м`;
    
    // Обновляем каждую минуту
    setTimeout(updateTimer, 60000);
}

updateTimer();

// Анимация появления при скролле
const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.style.opacity = '1';
            entry.target.style.transform = 'translateY(0)';
        }
    });
}, { threshold: 0.1 });

document.querySelectorAll('.category-card, .material-card, .user-card, .stats-mini').forEach(el => {
    el.style.opacity = '0';
    el.style.transform = 'translateY(20px)';
    el.style.transition = 'all 0.6s ease';
    observer.observe(el);
});

// Эффект свечения для премиума
setInterval(() => {
    if (premiumIndicator) {
        premiumIndicator.style.transform = 'scale(1.02)';
        setTimeout(() => {
            premiumIndicator.style.transform = 'scale(1)';
        }, 200);
    }
}, 3000);

// Стили для курсора на премиум индикатор
if (premiumIndicator) {
    premiumIndicator.style.cursor = 'pointer';
    premiumIndicator.style.transition = 'all 0.3s ease';
    
    premiumIndicator.addEventListener('mouseenter', function() {
        this.style.transform = 'scale(1.05)';
        this.style.boxShadow = '0 0 30px rgba(255, 215, 0, 0.8)';
    });
    
    premiumIndicator.addEventListener('mouseleave', function() {
        this.style.transform = 'scale(1)';
        this.style.boxShadow = '0 0 20px rgba(255, 215, 0, 0.5)';
    });
}