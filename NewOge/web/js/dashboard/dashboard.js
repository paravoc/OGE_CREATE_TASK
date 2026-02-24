// dashboard.js - ИСПРАВЛЕННАЯ ВЕРСИЯ

// Элементы для заполнения данными
const elements = {
    userFullName: document.getElementById('userFullName'),
    userNickname: document.getElementById('userNickname'),
    userRegisteredDate: document.getElementById('userRegisteredDate'),
    userStatus: document.getElementById('userStatus'),
    premiumIndicator: document.getElementById('premiumIndicator'),
    userFirstName: document.getElementById('userFirstName'),
    welcomeSubtitle: document.getElementById('welcomeSubtitle'),
    generationsLeft: document.getElementById('generationsLeft'),
    totalGenerations: document.getElementById('totalGenerations'),
    userCredits: document.getElementById('userCredits'),
    statTotalGenerations: document.getElementById('statTotalGenerations'),
    statTasksSolved: document.getElementById('statTasksSolved'),
    statAccuracy: document.getElementById('statAccuracy'),
    statCredits: document.getElementById('statCredits'),
    informaticsProgressFill: document.getElementById('informaticsProgressFill'),
    informaticsProgress: document.getElementById('informaticsProgress'),
    timeToReset: document.getElementById('timeToReset'),
    userAvatar: document.getElementById('userAvatar')
};

// Функция для обновления всех ссылок
function updateAllLinks() {
    document.querySelectorAll('a[href*="/generate"]').forEach(link => {
        link.href = '/generate';
    });
    document.querySelectorAll('a[href*="/dashboard"]').forEach(link => {
        link.href = '/dashboard';
    });
    document.querySelectorAll('a[href*="/status"]').forEach(link => {
        link.href = '/status';
    });
    document.querySelectorAll('a[href*="/buy_credits"]').forEach(link => {
        link.href = '/buy_credits';
    });
    document.querySelectorAll('a[href*="/upgrade"]').forEach(link => {
        link.href = '/upgrade';
    });
    console.log('✅ Ссылки обновлены');
}

// Функция для загрузки данных с сервера
async function loadUserData() {
    try {
        console.log('📥 Загрузка данных пользователя...');
        
        // Загружаем данные пользователя
        const userResponse = await fetch('/api/user/me');
        if (!userResponse.ok) {
            console.error('❌ Ошибка загрузки пользователя');
            window.location.href = '/login';
            return;
        }
        
        const userData = await userResponse.json();
        console.log('✅ Данные пользователя:', userData);
        
        // Загружаем прогресс
        let progressData = null;
        try {
            const progressResponse = await fetch('/api/user/progress');
            if (progressResponse.ok) {
                progressData = await progressResponse.json();
                console.log('✅ Прогресс загружен');
            }
        } catch (e) {
            console.warn('⚠️ Не удалось загрузить прогресс:', e);
        }
        
        // Обновляем UI
        updateUI(userData, progressData);
        
    } catch (error) {
        console.error('❌ Ошибка загрузки данных:', error);
    }
}

