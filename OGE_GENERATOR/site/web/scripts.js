document.addEventListener('DOMContentLoaded', function() {
    // Инициализация
    initializeProblems();
    
    // Слушатели событий
    document.addEventListener('click', function(e) {
        // Проверка ответа
        if (e.target.classList.contains('check-btn')) {
            const container = e.target.closest('.problem-container');
            checkAnswer(container);
        }
        
        // Сброс ответа
        if (e.target.classList.contains('reset-btn')) {
            const container = e.target.closest('.problem-container');
            resetAnswer(container);
        }
        
        // Показать/скрыть правильный ответ
        if (e.target.classList.contains('show-answer-btn')) {
            const container = e.target.closest('.problem-container');
            toggleCorrectAnswer(container);
        }
    });
    
    // Обработка нажатия Enter в поле ввода
    document.addEventListener('keydown', function(e) {
        if (e.target.classList.contains('answer-input') && e.key === 'Enter') {
            const container = e.target.closest('.problem-container');
            checkAnswer(container);
        }
    });
});

// Инициализация всех задач
function initializeProblems() {
    const problems = document.querySelectorAll('.problem-container');
    const totalProblems = problems.length;
    const solvedProblems = getSolvedProblemsCount();
    
    // Обновляем статистику
    updateStats(solvedProblems, totalProblems);
    
    // Восстанавливаем сохраненные ответы
    problems.forEach((problem, index) => {
        restoreAnswer(problem, index);
    });
}

// Проверка ответа
function checkAnswer(container) {
    const input = container.querySelector('.answer-input');
    const correctAnswer = container.querySelector('.correct-answer').textContent;
    const resultMessage = container.querySelector('.result-message');
    const problemIndex = parseInt(container.dataset.problemIndex);
    
    const userAnswer = input.value.trim().toLowerCase();
    const correctAnswerClean = correctAnswer.trim().toLowerCase();
    
    // Убираем "Правильный ответ:" из текста
    const cleanCorrect = correctAnswerClean.replace('правильный ответ:', '').trim();
    
    // Проверяем ответ
    const isCorrect = userAnswer === cleanCorrect;
    
    // Отображаем результат
    if (isCorrect) {
        resultMessage.textContent = '? Верно!';
        resultMessage.className = 'result-message result-correct';
        container.classList.add('highlight-correct');
        container.classList.remove('highlight-wrong');
        
        // Сохраняем успешный ответ
        saveAnswer(problemIndex, userAnswer, true);
    } else {
        resultMessage.textContent = '? Неверно. Попробуйте еще раз!';
        resultMessage.className = 'result-message result-wrong';
        container.classList.add('highlight-wrong');
        container.classList.remove('highlight-correct');
        
        // Сохраняем попытку
        saveAnswer(problemIndex, userAnswer, false);
    }
    
    // Обновляем статистику
    updateProgress();
}

// Сброс ответа
function resetAnswer(container) {
    const input = container.querySelector('.answer-input');
    const resultMessage = container.querySelector('.result-message');
    const problemIndex = parseInt(container.dataset.problemIndex);
    
    input.value = '';
    resultMessage.className = 'result-message';
    container.classList.remove('highlight-correct', 'highlight-wrong');
    
    // Убираем правильный ответ если показан
    const correctAnswerDiv = container.querySelector('.correct-answer');
    correctAnswerDiv.classList.remove('show');
    
    // Обновляем текст кнопки
    const button = container.querySelector('.show-answer-btn');
    button.textContent = 'Показать ответ';
    
    // Удаляем из localStorage
    localStorage.removeItem(`problem_${problemIndex}`);
    
    // Обновляем статистику
    updateProgress();
}

// Показать/скрыть правильный ответ
function toggleCorrectAnswer(container) {
    const correctAnswerDiv = container.querySelector('.correct-answer');
    const button = container.querySelector('.show-answer-btn');
    
    correctAnswerDiv.classList.toggle('show');
    
    if (correctAnswerDiv.classList.contains('show')) {
        button.textContent = 'Скрыть ответ';
    } else {
        button.textContent = 'Показать ответ';
    }
}

// Сохранение ответа в localStorage
function saveAnswer(problemIndex, answer, isCorrect) {
    const answerData = {
        answer: answer,
        isCorrect: isCorrect,
        timestamp: new Date().toISOString()
    };
    
    localStorage.setItem(`problem_${problemIndex}`, JSON.stringify(answerData));
}

// Восстановление ответа из localStorage
function restoreAnswer(container, problemIndex) {
    const savedData = localStorage.getItem(`problem_${problemIndex}`);
    
    if (savedData) {
        try {
            const data = JSON.parse(savedData);
            const input = container.querySelector('.answer-input');
            const resultMessage = container.querySelector('.result-message');
            
            input.value = data.answer;
            
            if (data.isCorrect) {
                resultMessage.textContent = '? Верно! (сохранено)';
                resultMessage.className = 'result-message result-correct';
                container.classList.add('highlight-correct');
            } else {
                resultMessage.textContent = '? Неверно (сохранено)';
                resultMessage.className = 'result-message result-wrong';
                container.classList.add('highlight-wrong');
            }
        } catch (e) {
            console.error('Ошибка при восстановлении ответа:', e);
        }
    }
}

// Получение количества решенных задач
function getSolvedProblemsCount() {
    let solved = 0;
    const totalProblems = document.querySelectorAll('.problem-container').length;
    
    for (let i = 0; i < totalProblems; i++) {
        const savedData = localStorage.getItem(`problem_${i}`);
        if (savedData) {
            try {
                const data = JSON.parse(savedData);
                if (data.isCorrect) {
                    solved++;
                }
            } catch (e) {
                // Пропускаем ошибки
            }
        }
    }
    
    return solved;
}

// Обновление статистики
function updateStats(solved, total) {
    const statsElement = document.querySelector('.stats');
    const progressBar = document.querySelector('.progress-bar');
    
    if (statsElement) {
        statsElement.innerHTML = 'Решено: <span>' + solved + '</span> из <span>' + total + '</span> задач';
    }
    
    if (progressBar) {
        const percentage = total > 0 ? (solved / total) * 100 : 0;
        progressBar.style.width = percentage + '%';
    }
}

// Обновление прогресса
function updateProgress() {
    const solved = getSolvedProblemsCount();
    const total = document.querySelectorAll('.problem-container').length;
    updateStats(solved, total);
}