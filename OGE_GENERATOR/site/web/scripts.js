// ============================================
// ГЕНЕРАТОР ЗАДАЧ - JavaScript
// Версия с уникальным префиксом для каждой страницы
// ============================================

document.addEventListener('DOMContentLoaded', function() {
    
    // ============================================
    // 1. НАСТРОЙКИ И ИНИЦИАЛИЗАЦИЯ
    // ============================================
    
    // Генерируем уникальный префикс для этой страницы
    const generatePageId = () => {
        // Используем заголовок страницы и время из подвала
        const title = document.title || 'Задачи на кодирование';
        const timestamp = document.querySelector('.timestamp')?.textContent || '';
        
        // Создаем простой хэш
        let hash = 0;
        const str = title + timestamp;
        
        for (let i = 0; i < str.length; i++) {
            hash = str.charCodeAt(i) + ((hash << 5) - hash);
        }
        
        // Преобразуем в строку и берем первые 8 символов
        return Math.abs(hash).toString(16).substring(0, 8);
    };
    
    const PAGE_ID = generatePageId();
    const STORAGE_PREFIX = `problems_${PAGE_ID}_`;
    
    console.log(`Инициализация страницы с ID: ${PAGE_ID}`);
    
    // Настройки
    const CONFIG = {
        enableLocalStorage: true,  // Включить/выключить сохранение ответов
        caseSensitive: false,      // Чувствительность к регистру
        showAnimations: true       // Показывать анимации
    };
    
    // ============================================
    // 2. ИНИЦИАЛИЗАЦИЯ СТРАНИЦЫ
    // ============================================
    
    function initializePage() {
        // Инициализируем все задачи
        initializeProblems();
        
        // Добавляем кнопку очистки
        addClearButton();
        
        // Обновляем статистику
        updateProgress();
        
        console.log(`Страница инициализирована. ID: ${PAGE_ID}, Префикс: ${STORAGE_PREFIX}`);
    }
    
    // Инициализация всех задач
    function initializeProblems() {
        const problems = document.querySelectorAll('.problem-container');
        const totalProblems = problems.length;
        
        console.log(`Найдено задач: ${totalProblems}`);
        
        // Восстанавливаем сохраненные ответы для каждой задачи
        problems.forEach((problem, index) => {
            restoreAnswer(problem, index);
        });
    }
    
    // ============================================
    // 3. ОБРАБОТКА СОБЫТИЙ
    // ============================================
    
    // Обработка кликов
    document.addEventListener('click', function(e) {
        const target = e.target;
        
        if (target.classList.contains('check-btn')) {
            const container = target.closest('.problem-container');
            checkAnswer(container);
        }
        
        else if (target.classList.contains('reset-btn')) {
            const container = target.closest('.problem-container');
            resetAnswer(container);
        }
        
        else if (target.classList.contains('show-answer-btn')) {
            const container = target.closest('.problem-container');
            toggleCorrectAnswer(container);
        }
        
        else if (target.classList.contains('clear-page-btn')) {
            clearAllAnswers();
        }
    });
    
    // Обработка нажатия Enter в поле ввода
    document.addEventListener('keydown', function(e) {
        if (e.target.classList.contains('answer-input') && e.key === 'Enter') {
            const container = e.target.closest('.problem-container');
            checkAnswer(container);
            
            // Предотвращаем стандартное поведение
            e.preventDefault();
        }
    });
    
    // ============================================
    // 4. ОСНОВНЫЕ ФУНКЦИИ
    // ============================================
    
    // Проверка ответа
    function checkAnswer(container) {
        if (!container) return;
        
        const input = container.querySelector('.answer-input');
        const correctAnswerElement = container.querySelector('.correct-answer');
        const resultMessage = container.querySelector('.result-message');
        const problemIndex = parseInt(container.dataset.problemIndex);
        
        if (!input || !correctAnswerElement || !resultMessage) return;
        
        // Получаем ответы
        const userAnswer = input.value.trim();
        let correctAnswer = correctAnswerElement.textContent;
        
        // Очищаем правильный ответ от лишнего текста
        correctAnswer = correctAnswer.replace(/Правильный ответ:\s*/gi, '').trim();
        
        // Проверяем ответ
        let isCorrect;
        if (CONFIG.caseSensitive) {
            isCorrect = userAnswer === correctAnswer;
        } else {
            isCorrect = userAnswer.toLowerCase() === correctAnswer.toLowerCase();
        }
        
        // Показываем результат
        if (isCorrect) {
            showResult(container, resultMessage, 'correct', '✅ Верно!');
        } else {
            showResult(container, resultMessage, 'wrong', '❌ Неверно. Попробуйте еще раз!');
        }
        
        // Сохраняем ответ
        if (CONFIG.enableLocalStorage) {
            saveAnswer(problemIndex, userAnswer, isCorrect);
        }
        
        // Обновляем статистику
        updateProgress();
    }
    
    // Показать результат
    function showResult(container, resultElement, type, message) {
        // Устанавливаем сообщение
        resultElement.textContent = message;
        
        // Устанавливаем классы
        resultElement.className = 'result-message';
        resultElement.classList.add(type === 'correct' ? 'result-correct' : 'result-wrong');
        
        // Устанавливаем классы контейнера
        container.classList.remove('highlight-correct', 'highlight-wrong');
        container.classList.add(type === 'correct' ? 'highlight-correct' : 'highlight-wrong');
        
        // Анимация
        if (CONFIG.showAnimations) {
            resultElement.style.animation = 'none';
            setTimeout(() => {
                resultElement.style.animation = 'fadeIn 0.5s ease';
            }, 10);
        }
    }
    
    // Сброс ответа
    function resetAnswer(container) {
        if (!container) return;
        
        const input = container.querySelector('.answer-input');
        const resultMessage = container.querySelector('.result-message');
        const correctAnswerDiv = container.querySelector('.correct-answer');
        const button = container.querySelector('.show-answer-btn');
        const problemIndex = parseInt(container.dataset.problemIndex);
        
        // Сбрасываем поле ввода
        if (input) input.value = '';
        
        // Сбрасываем сообщение
        if (resultMessage) {
            resultMessage.textContent = '';
            resultMessage.className = 'result-message';
        }
        
        // Сбрасываем подсветку
        container.classList.remove('highlight-correct', 'highlight-wrong');
        
        // Скрываем правильный ответ
        if (correctAnswerDiv) {
            correctAnswerDiv.classList.remove('show');
        }
        
        // Обновляем текст кнопки
        if (button) {
            button.textContent = 'Показать ответ';
        }
        
        // Удаляем из localStorage
        if (CONFIG.enableLocalStorage) {
            localStorage.removeItem(`${STORAGE_PREFIX}problem_${problemIndex}`);
        }
        
        // Обновляем статистику
        updateProgress();
    }
    
    // Показать/скрыть правильный ответ
    function toggleCorrectAnswer(container) {
        if (!container) return;
        
        const correctAnswerDiv = container.querySelector('.correct-answer');
        const button = container.querySelector('.show-answer-btn');
        
        if (!correctAnswerDiv || !button) return;
        
        // Переключаем видимость
        const isShowing = correctAnswerDiv.classList.toggle('show');
        
        // Обновляем текст кнопки
        button.textContent = isShowing ? 'Скрыть ответ' : 'Показать ответ';
        
        // Анимация
        if (CONFIG.showAnimations && isShowing) {
            correctAnswerDiv.style.animation = 'none';
            setTimeout(() => {
                correctAnswerDiv.style.animation = 'slideDown 0.3s ease';
            }, 10);
        }
    }
    
    // ============================================
    // 5. РАБОТА С LOCALSTORAGE
    // ============================================
    
    // Сохранение ответа
    function saveAnswer(problemIndex, answer, isCorrect) {
        if (!CONFIG.enableLocalStorage) return;
        
        const answerData = {
            answer: answer,
            isCorrect: isCorrect,
            timestamp: new Date().toISOString(),
            pageId: PAGE_ID
        };
        
        try {
            localStorage.setItem(`${STORAGE_PREFIX}problem_${problemIndex}`, JSON.stringify(answerData));
        } catch (e) {
            console.error('Ошибка сохранения в localStorage:', e);
        }
    }
    
    // Восстановление ответа
    function restoreAnswer(container, problemIndex) {
        if (!CONFIG.enableLocalStorage) return;
        
        const savedData = localStorage.getItem(`${STORAGE_PREFIX}problem_${problemIndex}`);
        
        if (!savedData) return;
        
        try {
            const data = JSON.parse(savedData);
            
            // Проверяем, что данные относятся к этой странице
            if (data.pageId !== PAGE_ID) {
                console.log(`Данные для задачи ${problemIndex} не относятся к этой странице`);
                return;
            }
            
            const input = container.querySelector('.answer-input');
            const resultMessage = container.querySelector('.result-message');
            
            if (input) input.value = data.answer;
            
            if (resultMessage && data.answer) {
                if (data.isCorrect) {
                    resultMessage.textContent = '✅ Верно! (сохранено)';
                    resultMessage.className = 'result-message result-correct';
                    container.classList.add('highlight-correct');
                } else {
                    resultMessage.textContent = '❌ Неверно (сохранено)';
                    resultMessage.className = 'result-message result-wrong';
                    container.classList.add('highlight-wrong');
                }
            }
            
        } catch (e) {
            console.error('Ошибка восстановления ответа:', e);
        }
    }
    
    // Получение количества решенных задач
    function getSolvedProblemsCount() {
        if (!CONFIG.enableLocalStorage) return 0;
        
        let solved = 0;
        const totalProblems = document.querySelectorAll('.problem-container').length;
        
        for (let i = 0; i < totalProblems; i++) {
            const savedData = localStorage.getItem(`${STORAGE_PREFIX}problem_${i}`);
            if (savedData) {
                try {
                    const data = JSON.parse(savedData);
                    if (data.pageId === PAGE_ID && data.isCorrect) {
                        solved++;
                    }
                } catch (e) {
                    // Игнорируем ошибки парсинга
                }
            }
        }
        
        return solved;
    }
    
    // Очистка всех ответов для этой страницы
    function clearAllAnswers() {
        if (!CONFIG.enableLocalStorage) {
            alert('Сохранение ответов отключено в настройках');
            return;
        }
        
        if (!confirm('Вы уверены, что хотите очистить все ответы для этой страницы?\nЭто действие нельзя отменить.')) {
            return;
        }
        
        const totalProblems = document.querySelectorAll('.problem-container').length;
        let cleared = 0;
        
        // Удаляем из localStorage
        for (let i = 0; i < totalProblems; i++) {
            const key = `${STORAGE_PREFIX}problem_${i}`;
            if (localStorage.getItem(key)) {
                localStorage.removeItem(key);
                cleared++;
            }
        }
        
        // Сбрасываем UI
        document.querySelectorAll('.problem-container').forEach(container => {
            const input = container.querySelector('.answer-input');
            const resultMessage = container.querySelector('.result-message');
            const correctAnswerDiv = container.querySelector('.correct-answer');
            const button = container.querySelector('.show-answer-btn');
            
            if (input) input.value = '';
            if (resultMessage) {
                resultMessage.textContent = '';
                resultMessage.className = 'result-message';
            }
            if (correctAnswerDiv) correctAnswerDiv.classList.remove('show');
            if (button) button.textContent = 'Показать ответ';
            
            container.classList.remove('highlight-correct', 'highlight-wrong');
        });
        
        // Обновляем статистику
        updateProgress();
        
        // Показываем сообщение
        alert(`Очищено ответов: ${cleared}\nВсе ответы для этой страницы удалены.`);
    }
    
    // ============================================
    // 6. СТАТИСТИКА И ПРОГРЕСС
    // ============================================
    
    // Обновление статистики
    function updateStats(solved, total) {
        const statsElement = document.querySelector('.stats');
        const progressBar = document.querySelector('.progress-bar');
        
        if (statsElement) {
            statsElement.innerHTML = `Решено: <span>${solved}</span> из <span>${total}</span> задач`;
        }
        
        if (progressBar) {
            const percentage = total > 0 ? (solved / total) * 100 : 0;
            progressBar.style.width = `${percentage}%`;
            
            // Меняем цвет в зависимости от прогресса
            if (percentage === 100) {
                progressBar.style.backgroundColor = '#2ecc71';
            } else if (percentage >= 50) {
                progressBar.style.backgroundColor = '#f39c12';
            } else {
                progressBar.style.backgroundColor = '#e74c3c';
            }
        }
    }
    
    // Обновление прогресса
    function updateProgress() {
        const solved = getSolvedProblemsCount();
        const total = document.querySelectorAll('.problem-container').length;
        updateStats(solved, total);
    }
    
    // ============================================
    // 7. ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
    // ============================================
    
    // Добавление кнопки очистки
    function addClearButton() {
        // Проверяем, не добавлена ли уже кнопка
        if (document.querySelector('.clear-page-btn')) return;
        
        const clearBtn = document.createElement('button');
        clearBtn.className = 'clear-page-btn';
        clearBtn.textContent = '🗑️ Очистить все ответы (эта страница)';
        clearBtn.title = 'Удалить все сохраненные ответы только для этой страницы';
        
        // Стили
        clearBtn.style.cssText = `
            display: block;
            margin: 20px auto;
            padding: 10px 20px;
            background: linear-gradient(135deg, #ff6b6b, #ee5a52);
            color: white;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            font-size: 14px;
            font-weight: bold;
            transition: all 0.3s ease;
            box-shadow: 0 3px 6px rgba(0,0,0,0.1);
        `;
        
        // Эффекты при наведении
        clearBtn.addEventListener('mouseenter', function() {
            this.style.transform = 'translateY(-2px)';
            this.style.boxShadow = '0 5px 15px rgba(0,0,0,0.2)';
        });
        
        clearBtn.addEventListener('mouseleave', function() {
            this.style.transform = 'translateY(0)';
            this.style.boxShadow = '0 3px 6px rgba(0,0,0,0.1)';
        });
        
        // Добавляем кнопку перед скриптом
        const scriptTag = document.querySelector('script');
        if (scriptTag && scriptTag.parentNode) {
            scriptTag.parentNode.insertBefore(clearBtn, scriptTag);
        } else {
            document.body.appendChild(clearBtn);
        }
    }
    
    // Отладка: показываем информацию о localStorage
    function debugLocalStorage() {
        if (!CONFIG.enableLocalStorage) return;
        
        console.log('=== DEBUG: LocalStorage для этой страницы ===');
        console.log(`Page ID: ${PAGE_ID}`);
        console.log(`Storage Prefix: ${STORAGE_PREFIX}`);
        
        let totalKeys = 0;
        let ourKeys = 0;
        
        for (let i = 0; i < localStorage.length; i++) {
            const key = localStorage.key(i);
            if (key.startsWith('problems_')) {
                totalKeys++;
                if (key.startsWith(STORAGE_PREFIX)) {
                    ourKeys++;
                    try {
                        const data = JSON.parse(localStorage.getItem(key));
                        console.log(`  ${key}: ${data.answer} (${data.isCorrect ? 'correct' : 'wrong'})`);
                    } catch (e) {
                        console.log(`  ${key}: ERROR parsing`);
                    }
                }
            }
        }
        
        console.log(`Всего ключей problems_: ${totalKeys}`);
        console.log(`Наших ключей: ${ourKeys}`);
        console.log('==========================================');
    }
    
    // ============================================
    // 8. ЗАПУСК
    // ============================================
    
    // Запускаем инициализацию
    initializePage();
    
    // Для отладки (можно удалить в продакшене)
    if (window.location.hash === '#debug') {
        debugLocalStorage();
    }
    
    // Экспортируем функции для глобального доступа (опционально)
    window.ProblemChecker = {
        checkAnswer: (container) => checkAnswer(container),
        resetAnswer: (container) => resetAnswer(container),
        clearAllAnswers: () => clearAllAnswers(),
        getSolvedCount: () => getSolvedProblemsCount(),
        getPageId: () => PAGE_ID
    };
    
    console.log('Генератор задач успешно инициализирован!');
    
});

