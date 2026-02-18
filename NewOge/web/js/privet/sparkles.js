// Создание искр для кнопки
function initSparkles(buttonSelector, count = 10) {
    const sparkleBtn = document.querySelector(buttonSelector);
    if (!sparkleBtn) return;
    
    sparkleBtn.style.position = 'relative';
    
    for (let i = 0; i < count; i++) {
        const spark = document.createElement('div');
        spark.style.position = 'absolute';
        spark.style.top = `${Math.random() * 100}%`;
        spark.style.left = `${Math.random() * 100}%`;
        spark.style.width = `${Math.random() * 8 + 2}px`;
        spark.style.height = `${Math.random() * 8 + 2}px`;
        spark.style.background = 'white';
        spark.style.borderRadius = '50%';
        spark.style.boxShadow = '0 0 20px #ffb347, 0 0 30px #ff8c00';
        spark.style.animation = `sparkle ${Math.random() * 2 + 1.5}s infinite`;
        spark.style.animationDelay = `${Math.random() * 2}s`;
        spark.style.opacity = '0';
        spark.style.pointerEvents = 'none';
        sparkleBtn.appendChild(spark);
    }
}

// Добавить CSS для искр (если ещё нет в CSS файле)
function addSparkleCSS() {
    const style = document.createElement('style');
    style.textContent = `
        @keyframes sparkle {
            0% {
                transform: scale(0);
                opacity: 0;
            }
            20% {
                transform: scale(1.5);
                opacity: 1;
            }
            40% {
                transform: scale(0.8);
                opacity: 0.8;
            }
            60% {
                transform: scale(1.2) translate(10px, -20px);
                opacity: 0.5;
            }
            100% {
                transform: scale(0) translate(30px, -40px);
                opacity: 0;
            }
        }
    `;
    document.head.appendChild(style);
}