// Обновление интерфейса
function updateUI(user, progress) {
    console.log('🔄 Обновление UI...');
    
    // ===== 1. ИНФОРМАЦИЯ О ПОЛЬЗОВАТЕЛЕ =====
    const fullName = user.full_name || user.username;
    if (elements.userFullName) elements.userFullName.textContent = fullName;
    if (elements.userNickname) elements.userNickname.textContent = '@' + user.username;
    
    // Дата регистрации
    if (elements.userRegisteredDate && user.registered_at) {
        const date = new Date(user.registered_at * 1000);
        elements.userRegisteredDate.textContent = `📅 С нами с ${date.toLocaleDateString('ru-RU')}`;
    }
    
    // Имя для приветствия
    if (elements.userFirstName) {
        const firstName = (user.full_name || user.username).split(' ')[0];
        elements.userFirstName.textContent = firstName;
    }
    
    // Аватар
    if (elements.userAvatar && user.username) {
        elements.userAvatar.src = `https://ui-avatars.com/api/?name=${encodeURIComponent(user.username)}&background=ffb347&color=000&size=100`;
    }
    
    // ===== 2. PREMIUM СТАТУС =====
    if (user.is_premium) {
        if (elements.userStatus) {
            elements.userStatus.innerHTML = '⭐ PREMIUM';
            elements.userStatus.style.color = '#ffb347';
        }
        if (elements.premiumIndicator) {
            elements.premiumIndicator.style.display = 'flex';
        }
    } else {
        if (elements.userStatus) {
            elements.userStatus.innerHTML = '⭐ FREE';
            elements.userStatus.style.color = '#aaa';
        }
        if (elements.premiumIndicator) {
            elements.premiumIndicator.style.display = 'none';
        }
    }
    
    // ===== 3. КРЕДИТЫ =====
    const credits = user.credits || 0;
    if (elements.userCredits) elements.userCredits.textContent = credits;
    if (elements.statCredits) elements.statCredits.textContent = credits;
    
    // ===== 4. ГЕНЕРАЦИИ =====
    const generationsToday = user.generations_today || 0;
    const totalGenerations = user.total_generations || 0;
    const dailyLimit = user.is_premium ? 1000 : 3;
    const remaining = dailyLimit - generationsToday;
    
    if (elements.generationsLeft) {
        elements.generationsLeft.textContent = `${generationsToday}/${dailyLimit}`;
    }
    if (elements.totalGenerations) {
        elements.totalGenerations.textContent = totalGenerations;
    }
    if (elements.statTotalGenerations) {
        elements.statTotalGenerations.textContent = totalGenerations;
    }
    
    // Подпись приветствия
    if (elements.welcomeSubtitle) {
        elements.welcomeSubtitle.textContent = 
            `Продолжим подготовку? У вас ${remaining} ${getGenerationWord(remaining)} на сегодня`;
    }
    
    // ===== 5. ЗАДАЧИ =====
    const tasksSolved = user.tasks_solved || 0;
    const accuracy = user.accuracy || 0;
    
    if (elements.statTasksSolved) elements.statTasksSolved.textContent = tasksSolved;
    if (elements.statAccuracy) elements.statAccuracy.textContent = accuracy + '%';
    
    // ===== 6. ПРОГРЕСС ПО ИНФОРМАТИКЕ =====
    if (progress && progress.subjects && progress.subjects.informatics) {
        const informatics = progress.subjects.informatics;
        const completed = informatics.completed_lessons || 0;
        const total = informatics.total_lessons || 18;
        const percent = total > 0 ? Math.round((completed / total) * 100) : 0;
        
        if (elements.informaticsProgressFill) {
            elements.informaticsProgressFill.style.width = percent + '%';
        }
        if (elements.informaticsProgress) {
            elements.informaticsProgress.textContent = `${completed}/${total} уроков`;
        }
    }
    
    console.log('✅ UI обновлен');
}

// Склонение слова "генерация"
function getGenerationWord(count) {
    if (count % 10 === 1 && count % 100 !== 11) return 'генерация';
    if ([2, 3, 4].includes(count % 10) && ![12, 13, 14].includes(count % 100)) return 'генерации';
    return 'генераций';
}

// Обновление времени до сброса
function updateTimer() {
    if (!elements.timeToReset) return;
    
    const now = new Date();
    const midnight = new Date(now);
    midnight.setHours(24, 0, 0, 0);
    
    const diffMs = midnight - now;
    const diffHours = Math.floor(diffMs / (1000 * 60 * 60));
    const diffMinutes = Math.floor((diffMs % (1000 * 60 * 60)) / (1000 * 60));
    
    elements.timeToReset.textContent = `${diffHours}ч ${diffMinutes}м`;
    
    setTimeout(updateTimer, 60000);
}

