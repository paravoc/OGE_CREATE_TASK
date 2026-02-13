// progress.js - Управление прогресс-баром
const ProgressManager = {
    init: function() {
        const totalSpan = document.getElementById('total-count');
        if (totalSpan) {
            AppState.stats.totalProblems = parseInt(totalSpan.textContent) || 0;
        }
        this.update();
    },
    
    update: function() {
        const progressBar = document.getElementById('main-progress');
        const correctSpan = document.getElementById('correct-count');
        const totalSpan = document.getElementById('total-count');
        
        if (correctSpan) correctSpan.textContent = AppState.stats.correct;
        if (totalSpan) totalSpan.textContent = AppState.stats.totalProblems;
        
        if (progressBar && AppState.stats.totalProblems > 0) {
            const percent = (AppState.stats.correct / AppState.stats.totalProblems) * 100;
            progressBar.value = percent;
        }
    },
    
    incrementCorrect: function() {
        AppState.stats.correct++;
        AppState.stats.total++;
        this.update();
    },
    
    incrementIncorrect: function() {
        AppState.stats.total++;
        this.update();
    }
};

window.ProgressManager = ProgressManager;