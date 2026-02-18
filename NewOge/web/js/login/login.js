// Обработка социальных сетей
function handleSocial(provider) {
    console.log(`Вход через ${provider}`);
    // Здесь будет реальная интеграция с соцсетями
    alert(`Вход через ${provider} будет доступен soon!`);
}

// Восстановление пароля
function forgotPassword() {
    const email = document.getElementById('loginEmail').value;
    if (email) {
        alert(`Инструкция по восстановлению отправлена на ${email}`);
    } else {
        alert('Пожалуйста, введите email в поле выше');
    }
}

// Обработка формы входа
document.getElementById('loginForm').addEventListener('submit', function(e) {
    e.preventDefault();
    
    const email = document.getElementById('loginEmail').value;
    const password = document.getElementById('loginPassword').value;
    const remember = document.getElementById('rememberMe').checked;
    
    // Здесь будет реальная авторизация
    console.log('Вход:', { email, password, remember });
    
    // Имитация успешного входа
    alert('Вход выполнен успешно! Перенаправление...');
    
    // Анимация кнопки
    const btn = document.querySelector('.submit-btn-reg');
    btn.style.transform = 'scale(0.95)';
    setTimeout(() => {
        btn.style.transform = '';
        // Перенаправление на главную
        window.location.href = 'index.html';
    }, 500);
});

// Анимация полей при фокусе
document.querySelectorAll('.form-group-reg input').forEach(input => {
    input.addEventListener('focus', function() {
        this.parentElement.classList.add('focused');
    });
    
    input.addEventListener('blur', function() {
        if (!this.value) {
            this.parentElement.classList.remove('focused');
        }
    });
});

// Проверка заполненных полей при загрузке
document.querySelectorAll('.form-group-reg input').forEach(input => {
    if (input.value) {
        input.parentElement.classList.add('focused');
    }
});

// Добавляем блеск на кнопки соцсетей
document.querySelectorAll('.social-btn-reg').forEach(btn => {
    btn.addEventListener('mouseenter', function() {
        this.style.transform = 'translateY(-3px)';
    });
    
    btn.addEventListener('mouseleave', function() {
        this.style.transform = '';
    });
});

// Эффект печати для заголовка
const title = document.querySelector('.reg-title');
if (title) {
    const text = title.textContent;
    title.textContent = '';
    let i = 0;
    
    function typeWriter() {
        if (i < text.length) {
            title.textContent += text.charAt(i);
            i++;
            setTimeout(typeWriter, 100);
        }
    }
    
    // Запускаем печать через небольшую задержку
    setTimeout(typeWriter, 500);
}

// Валидация email в реальном времени
document.getElementById('loginEmail').addEventListener('input', function() {
    const email = this.value;
    const pattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    
    if (email && !pattern.test(email)) {
        this.style.borderBottomColor = '#ff4757';
    } else {
        this.style.borderBottomColor = '';
    }
});

// Предотвращаем закрытие при нажатии Enter на пустых полях
document.querySelectorAll('.form-group-reg input').forEach(input => {
    input.addEventListener('keypress', function(e) {
        if (e.key === 'Enter') {
            e.preventDefault();
            document.querySelector('.submit-btn-reg').click();
        }
    });
});

// Показываем/скрываем пароль
const passwordField = document.getElementById('loginPassword');
if (passwordField) {
    // Создаем кнопку показа пароля
    const toggleBtn = document.createElement('span');
    toggleBtn.innerHTML = '👁️';
    toggleBtn.style.cssText = `
        position: absolute;
        right: 0;
        top: 50%;
        transform: translateY(-50%);
        cursor: pointer;
        opacity: 0.5;
        transition: 0.3s;
        z-index: 10;
    `;
    
    toggleBtn.addEventListener('mouseenter', () => toggleBtn.style.opacity = '1');
    toggleBtn.addEventListener('mouseleave', () => toggleBtn.style.opacity = '0.5');
    
    toggleBtn.addEventListener('click', function() {
        const type = passwordField.getAttribute('type') === 'password' ? 'text' : 'password';
        passwordField.setAttribute('type', type);
        this.innerHTML = type === 'password' ? '👁️' : '👁️‍🗨️';
    });
    
    passwordField.parentElement.style.position = 'relative';
    passwordField.parentElement.appendChild(toggleBtn);
}

// Сохранение состояния "Запомнить меня"
const rememberCheckbox = document.getElementById('rememberMe');
if (rememberCheckbox) {
    // Проверяем localStorage
    const saved = localStorage.getItem('rememberMe') === 'true';
    rememberCheckbox.checked = saved;
    
    rememberCheckbox.addEventListener('change', function() {
        localStorage.setItem('rememberMe', this.checked);
    });
}