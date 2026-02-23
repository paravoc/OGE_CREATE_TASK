document.addEventListener('DOMContentLoaded', function() {
    loadUserData();
    initDashboardMenu();
    initUserMenu();
    loadRecentVariants();
});

// Загрузка данных пользователя
async function loadUserData() {
    try {
        const response = await fetch('/api/user/me');
        const data = await response.json();

        if (data.authenticated && data.user) {
            displayUserData(data.user);
        } else {
            window.location.href = '/login';
        }
    } catch (error) {
        console.error('Error loading user data:', error);
        window.location.href = '/login';
    }
}

// Отображение данных пользователя
function displayUserData(user) {
    // Приветствие
    const welcomeMessage = document.getElementById('welcome-message');
    if (welcomeMessage) {
        welcomeMessage.textContent = 'С возвращением, ' + user.name + '!';
    }

    // Статус аккаунта
    const accountStatus = document.getElementById('account-status');
    if (accountStatus) {
        var status = user.is_premium ? 'Премиум ⭐' : 'Бесплатный';
        accountStatus.textContent = 'Статус: ' + status;
    }

    // Кредиты в шапке
    const creditsSpan = document.getElementById('user-credits');
    if (creditsSpan) {
        creditsSpan.textContent = 'Кредиты: ' + user.credits;
    }

    // Имя пользователя
    const userName = document.getElementById('user-name');
    if (userName) {
        userName.textContent = user.name;
    }

    // Инициалы
    const userInitials = document.getElementById('user-initials');
    if (userInitials) {
        var nameParts = user.name.split(' ');
        var initials = '';
        for (var i = 0; i < Math.min(2, nameParts.length); i++) {
            if (nameParts[i].length > 0) {
                initials += nameParts[i][0].toUpperCase();
            }
        }
        userInitials.textContent = initials;
    }

    // Обновляем статистику
    updateStats(user);

    // Заполняем профиль если есть
    fillProfile(user);
}

// Обновление статистики
function updateStats(user) {
    var freeGenerations = user.is_premium ? 100 : 3;
    var used = user.generations_used || 0;
    var left = Math.max(0, freeGenerations - used);

    var generationsLeft = document.getElementById('generations-left');
    if (generationsLeft) {
        generationsLeft.textContent = left;
    }

    var totalVariants = document.getElementById('total-variants');
    if (totalVariants) {
        totalVariants.textContent = user.total_variants || 0;
    }

    var solvedTasks = document.getElementById('solved-tasks');
    if (solvedTasks) {
        solvedTasks.textContent = user.solved_tasks || 0;
    }

    var credits = document.getElementById('credits');
    if (credits) {
        credits.textContent = user.credits || 0;
    }
}

// Заполнение профиля
function fillProfile(user) {
    var profileName = document.getElementById('profile-name');
    if (profileName) {
        profileName.value = user.name || '';
    }

    var profileEmail = document.getElementById('profile-email');
    if (profileEmail) {
        profileEmail.value = user.email || '';
    }

    var profileStatus = document.getElementById('profile-status');
    if (profileStatus) {
        profileStatus.textContent = user.is_premium ? 'Премиум ⭐' : 'Бесплатный';
    }

    var profileCredits = document.getElementById('profile-credits');
    if (profileCredits) {
        profileCredits.textContent = user.credits || 0;
    }

    var profileGenerations = document.getElementById('profile-generations');
    if (profileGenerations) {
        profileGenerations.textContent = (user.generations_used || 0) + ' / ' + (user.is_premium ? 100 : 3);
    }

    var statVariants = document.getElementById('stat-variants');
    if (statVariants) {
        statVariants.textContent = user.total_variants || 0;
    }

    var statSolved = document.getElementById('stat-solved');
    if (statSolved) {
        statSolved.textContent = user.solved_tasks || 0;
    }

    var statCorrect = document.getElementById('stat-correct');
    if (statCorrect) {
        statCorrect.textContent = user.correct_tasks || 0;
    }
}

// Инициализация меню дашборда
function initDashboardMenu() {
    var menuButton = document.getElementById('dashboard-menu-button');
    var dropdown = document.getElementById('dashboard-dropdown');

    if (menuButton && dropdown) {
        menuButton.addEventListener('click', function(e) {
            e.stopPropagation();
            dropdown.classList.toggle('hidden');
        });
    }
}

// Инициализация пользовательского меню
function initUserMenu() {
    var userMenuButton = document.getElementById('user-menu-button');
    var userDropdown = document.getElementById('user-dropdown');

    if (userMenuButton && userDropdown) {
        userMenuButton.addEventListener('click', function(e) {
            e.stopPropagation();
            userDropdown.classList.toggle('hidden');
        });
    }
}

// Загрузка последних вариантов
async function loadRecentVariants() {
    var variantsList = document.getElementById('recent-variants');
    if (!variantsList) return;

    try {
        var response = await fetch('/api/user/variants?limit=5');
        var data = await response.json();

        if (data.variants && data.variants.length > 0) {
            var html = '';
            for (var i = 0; i < data.variants.length; i++) {
                var variant = data.variants[i];
                var statusClass = variant.status === 'completed' ? 'success' : 'info';
                var statusText = variant.status === 'completed' ? 'Завершен' : 'В процессе';
                
                html += 
                    '<li class="variant-item">' +
                        '<a href="/variant/' + variant.id + '" class="variant-link">' +
                            '<div class="variant-info">' +
                                '<h3>Вариант #' + variant.id + '</h3>' +
                                '<div class="variant-meta">Создан: ' + formatDate(variant.created_at) + '</div>' +
                            '</div>' +
                            '<div class="variant-stats">' +
                                '<span class="variant-count">Заданий: ' + variant.tasks_count + '</span>' +
                                '<span class="status-badge ' + statusClass + '">' + statusText + '</span>' +
                            '</div>' +
                        '</a>' +
                    '</li>';
            }
            variantsList.innerHTML = html;
        } else {
            variantsList.innerHTML = 
                '<li class="empty-state">' +
                    '<div class="empty-state-icon">' +
                        '<svg viewBox="0 0 24 24" fill="none" stroke="currentColor">' +
                            '<path stroke-linecap="round" stroke-linecap="round" stroke-width="2" d="M9 12h6m-6 4h6m2 5H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z" />' +
                        '</svg>' +
                    '</div>' +
                    '<p class="empty-state-text">У вас пока нет созданных вариантов</p>' +
                    '<a href="/generate" class="primary-button">Создать первый вариант</a>' +
                '</li>';
        }
    } catch (error) {
        console.error('Error loading variants:', error);
        variantsList.innerHTML = 
            '<li class="variant-item" style="text-align: center; padding: 1rem; color: #f87171;">' +
                'Ошибка загрузки вариантов' +
            '</li>';
    }
}

// Форматирование даты
function formatDate(dateString) {
    var date = new Date(dateString);
    var day = date.getDate().toString().padStart(2, '0');
    var month = (date.getMonth() + 1).toString().padStart(2, '0');
    var year = date.getFullYear();
    var hours = date.getHours().toString().padStart(2, '0');
    var minutes = date.getMinutes().toString().padStart(2, '0');
    
    return day + '.' + month + '.' + year + ' ' + hours + ':' + minutes;
}