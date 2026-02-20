// register.js - исправленная версия

// Обработка соцсетей
function handleSocial(provider) {
    const btn = event.currentTarget;
    const originalText = btn.innerHTML;
    
    // Эффект загрузки
    btn.innerHTML = '⏳';
    btn.style.pointerEvents = 'none';
    
    // Показываем уведомление
    showNotification(`Вход через ${provider}`, 'info');
    
    // Имитация перехода
    setTimeout(() => {
        btn.innerHTML = originalText;
        btn.style.pointerEvents = 'auto';
        showNotification('Сервис временно недоступен', 'error');
    }, 1500);
}

// Валидация email
function isValidEmail(email) {
    const re = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return re.test(email);
}

// Уведомления
function showNotification(message, type) {
    const notif = document.createElement('div');
    notif.className = `register-notification ${type}`;
    notif.textContent = message;
    document.body.appendChild(notif);
    
    setTimeout(() => notif.classList.add('show'), 10);
    setTimeout(() => {
        notif.classList.remove('show');
        setTimeout(() => notif.remove(), 300);
    }, 3000);
}

// Добавляем стили уведомлений
const style = document.createElement('style');
style.textContent = `
    .register-notification {
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
        box-shadow: 0 4px 15px rgba(0,0,0,0.2);
    }
    
    .register-notification.show {
        transform: translateX(0);
    }
    
    .register-notification.info {
        background: linear-gradient(45deg, #3b82f6, #8b5cf6);
    }
    
    .register-notification.success {
        background: linear-gradient(45deg, #10b981, #34d399);
    }
    
    .register-notification.error {
        background: linear-gradient(45deg, #ef4444, #f87171);
    }
`;
document.head.appendChild(style);

// Инициализация при загрузке
document.addEventListener('DOMContentLoaded', function() {
    const registerForm = document.getElementById('registerForm');
    
    if (registerForm) {
        registerForm.addEventListener('submit', function(e) {
            const name = document.getElementById('regName').value;
            const email = document.getElementById('regEmail').value;
            const password = document.getElementById('regPassword').value;
            const confirm = document.getElementById('regConfirm').value;
            
            // Валидация
            if (!name || !email || !password || !confirm) {
                e.preventDefault();
                showNotification('❌ Заполните все поля!', 'error');
                return;
            }
            
            if (password !== confirm) {
                e.preventDefault();
                showNotification('❌ Пароли не совпадают!', 'error');
                return;
            }
            
            if (password.length < 6) {
                e.preventDefault();
                showNotification('❌ Пароль минимум 6 символов', 'error');
                return;
            }
            
            if (!isValidEmail(email)) {
                e.preventDefault();
                showNotification('❌ Некорректный email', 'error');
                return;
            }
            
            // Если всё ок - форма отправится на сервер
            const submitBtn = document.querySelector('.submit-btn-reg');
            submitBtn.textContent = '⏳ РЕГИСТРАЦИЯ...';
            submitBtn.disabled = true;
            
            showNotification('⏳ Отправка данных...', 'info');
        });
    }
    
    // Анимация полей
    const inputs = document.querySelectorAll('.form-group-reg input');
    inputs.forEach(input => {
        input.addEventListener('focus', function() {
            this.parentElement.style.transform = 'scale(1.02)';
        });
        
        input.addEventListener('blur', function() {
            this.parentElement.style.transform = 'scale(1)';
        });
    });
});