(function() {
    // Текст в том виде, как вы его прислали (с переносами)
    const textLines = [
        "ДОБРО ПОЖАЛОВАТЬ",
        "НА САЙТ ПО",
        "БЕЗЛИМИТНОЙ ГЕНЕРАЦИИ",
        "ЗАДАЧ И ПОДГОТОВКЕ",
        "К ЭКЗАМЕНАМ"
    ];
    
    // Контейнер, куда будем вставлять буквы
    const container = document.getElementById('animatedText');
    const leftContent = document.querySelector('.left-content');
    const rightContent = document.getElementById('tarifPanel');
    const menu = document.getElementById('mainMenu');
    
    // Очищаем контейнер
    container.innerHTML = '';
    
    let letterCounter = 0;
    
    // Проходим по каждой строке
    textLines.forEach((line, lineIndex) => {
        // Разбиваем строку на символы
        for (let char of line) {
            const span = document.createElement('span');
            if (char === ' ') {
                span.innerHTML = '&nbsp;';
                span.style.minWidth = '0.5em';
            } else {
                span.textContent = char;
            }
            span.setAttribute('data-letter', (char === ' ') ? ' ' : char);
            container.appendChild(span);
            letterCounter++;
        }
        
        // Добавляем перенос строки, если это не последняя строка
        if (lineIndex < textLines.length - 1) {
            // Вариант 1: Добавляем span-пробел и принудительный перенос
            const lineBreak = document.createElement('span');
            lineBreak.innerHTML = '&nbsp;';
            lineBreak.style.display = 'block';
            lineBreak.style.width = '100%';
            lineBreak.style.height = '0';
            lineBreak.style.opacity = '0';
            lineBreak.style.pointerEvents = 'none';
            container.appendChild(lineBreak);
            
            // Вариант 2: Можно добавить <br> но он не будет анимироваться
            // Поэтому используем подход с block span
        }
    });
    
    // Добавляем CSS-правило для переносов
    const style = document.createElement('style');
    style.textContent = `
        .three-d-title {
            display: flex;
            flex-wrap: wrap;
            justify-content: flex-start;
            align-items: baseline;
            gap: 0 0; /* Убираем промежутки */
        }
        
        .three-d-title span {
            display: inline-block;
        }
        
        /* Каждая 6-я группа букв создает перенос (подгоняем под ваш текст) */
        /* Можно точнее определить места переносов через JS */
    `;
    document.head.appendChild(style);
    
    // Более точный способ: вставляем переносы после определённых букв
    // Но проще использовать flex-wrap
    
    // Функция активации меню и тарифа
    function showElements() {
        menu.classList.add('active');
        leftContent.classList.add('shrink');
        rightContent.classList.add('visible');
    }
    
    // Находим последний span для отслеживания окончания анимации
    const allSpans = container.querySelectorAll('span');
    const lastSpan = allSpans[allSpans.length - 1];
    
    // Если последний span существует, слушаем окончание его анимации
    if (lastSpan) {
        lastSpan.addEventListener('animationend', function onAnimEnd(e) {
            if (e.animationName === 'slideInRight') {
                showElements();
                lastSpan.removeEventListener('animationend', onAnimEnd);
            }
        });
    } else {
        setTimeout(showElements, 1000);
    }
    
    // Страховочный таймер
    setTimeout(showElements, 5000);
    
    // Клик по hint для теста
    const hint = document.querySelector('.hint');
    if (hint) {
        hint.addEventListener('click', () => {
            menu.classList.toggle('active');
            leftContent.classList.toggle('shrink');
            rightContent.classList.toggle('visible');
        });
    }
    
    // Искры для кнопки
    const sparkleBtn = document.querySelector('.sparkle-btn');
    if (sparkleBtn) {
        for (let i = 0; i < 5; i++) {
            const spark = document.createElement('div');
            spark.className = 'sparkles';
            spark.style.position = 'absolute';
            spark.style.width = '100%';
            spark.style.height = '100%';
            spark.style.top = '0';
            spark.style.left = '0';
            spark.style.pointerEvents = 'none';
            
            const sparkInner = document.createElement('div');
            sparkInner.style.cssText = `
                position: absolute;
                top: ${Math.random() * 100}%;
                left: ${Math.random() * 100}%;
                width: ${Math.random() * 8 + 3}px;
                height: ${Math.random() * 8 + 3}px;
                background: white;
                border-radius: 50%;
                box-shadow: 0 0 20px #ffb347;
                animation: sparkle ${Math.random() * 2 + 1}s infinite;
                animation-delay: ${Math.random() * 2}s;
                opacity: 0;
            `;
            spark.appendChild(sparkInner);
            sparkleBtn.appendChild(spark);
        }
    }
})();