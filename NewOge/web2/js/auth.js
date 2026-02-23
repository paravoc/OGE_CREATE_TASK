document.addEventListener('DOMContentLoaded', function() {
    initLoginForm();
    initRegisterForm();
    initPasswordStrength();
});

// Инициализация формы входа
function initLoginForm() {
    const form = document.getElementById('login-form');
    if (!form) return;

    form.addEventListener('submit', async function(e) {
        e.preventDefault();

        const email = document.getElementById('email').value;
        const password = document.getElementById('password').value;
        const remember = document.getElementById('remember') ? document.getElementById('remember').checked : false;

        // Валидация
        if (!validateEmail(email)) {
            showError('Введите корректный email');
            return;
        }

        if (!password) {
            showError('Введите пароль');
            return;
        }

        // Показываем загрузку
        const submitButton = form.querySelector('button[type="submit"]');
        const originalText = submitButton.textContent;
        submitButton.disabled = true;
        submitButton.innerHTML = '<span class="loader"></span> Вход...';

        try {
            const response = await fetch('/do_login', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ email: email, password: password, remember: remember })
            });

            const data = await response.json();

            if (data.success) {
                // Успешный вход
                window.location.href = data.redirect || '/dashboard';
            } else {
                var message = data.message || 'Ошибка входа';
                showError(message);
                resetButton(submitButton, originalText);
            }
        } catch (error) {
            console.error('Login error:', error);
            showError('Ошибка соединения');
            resetButton(submitButton, originalText);
        }
    });
}

// Инициализация формы регистрации
function initRegisterForm() {
    const form = document.getElementById('register-form');
    if (!form) return;

    form.addEventListener('submit', async function(e) {
        e.preventDefault();

        const name = document.getElementById('name').value;
        const email = document.getElementById('email').value;
        const password = document.getElementById('password').value;
        const confirmPassword = document.getElementById('confirm_password').value;
        const terms = document.getElementById('terms') ? document.getElementById('terms').checked : false;

        // Валидация
        if (!name || name.length < 2) {
            showError('Имя должно содержать минимум 2 символа');
            return;
        }

        if (!validateEmail(email)) {
            showError('Введите корректный email');
            return;
        }

        if (!validatePassword(password)) {
            showError('Пароль должен содержать минимум 6 символов');
            return;
        }

        if (password !== confirmPassword) {
            showError('Пароли не совпадают');
            return;
        }

        if (!terms) {
            showError('Необходимо принять условия использования');
            return;
        }

        // Показываем загрузку
        const submitButton = form.querySelector('button[type="submit"]');
        const originalText = submitButton.textContent;
        submitButton.disabled = true;
        submitButton.innerHTML = '<span class="loader"></span> Регистрация...';

        try {
            const response = await fetch('/do_register', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ name: name, email: email, password: password })
            });

            const data = await response.json();

            if (data.success) {
                // Успешная регистрация
                window.location.href = data.redirect || '/register_success';
            } else {
                var errorMessage = 'Ошибка регистрации';
                
                if (data.error === 'email_exists') {
                    errorMessage = 'Пользователь с таким email уже существует';
                } else if (data.error === 'invalid_email') {
                    errorMessage = 'Некорректный email';
                } else if (data.error === 'password_short') {
                    errorMessage = 'Пароль слишком короткий';
                } else if (data.message) {
                    errorMessage = data.message;
                }
                
                showError(errorMessage);
                resetButton(submitButton, originalText);
            }
        } catch (error) {
            console.error('Register error:', error);
            showError('Ошибка соединения');
            resetButton(submitButton, originalText);
        }
    });
}

// Индикатор сложности пароля
function initPasswordStrength() {
    const passwordInput = document.getElementById('password');
    if (!passwordInput) return;

    // Создаем индикатор
    const strengthIndicator = document.createElement('div');
    strengthIndicator.className = 'password-strength';
    
    const strengthBar = document.createElement('div');
    strengthBar.className = 'password-strength-bar';
    
    strengthIndicator.appendChild(strengthBar);
    passwordInput.parentElement.appendChild(strengthIndicator);

    passwordInput.addEventListener('input', function() {
        const password = passwordInput.value;
        const strength = calculatePasswordStrength(password);

        // Обновляем класс и ширину
        strengthBar.className = 'password-strength-bar ' + strength.class;
        
        if (password.length === 0) {
            strengthBar.style.width = '0';
        }
    });
}

// Расчет сложности пароля
function calculatePasswordStrength(password) {
    let score = 0;
    
    if (password.length >= 8) score++;
    if (password.length >= 12) score++;
    if (/[A-Z]/.test(password)) score++;
    if (/[a-z]/.test(password)) score++;
    if (/[0-9]/.test(password)) score++;
    if (/[^A-Za-z0-9]/.test(password)) score++;

    if (score <= 2) return { class: 'weak', text: 'Слабый' };
    if (score <= 4) return { class: 'medium', text: 'Средний' };
    return { class: 'strong', text: 'Сильный' };
}

// Валидация email
function validateEmail(email) {
    const re = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return re.test(email);
}

// Валидация пароля
function validatePassword(password) {
    return password && password.length >= 6;
}

// Показ ошибки
function showError(message) {
    const errorDiv = document.getElementById('error-message');
    if (errorDiv) {
        errorDiv.textContent = message;
        errorDiv.classList.remove('hidden');
        errorDiv.classList.add('error-shake');
        
        setTimeout(function() {
            errorDiv.classList.remove('error-shake');
        }, 500);
    }
}

// Сброс кнопки
function resetButton(button, text) {
    button.disabled = false;
    button.textContent = text;
}