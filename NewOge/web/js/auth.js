// js/auth.js - общий файл для регистрации и входа

// Показать сообщение пользователю
function showMessage(text, type) {
    const msg = document.createElement('div');
    msg.style.position = 'fixed';
    msg.style.top = '20px';
    msg.style.right = '20px';
    msg.style.padding = '15px 25px';
    msg.style.borderRadius = '10px';
    msg.style.color = 'white';
    msg.style.zIndex = '9999';
    msg.style.fontWeight = 'bold';
    msg.style.boxShadow = '0 4px 6px rgba(0,0,0,0.1)';
    msg.style.animation = 'slideIn 0.3s ease';
    
    if (type === 'error') msg.style.background = '#ef4444';
    else if (type === 'success') msg.style.background = '#10b981';
    else msg.style.background = '#8b5cf6';
    
    msg.textContent = text;
    document.body.appendChild(msg);
    
    setTimeout(() => {
        msg.style.animation = 'slideOut 0.3s ease';
        setTimeout(() => msg.remove(), 300);
    }, 3000);
}

// Добавляем стили для анимации
const style = document.createElement('style');
style.textContent = `
    @keyframes slideIn {
        from { transform: translateX(100%); opacity: 0; }
        to { transform: translateX(0); opacity: 1; }
    }
    @keyframes slideOut {
        from { transform: translateX(0); opacity: 1; }
        to { transform: translateX(100%); opacity: 0; }
    }
`;
document.head.appendChild(style);

// При загрузке страницы
document.addEventListener('DOMContentLoaded', function() {
    console.log('✅ auth.js загружен');
    
    // Проверка URL параметров ошибок
    const urlParams = new URLSearchParams(window.location.search);
    const error = urlParams.get('error');
    
    if (error) {
        let message = '';
        switch(error) {
            case 'empty':
                message = '❌ Заполните все поля';
                break;
            case 'email_exists':
                message = '❌ Этот email уже зарегистрирован';
                break;
            case 'invalid':
                message = '❌ Неверный email или пароль';
                break;
            case 'password_mismatch':
                message = '❌ Пароли не совпадают';
                break;
            case 'password_short':
                message = '❌ Пароль должен быть минимум 6 символов';
                break;
            default:
                message = '❌ Ошибка: ' + error;
        }
        showMessage(message, 'error');
    }
    
    // Обработка формы регистрации
    const registerForm = document.getElementById('registerForm');
    if (registerForm) {
        console.log('📝 Форма регистрации найдена');
        
        registerForm.addEventListener('submit', function(event) {
            console.log('📤 Отправка формы регистрации');
            
            const username = document.getElementById('regName')?.value;
            const email = document.getElementById('regEmail')?.value;
            const pass = document.getElementById('regPassword')?.value;
            const confirm = document.getElementById('regConfirm')?.value;
            
            // Клиентская валидация
            if (!username || !email || !pass || !confirm) {
                event.preventDefault();
                showMessage('❌ Заполните все поля', 'error');
                return;
            }
            
            if (pass !== confirm) {
                event.preventDefault();
                showMessage('❌ Пароли не совпадают', 'error');
                return;
            }
            
            if (pass.length < 6) {
                event.preventDefault();
                showMessage('❌ Пароль должен быть минимум 6 символов', 'error');
                return;
            }
            
            // Показываем сообщение о загрузке
            showMessage('⏳ Регистрация...', 'info');
        });
    }
    
    // Обработка формы входа
    const loginForm = document.getElementById('loginForm');
    if (loginForm) {
        console.log('🔐 Форма входа найдена');
        
        loginForm.addEventListener('submit', function(event) {
            console.log('📤 Отправка формы входа');
            
            const email = document.getElementById('loginEmail')?.value;
            const password = document.getElementById('loginPassword')?.value;
            
            if (!email || !password) {
                event.preventDefault();
                showMessage('❌ Заполните все поля', 'error');
                return;
            }
            
            showMessage('⏳ Вход...', 'info');
        });
    }
    
    // Обработка кнопок соцсетей
    const socialButtons = document.querySelectorAll('.social-btn-reg');
    socialButtons.forEach(btn => {
        btn.addEventListener('click', function() {
            const provider = this.textContent.trim();
            showMessage(`⚡ Вход через ${provider} будет доступен позже`, 'info');
        });
    });
});