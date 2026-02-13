// handlers.js - Обработчики для разных типов задач
// ВАЖНО: Функции должны быть в глобальной области видимости!

window.checkAnswer = function(problemId) {
    const input = document.getElementById(problemId);
    if (!input) {
        console.error('Input not found:', problemId);
        return;
    }
    
    // Проверяем, отвечали ли уже
    if (input.dataset.answered === 'true') {
        return;
    }
    
    const userAnswer = input.value.trim();
    const correctAnswer = input.dataset.correctAnswer;
    const score = parseInt(input.dataset.score) || 10;
    
    const isCorrect = AnswerCheckers.math(userAnswer, correctAnswer);
    const feedback = Utils.getOrCreateFeedback(problemId, input.parentNode);
    
    if (isCorrect) {
        feedback.textContent = '? +' + score + ' баллов';
        feedback.className = 'answer-feedback correct';
        input.classList.add('correct');
        input.style.borderColor = '#4CAF50';
        ProgressManager.incrementCorrect();
    } else {
        feedback.textContent = '? Правильно: ' + correctAnswer;
        feedback.className = 'answer-feedback incorrect';
        input.classList.add('incorrect');
        input.style.borderColor = '#f44336';
        ProgressManager.incrementIncorrect();
    }
    
    input.dataset.answered = 'true';
};

window.checkDecimalAnswer = function(problemId) {
    const input = document.getElementById(problemId);
    if (!input) {
        console.error('Input not found:', problemId);
        return;
    }
    
    // Проверяем, отвечали ли уже
    if (input.dataset.answered === 'true') {
        return;
    }
    
    const userAnswer = input.value;
    const correctAnswer = input.dataset.correctAnswer;
    const score = parseInt(input.dataset.score) || 15;
    const decimalPlaces = parseInt(input.dataset.decimalPlaces) || 2;
    
    const isCorrect = AnswerCheckers.decimal(userAnswer, correctAnswer, decimalPlaces);
    const feedback = Utils.getOrCreateFeedback(problemId, input.parentNode);
    
    if (isCorrect) {
        feedback.textContent = '? +' + score + ' баллов';
        feedback.className = 'answer-feedback correct';
        input.classList.add('correct');
        input.style.borderColor = '#4CAF50';
        ProgressManager.incrementCorrect();
    } else {
        feedback.textContent = '? Правильно: ' + correctAnswer;
        feedback.className = 'answer-feedback incorrect';
        input.classList.add('incorrect');
        input.style.borderColor = '#f44336';
        ProgressManager.incrementIncorrect();
    }
    
    input.dataset.answered = 'true';
};