// Инициализация
document.addEventListener('DOMContentLoaded', function() {
    console.log('📊 Dashboard загружен');
    updateAllLinks();
    loadUserData();
    updateTimer();
    
    // Настройка модального окна
    setupLogoutModal();
    setupPremiumIndicator();
    setupCategoryCards();
    setupMaterials();
});

// ===== МОДАЛЬНОЕ ОКНО ВЫХОДА =====
function setupLogoutModal() {
    const logoutBtn = document.getElementById('logoutBtn');
    const logoutModal = document.getElementById('logoutModal');
    const cancelLogout = document.getElementById('cancelLogout');
    const confirmLogout = document.getElementById('confirmLogout');
    const modalOverlay = document.querySelector('.modal-overlay');
    
    if (!logoutBtn || !logoutModal) return;
    
    // Открытие
    logoutBtn.addEventListener('click', (e) => {
        e.preventDefault();
        logoutModal.classList.add('active');
        document.body.style.overflow = 'hidden';
    });
    
    // Закрытие
    function closeModal() {
        logoutModal.classList.remove('active');
        document.body.style.overflow = '';
    }
    
    if (cancelLogout) cancelLogout.addEventListener('click', closeModal);
    if (modalOverlay) modalOverlay.addEventListener('click', closeModal);
    
    // Подтверждение выхода
    if (confirmLogout) {
        confirmLogout.addEventListener('click', async () => {
            confirmLogout.textContent = '⏳ Выход...';
            confirmLogout.style.pointerEvents = 'none';
            
            try {
                await fetch('/logout');
                window.location.href = '/login';
            } catch (error) {
                console.error('Ошибка при выходе:', error);
                window.location.href = '/login';
            }
        });
    }
    
    // Закрытие по Escape
    document.addEventListener('keydown', (e) => {
        if (e.key === 'Escape' && logoutModal?.classList.contains('active')) {
            closeModal();
        }
    });
}

// ===== PREMIUM ИНДИКАТОР =====
function setupPremiumIndicator() {
    const premiumIndicator = document.getElementById('premiumIndicator');
    if (!premiumIndicator) return;
    
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
    
    premiumIndicator.addEventListener('click', function(e) {
        e.preventDefault();
        e.stopPropagation();
        
        if (window.userData?.is_premium) {
            showPremiumInfo();
        } else {
            window.location.href = '/upgrade';
        }
    });
    
    // Эффект пульсации
    setInterval(() => {
        if (premiumIndicator.style.display !== 'none') {
            premiumIndicator.style.transform = 'scale(1.02)';
            setTimeout(() => {
                premiumIndicator.style.transform = 'scale(1)';
            }, 200);
        }
    }, 3000);
}

// Показать информацию о премиум
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

// ===== КАРТОЧКИ КАТЕГОРИЙ =====
function setupCategoryCards() {
    document.querySelectorAll('.category-card:not(.coming-soon) .category-btn').forEach(btn => {
        btn.addEventListener('click', function(e) {
            e.stopPropagation();
            const card = this.closest('.category-card');
            const category = card?.dataset.category;
            
            this.style.transform = 'scale(0.95)';
            setTimeout(() => {
                this.style.transform = '';
                if (category) {
                    window.location.href = `/category?cat=${category}`;
                } else {
                    window.location.href = '/oge/informatics';
                }
            }, 300);
        });
    });
}

// ===== ПРЕМИУМ МАТЕРИАЛЫ =====
function setupMaterials() {
    document.querySelectorAll('.premium-locked').forEach(card => {
        card.addEventListener('click', function() {
            this.style.transform = 'scale(1.02)';
            setTimeout(() => {
                this.style.transform = '';
            }, 200);
            
            fetch('/api/user/me')
                .then(res => res.json())
                .then(userData => {
                    if (userData.is_premium) {
                        alert('✨ У вас уже есть премиум! Наслаждайтесь материалами.');
                    } else {
                        window.location.href = '/upgrade';
                    }
                })
                .catch(() => {
                    window.location.href = '/upgrade';
                });
        });
    });
}

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