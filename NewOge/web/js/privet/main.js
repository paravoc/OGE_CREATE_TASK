// Главный файл, инициализация всего
(async function() {
    // Инициализация UI
    initUI();
    
    // Проверяем авторизацию для определения скорости анимации
    let isAuthenticated = false;
    try {
        const response = await fetch('/api/user/me');
        isAuthenticated = response.ok;
    } catch (error) {
        console.error('Ошибка проверки авторизации:', error);
    }
    
    // Инициализация текстовой анимации с нужной скоростью
    const speed = isAuthenticated ? 'normal' : 'fast';
    const allSpans = initTextAnimation('animatedText', speed);
    const lastSpan = getLastSpan();
    
    // Функция активации всех элементов
    function activateAll() {
        makeAllSpansVisible();
        
        // Показываем меню и контент
        showElements(() => {
            addRainbowAnimation(300);
            
            // Для неавторизованных показываем кнопку быстрого старта
            if (!isAuthenticated) {
                showQuickStart();
            }
        });
    }
    
    // Отслеживаем окончание анимации последней буквы
    if (lastSpan) {
        lastSpan.addEventListener('animationend', function onAnimEnd(e) {
            if (e.animationName === 'slideInRight') {
                activateAll();
                lastSpan.removeEventListener('animationend', onAnimEnd);
            }
        });
    } else {
        setTimeout(activateAll, isAuthenticated ? 1000 : 500);
    }
    
    // Страховочный таймер (быстрее для неавторизованных)
    setTimeout(activateAll, isAuthenticated ? 5000 : 2000);
    
    // Клик по hint для теста
    const hint = document.querySelector('.hint');
    if (hint) {
        hint.addEventListener('click', () => {
            toggleElements();
            
            // Для теста - добавляем/убираем радугу при клике
            const allSpans = document.querySelectorAll('.three-d-title span');
            allSpans.forEach(span => {
                span.classList.toggle('rainbow');
            });
            
            // Тест показа/скрытия кнопки
            if (document.querySelector('.quick-start-btn')?.classList.contains('hidden')) {
                showQuickStart();
            } else {
                hideQuickStart();
            }
        });
    }
    
    // Инициализация искр для кнопки
    addSparkleCSS();
    initSparkles('.sparkle-btn', 10);
    
    // Инициализация кнопки быстрого старта
    initQuickStart();
})();