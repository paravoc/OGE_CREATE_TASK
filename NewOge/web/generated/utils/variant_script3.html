// ===== УПРАВЛЕНИЕ УВЕДОМЛЕНИЯМИ =====
const NotificationManager = {
    show(message, type = 'info', duration = 3000) {
        const notification = document.createElement('div');
        notification.className = `notification ${type}`;
        notification.textContent = message;
        
        document.body.appendChild(notification);
        
        setTimeout(() => {
            notification.style.animation = 'slideOut 0.3s ease';
            setTimeout(() => notification.remove(), 300);
        }, duration);
    },
    
    success(message) {
        this.show(message, 'success');
    },
    
    error(message) {
        this.show(message, 'error');
    },
    
    info(message) {
        this.show(message, 'info');
    }
};

// ===== УПРАВЛЕНИЕ РЕШЕНИЯМИ =====
const SolutionManager = {
    toggle(problemId) {
        const wrapper = document.querySelector(`[data-task-id="${problemId}"]`);
        if (!wrapper) return;
        
        const solution = wrapper.querySelector('.solution-content');
        const btn = wrapper.querySelector('.solution-toggle-btn');
        
        if (!solution || !btn) return;
        
        if (solution.style.display === 'none' || !solution.style.display) {
            solution.style.display = 'block';
            btn.innerHTML = '?? Скрыть решение';
        } else {
            solution.style.display = 'none';
            btn.innerHTML = '?? Показать решение';
        }
    }
};

// ===== ПРОВЕРКА ОТВЕТОВ =====
function checkAnswer(input) {
    const answer = input.value.trim();
    const correctAnswer = input.dataset.correctAnswer;
    const score = input.dataset.score;
    
    if (!answer) {
        NotificationManager.error('Введите ответ');
        return;
    }
    
    // Нормализация ответов (приводим к верхнему регистру для букв)
    const normalizedAnswer = answer.toUpperCase();
    const normalizedCorrect = correctAnswer.toUpperCase();
    
    if (normalizedAnswer === normalizedCorrect) {
        input.classList.add('correct');
        input.classList.remove('incorrect');
        NotificationManager.success(`? Правильно! +${score} баллов`);
        
        // Отмечаем как решённое
        input.dataset.solved = 'true';
        
        // Обновляем прогресс
        updateProgress();
    } else {
        input.classList.add('incorrect');
        input.classList.remove('correct');
        NotificationManager.error(`? Неправильно. Правильный ответ: ${correctAnswer}`);
    }
}

// ===== ОБНОВЛЕНИЕ ПРОГРЕССА =====
function updateProgress() {
    const inputs = document.querySelectorAll('.answer-input');
    const total = inputs.length;
    const solved = Array.from(inputs).filter(input => input.dataset.solved === 'true').length;
    
    // Обновляем прогресс-бар, если он есть
    const progressBar = document.getElementById('progress-bar');
    const progressText = document.getElementById('progress-text');
    
    if (progressBar) {
        const percent = total > 0 ? (solved / total) * 100 : 0;
        progressBar.style.width = `${percent}%`;
    }
    
    if (progressText) {
        progressText.textContent = `${solved}/${total} решено`;
    }
    
    // Если всё решено
    if (solved === total && total > 0) {
        NotificationManager.success('?? Поздравляем! Все задачи решены!');
    }
}

// ===== СОХРАНЕНИЕ ПРОГРЕССА =====
function saveProgress() {
    const inputs = document.querySelectorAll('.answer-input');
    const progress = {};
    
    inputs.forEach(input => {
        const wrapper = input.closest('.problem-wrapper');
        if (wrapper) {
            const taskId = wrapper.dataset.taskId;
            progress[taskId] = {
                solved: input.dataset.solved === 'true',
                value: input.value
            };
        }
    });
    
    localStorage.setItem('variantProgress', JSON.stringify(progress));
    NotificationManager.success('Прогресс сохранён');
}

