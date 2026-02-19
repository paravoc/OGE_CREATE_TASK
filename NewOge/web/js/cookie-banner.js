// Функция для отладки
function debugCookieStatus() {
    const consent = localStorage.getItem('cookieConsent');
    const userId = window.USER_ID;
    
    console.log('%c=== COOKIE DEBUG ===', 'color: #8b5cf6; font-size: 14px');
    console.log('localStorage cookieConsent:', consent);
    console.log('Тип значения:', typeof consent);
    console.log('Длина:', consent ? consent.length : 0);
    console.log('USER_ID:', userId);
    console.log('Авторизован:', !!userId);
    
    if (consent === 'true') {
        console.log('%c✅ СОГЛАСИЕ ДАНО', 'color: #10b981');
    } else if (consent === null) {
        console.log('%c❌ СОГЛАСИЕ НЕ ДАНО (первый визит)', 'color: #ef4444');
    } else {
        console.log('%c⚠️ НЕИЗВЕСТНОЕ ЗНАЧЕНИЕ: ' + consent, 'color: #f59e0b');
    }
}


// Cookie Banner Management
const CookieBanner = {
    init: function() {
        // Проверяем, показывали ли уже баннер
        if (!localStorage.getItem('cookieConsent')) {
            this.showBanner();
        }
        
        // Добавляем стили для модального окна если их нет
        this.addModalStyles();
    },
    
    showBanner: function() {
        // Создаем баннер если его нет
        if (!document.getElementById('cookieConsentBanner')) {
            this.createBanner();
        }
        document.getElementById('cookieConsentBanner').style.display = 'block';
    },
    
    createBanner: function() {
        const banner = document.createElement('div');
        banner.id = 'cookieConsentBanner';
        banner.className = 'cookie-banner';
        banner.innerHTML = `
            <div class="cookie-content">
                <div class="cookie-icon">🍪</div>
                <div class="cookie-text">
                    <h3>Мы используем куки</h3>
                    <p>Это помогает нам работать — вы остаётесь в аккаунте, а ваши варианты не теряются.</p>
                </div>
                <div class="cookie-buttons">
                    <button onclick="CookieBanner.accept()" class="cookie-btn">Понятно</button>
                    <button onclick="CookieBanner.showInfo()" class="cookie-link">Подробнее</button>
                </div>
            </div>
        `;
        document.body.appendChild(banner);
    },
    
    accept: function() {
        document.getElementById('cookieConsentBanner').style.display = 'none';
        localStorage.setItem('cookieConsent', 'true');
        
        // Если пользователь авторизован - отправляем на сервер
        if (window.USER_ID) {
            fetch(`/api/cookie-consent?user_id=${window.USER_ID}`, {
                method: 'POST'
            }).catch(err => console.log('Cookie consent saved locally only'));
        }
    },
    
    showInfo: function() {
        // Создаем модальное окно если его нет
        if (!document.getElementById('cookieInfoModal')) {
            this.createModal();
        }
        document.getElementById('cookieInfoModal').style.display = 'flex';
    },
    
    createModal: function() {
        const modal = document.createElement('div');
        modal.id = 'cookieInfoModal';
        modal.className = 'cookie-modal';
        modal.innerHTML = `
            <div class="cookie-modal-content">
                <h2>🍪 О куки-файлах</h2>
                <p>Мы используем только необходимые куки для:</p>
                <ul>
                    <li>✅ Входа в аккаунт</li>
                    <li>✅ Сохранения сессии</li>
                    <li>✅ Безопасности</li>
                </ul>
                <p>Мы НЕ используем куки для отслеживания или рекламы.</p>
                <button onclick="CookieBanner.closeModal()" class="cookie-btn">Закрыть</button>
            </div>
        `;
        document.body.appendChild(modal);
    },
    
    closeModal: function() {
        document.getElementById('cookieInfoModal').style.display = 'none';
    },
    
    addModalStyles: function() {
        // Добавляем стили для модального окна если их нет в CSS
        if (!document.querySelector('#cookie-modal-styles')) {
            const style = document.createElement('style');
            style.id = 'cookie-modal-styles';
            style.textContent = `
                .cookie-modal {
                    position: fixed;
                    top: 0;
                    left: 0;
                    right: 0;
                    bottom: 0;
                    background: rgba(0, 0, 0, 0.8);
                    backdrop-filter: blur(5px);
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    z-index: 10001;
                    animation: fadeIn 0.3s ease;
                }
                
                @keyframes fadeIn {
                    from { opacity: 0; }
                    to { opacity: 1; }
                }
                
                .cookie-modal-content {
                    background: rgba(26, 30, 42, 0.95);
                    backdrop-filter: blur(10px);
                    border: 1px solid #8b5cf6;
                    border-radius: 30px;
                    padding: 40px;
                    max-width: 400px;
                    color: white;
                    box-shadow: 0 30px 60px rgba(0, 0, 0, 0.5), 0 0 50px #8b5cf6;
                    animation: scaleIn 0.3s ease;
                    margin: 20px;
                }
                
                @keyframes scaleIn {
                    from { transform: scale(0.9); opacity: 0; }
                    to { transform: scale(1); opacity: 1; }
                }
                
                .cookie-modal-content h2 {
                    color: #8b5cf6;
                    margin-bottom: 20px;
                }
            `;
            document.head.appendChild(style);
        }
    }
};

// Инициализация при загрузке страницы
document.addEventListener('DOMContentLoaded', function() {
    debugCookieStatus();
    CookieBanner.init();
});

// Для глобального доступа
window.CookieBanner = CookieBanner;