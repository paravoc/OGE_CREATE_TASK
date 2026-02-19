// Текст с переносами
const textLines = [
    "ДОБРО ПОЖАЛОВАТЬ",
    "НА САЙТ ПО",
    "БЕЗЛИМИТНОЙ ГЕНЕРАЦИИ",
    "ЗАДАЧ И ПОДГОТОВКЕ",
    "К ЭКЗАМЕНАМ"
];

// Контейнер для букв
let container;
let allSpans = [];

// Инициализация текстовой анимации
function initTextAnimation(containerId, speed = 'normal') {
    container = document.getElementById(containerId);
    if (!container) return [];
    
    container.innerHTML = '';
    
    // Определяем скорость анимации
    const duration = speed === 'fast' ? '0.3s' : '0.5s';
    const delay = speed === 'fast' ? '0.02s' : '0.03s';
    
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
            span.style.animationDuration = duration;
            container.appendChild(span);
        }
        
        // Добавляем перенос строки
        if (lineIndex < textLines.length - 1) {
            const lineBreak = document.createElement('span');
            lineBreak.innerHTML = '&nbsp;';
            lineBreak.style.display = 'block';
            lineBreak.style.width = '100%';
            lineBreak.style.height = '0';
            lineBreak.style.opacity = '0';
            lineBreak.style.pointerEvents = 'none';
            container.appendChild(lineBreak);
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
            gap: 0 0;
        }
        
        .three-d-title span {
            display: inline-block;
            opacity: 0;
            animation: slideInRight var(--duration, 0.5s) ease forwards;
            animation-delay: calc(var(--index, 0) * var(--delay, 0.03s));
        }
    `;
    document.head.appendChild(style);
    
    allSpans = document.querySelectorAll('#animatedText span');
    
    // Устанавливаем индивидуальные задержки
    allSpans.forEach((span, index) => {
        span.style.setProperty('--index', index);
    });
    
    if (speed === 'fast') {
        container.style.setProperty('--duration', '0.3s');
        container.style.setProperty('--delay', '0.02s');
    }
    
    return allSpans;
}

// Получить последний span
function getLastSpan() {
    return allSpans[allSpans.length - 1];
}

// Добавить радужную анимацию буквам
function addRainbowAnimation(delay = 300) {
    setTimeout(() => {
        allSpans.forEach((span, index) => {
            setTimeout(() => {
                span.classList.add('rainbow');
            }, index * 15);
        });
    }, delay);
}

// Сделать все буквы видимыми
function makeAllSpansVisible() {
    allSpans.forEach(span => {
        span.style.opacity = '1';
    });
}