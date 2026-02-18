// Главный файл, инициализация всего
(function() {
    // Инициализация UI
    initUI();
    
    // Инициализация текстовой анимации
    const allSpans = initTextAnimation('animatedText');
    const lastSpan = getLastSpan();
    
    // Функция активации всех элементов
    function activateAll() {
        // Делаем буквы видимыми (на всякий случай)
        makeAllSpansVisible();
        
        // Показываем меню и тариф
        showElements(() => {
            // Добавляем радужную анимацию
            addRainbowAnimation(300);
            
            // Показываем кнопку быстрого старта
            showQuickStart();
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
        setTimeout(activateAll, 1000);
    }
    
    // Страховочный таймер
    setTimeout(activateAll, 5000);
    
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
            if (document.querySelector('.quick-start-btn').classList.contains('hidden')) {
                showQuickStart();
            } else {
                hideQuickStart();
            }
        });
    }
    
    // Инициализация искр для кнопки
    addSparkleCSS(); // Если нужно добавить CSS для искр
    initSparkles('.sparkle-btn', 10);
    
    // Инициализация кнопки быстрого старта
    initQuickStart();
})();