// ===== ЗАГРУЗКА ПРОГРЕССА =====
function loadProgress() {
    const saved = localStorage.getItem('variantProgress');
    if (!saved) return;
    
    try {
        const progress = JSON.parse(saved);
        
        Object.entries(progress).forEach(([taskId, data]) => {
            const wrapper = document.querySelector(`[data-task-id="${taskId}"]`);
            if (!wrapper) return;
            
            const input = wrapper.querySelector('.answer-input');
            if (!input) return;
            
            if (data.solved) {
                input.dataset.solved = 'true';
                input.value = data.value || '';
                input.classList.add('correct');
            }
        });
        
        updateProgress();
        NotificationManager.info('Прогресс восстановлен');
    } catch (e) {
        console.error('Ошибка загрузки прогресса:', e);
    }
}

// ===== СБРОС ПРОГРЕССА =====
function resetProgress() {
    if (!confirm('Сбросить весь прогресс?')) return;
    
    const inputs = document.querySelectorAll('.answer-input');
    inputs.forEach(input => {
        input.dataset.solved = 'false';
        input.value = '';
        input.classList.remove('correct', 'incorrect');
    });
    
    localStorage.removeItem('variantProgress');
    updateProgress();
    NotificationManager.info('Прогресс сброшен');
}

// ===== ИНИЦИАЛИЗАЦИЯ =====
function initEventListeners() {
    console.log('? Инициализация обработчиков событий');
    
    // Находим все кнопки проверки
    document.querySelectorAll('.check-btn').forEach(btn => {
        // Удаляем старые обработчики, чтобы не было дубликатов
        btn.removeEventListener('click', handleCheckClick);
        btn.addEventListener('click', handleCheckClick);
    });
    
    // Обработчики для Enter
    document.querySelectorAll('.answer-input').forEach(input => {
        input.removeEventListener('keypress', handleKeyPress);
        input.addEventListener('keypress', handleKeyPress);
        
        // Очищаем стили при новом вводе
        input.removeEventListener('input', handleInput);
        input.addEventListener('input', handleInput);
    });
    
    // Кнопки решений
    document.querySelectorAll('.solution-toggle-btn').forEach(btn => {
        btn.removeEventListener('click', handleSolutionClick);
        btn.addEventListener('click', handleSolutionClick);
    });
}

// Обработчики событий
function handleCheckClick(e) {
    const wrapper = e.target.closest('.problem-wrapper');
    const input = wrapper.querySelector('.answer-input');
    if (input) checkAnswer(input);
}

function handleKeyPress(e) {
    if (e.key === 'Enter') {
        e.preventDefault();
        const wrapper = e.target.closest('.problem-wrapper');
        const btn = wrapper.querySelector('.check-btn');
        if (btn) btn.click();
    }
}

function handleInput(e) {
    e.target.classList.remove('correct', 'incorrect');
}

function handleSolutionClick(e) {
    const wrapper = e.target.closest('.problem-wrapper');
    const taskId = wrapper.dataset.taskId;
    if (taskId) SolutionManager.toggle(taskId);
}

// Запускаем после загрузки DOM
document.addEventListener('DOMContentLoaded', () => {
    console.log('? OGE Generator: скрипты загружены');
    
    // Инициализируем обработчики
    initEventListeners();
    
    // Загружаем сохранённый прогресс
    loadProgress();
    
    // Добавляем кнопки управления, если их нет
    addControlButtons();
});

// Также запускаем сразу, если DOM уже загружен
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initEventListeners);
} else {
    // DOM уже загружен
    setTimeout(initEventListeners, 100);
}

