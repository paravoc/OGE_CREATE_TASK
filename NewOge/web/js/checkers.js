// checkers.js - Функции проверки ответов
const AnswerCheckers = {
    math: function(userAnswer, correctAnswer) {
        return userAnswer.trim() === correctAnswer.trim();
    },
    
    decimal: function(userAnswer, correctAnswer, decimalPlaces) {
        try {
            const userVal = parseFloat(Utils.normalizeDecimal(userAnswer));
            const correctVal = parseFloat(correctAnswer);
            if (isNaN(userVal)) return false;
            const diff = Math.abs(userVal - correctVal);
            const tolerance = AppConfig.decimalTolerance(decimalPlaces);
            return diff <= tolerance;
        } catch (e) {
            return false;
        }
    }
};

window.AnswerCheckers = AnswerCheckers;