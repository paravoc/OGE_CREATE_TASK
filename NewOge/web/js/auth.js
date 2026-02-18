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
    
    if (type === 'error') msg.style.background = '#ef4444';
    else if (type === 'success') msg.style.background = '#10b981';
    else msg.style.background = '#8b5cf6';
    
    msg.textContent = text;
    document.body.appendChild(msg);
    
    setTimeout(() => msg.remove(), 3000);
}

// При загрузке страницы
document.addEventListener('DOMContentLoaded', function() {
    console.log('✅ auth.js загружен');
    
    // Обработка формы регистрации
    const registerForm = document.getElementById('registerForm');
    if (registerForm) {
        console.log('📝 Форма регистрации найдена');
        
        registerForm.addEventListener('submit', function(event) {
            // Не отменяем стандартную отправку - пусть сервер обрабатывает
            console.log('📤 Отправка формы регистрации');
            
            // Можно добавить проверку паролей
            const pass = document.getElementById('regPassword')?.value;
            const confirm = document.getElementById('regConfirm')?.value;
            
            if (pass && confirm && pass !== confirm) {
                event.preventDefault();
                showMessage('❌ Пароли не совпадают', 'error');
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
            showMessage('⏳ Вход...', 'info');
            // Не отменяем - пусть сервер обрабатывает
        });
    }
    
    // Обработка кнопок соцсетей
    const socialButtons = document.querySelectorAll('.social-btn-reg');
    socialButtons.forEach(btn => {
        btn.addEventListener('click', function() {
            showMessage('⚡ Вход через соцсети будет позже', 'info');
        });
    });
});