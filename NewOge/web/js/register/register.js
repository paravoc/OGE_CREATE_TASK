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

// Обработка формы регистрации
document.addEventListener('DOMContentLoaded', function() {
    const registerForm = document.getElementById('registerForm');
    
    if (registerForm) {
        registerForm.addEventListener('submit', function(e) {
            e.preventDefault();
            
            const name = document.getElementById('regName').value;
            const email = document.getElementById('regEmail').value;
            const password = document.getElementById('regPassword').value;
            const confirm = document.getElementById('regConfirm').value;
            
            // Валидация
            if (!name || !email || !password || !confirm) {
                showNotification('Заполните все поля!', 'error');
                return;
            }
            
            if (password !== confirm) {
                showNotification('Пароли не совпадают!', 'error');
                return;
            }
            
            if (password.length < 6) {
                showNotification('Пароль минимум 6 символов', 'error');
                return;
            }
            
            if (!isValidEmail(email)) {
                showNotification('Некорректный email', 'error');
                return;
            }
            
            // Показываем загрузку
            const submitBtn = document.querySelector('.submit-btn-reg');
            submitBtn.textContent = '⏳ РЕГИСТРАЦИЯ...';
            submitBtn.disabled = true;
            
            // Успешная регистрация
            setTimeout(() => {
                showNotification('Регистрация успешна!', 'success');
                
                setTimeout(() => {
                    window.location.href = 'index.html';
                }, 1500);
            }, 1500);
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

// Валидация email
function isValidEmail(email) {
    const re = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return re.test(email);
}

// Переход на логин
function goToLogin() {
    showNotification('Переход на страницу входа', 'info');
    setTimeout(() => {
        window.location.href = 'login.html';
    }, 500);
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
    .register-notification.info {
        background: #3b82f6;
        box-shadow: 0 0 20px #3b82f6;
    }
`;
document.head.appendChild(style);