// ============================================
// 9. ГЛОБАЛЬНЫЕ ФУНКЦИИ (для консоли)
// ============================================

// Очистить ВСЕ данные всех страниц (осторожно!)
function clearAllProblemData() {
    if (!confirm('ВНИМАНИЕ: Это удалит ВСЕ сохраненные ответы для ВСЕХ страниц!\nВы уверены?')) {
        return;
    }
    
    let cleared = 0;
    for (let i = 0; i < localStorage.length; i++) {
        const key = localStorage.key(i);
        if (key.startsWith('problems_')) {
            localStorage.removeItem(key);
            cleared++;
            i--; // Так как массив изменился
        }
    }
    
    alert(`Удалено ключей: ${cleared}\nВсе данные о задачах очищены.`);
    location.reload(); // Перезагружаем страницу
}

// Показать информацию о сохраненных данных
function showStorageInfo() {
    console.log('=== ИНФОРМАЦИЯ О LOCALSTORAGE ===');
    
    const problemKeys = [];
    const otherKeys = [];
    
    for (let i = 0; i < localStorage.length; i++) {
        const key = localStorage.key(i);
        if (key.startsWith('problems_')) {
            problemKeys.push(key);
        } else {
            otherKeys.push(key);
        }
    }
    
    console.log(`Ключей с задачами: ${problemKeys.length}`);
    console.log(`Других ключей: ${otherKeys.length}`);
    console.log('Ключи задач:', problemKeys);
    console.log('===============================');
}