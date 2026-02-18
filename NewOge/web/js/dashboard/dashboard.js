// Элементы модального окна
const logoutBtn = document.getElementById('logoutBtn');
const logoutModal = document.getElementById('logoutModal');
const cancelLogout = document.getElementById('cancelLogout');
const confirmLogout = document.getElementById('confirmLogout');
const modalOverlay = document.querySelector('.modal-overlay');

// Премиум индикатор
const premiumIndicator = document.querySelector('.premium-indicator');

// Открытие модального окна
logoutBtn.addEventListener('click', (e) => {
    e.preventDefault();
    logoutModal.classList.add('active');
    document.body.style.overflow = 'hidden';
});

// Закрытие модального окна
function closeModal() {
    logoutModal.classList.remove('active');
    document.body.style.overflow = '';
}

cancelLogout.addEventListener('click', closeModal);
modalOverlay.addEventListener('click', closeModal);

// Подтверждение выхода
confirmLogout.addEventListener('click', () => {
    confirmLogout.style.transform = 'scale(0.95)';
    
    setTimeout(() => {
        window.location.href = 'index.html';
    }, 300);
});

// Закрытие по Escape
document.addEventListener('keydown', (e) => {
    if (e.key === 'Escape' && logoutModal.classList.contains('active')) {
        closeModal();
    }
});

// Обработка клика на премиум индикатор
if (premiumIndicator) {
    premiumIndicator.addEventListener('click', function(e) {
        e.preventDefault();
        e.stopPropagation();
        
        // Анимация клика
        this.style.transform = 'scale(0.95)';
        setTimeout(() => {
            this.style.transform = '';
        }, 200);
        
        // Здесь можно открыть модальное окно с информацией о премиум
        // или перейти на страницу тарифов
        alert('✨ Премиум подписка активна! Спасибо что вы с нами!');
        
        // Или можно открыть модальное окно с информацией
        // showPremiumInfo();
    });
}

// Функция для показа информации о премиум
function showPremiumInfo() {
    const premiumModal = document.createElement('div');
    premiumModal.className = 'logout-modal active'; // Используем те же стили что и для выхода
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
    
    // Закрытие модального окна
    const closeBtn = document.getElementById('closePremiumModal');
    closeBtn.addEventListener('click', () => {
        premiumModal.remove();
        document.body.style.overflow = '';
    });
    
    // Закрытие по клику на оверлей
    premiumModal.querySelector('.modal-overlay').addEventListener('click', () => {
        premiumModal.remove();
        document.body.style.overflow = '';
    });
}

// Клик по заблокированным материалам
document.querySelectorAll('.premium-locked').forEach(card => {
    card.addEventListener('click', function() {
        // Анимация
        this.style.transform = 'scale(1.02)';
        setTimeout(() => {
            this.style.transform = '';
        }, 200);
        
        // Проверяем премиум статус (временно всегда показываем сообщение о необходимости премиум)
        alert('⭐ Этот материал доступен только с PREMIUM подпиской! Оформите подписку чтобы получить доступ.');
    });
});

// Переход в категорию
document.querySelectorAll('.category-card:not(.coming-soon) .category-btn').forEach(btn => {
    btn.addEventListener('click', function(e) {
        e.stopPropagation();
        const card = this.closest('.category-card');
        const category = card.dataset.category;
        
        this.style.transform = 'scale(0.95)';
        setTimeout(() => {
            this.style.transform = '';
            window.location.href = `category.html?cat=${category}`;
        }, 300);
    });
});

// Открытие материалов
document.querySelectorAll('.material-card:not(.premium-locked) .material-btn').forEach(btn => {
    btn.addEventListener('click', function(e) {
        e.stopPropagation();
        
        this.style.transform = 'scale(0.95)';
        setTimeout(() => {
            this.style.transform = '';
            alert('Открытие материала...');
        }, 200);
    });
});

// Обновление времени до сброса
function updateTimer() {
    const timerElement = document.querySelector('.stat-mini-item:last-child .stat-mini-value');
    if (!timerElement) return;
    
    let hours = 2;
    let minutes = 15;
    
    setInterval(() => {
        minutes--;
        if (minutes < 0) {
            minutes = 59;
            hours--;
        }
        
        if (hours < 0) {
            hours = 2;
            minutes = 15;
        }
        
        timerElement.textContent = `${hours}ч ${minutes}м`;
    }, 60000);
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

// Добавляем стили для курсора на премиум индикатор
if (premiumIndicator) {
    premiumIndicator.style.cursor = 'pointer';
    premiumIndicator.style.transition = 'all 0.3s ease';
    
    // Эффект при наведении
    premiumIndicator.addEventListener('mouseenter', function() {
        this.style.transform = 'scale(1.05)';
        this.style.boxShadow = '0 0 30px rgba(255, 215, 0, 0.8)';
    });
    
    premiumIndicator.addEventListener('mouseleave', function() {
        this.style.transform = 'scale(1)';
        this.style.boxShadow = '0 0 20px rgba(255, 215, 0, 0.5)';
    });
}