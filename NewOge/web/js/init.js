// init.js - Инициализация приложения
document.addEventListener('DOMContentLoaded', function() {
    console.log('OGE System initializing...');
    
    // Инициализация прогресса
    ProgressManager.init();
    
    // Инициализация менеджера решений
    // По умолчанию true, но может быть переопределено из C++
    SolutionManager.init(window.CAN_VIEW_SOLUTIONS !== false);
    
    // Настройка инпутов
    document.querySelectorAll('input[type="text"]').forEach(input => {
        // Устанавливаем флаг answered, если его нет
        if (!input.dataset.answered) {
            input.dataset.answered = 'false';
        }
        
        // Обработчик Enter
        input.addEventListener('keypress', function(e) {
            if (e.key === 'Enter') {
                const problemId = this.id;
                // Определяем тип задачи и вызываем нужную функцию
                if (this.dataset.correctAnswer) {
                    // Проверяем, есть ли точка в ответе - тогда десятичная дробь
                    if (this.dataset.correctAnswer.includes('.')) {
                        window.checkDecimalAnswer(problemId);
                    } else {
                        window.checkAnswer(problemId);
                    }
                }
            }
        });
    });
    
    console.log('OGE System initialized');
});