// ===== ДОБАВЛЕНИЕ КНОПОК УПРАВЛЕНИЯ =====
function addControlButtons() {
    const header = document.querySelector('.header');
    if (!header) return;
    
    // Проверяем, есть ли уже кнопки
    if (document.getElementById('control-buttons')) return;
    
    const controls = document.createElement('div');
    controls.id = 'control-buttons';
    controls.style.cssText = `
        display: flex;
        gap: 1rem;
        margin-top: 1rem;
        justify-content: flex-end;
    `;
    
    const saveBtn = document.createElement('button');
    saveBtn.className = 'yandex-btn';
    saveBtn.innerHTML = '?? Сохранить прогресс';
    saveBtn.onclick = saveProgress;
    
    const resetBtn = document.createElement('button');
    resetBtn.className = 'yandex-btn';
    resetBtn.innerHTML = '?? Сбросить';
    resetBtn.style.background = 'linear-gradient(135deg, #ff4757, #c53030)';
    resetBtn.onclick = resetProgress;
    
    controls.appendChild(saveBtn);
    controls.appendChild(resetBtn);
    header.appendChild(controls);
}

// ===== НАВИГАЦИЯ ПО ЗАДАЧАМ =====
const TaskNavigation = {
    init() {
        this.createSidebar();
        this.collectTasks();
        this.updateActiveTask();
        this.setupScrollListener();
        this.setupResizeHandler();
    },

    createSidebar() {
        // Удаляем старую панель, если есть
        const oldSidebar = document.getElementById('task-navigation');
        if (oldSidebar) oldSidebar.remove();

        const sidebar = document.createElement('div');
        sidebar.id = 'task-navigation';
        sidebar.innerHTML = `
            <div class="task-nav-header">
                <div class="task-nav-title">?? Задачи</div>
                <div class="task-nav-count" id="taskCount">0/0</div>
            </div>
            <div class="task-nav-list" id="taskNavList"></div>
            <div class="task-nav-footer">
                <div class="task-nav-progress">
                    <div class="task-nav-progress-bar" id="navProgressBar"></div>
                </div>
            </div>
        `;

        // Добавляем стили для панели
        const styles = document.createElement('style');
        styles.textContent = `
            #task-navigation {
                position: fixed;
                left: 20px;
                top: 50%;
                transform: translateY(-50%);
                width: 60px;
                background: rgba(22, 26, 35, 0.85);
                backdrop-filter: blur(20px);
                -webkit-backdrop-filter: blur(20px);
                border: 1px solid rgba(255, 200, 80, 0.15);
                border-radius: 40px;
                padding: 20px 0;
                box-shadow: 0 8px 32px rgba(0, 0, 0, 0.4);
                transition: all 0.3s ease;
                z-index: 1000;
                overflow-x: hidden;
		overflow-y: visible;
            }

            #task-navigation:hover {
                width: 280px;
                background: rgba(30, 35, 45, 0.95);
                box-shadow: 0 20px 40px rgba(255, 180, 70, 0.2);
                border-color: #ffb347;
            }

            #task-navigation:hover .task-nav-item span {
                opacity: 1;
                transform: translateX(0);
            }

            #task-navigation:hover .task-nav-header {
                padding: 0 20px 15px 20px;
            }

            #task-navigation:hover .task-nav-footer {
                padding: 15px 20px 0 20px;
            }

            .task-nav-header {
                padding: 0 15px 15px 15px;
                border-bottom: 1px solid rgba(255, 200, 80, 0.15);
                display: flex;
                justify-content: space-between;
                align-items: center;
                transition: padding 0.3s;
                white-space: nowrap;
            }

            .task-nav-title {
                color: #ffb347;
                font-weight: 600;
                font-size: 1rem;
                opacity: 0;
                transition: opacity 0.3s;
            }

            #task-navigation:hover .task-nav-title {
                opacity: 1;
            }

            .task-nav-count {
                color: #aaa;
                font-size: 0.9rem;
                background: rgba(255, 180, 70, 0.1);
                padding: 2px 8px;
                border-radius: 20px;
                opacity: 0;
                transition: opacity 0.3s;
            }

            #task-navigation:hover .task-nav-count {
                opacity: 1;
            }

            .task-nav-list {
                max-height: 400px;
                overflow-y: auto; /* Только вертикальный скролл */
                overflow-x: hidden; /* Убираем горизонтальный */
                padding: 10px 0;
                scrollbar-width: thin;
                scrollbar-color: #ffb347 rgba(255, 200, 80, 0.1);
            }

            .task-nav-list::-webkit-scrollbar {
                width: 4px;
            }

            .task-nav-list::-webkit-scrollbar-track {
                background: rgba(255, 200, 80, 0.1);
                border-radius: 2px;
            }

            .task-nav-list::-webkit-scrollbar-thumb {
                background: #ffb347;
                border-radius: 2px;
            }

            .task-nav-item {
                display: flex;
                align-items: center;
                padding: 8px 15px;
                margin: 2px 0;
                cursor: pointer;
                transition: all 0.2s;
                white-space: nowrap;
                position: relative;
                border-left: 3px solid transparent;
            }

            .task-nav-item:hover {
                background: rgba(255, 180, 70, 0.15);
                border-left-color: #ffb347;
            }

            .task-nav-item.active {
                background: rgba(255, 180, 70, 0.2);
                border-left-color: #ffb347;
            }

            .task-nav-item.active .task-nav-badge {
                background: #ffb347;
                color: #000;
            }

            .task-nav-number {
                min-width: 24px;
                height: 24px;
                background: rgba(255, 255, 255, 0.1);
                border-radius: 12px;
                display: flex;
                align-items: center;
                justify-content: center;
                font-size: 0.8rem;
                color: #ffb347;
                margin-right: 10px;
                transition: all 0.3s;
            }

            .task-nav-item.solved .task-nav-number {
                background: rgba(72, 187, 120, 0.2);
                color: #48bb78;
            }

            .task-nav-item span {
                opacity: 0;
                transform: translateX(-10px);
                transition: all 0.3s;
                color: #ddd;
                flex: 1;
            }

            #task-navigation:hover .task-nav-item span {
                opacity: 1;
                transform: translateX(0);
            }

            .task-nav-badge {
                min-width: 20px;
                height: 20px;
                background: rgba(255, 180, 70, 0.2);
                border-radius: 10px;
                display: flex;
                align-items: center;
                justify-content: center;
                font-size: 0.7rem;
                color: #ffb347;
                margin-left: 8px;
                opacity: 0;
                transition: opacity 0.3s;
            }

            #task-navigation:hover .task-nav-badge {
                opacity: 1;
            }

            .task-nav-footer {
                padding: 0 15px;
                border-top: 1px solid rgba(255, 200, 80, 0.15);
                transition: padding 0.3s;
            }

            .task-nav-progress {
                height: 4px;
                background: rgba(255, 255, 255, 0.1);
                border-radius: 2px;
                overflow: hidden;
                opacity: 0;
                transition: opacity 0.3s;
            }

            #task-navigation:hover .task-nav-progress {
                opacity: 1;
            }

            .task-nav-progress-bar {
                height: 100%;
                background: linear-gradient(90deg, #ffb347, #ff8c00);
                width: 0%;
                transition: width 0.3s;
            }

            /* Мини-карточка предпросмотра */
            .task-preview-tooltip {
                position: absolute;
                left: 100%;
                top: 50%;
                transform: translateY(-50%);
                background: rgba(30, 35, 45, 0.95);
                backdrop-filter: blur(10px);
                border: 1px solid #ffb347;
                border-radius: 20px;
                padding: 15px;
                margin-left: 10px;
                width: 250px;
                box-shadow: 0 10px 30px rgba(0, 0, 0, 0.3);
                z-index: 1001;
                pointer-events: none;
                animation: fadeIn 0.2s ease;
            }

            .task-preview-tooltip .preview-title {
                color: #ffb347;
                font-weight: 600;
                margin-bottom: 5px;
                font-size: 0.9rem;
            }

            .task-preview-tooltip .preview-text {
                color: #ddd;
                font-size: 0.85rem;
                line-height: 1.4;
                max-height: 100px;
                overflow: hidden;
            }

            @keyframes fadeIn {
                from {
                    opacity: 0;
                    transform: translateY(-50%) translateX(-10px);
                }
                to {
                    opacity: 1;
                    transform: translateY(-50%) translateX(0);
                }
            }

            /* Адаптация для мобильных */
            @media (max-width: 768px) {
                #task-navigation {
                    left: 10px;
                    width: 40px;
                }

                #task-navigation:hover {
                    width: 240px;
                }

                .task-preview-tooltip {
                    display: none;
                }
            }

            /* Корректировка основного контента */
            body.has-task-nav .container {
                margin-left: 80px;
                transition: margin-left 0.3s;
            }

            @media (max-width: 768px) {
                body.has-task-nav .container {
                    margin-left: 50px;
                }
            }
        `;

        document.head.appendChild(styles);
        document.body.appendChild(sidebar);
        document.body.classList.add('has-task-nav');
    },

    collectTasks() {
        const wrappers = document.querySelectorAll('.problem-wrapper');
        const taskList = document.getElementById('taskNavList');
        const taskCount = document.getElementById('taskCount');
        
        if (!taskList || !wrappers.length) return;

        let html = '';
        wrappers.forEach((wrapper, index) => {
            const taskId = wrapper.dataset.taskId;
            const taskNum = index + 1;
            
            // Пытаемся получить заголовок задачи
            let taskTitle = '';
            const problemContent = wrapper.querySelector('.problem-content');
            if (problemContent) {
                const text = problemContent.textContent || '';
                taskTitle = text.substring(0, 50) + (text.length > 50 ? '...' : '');
            }

            html += `
                <div class="task-nav-item" data-task-id="${taskId}" data-task-index="${taskNum}" data-preview="${taskTitle.replace(/"/g, '&quot;')}">
                    <div class="task-nav-number">${taskNum}</div>
                    <span>Задача ${taskNum}</span>
                    <div class="task-nav-badge" id="badge-${taskId}"></div>
                </div>
            `;
        });

        taskList.innerHTML = html;
        taskCount.textContent = `0/${wrappers.length}`;

        // Добавляем обработчики событий
        document.querySelectorAll('.task-nav-item').forEach(item => {
            item.addEventListener('click', (e) => {
                const taskId = item.dataset.taskId;
                this.scrollToTask(taskId);
            });

            item.addEventListener('mouseenter', (e) => {
                this.showPreview(e);
            });

            item.addEventListener('mouseleave', () => {
                this.hidePreview();
            });
        });
    },

    scrollToTask(taskId) {
        const wrapper = document.querySelector(`[data-task-id="${taskId}"]`);
        if (!wrapper) return;

        wrapper.scrollIntoView({
            behavior: 'smooth',
            block: 'center'
        });

        // Подсвечиваем задачу
        wrapper.style.transition = 'box-shadow 0.3s';
        wrapper.style.boxShadow = '0 0 30px rgba(255, 180, 70, 0.5)';
        setTimeout(() => {
            wrapper.style.boxShadow = '';
        }, 1000);
    },

    showPreview(e) {
        const item = e.currentTarget;
        const previewText = item.dataset.preview || 'Нет предпросмотра';
        const taskNum = item.dataset.taskIndex;

        const tooltip = document.createElement('div');
        tooltip.className = 'task-preview-tooltip';
        tooltip.innerHTML = `
            <div class="preview-title">Задача ${taskNum}</div>
            <div class="preview-text">${previewText}</div>
        `;

        // Позиционируем подсказку
        const rect = item.getBoundingClientRect();
        tooltip.style.left = rect.width + 'px';
        
        // Удаляем старую подсказку
        this.hidePreview();
        
        item.appendChild(tooltip);
    },

    hidePreview() {
        document.querySelectorAll('.task-preview-tooltip').forEach(el => el.remove());
    },

    updateActiveTask() {
        const observer = new IntersectionObserver((entries) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    const taskId = entry.target.dataset.taskId;
                    this.setActiveTask(taskId);
                }
            });
        }, {
            threshold: 0.5,
            rootMargin: '-100px 0px -100px 0px'
        });

        document.querySelectorAll('.problem-wrapper').forEach(wrapper => {
            observer.observe(wrapper);
        });
    },

    setActiveTask(taskId) {
        document.querySelectorAll('.task-nav-item').forEach(item => {
            item.classList.remove('active');
            if (item.dataset.taskId === taskId) {
                item.classList.add('active');
            }
        });
    },

    updateTaskStatus() {
        const wrappers = document.querySelectorAll('.problem-wrapper');
        const total = wrappers.length;
        let solved = 0;

        wrappers.forEach((wrapper, index) => {
            const taskId = wrapper.dataset.taskId;
            const input = wrapper.querySelector('.answer-input');
            const navItem = document.querySelector(`.task-nav-item[data-task-id="${taskId}"]`);
            const badge = document.getElementById(`badge-${taskId}`);

            if (input && input.dataset.solved === 'true') {
                solved++;
                if (navItem) navItem.classList.add('solved');
                if (badge) {
                    badge.textContent = '?';
                    badge.style.background = '#48bb78';
                    badge.style.color = '#000';
                }
            } else {
                if (navItem) navItem.classList.remove('solved');
                if (badge) {
                    badge.textContent = '';
                    badge.style.background = '';
                }
            }
        });

        // Обновляем счетчик
        const taskCount = document.getElementById('taskCount');
        if (taskCount) {
            taskCount.textContent = `${solved}/${total}`;
        }

        // Обновляем прогресс-бар
        const progressBar = document.getElementById('navProgressBar');
        if (progressBar) {
            const percent = total > 0 ? (solved / total) * 100 : 0;
            progressBar.style.width = `${percent}%`;
        }

        return solved;
    },

    setupScrollListener() {
        let scrollTimeout;
        window.addEventListener('scroll', () => {
            clearTimeout(scrollTimeout);
            scrollTimeout = setTimeout(() => {
                this.updateTaskStatus();
            }, 100);
        });
    },

    setupResizeHandler() {
        let resizeTimeout;
        window.addEventListener('resize', () => {
            clearTimeout(resizeTimeout);
            resizeTimeout = setTimeout(() => {
                // Перестраиваем панель при изменении размера окна
                this.init();
            }, 250);
        });
    }
};

