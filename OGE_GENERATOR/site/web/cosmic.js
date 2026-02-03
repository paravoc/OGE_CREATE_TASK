/* Дополнительные стили для JavaScript функционала */

/* Анимация уведомлений */
@keyframes slideIn {
    from { transform: translateX(100%); opacity: 0; }
    to { transform: translateX(0); opacity: 1; }
}

@keyframes slideOut {
    from { transform: translateX(0); opacity: 1; }
    to { transform: translateX(100%); opacity: 0; }
}

.notification {
    position: fixed;
    top: 20px;
    right: 20px;
    padding: 15px 20px;
    background: #64ffda;
    color: #0a192f;
    border-radius: 8px;
    box-shadow: 0 4px 20px rgba(0, 0, 0, 0.2);
    z-index: 1000;
    display: flex;
    align-items: center;
    gap: 15px;
    animation: slideIn 0.3s ease;
}

.notification.error {
    background: #ff6b6b;
    color: white;
}

.notification.info {
    background: #29b6f6;
    color: white;
}

.notification.warning {
    background: #ffa726;
    color: #000;
}

.close-notification {
    background: none;
    border: none;
    font-size: 24px;
    cursor: pointer;
    padding: 0;
    line-height: 1;
    color: inherit;
    opacity: 0.8;
    transition: opacity 0.2s;
}

.close-notification:hover {
    opacity: 1;
}

/* Активное состояние карточки */
.problem-card.active {
    background: rgba(100, 255, 218, 0.05);
    border-color: #64ffda;
    box-shadow: 0 10px 40px -15px rgba(100, 255, 218, 0.2);
}

.problem-card.active .problem-number {
    background: linear-gradient(135deg, #64ffda, #52dbbf);
    color: #0a192f;
    transform: scale(1.1);
}

/* Интерактивные элементы */
.problem-card .answer:hover {
    background: rgba(100, 255, 218, 0.15);
    transform: translateY(-2px);
    cursor: pointer;
}

.problem-card .answer:active {
    transform: translateY(0);
}

/* Индикатор загрузки */
.loading {
    display: inline-block;
    width: 20px;
    height: 20px;
    border: 3px solid rgba(100, 255, 218, 0.3);
    border-radius: 50%;
    border-top-color: #64ffda;
    animation: spin 1s ease-in-out infinite;
}

@keyframes spin {
    to { transform: rotate(360deg); }
}

/* Кнопка "Наверх" */
.scroll-top-btn {
    position: fixed;
    bottom: 30px;
    right: 30px;
    width: 50px;
    height: 50px;
    background: rgba(100, 255, 218, 0.9);
    border: none;
    border-radius: 50%;
    color: #0a192f;
    font-size: 24px;
    cursor: pointer;
    display: flex;
    align-items: center;
    justify-content: center;
    opacity: 0;
    transform: translateY(20px);
    transition: all 0.3s;
    z-index: 99;
    box-shadow: 0 4px 15px rgba(100, 255, 218, 0.3);
}

.scroll-top-btn.visible {
    opacity: 1;
    transform: translateY(0);
}

.scroll-top-btn:hover {
    background: #64ffda;
    transform: translateY(-5px);
}

/* Эффект печати для текста задач */
.print-text {
    font-family: 'Courier New', monospace;
    background: rgba(17, 34, 64, 0.5);
    padding: 20px;
    border-radius: 8px;
    border-left: 3px solid #64ffda;
    margin: 15px 0;
    position: relative;
    overflow: hidden;
}

.print-text:before {
    content: '';
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    height: 2px;
    background: linear-gradient(90deg, #64ffda, transparent);
}

/* Анимация выделения */
@keyframes highlight {
    0% { background-color: rgba(100, 255, 218, 0); }
    50% { background-color: rgba(100, 255, 218, 0.3); }
    100% { background-color: rgba(100, 255, 218, 0); }
}

.highlight {
    animation: highlight 2s ease;
}

/* Адаптивные стили для мобильных */
@media (max-width: 768px) {
    .notification {
        left: 20px;
        right: 20px;
        top: 10px;
        animation: slideInMobile 0.3s ease;
    }
    
    @keyframes slideInMobile {
        from { transform: translateY(-100%); opacity: 0; }
        to { transform: translateY(0); opacity: 1; }
    }
    
    .scroll-top-btn {
        bottom: 20px;
        right: 20px;
        width: 45px;
        height: 45px;
        font-size: 20px;
    }
}