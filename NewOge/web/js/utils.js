// utils.js - Вспомогательные функции
const Utils = {
    // Создание или получение элемента обратной связи
    getOrCreateFeedback: function(problemId, parentNode) {
        let feedback = document.getElementById('feedback-' + problemId);
        if (!feedback) {
            feedback = document.createElement('div');
            feedback.id = 'feedback-' + problemId;
            feedback.className = 'answer-feedback';
            parentNode.appendChild(feedback);
        }
        return feedback;
    },
    
    // Обновление визуального состояния инпута
    setInputState: function(input, isCorrect) {
        input.classList.remove('correct', 'incorrect');
        input.classList.add(isCorrect ? 'correct' : 'incorrect');
        input.style.borderColor = isCorrect ? '#4CAF50' : '#f44336';
    },
    
    // Форматирование ответа для десятичных дробей
    normalizeDecimal: function(value) {
        return value.trim().replace(',', '.');
    }
};

window.Utils = Utils;