// Переопределяем updateProgress для обновления навигации
const originalUpdateProgress = updateProgress;
updateProgress = function() {
    if (originalUpdateProgress) originalUpdateProgress();
    TaskNavigation.updateTaskStatus();
};

// Переопределяем loadProgress
const originalLoadProgress = loadProgress;
loadProgress = function() {
    if (originalLoadProgress) originalLoadProgress();
    setTimeout(() => TaskNavigation.updateTaskStatus(), 100);
};

// Переопределяем resetProgress
const originalResetProgress = resetProgress;
resetProgress = function() {
    if (originalResetProgress) originalResetProgress();
    setTimeout(() => TaskNavigation.updateTaskStatus(), 100);
};

// Инициализация при загрузке страницы
document.addEventListener('DOMContentLoaded', () => {
    // Немного ждем, чтобы основной контент загрузился
    setTimeout(() => {
        TaskNavigation.init();
    }, 500);
});

// Экспортируем для использования в консоли
window.TaskNavigation = TaskNavigation;

// ===== ЭКСПОРТ ДЛЯ ИСПОЛЬЗОВАНИЯ В HTML =====
window.NotificationManager = NotificationManager;
window.SolutionManager = SolutionManager;
window.checkAnswer = checkAnswer;
window.saveProgress = saveProgress;
window.resetProgress = resetProgress;