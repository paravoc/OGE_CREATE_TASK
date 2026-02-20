// password-manager.js - Универсальный менеджер смены пароля

const PasswordManager = {
    // Текущий режим: 'change' (смена) или 'reset' (восстановление)
    mode: 'change',
    
    // Данные пользователя (если авторизован)
    userData: null,
    
    // Элементы формы
    elements: {
        form: null,
        currentPassGroup: null,
        emailGroup: null,
        newPass: null,
        confirmPass: null,
        submitBtn: null,
        toggleModeBtn: null,
        messageContainer: null
    },
    
    // Инициализация
    init: function(mode = 'change', userData = null) {
        this.mode = mode;
        this.userData = userData;
        
        console.log(`🔐 PasswordManager инициализирован в режиме: ${mode}`);
        
        // Получаем элементы формы
        this.elements.form = document.getElementById('passwordForm');
        this.elements.currentPassGroup = document.getElementById('currentPassGroup');
        this.elements.emailGroup = document.getElementById('emailGroup');
        this.elements.newPass = document.getElementById('newPass');
        this.elements.confirmPass = document.getElementById('confirmPass');
        this.elements.submitBtn = document.getElementById('submitBtn');
        this.elements.toggleModeBtn = document.getElementById('toggleModeBtn');
        this.elements.messageContainer = document.getElementById('passwordMessage');
        
        if (!this.elements.form) {
            console.error('❌ Форма смены пароля не найдена');
            return;
        }
        
        // Настраиваем интерфейс в зависимости от режима
        this.setupUI();
        
        // Добавляем обработчики
        this.addEventListeners();
        
        // Проверяем авторизацию
        this.checkAuth();
    },
    
    // Проверка авторизации
    checkAuth: async function() {
        // Если данные уже переданы через userData
        if (this.userData) {
            console.log('✅ Пользователь авторизован:', this.userData.username);
            return true;
        }
        
        // Пробуем получить через API
        try {
            const response = await fetch('/api/user/me');
            if (response.ok) {
                this.userData = await response.json();
                console.log('✅ Пользователь авторизован:', this.userData.username);
                return true;
            }
        } catch (error) {
            console.error('Ошибка проверки авторизации:', error);
        }
        
        console.log('❌ Пользователь не авторизован');
        return false;
    },
    
    // Настройка интерфейса в зависимости от режима
    setupUI: function() {
        if (this.mode === 'change') {
            // Режим смены пароля (пользователь уже вошёл)
            if (this.elements.currentPassGroup) {
                this.elements.currentPassGroup.style.display = 'block';
            }
            if (this.elements.emailGroup) {
                this.elements.emailGroup.style.display = 'none';
            }
            if (this.elements.toggleModeBtn) {
                this.elements.toggleModeBtn.textContent = 'Забыли пароль?';
                this.elements.toggleModeBtn.href = '#';
            }
            
            // Обновляем заголовок
            const title = document.querySelector('.password-title');
            if (title) {
                title.innerHTML = '🔐 <span class="gradient-text">СМЕНА ПАРОЛЯ</span>';
            }
            
            const subtitle = document.querySelector('.password-subtitle');
            if (subtitle) {
                subtitle.textContent = 'Придумайте новый надёжный пароль для вашего аккаунта';
            }
            
        } else {
            // Режим восстановления (забыл пароль)
            if (this.elements.currentPassGroup) {
                this.elements.currentPassGroup.style.display = 'none';
            }
            if (this.elements.emailGroup) {
                this.elements.emailGroup.style.display = 'block';
            }
            if (this.elements.toggleModeBtn) {
                this.elements.toggleModeBtn.textContent = 'Вспомнили пароль?';
                this.elements.toggleModeBtn.href = '/login';
            }
            
            // Обновляем заголовок
            const title = document.querySelector('.password-title');
            if (title) {
                title.innerHTML = '🔑 <span class="gradient-text">ВОССТАНОВЛЕНИЕ</span>';
            }
            
            const subtitle = document.querySelector('.password-subtitle');
            if (subtitle) {
                subtitle.textContent = 'Введите email и новый пароль для восстановления доступа';
            }
        }
    },
    
    // Добавление обработчиков
    addEventListeners: function() {
        if (this.elements.form) {
            this.elements.form.addEventListener('submit', (e) => this.handleSubmit(e));
        }
        
        if (this.elements.newPass) {
            this.elements.newPass.addEventListener('input', () => this.validatePassword());
        }
        
        if (this.elements.confirmPass) {
            this.elements.confirmPass.addEventListener('input', () => this.checkMatch());
        }
        
        if (this.elements.toggleModeBtn) {
            this.elements.toggleModeBtn.addEventListener('click', (e) => this.toggleMode(e));
        }
        
        // Добавляем переключатели видимости пароля
        document.querySelectorAll('.password-toggle').forEach(btn => {
            btn.addEventListener('click', (e) => {
                const input = e.currentTarget.closest('.input-wrapper').querySelector('input');
                this.togglePasswordVisibility(input);
            });
        });
    },
    
    // Переключение режима
    toggleMode: function(e) {
        e.preventDefault();
        
        if (this.mode === 'change') {
            // Переключаем на восстановление
            this.mode = 'reset';
            this.setupUI();
        } else {
            // Переключаем на смену (только если авторизован)
            this.checkAuth().then(auth => {
                if (auth) {
                    this.mode = 'change';
                    this.setupUI();
                } else {
                    window.location.href = '/login';
                }
            });
        }
    },
    
    // Переключение видимости пароля
    togglePasswordVisibility: function(input) {
        if (!input) return;
        
        const type = input.getAttribute('type') === 'password' ? 'text' : 'password';
        input.setAttribute('type', type);
        
        const btn = input.closest('.input-wrapper').querySelector('.password-toggle');
        const eyeIcon = btn.querySelector('.eye-icon');
        if (eyeIcon) {
            eyeIcon.textContent = type === 'password' ? '👁️' : '👁️‍🗨️';
        }
    },
    
    // Валидация пароля
    validatePassword: function() {
        const password = this.elements.newPass?.value || '';
        
        // Требования к паролю
        const requirements = {
            length: password.length >= 8,
            number: /[0-9]/.test(password),
            upper: /[A-Z]/.test(password),
            special: /[!@#$%^&*()_+\-=\[\]{};':"\\|,.<>\/?]/.test(password)
        };
        
        // Обновляем иконки требований
        Object.keys(requirements).forEach(key => {
            const reqElement = document.getElementById(`req${key.charAt(0).toUpperCase() + key.slice(1)}`);
            if (reqElement) {
                const icon = reqElement.querySelector('.req-icon');
                if (icon) {
                    icon.textContent = requirements[key] ? '●' : '○';
                    icon.style.color = requirements[key] ? '#10b981' : '#ef4444';
                }
            }
        });
        
        // Проверяем все требования
        const isValid = Object.values(requirements).every(v => v === true);
        
        // Обновляем кнопку
        if (this.elements.submitBtn) {
            if (isValid && this.checkMatch(true)) {
                this.elements.submitBtn.disabled = false;
            } else {
                this.elements.submitBtn.disabled = true;
            }
        }
        
        return isValid;
    },
    
    // Проверка совпадения паролей
    checkMatch: function(silent = false) {
        const password = this.elements.newPass?.value || '';
        const confirm = this.elements.confirmPass?.value || '';
        
        const matchElement = document.getElementById('passwordMatch');
        if (!matchElement) return password === confirm && password.length > 0;
        
        if (!password || !confirm) {
            matchElement.textContent = '';
            matchElement.className = 'password-match';
            return false;
        }
        
        if (password === confirm) {
            matchElement.textContent = '✓ Пароли совпадают';
            matchElement.className = 'password-match success';
            return true;
        } else {
            matchElement.textContent = '✗ Пароли не совпадают';
            matchElement.className = 'password-match error';
            return false;
        }
    },
    
    // Показать сообщение
    showMessage: function(text, type) {
        if (!this.elements.messageContainer) return;
        
        this.elements.messageContainer.textContent = text;
        this.elements.messageContainer.className = `password-message ${type}`;
        this.elements.messageContainer.style.display = 'block';
        
        // Автоматически скрываем через 5 секунд
        setTimeout(() => {
            this.elements.messageContainer.style.display = 'none';
        }, 5000);
    },
    
    // Обработка отправки формы
    handleSubmit: async function(e) {
        e.preventDefault();
        
        if (!this.validatePassword()) {
            this.showMessage('❌ Пароль не соответствует требованиям', 'error');
            return;
        }
        
        if (!this.checkMatch()) {
            this.showMessage('❌ Пароли не совпадают', 'error');
            return;
        }
        
        // Блокируем кнопку
        if (this.elements.submitBtn) {
            this.elements.submitBtn.disabled = true;
            this.elements.submitBtn.textContent = '⏳ Обработка...';
        }
        
        // Собираем данные в зависимости от режима
        let data = {};
        
        if (this.mode === 'change') {
            // Режим смены пароля
            const currentPass = document.getElementById('currentPass')?.value;
            
            if (!currentPass) {
                this.showMessage('❌ Введите текущий пароль', 'error');
                this.elements.submitBtn.disabled = false;
                this.elements.submitBtn.textContent = 'Сменить пароль';
                return;
            }
            
            data = {
                current_pass: currentPass,
                new_pass: this.elements.newPass.value,
                confirm_pass: this.elements.confirmPass.value
            };
            
        } else {
            // Режим восстановления
            const email = document.getElementById('resetEmail')?.value;
            
            if (!email) {
                this.showMessage('❌ Введите email', 'error');
                this.elements.submitBtn.disabled = false;
                this.elements.submitBtn.textContent = 'Восстановить пароль';
                return;
            }
            
            data = {
                email: email,
                new_pass: this.elements.newPass.value,
                confirm_pass: this.elements.confirmPass.value
            };
        }
        
        try {
            const response = await fetch(this.mode === 'change' ? '/api/change-password' : '/api/reset-password', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(data)
            });
            
            const result = await response.json();
            
            if (response.ok) {
                this.showMessage(result.message || '✅ Пароль успешно изменён!', 'success');
                
                // Перенаправляем через 2 секунды
                setTimeout(() => {
                    window.location.href = this.mode === 'change' ? '/dashboard' : '/login';
                }, 2000);
                
            } else {
                this.showMessage(result.error || '❌ Ошибка при смене пароля', 'error');
                
                // Разблокируем кнопку
                if (this.elements.submitBtn) {
                    this.elements.submitBtn.disabled = false;
                    this.elements.submitBtn.textContent = this.mode === 'change' ? 'Сменить пароль' : 'Восстановить пароль';
                }
            }
            
        } catch (error) {
            console.error('Ошибка:', error);
            this.showMessage('❌ Ошибка соединения с сервером', 'error');
            
            if (this.elements.submitBtn) {
                this.elements.submitBtn.disabled = false;
                this.elements.submitBtn.textContent = this.mode === 'change' ? 'Сменить пароль' : 'Восстановить пароль';
            }
        }
    }
};

// Автоматическая инициализация
document.addEventListener('DOMContentLoaded', function() {
    // Определяем режим по URL или атрибуту
    const mode = document.body.dataset.passwordMode || 'change';
    
    // Передаём userData из window если есть
    PasswordManager.init(mode, window.USER_DATA);
});