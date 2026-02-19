// categories.js - управление категориями

// Делаем функцию глобальной
window.initCategories = function() {
    console.log('🎯 initCategories вызван');
    const categoryCards = document.querySelectorAll('.category-card');
    console.log('Найдено карточек:', categoryCards.length);
    
    if (categoryCards.length === 0) {
        console.warn('❌ Нет категорий для инициализации');
        return;
    }
    
    categoryCards.forEach((card, index) => {
        // Устанавливаем задержку анимации
        card.style.setProperty('--i', index + 1);
        
        // Удаляем старый обработчик, если был
        card.removeEventListener('click', handleCategoryClick);
        // Добавляем новый
        card.addEventListener('click', handleCategoryClick);
    });
    
    // Обработчики для подкатегорий
    document.querySelectorAll('.subcategory-item[data-topic]').forEach(item => {
        item.removeEventListener('click', handleSubcategoryClick);
        item.addEventListener('click', handleSubcategoryClick);
    });
    
    // Обработчики для уровней
    document.querySelectorAll('.level-item').forEach(item => {
        item.removeEventListener('click', handleLevelClick);
        item.addEventListener('click', handleLevelClick);
    });
    
    // Обработчики для магазина
    document.querySelectorAll('.shop-item-btn').forEach(btn => {
        btn.removeEventListener('click', handleShopClick);
        btn.addEventListener('click', handleShopClick);
    });
    
    console.log('✅ Категории инициализированы');
};

// Обработчик клика по категории
function handleCategoryClick(e) {
    const card = this;
    
    // Проверяем, не кликнули ли по подкатегории
    if (e.target.closest('.subcategory-item') || 
        e.target.closest('.level-item') || 
        e.target.closest('.shop-item') ||
        e.target.closest('.shop-item-btn')) {
        return;
    }
    
    // Закрываем все другие раскрытые категории
    document.querySelectorAll('.category-card').forEach(otherCard => {
        if (otherCard !== card && otherCard.classList.contains('expanded')) {
            otherCard.classList.remove('expanded');
        }
    });
    
    // Переключаем текущую
    card.classList.toggle('expanded');
    console.log('Категория кликнута, expanded:', card.classList.contains('expanded'));
    
    // Добавляем анимацию для подкатегорий
    if (card.classList.contains('expanded')) {
        animateSubItems(card);
    }
}

// Обработчик клика по подкатегории
function handleSubcategoryClick(e) {
    e.stopPropagation();
    const topic = this.dataset.topic;
    console.log('Клик по подкатегории:', topic);
    
    switch(topic) {
        case 'informatics':
            window.location.href = '/generate/informatics';
            break;
        case 'algebra':
            showComingSoon('Алгебра');
            break;
        case 'geometry':
            showComingSoon('Геометрия');
            break;
        case 'python':
            showComingSoon('Python');
            break;
    }
}

// Обработчик клика по уровню
function handleLevelClick(e) {
    e.stopPropagation();
    const level = this.dataset.level;
    showNotification(`Уровень "${level}" в разработке`, 'info');
}

// Обработчик клика по магазину
function handleShopClick(e) {
    e.stopPropagation();
    const item = this.closest('.shop-item').dataset.item;
    console.log('Клик по магазину:', item);
    
    switch(item) {
        case 'credits':
            window.location.href = '/buy_credits';
            break;
        case 'premium':
            window.location.href = '/upgrade';
            break;
        case 'tasks':
            showComingSoon('Пакеты задач');
            break;
    }
}

function animateSubItems(card) {
    const subItems = card.querySelectorAll('.subcategory-item, .level-item, .shop-item');
    subItems.forEach((item, index) => {
        item.style.setProperty('--i', index + 1);
        item.style.animation = 'none';
        item.offsetHeight; // рефлоу
        item.style.animation = null;
    });
}

function showComingSoon(name) {
    showNotification(`${name} скоро появится!`, 'info');
}

function showNotification(message, type) {
    const notif = document.createElement('div');
    notif.className = `category-notification ${type}`;
    notif.textContent = message;
    document.body.appendChild(notif);
    
    setTimeout(() => notif.classList.add('show'), 10);
    setTimeout(() => {
        notif.classList.remove('show');
        setTimeout(() => notif.remove(), 300);
    }, 3000);
}

// Добавляем стили для уведомлений (если ещё нет)
if (!document.querySelector('#category-notification-styles')) {
    const style = document.createElement('style');
    style.id = 'category-notification-styles';
    style.textContent = `
        .category-notification {
            position: fixed;
            top: 20px;
            right: 20px;
            padding: 15px 25px;
            border-radius: 10px;
            color: white;
            font-weight: bold;
            z-index: 9999;
            transform: translateX(400px);
            transition: transform 0.3s ease;
            background: linear-gradient(135deg, #8b5cf6, #6d28d9);
            box-shadow: 0 0 20px #8b5cf6;
        }
        
        .category-notification.show {
            transform: translateX(0);
        }
        
        .category-notification.info {
            background: linear-gradient(135deg, #3b82f6, #2563eb);
        }
        
        .category-notification.success {
            background: linear-gradient(135deg, #10b981, #059669);
        }
        
        .category-notification.error {
            background: linear-gradient(135deg, #ef4444, #dc2626);
        }
    `;
    document.head.appendChild(style);
}

// Не вызываем автоматически, ждём когда auth-aware.js вызовет
console.log('📦 categories.js загружен, функция initCategories доступна как window.initCategories');