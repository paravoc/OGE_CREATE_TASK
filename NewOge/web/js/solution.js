const SolutionManager = {
    toggleSolution: function(problemId) {
        const solutionDiv = document.getElementById('solution-' + problemId);
        const button = event.currentTarget;
        
        if (solutionDiv.style.display === 'none') {
            solutionDiv.style.display = 'block';
            button.innerHTML = '?? Скрыть решение';
        } else {
            solutionDiv.style.display = 'none';
            button.innerHTML = '?? Показать решение';
        }
    }
};

window.SolutionManager = SolutionManager;