// Прогресс бар
let stats = {
    total: 0,      // количество отвеченных задач
    correct: 0,    // количество правильных ответов
    totalProblems: 0 // общее количество задач (устанавливается из HTML)
};

window.checkAnswer = function(problemId) {
    const input = document.getElementById(problemId);
    const userAnswer = input.value.trim();
    const correctAnswer = input.dataset.correctAnswer;
    const score = parseInt(input.dataset.score) || 10;
    
    // Проверяем, отвечали ли уже на эту задачу
    if (input.dataset.answered === 'true') {
        return; // Если уже отвечали, ничего не делаем
    }
    
    let feedback = document.getElementById('feedback-' + problemId);
    if (!feedback) {
        feedback = document.createElement('div');
        feedback.id = 'feedback-' + problemId;
        feedback.className = 'answer-feedback';
        input.parentNode.appendChild(feedback);
    }
    
    const isCorrect = (userAnswer === correctAnswer);
    
    if (isCorrect) {
        feedback.textContent = '✅ +' + score + ' баллов';
        feedback.className = 'answer-feedback correct';
        input.style.borderColor = '#4CAF50';
        stats.correct++;
    } else {
        feedback.textContent = '❌ Правильно: ' + correctAnswer;
        feedback.className = 'answer-feedback incorrect';
        input.style.borderColor = '#f44336';
    }
    
    // Отмечаем, что на задачу ответили
    input.dataset.answered = 'true';
    stats.total++;
    
    updateProgress();
};

window.checkDecimalAnswer = function(problemId) {
    const input = document.getElementById(problemId);
    
    // Проверяем, отвечали ли уже на эту задачу
    if (input.dataset.answered === 'true') {
        return; // Если уже отвечали, ничего не делаем
    }
    
    let userAnswer = input.value.trim().replace(',', '.');
    const correctAnswer = input.dataset.correctAnswer;
    const score = parseInt(input.dataset.score) || 15;
    const decimalPlaces = parseInt(input.dataset.decimalPlaces) || 2;
    
    let feedback = document.getElementById('feedback-' + problemId);
    if (!feedback) {
        feedback = document.createElement('div');
        feedback.id = 'feedback-' + problemId;
        feedback.className = 'answer-feedback';
        input.parentNode.appendChild(feedback);
    }
    
    try {
        const userVal = parseFloat(userAnswer);
        const correctVal = parseFloat(correctAnswer);
        const diff = Math.abs(userVal - correctVal);
        const tolerance = Math.pow(0.1, decimalPlaces) / 2;
        const isCorrect = diff <= tolerance;
        
        if (isCorrect) {
            feedback.textContent = '✅ +' + score + ' баллов';
            feedback.className = 'answer-feedback correct';
            input.style.borderColor = '#4CAF50';
            stats.correct++;
        } else {
            feedback.textContent = '❌ Правильно: ' + correctAnswer;
            feedback.className = 'answer-feedback incorrect';
            input.style.borderColor = '#f44336';
        }
        
        // Отмечаем, что на задачу ответили
        input.dataset.answered = 'true';
        stats.total++;
        
        updateProgress();
    } catch (e) {
        feedback.textContent = '❌ Ошибка';
        feedback.className = 'answer-feedback incorrect';
    }
};

// Функция обновления прогресс-бара
function updateProgress() {
    const progressBar = document.getElementById('main-progress');
    const correctSpan = document.getElementById('correct-count');
    const totalSpan = document.getElementById('total-count');
    
    if (!stats.totalProblems) {
        // Получаем общее количество задач из HTML
        const totalText = totalSpan ? totalSpan.textContent : '0';
        stats.totalProblems = parseInt(totalText) || 0;
    }
    
    if (correctSpan) correctSpan.textContent = stats.correct;
    if (totalSpan) totalSpan.textContent = stats.totalProblems;
    
    if (progressBar && stats.totalProblems > 0) {
        const percent = (stats.correct / stats.totalProblems) * 100;
        progressBar.value = percent;
    }
}

// Enter для отправки
document.addEventListener('DOMContentLoaded', function() {
    // Получаем общее количество задач
    const totalSpan = document.getElementById('total-count');
    if (totalSpan) {
        stats.totalProblems = parseInt(totalSpan.textContent) || 0;
    }
    
    // Добавляем data-атрибут answered всем инпутам
    document.querySelectorAll('input[type="text"]').forEach(input => {
        input.dataset.answered = 'false';
        
        input.addEventListener('keypress', function(e) {
            if (e.key === 'Enter') {
                const problemId = this.id;
                const button = this.nextElementSibling;
                if (button && button.onclick) button.onclick();
            }
        });
    });
    
    updateProgress();
});