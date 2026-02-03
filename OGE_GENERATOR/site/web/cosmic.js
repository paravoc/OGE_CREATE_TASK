// cosmic.js
class CosmicProblemManager {
    constructor() {
        this.problems = []; // Пустой массив задач
        this.currentProblemIndex = 0;
        this.filter = 'all';
        this.startTime = null;
        this.init();
    }
    
    init() {
        this.setupEventListeners();
        this.setupNavigation();
        this.updateNavigation();
        this.startTime = Date.now();
        
        // Убираем ненужные кнопки
        this.disableUnneededButtons();
        
        setTimeout(() => this.showNotification('Система инициализирована. Данные будут загружены с сервера.', 'info'), 1000);
    }
    
    disableUnneededButtons() {
        // Отключаем кнопку "Новая задача"
        const newProblemBtn = document.querySelector('.cosmic-btn-primary');
        if (newProblemBtn) {
            newProblemBtn.disabled = true;
            newProblemBtn.innerHTML = '<i class="fas fa-server"></i><span>ОЖИДАНИЕ ДАННЫХ</span>';
            newProblemBtn.style.opacity = '0.6';
            newProblemBtn.style.cursor = 'not-allowed';
        }
        
        // Отключаем кнопку генерации
        const generateBtn = document.querySelector('.cosmic-btn-generate');
        if (generateBtn) {
            generateBtn.disabled = true;
            generateBtn.innerHTML = '<i class="fas fa-database"></i><span>ГЕНЕРАЦИЯ ЧЕРЕЗ С++</span>';
            generateBtn.style.opacity = '0.6';
            generateBtn.style.cursor = 'not-allowed';
        }
    }
    
    setupEventListeners() {
        // Кнопки фильтрации
        document.addEventListener('click', (e) => {
            if (e.target.closest('.filter-btn')) {
                const filter = e.target.closest('.filter-btn').dataset.filter;
                this.setFilter(filter);
            }
        });
        
        // Глобальные обработчики для динамически созданных элементов
        document.addEventListener('click', (e) => {
            // Проверка ответа
            if (e.target.closest('.cosmic-btn-check')) {
                this.checkAnswer(e.target.closest('.cosmic-problem'));
            }
            
            // Показать/скрыть решение
            if (e.target.closest('.show-solution-btn')) {
                this.toggleSolution(e.target.closest('.cosmic-problem'), true);
            }
            
            if (e.target.closest('.hide-solution-btn')) {
                this.toggleSolution(e.target.closest('.cosmic-problem'), false);
            }
            
            // Раскрыть/свернуть задачу
            if (e.target.closest('.problem-expand')) {
                this.toggleProblem(e.target.closest('.cosmic-problem'));
            }
            
            // Переход к задаче по навигации
            if (e.target.closest('.nav-dot')) {
                const index = parseInt(e.target.closest('.nav-dot').dataset.index);
                this.goToProblem(index);
            }
        });
    }
    
    setupNavigation() {
        const navContainer = document.createElement('div');
        navContainer.className = 'problems-navigation';
        navContainer.innerHTML = '<div class="nav-title">ЗАДАЧИ</div>';
        document.querySelector('.cosmic-container').appendChild(navContainer);
        this.navContainer = navContainer;
    }
    
    renderProblems() {
        const container = document.querySelector('.problems-container');
        if (!container) return;
        
        // Получаем заголовок
        const header = container.querySelector('.problems-header');
        
        // Полностью очищаем контейнер
        container.innerHTML = '';
        
        // Восстанавливаем заголовок
        if (header) {
            container.appendChild(header);
        }
        
        // Добавляем панель фильтрации (только если есть задачи)
        if (this.problems.length > 0) {
            const filterPanel = this.createFilterPanel();
            container.appendChild(filterPanel);
        }
        
        // Применяем фильтр
        let filteredProblems = [];
        if (this.filter === 'solved') {
            filteredProblems = this.problems.filter(p => p.solved);
        } else if (this.filter === 'unsolved') {
            filteredProblems = this.problems.filter(p => !p.solved);
        } else if (this.filter === 'incorrect') {
            filteredProblems = this.problems.filter(p => p.answerChecked && !p.solved);
        } else {
            filteredProblems = this.problems;
        }
        
        // Если нет задач, показываем сообщение о загрузке
        if (filteredProblems.length === 0) {
            const emptyMessage = document.createElement('div');
            emptyMessage.className = 'empty-message';
            
            if (this.problems.length === 0) {
                emptyMessage.innerHTML = `
                    <div class="empty-icon">
                        <i class="fas fa-database"></i>
                    </div>
                    <h3>ДАННЫЕ ОТСУТСТВУЮТ</h3>
                    <p>Задачи будут загружены через С++ сервер</p>
                    <div class="loading-animation">
                        <div class="loading-dot"></div>
                        <div class="loading-dot"></div>
                        <div class="loading-dot"></div>
                    </div>
                `;
            } else {
                emptyMessage.innerHTML = `
                    <div class="empty-icon">
                        <i class="fas fa-search"></i>
                    </div>
                    <h3>Нет задач по выбранному фильтру</h3>
                    <p>Попробуйте изменить фильтр</p>
                `;
            }
            container.appendChild(emptyMessage);
        } else {
            // Рендерим отфильтрованные задачи
            filteredProblems.forEach((problem, displayIndex) => {
                const problemElement = this.createProblemElement(problem, displayIndex);
                container.appendChild(problemElement);
            });
        }
        
        // Добавляем панель сдачи только если есть задачи
        if (this.problems.length > 0) {
            this.addSubmissionPanel();
        }
        
        this.updateNavigation();
        this.updateCounter();
    }
    
    createFilterPanel() {
        const panel = document.createElement('div');
        panel.className = 'filter-panel';
        panel.innerHTML = `
            <button class="filter-btn ${this.filter === 'all' ? 'active' : ''}" data-filter="all">Все задачи</button>
            <button class="filter-btn ${this.filter === 'unsolved' ? 'active' : ''}" data-filter="unsolved">Не решённые</button>
            <button class="filter-btn ${this.filter === 'solved' ? 'active' : ''}" data-filter="solved">Решённые</button>
            <button class="filter-btn ${this.filter === 'incorrect' ? 'active' : ''}" data-filter="incorrect">С ошибками</button>
        `;
        return panel;
    }
    
    createProblemElement(problem, displayIndex) {
        const element = document.createElement('article');
        element.className = `cosmic-problem ${problem.solved ? 'solved' : ''} ${problem.answerChecked && !problem.solved ? 'incorrect' : ''}`;
        element.dataset.id = problem.id;
        element.dataset.displayIndex = displayIndex;
        
        // Находим оригинальный индекс в основном массиве
        const originalIndex = this.problems.findIndex(p => p.id === problem.id);
        element.dataset.originalIndex = originalIndex;
        
        const statusText = problem.solved ? 'Решено' : 
                          problem.answerChecked ? 'Ошибка' : 'Не решено';
        
        element.innerHTML = `
            <div class="problem-header">
                <div class="problem-id">
                    <span class="id-number">#${String(problem.id).padStart(3, '0')}</span>
                    <span class="id-type">ТИП ${problem.type ? problem.type.toUpperCase().replace('TYPE', '') : '1'}</span>
                </div>
                <div class="problem-meta">
                    <span class="meta-item">
                        <i class="fas fa-microchip"></i>
                        ${problem.data?.encoding || 'UTF-8'}
                    </span>
                    <span class="meta-item">
                        <i class="fas fa-brain"></i>
                        ${this.getDifficultyText(problem.difficulty || 2)}
                    </span>
                    <span class="meta-item">
                        <i class="fas fa-clock"></i>
                        ${this.getTimeEstimate(problem.difficulty || 2)}
                    </span>
                </div>
                <button class="problem-expand">
                    <i class="fas fa-chevron-down"></i>
                </button>
            </div>
            
            <div class="problem-content" style="max-height: 0; opacity: 0; overflow: hidden;">
                <div class="problem-text">
                    <p>В кодировке <span class="highlight">${problem.data?.encoding || 'UTF-8'}</span> каждый символ кодируется <span class="highlight">${problem.data?.bits || 8} битами (${(problem.data?.bits || 8)/8} байт на символ)</span>.</p>
                    <p class="quote">Андрей написал текст (в нём нет лишних пробелов):</p>
                    <blockquote class="cosmic-quote">
                        «${problem.data?.originalText || 'Текст задачи будет загружен...'}».
                    </blockquote>
                    <p>Ученик вычеркнул из списка название одного элемента. Заодно он вычеркнул ставшие лишними запятые и пробелы.</p>
                    <p>При этом размер нового предложения в данной кодировке оказался на <span class="highlight">${problem.data?.sizeDiff || 0} байт</span> меньше, чем размер исходного предложения.</p>
                    <p class="question">Напишите в ответе вычеркнутое название.</p>
                </div>
                
                <div class="problem-actions">
                    <div class="answer-field">
                        <input type="text" 
                               placeholder="Введите ваш ответ..." 
                               class="cosmic-input ${problem.userAnswer ? 'answered' : ''}"
                               value="${problem.userAnswer || ''}"
                               ${problem.solved ? 'disabled' : ''}>
                        <button class="cosmic-btn cosmic-btn-check" ${problem.solved ? 'disabled' : ''}>
                            <i class="fas fa-check"></i>
                            ${problem.answerChecked ? 'ПРОВЕРИТЬ СНОВА' : 'ПРОВЕРИТЬ'}
                        </button>
                    </div>
                    
                    <div class="solution-controls">
                        <button class="show-solution-btn">
                            <i class="fas fa-eye"></i>
                            ПОКАЗАТЬ РЕШЕНИЕ
                        </button>
                        <button class="hide-solution-btn" style="display: none;">
                            <i class="fas fa-eye-slash"></i>
                            СКРЫТЬ РЕШЕНИЕ
                        </button>
                    </div>
                </div>
                
                <div class="problem-solution hidden">
                    <div class="solution-header">
                        <h3><i class="fas fa-cogs"></i> РЕШЕНИЕ</h3>
                    </div>
                    <div class="solution-content">
                        <p>${problem.data?.explanation || 'Решение будет показано после проверки...'}</p>
                        <p class="answer-final">ОТВЕТ: <strong>${problem.correctAnswer || 'Ожидается...'}</strong></p>
                        ${problem.userAnswer ? `
                            <p class="user-answer ${problem.solved ? 'correct' : 'incorrect'}">
                                Ваш ответ: <strong>${problem.userAnswer}</strong> - 
                                ${problem.solved ? 'ПРАВИЛЬНО!' : 'НЕПРАВИЛЬНО'}
                            </p>
                        ` : ''}
                    </div>
                </div>
            </div>
            
            <div class="problem-footer">
                <div class="status-indicator">
                    <div class="status-dot ${problem.solved ? 'solved' : problem.answerChecked ? 'incorrect' : ''}"></div>
                    <span>${statusText}</span>
                </div>
                <div class="problem-stats">
                    <span><i class="fas fa-users"></i> ${this.getSuccessRate(problem.difficulty || 2)}% решили правильно</span>
                    <span><i class="fas fa-chart-line"></i> Популярность: ${this.getPopularity(problem.difficulty || 2)}</span>
                </div>
            </div>
        `;
        
        // Если задача раскрыта по умолчанию
        if (displayIndex === 0 && this.currentProblemIndex === 0) {
            const content = element.querySelector('.problem-content');
            content.style.maxHeight = content.scrollHeight + 'px';
            content.style.opacity = '1';
            element.querySelector('.problem-expand i').className = 'fas fa-chevron-up';
            element.classList.add('active');
        }
        
        return element;
    }
    
    addSubmissionPanel() {
        const container = document.querySelector('.problems-container');
        
        // Удаляем старую панель, если есть
        const oldPanel = container.querySelector('.submission-panel');
        if (oldPanel) oldPanel.remove();
        
        // Создаем новую панель только если есть задачи
        if (this.problems.length === 0) return;
        
        const panel = document.createElement('div');
        panel.className = 'submission-panel';
        
        // Рассчитываем статистику
        const solvedCount = this.problems.filter(p => p.solved).length;
        const totalCount = this.problems.length;
        const accuracy = totalCount > 0 ? Math.round((solvedCount / totalCount) * 100) : 0;
        
        // Рассчитываем прошедшее время
        const elapsedTime = Math.floor((Date.now() - this.startTime) / 60000);
        
        panel.innerHTML = `
            <div class="submission-stats">
                <div class="stat-item">
                    <div class="stat-icon">
                        <i class="fas fa-check-circle"></i>
                    </div>
                    <div class="stat-info">
                        <div class="stat-value">${solvedCount}/${totalCount}</div>
                        <div class="stat-label">Задач решено</div>
                    </div>
                </div>
                
                <div class="stat-item">
                    <div class="stat-icon">
                        <i class="fas fa-percentage"></i>
                    </div>
                    <div class="stat-info">
                        <div class="stat-value">${accuracy}%</div>
                        <div class="stat-label">Точность</div>
                    </div>
                </div>
                
                <div class="stat-item">
                    <div class="stat-icon">
                        <i class="fas fa-clock"></i>
                    </div>
                    <div class="stat-info">
                        <div class="stat-value">${elapsedTime} мин</div>
                        <div class="stat-label">Время работы</div>
                    </div>
                </div>
            </div>
            
            <div class="submission-actions">
                <button class="cosmic-btn cosmic-btn-submit">
                    <i class="fas fa-paper-plane"></i>
                    СДАТЬ ЗАДАНИЕ
                </button>
                <button class="cosmic-btn cosmic-btn-review">
                    <i class="fas fa-chart-bar"></i>
                    ПОСМОТРЕТЬ СТАТИСТИКУ
                </button>
            </div>
        `;
        
        container.appendChild(panel);
        
        // Добавляем обработчики событий
        panel.querySelector('.cosmic-btn-submit').addEventListener('click', () => {
            this.submitAssignment();
        });
        
        panel.querySelector('.cosmic-btn-review').addEventListener('click', () => {
            this.showDetailedStats();
        });
    }
    
    updateNavigation() {
        if (!this.navContainer) return;
        
        // Удаляем старые точки кроме заголовка
        const title = this.navContainer.querySelector('.nav-title');
        this.navContainer.innerHTML = '';
        if (title) this.navContainer.appendChild(title);
        
        // Если нет задач, показываем пустое состояние
        if (this.problems.length === 0) {
            const emptyDot = document.createElement('div');
            emptyDot.className = 'nav-dot empty';
            emptyDot.innerHTML = '<div class="nav-tooltip">Задачи отсутствуют</div>';
            this.navContainer.appendChild(emptyDot);
            return;
        }
        
        // Создаем новые точки
        this.problems.forEach((problem, index) => {
            const dot = document.createElement('div');
            dot.className = `nav-dot ${index === this.currentProblemIndex ? 'active' : ''} 
                           ${problem.solved ? 'solved' : ''} 
                           ${problem.answerChecked && !problem.solved ? 'incorrect' : ''}`;
            dot.dataset.index = index;
            
            const tooltip = document.createElement('div');
            tooltip.className = 'nav-tooltip';
            tooltip.textContent = `Задача #${String(problem.id).padStart(3, '0')} - ${this.getDifficultyText(problem.difficulty || 2)}`;
            dot.appendChild(tooltip);
            
            this.navContainer.appendChild(dot);
        });
    }
    
    updateCounter() {
        const counter = document.querySelector('.counter-number');
        const text = document.querySelector('.counter-text');
        if (counter && text) {
            const unsolved = this.problems.filter(p => !p.solved).length;
            counter.textContent = unsolved;
            text.textContent = `${this.getTaskWord(unsolved)} не решено`;
        }
    }
    
    setFilter(filter) {
        this.filter = filter;
        document.querySelectorAll('.filter-btn').forEach(btn => {
            btn.classList.toggle('active', btn.dataset.filter === filter);
        });
        
        // Закрываем все открытые задачи перед фильтрацией
        document.querySelectorAll('.cosmic-problem.active').forEach(problem => {
            this.toggleProblem(problem);
        });
        
        this.renderProblems();
        
        // Сбрасываем текущий индекс
        this.currentProblemIndex = 0;
        
        // Показываем первую задачу из отфильтрованного списка
        setTimeout(() => {
            const firstProblem = document.querySelector('.cosmic-problem');
            if (firstProblem) {
                this.toggleProblem(firstProblem);
                this.updateNavigation();
            }
        }, 100);
    }
    
    toggleProblem(problemElement) {
        const content = problemElement.querySelector('.problem-content');
        const icon = problemElement.querySelector('.problem-expand i');
        
        // Обновляем текущий индекс на основе оригинального индекса
        const originalIndex = parseInt(problemElement.dataset.originalIndex);
        this.currentProblemIndex = originalIndex;
        
        if (content.style.maxHeight && content.style.maxHeight !== '0px') {
            content.style.maxHeight = '0px';
            content.style.opacity = '0';
            icon.className = 'fas fa-chevron-down';
            problemElement.classList.remove('active');
        } else {
            // Закрываем все другие открытые задачи
            document.querySelectorAll('.cosmic-problem.active').forEach(prob => {
                if (prob !== problemElement) {
                    const otherContent = prob.querySelector('.problem-content');
                    const otherIcon = prob.querySelector('.problem-expand i');
                    otherContent.style.maxHeight = '0px';
                    otherContent.style.opacity = '0';
                    otherIcon.className = 'fas fa-chevron-down';
                    prob.classList.remove('active');
                }
            });
            
            content.style.maxHeight = content.scrollHeight + 'px';
            content.style.opacity = '1';
            icon.className = 'fas fa-chevron-up';
            problemElement.classList.add('active');
        }
        
        this.updateNavigation();
    }
    
    checkAnswer(problemElement) {
        const input = problemElement.querySelector('.cosmic-input');
        const answer = input.value.trim();
        const problemId = parseInt(problemElement.dataset.id);
        const problem = this.problems.find(p => p.id === problemId);
        
        if (!problem || !problem.correctAnswer) {
            this.showNotification('Задача не полностью загружена', 'error');
            return;
        }
        
        if (!answer) {
            input.classList.add('answer-incorrect');
            setTimeout(() => input.classList.remove('answer-incorrect'), 500);
            this.showNotification('Введите ответ!', 'error');
            return;
        }
        
        problem.userAnswer = answer;
        problem.answerChecked = true;
        problem.solved = (answer.toLowerCase() === problem.correctAnswer.toLowerCase());
        
        // Обновляем UI
        const checkBtn = problemElement.querySelector('.cosmic-btn-check');
        const statusDot = problemElement.querySelector('.status-dot');
        const statusText = problemElement.querySelector('.status-indicator span');
        
        if (problem.solved) {
            problemElement.classList.add('solved');
            problemElement.classList.remove('incorrect');
            statusDot.className = 'status-dot solved';
            statusText.textContent = 'Решено';
            input.classList.add('answer-correct');
            checkBtn.disabled = true;
            checkBtn.innerHTML = '<i class="fas fa-check-circle"></i> РЕШЕНО';
            this.showNotification('Правильно! Отличная работа!', 'success');
        } else {
            problemElement.classList.add('incorrect');
            problemElement.classList.remove('solved');
            statusDot.className = 'status-dot incorrect';
            statusText.textContent = 'Ошибка';
            input.classList.add('answer-incorrect');
            this.showNotification(`Неправильно. Попробуйте ещё раз!`, 'error');
        }
        
        // Обновляем решение
        const solution = problemElement.querySelector('.problem-solution');
        if (solution) {
            const userAnswerElem = solution.querySelector('.user-answer') || document.createElement('p');
            userAnswerElem.className = `user-answer ${problem.solved ? 'correct' : 'incorrect'}`;
            userAnswerElem.innerHTML = `
                Ваш ответ: <strong>${problem.userAnswer}</strong> - 
                ${problem.solved ? 'ПРАВИЛЬНО!' : 'НЕПРАВИЛЬНО'}
            `;
            const solutionContent = solution.querySelector('.solution-content');
            const existingUserAnswer = solutionContent.querySelector('.user-answer');
            if (existingUserAnswer) {
                existingUserAnswer.replaceWith(userAnswerElem);
            } else {
                solutionContent.appendChild(userAnswerElem);
            }
        }
        
        setTimeout(() => {
            input.classList.remove('answer-correct', 'answer-incorrect');
        }, 1000);
        
        this.updateNavigation();
        this.updateCounter();
        
        // Обновляем панель сдачи
        this.addSubmissionPanel();
    }
    
    toggleSolution(problemElement, show) {
        const solution = problemElement.querySelector('.problem-solution');
        const showBtn = problemElement.querySelector('.show-solution-btn');
        const hideBtn = problemElement.querySelector('.hide-solution-btn');
        
        if (show) {
            solution.classList.remove('hidden');
            solution.style.animation = 'slideIn 0.3s ease';
            showBtn.style.display = 'none';
            hideBtn.style.display = 'flex';
            
            // Обновляем высоту контента
            const content = problemElement.querySelector('.problem-content');
            content.style.maxHeight = content.scrollHeight + solution.scrollHeight + 'px';
        } else {
            solution.classList.add('hidden');
            showBtn.style.display = 'flex';
            hideBtn.style.display = 'none';
            
            // Обновляем высоту контента
            const content = problemElement.querySelector('.problem-content');
            const otherElements = content.querySelectorAll(':not(.problem-solution)');
            let totalHeight = 0;
            otherElements.forEach(el => {
                totalHeight += el.scrollHeight;
            });
            content.style.maxHeight = totalHeight + 'px';
        }
    }
    
    goToProblem(index) {
        if (index < 0 || index >= this.problems.length) return;
        
        // Закрываем текущую задачу
        const currentProblem = document.querySelector(`.cosmic-problem[data-original-index="${this.currentProblemIndex}"]`);
        if (currentProblem) {
            this.toggleProblem(currentProblem);
        }
        
        // Открываем новую задачу
        this.currentProblemIndex = index;
        const newProblem = document.querySelector(`.cosmic-problem[data-original-index="${index}"]`);
        if (newProblem) {
            this.toggleProblem(newProblem);
            
            // Прокрутка к задаче
            newProblem.scrollIntoView({ behavior: 'smooth', block: 'center' });
        }
        
        this.updateNavigation();
    }
    
    submitAssignment() {
        if (this.problems.length === 0) {
            this.showNotification('Нет задач для сдачи', 'error');
            return;
        }
        
        const solvedCount = this.problems.filter(p => p.solved).length;
        const totalCount = this.problems.length;
        const accuracy = totalCount > 0 ? Math.round((solvedCount / totalCount) * 100) : 0;
        const elapsedTime = Math.floor((Date.now() - this.startTime) / 60000);
        
        // Создаем модальное окно с результатами
        const modal = document.createElement('div');
        modal.className = 'cosmic-modal';
        modal.innerHTML = `
            <div class="modal-content" style="max-width: 600px;">
                <div class="modal-header">
                    <h2><i class="fas fa-trophy"></i> РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ</h2>
                    <button class="modal-close">
                        <i class="fas fa-times"></i>
                    </button>
                </div>
                <div class="modal-body">
                    <div class="result-stats">
                        <div class="result-item result-total">
                            <div class="result-value">${solvedCount}/${totalCount}</div>
                            <div class="result-label">Верных ответов</div>
                        </div>
                        
                        <div class="result-item result-accuracy">
                            <div class="result-value">${accuracy}%</div>
                            <div class="result-label">Точность</div>
                        </div>
                        
                        <div class="result-item result-time">
                            <div class="result-value">${elapsedTime} мин</div>
                            <div class="result-label">Время работы</div>
                        </div>
                    </div>
                    
                    <div class="grade-display">
                        <div class="grade-value">${this.calculateGrade(accuracy)}</div>
                        <div class="grade-label">ОЦЕНКА</div>
                    </div>
                    
                    <div class="result-details">
                        <h3>Детальная статистика:</h3>
                        <ul>
                            ${this.problems.map(problem => `
                                <li>
                                    <span class="problem-id">#${String(problem.id).padStart(3, '0')}</span>
                                    <span class="problem-status ${problem.solved ? 'solved' : 'unsolved'}">
                                        ${problem.solved ? '✓ Решено' : problem.answerChecked ? '✗ Ошибка' : '○ Не решено'}
                                    </span>
                                    <span class="problem-time">${this.getTimeEstimate(problem.difficulty || 2)}</span>
                                </li>
                            `).join('')}
                        </ul>
                    </div>
                    
                    <div class="result-actions">
                        <button class="cosmic-btn cosmic-btn-secondary" id="close-btn">
                            <i class="fas fa-times"></i>
                            ЗАКРЫТЬ
                        </button>
                    </div>
                </div>
            </div>
        `;
        
        document.body.appendChild(modal);
        
        // Включаем скролл на фоновой странице
        document.body.style.overflow = 'hidden';
        
        // Показываем модальное окно
        setTimeout(() => {
            modal.style.display = 'flex';
            setTimeout(() => modal.classList.add('show'), 10);
        }, 10);
        
        // Обработчики закрытия
        const closeModal = () => {
            modal.classList.remove('show');
            setTimeout(() => {
                modal.remove();
                document.body.style.overflow = ''; // Восстанавливаем скролл
            }, 300);
        };
        
        modal.querySelector('.modal-close').addEventListener('click', closeModal);
        modal.querySelector('#close-btn').addEventListener('click', closeModal);
        
        // Закрытие по клику вне модального окна
        modal.addEventListener('click', (e) => {
            if (e.target === modal) {
                closeModal();
            }
        });
        
        // Закрытие по Escape
        const handleEscape = (e) => {
            if (e.key === 'Escape') {
                closeModal();
                document.removeEventListener('keydown', handleEscape);
            }
        };
        document.addEventListener('keydown', handleEscape);
    }
    
    showDetailedStats() {
        if (this.problems.length === 0) {
            this.showNotification('Нет данных для статистики', 'error');
            return;
        }
        
        // Подробная статистика
        const solvedCount = this.problems.filter(p => p.solved).length;
        const incorrectCount = this.problems.filter(p => p.answerChecked && !p.solved).length;
        const unsolvedCount = this.problems.filter(p => !p.answerChecked && !p.solved).length;
        const totalCount = this.problems.length;
        
        const modal = document.createElement('div');
        modal.className = 'cosmic-modal';
        modal.innerHTML = `
            <div class="modal-content" style="max-width: 700px;">
                <div class="modal-header">
                    <h2><i class="fas fa-chart-pie"></i> ДЕТАЛЬНАЯ СТАТИСТИКА</h2>
                    <button class="modal-close">
                        <i class="fas fa-times"></i>
                    </button>
                </div>
                <div class="modal-body">
                    <div class="stats-grid">
                        <div class="stat-card">
                            <div class="stat-card-icon solved">
                                <i class="fas fa-check"></i>
                            </div>
                            <h3>Решено</h3>
                            <div class="stat-card-value">${solvedCount}</div>
                            <div class="stat-card-percent">${Math.round((solvedCount / totalCount) * 100)}%</div>
                        </div>
                        
                        <div class="stat-card">
                            <div class="stat-card-icon incorrect">
                                <i class="fas fa-times"></i>
                            </div>
                            <h3>С ошибками</h3>
                            <div class="stat-card-value">${incorrectCount}</div>
                            <div class="stat-card-percent">${Math.round((incorrectCount / totalCount) * 100)}%</div>
                        </div>
                        
                        <div class="stat-card">
                            <div class="stat-card-icon unsolved">
                                <i class="fas fa-question"></i>
                            </div>
                            <h3>Не решено</h3>
                            <div class="stat-card-value">${unsolvedCount}</div>
                            <div class="stat-card-percent">${Math.round((unsolvedCount / totalCount) * 100)}%</div>
                        </div>
                    </div>
                    
                    <div class="time-stats">
                        <h3>Распределение по сложности:</h3>
                        <div class="time-bars">
                            ${[1, 2, 3].map(difficulty => {
                                const count = this.problems.filter(p => p.difficulty === difficulty).length;
                                const solved = this.problems.filter(p => p.difficulty === difficulty && p.solved).length;
                                const percent = count > 0 ? Math.round((solved / count) * 100) : 0;
                                return `
                                    <div class="time-bar">
                                        <div class="time-label">
                                            <span>${this.getDifficultyText(difficulty)}</span>
                                            <span>${solved}/${count}</span>
                                        </div>
                                        <div class="time-progress">
                                            <div class="time-fill" style="width: ${percent}%"></div>
                                        </div>
                                        <div class="time-value">${percent}% успеха</div>
                                    </div>
                                `;
                            }).join('')}
                        </div>
                    </div>
                    
                    <div class="performance-stats">
                        <h3>Производительность:</h3>
                        <div class="performance-item">
                            <span>Общее время работы:</span>
                            <span>${Math.floor((Date.now() - this.startTime) / 60000)} минут</span>
                        </div>
                        <div class="performance-item">
                            <span>Среднее время на задачу:</span>
                            <span>${totalCount > 0 ? Math.floor((Date.now() - this.startTime) / 60000 / totalCount) : 0} минут</span>
                        </div>
                        <div class="performance-item">
                            <span>Скорость решения:</span>
                            <span>${Math.floor((solvedCount / ((Date.now() - this.startTime) / 60000)) * 60) || 0} задач/час</span>
                        </div>
                    </div>
                </div>
            </div>
        `;
        
        document.body.appendChild(modal);
        
        // Включаем скролл на фоновой странице
        document.body.style.overflow = 'hidden';
        
        setTimeout(() => {
            modal.style.display = 'flex';
            setTimeout(() => modal.classList.add('show'), 10);
        }, 10);
        
        // Обработчики закрытия
        const closeModal = () => {
            modal.classList.remove('show');
            setTimeout(() => {
                modal.remove();
                document.body.style.overflow = ''; // Восстанавливаем скролл
            }, 300);
        };
        
        modal.querySelector('.modal-close').addEventListener('click', closeModal);
        
        // Закрытие по клику вне модального окна
        modal.addEventListener('click', (e) => {
            if (e.target === modal) {
                closeModal();
            }
        });
        
        // Закрытие по Escape
        const handleEscape = (e) => {
            if (e.key === 'Escape') {
                closeModal();
                document.removeEventListener('keydown', handleEscape);
            }
        };
        document.addEventListener('keydown', handleEscape);
    }
    
    // Вспомогательные методы
    getTaskWord(num) {
        const lastDigit = num % 10;
        const lastTwoDigits = num % 100;
        
        if (lastDigit === 1 && lastTwoDigits !== 11) return 'задача';
        if (lastDigit >= 2 && lastDigit <= 4 && (lastTwoDigits < 10 || lastTwoDigits >= 20)) {
            return 'задачи';
        }
        return 'задач';
    }
    
    getDifficultyText(level) {
        return ['Лёгкая', 'Средняя', 'Сложная'][level - 1] || 'Средняя';
    }
    
    getTimeEstimate(level) {
        return `${level * 2 + 1} мин`;
    }
    
    getSuccessRate(level) {
        return [85, 70, 55][level - 1] || 70;
    }
    
    getPopularity(level) {
        return ['Высокая', 'Средняя', 'Низкая'][level - 1] || 'Средняя';
    }
    
    calculateGrade(accuracy) {
        if (accuracy >= 90) return '5';
        if (accuracy >= 75) return '4';
        if (accuracy >= 50) return '3';
        return '2';
    }
    
    showNotification(message, type) {
        // Создаем уведомление
        const notification = document.createElement('div');
        notification.className = `cosmic-notification ${type}`;
        
        const icons = {
            success: 'check-circle',
            error: 'exclamation-circle',
            info: 'info-circle',
            warning: 'exclamation-triangle'
        };
        
        notification.innerHTML = `
            <div class="notification-content">
                <i class="fas fa-${icons[type] || 'bell'}"></i>
                <span>${message}</span>
            </div>
        `;
        
        document.body.appendChild(notification);
        
        // Анимация появления
        setTimeout(() => notification.classList.add('show'), 10);
        
        // Удаление через 3 секунды
        setTimeout(() => {
            notification.classList.remove('show');
            setTimeout(() => notification.remove(), 300);
        }, 3000);
    }
    
    // Метод для загрузки задач из C++
    loadProblemsFromServer(problemsData) {
        this.problems = problemsData;
        this.renderProblems();
        this.showNotification(`Загружено ${problemsData.length} задач`, 'success');
    }
    
    // Метод для добавления одной задачи из C++
    addProblemFromServer(problemData) {
        this.problems.push(problemData);
        this.renderProblems();
    }
}

// Стили для уведомлений и анимаций
const style = document.createElement('style');
style.textContent = `
    .cosmic-notification {
        position: fixed;
        top: 20px;
        right: 20px;
        background: rgba(26, 26, 46, 0.95);
        border: 1px solid;
        border-radius: 10px;
        padding: 1rem 1.5rem;
        min-width: 300px;
        transform: translateX(120%);
        transition: transform 0.3s ease;
        z-index: 10000;
        backdrop-filter: blur(10px);
        box-shadow: 0 10px 30px rgba(0, 0, 0, 0.5);
    }
    
    .cosmic-notification.show {
        transform: translateX(0);
    }
    
    .cosmic-notification.success {
        border-color: #00ff88;
        border-left: 4px solid #00ff88;
    }
    
    .cosmic-notification.error {
        border-color: #ff4444;
        border-left: 4px solid #ff4444;
    }
    
    .cosmic-notification.info {
        border-color: var(--neon-blue);
        border-left: 4px solid var(--neon-blue);
    }
    
    .cosmic-notification.warning {
        border-color: var(--supernova);
        border-left: 4px solid var(--supernova);
    }
    
    .notification-content {
        display: flex;
        align-items: center;
        gap: 1rem;
        color: var(--star-white);
    }
    
    .notification-content i {
        font-size: 1.2rem;
    }
    
    .notification-content i.fa-check-circle {
        color: #00ff88;
    }
    
    .notification-content i.fa-exclamation-circle {
        color: #ff4444;
    }
    
    .notification-content i.fa-info-circle {
        color: var(--neon-blue);
    }
    
    .notification-content i.fa-exclamation-triangle {
        color: var(--supernova);
    }
    
    .cosmic-modal {
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background: rgba(10, 10, 15, 0.95);
        backdrop-filter: blur(10px);
        display: none;
        align-items: center;
        justify-content: center;
        z-index: 10000;
        opacity: 0;
        transition: opacity 0.3s ease;
        overflow-y: auto;
        padding: 20px;
    }
    
    .cosmic-modal.show {
        display: flex;
        opacity: 1;
    }
    
    .modal-content {
        background: rgba(26, 26, 46, 0.95);
        border: 1px solid var(--event-horizon);
        border-radius: 20px;
        padding: 2rem;
        width: 100%;
        max-width: 600px;
        max-height: 90vh;
        overflow-y: auto;
        box-shadow: var(--shadow-neon);
        position: relative;
        margin: auto;
    }
    
    .grade-display {
        text-align: center;
        margin: 2rem 0;
        padding: 2rem;
        background: linear-gradient(45deg, var(--cosmic-purple), var(--neon-blue));
        border-radius: 15px;
    }
    
    .grade-value {
        font-family: 'Orbitron', monospace;
        font-size: 4rem;
        font-weight: 900;
        color: var(--star-white);
        text-shadow: 0 0 20px rgba(0, 0, 0, 0.5);
    }
    
    .grade-label {
        font-size: 1.2rem;
        color: var(--star-white);
        text-transform: uppercase;
        letter-spacing: 2px;
        opacity: 0.9;
    }
    
    .performance-stats {
        margin-top: 2rem;
        padding: 1.5rem;
        background: rgba(26, 26, 46, 0.6);
        border-radius: 12px;
        border: 1px solid var(--event-horizon);
    }
    
    .performance-stats h3 {
        color: var(--neon-blue);
        margin-bottom: 1rem;
        font-family: 'Orbitron', monospace;
    }
    
    .performance-item {
        display: flex;
        justify-content: space-between;
        padding: 0.8rem 0;
        border-bottom: 1px solid rgba(255, 255, 255, 0.1);
    }
    
    .performance-item:last-child {
        border-bottom: none;
    }
    
    .performance-item span:first-child {
        color: var(--starlight);
    }
    
    .performance-item span:last-child {
        color: var(--quantum-teal);
        font-weight: 600;
    }
    
    .loading-animation {
        display: flex;
        justify-content: center;
        gap: 10px;
        margin-top: 20px;
    }
    
    .loading-dot {
        width: 12px;
        height: 12px;
        background: var(--neon-blue);
        border-radius: 50%;
        animation: loading-bounce 1.4s infinite ease-in-out both;
    }
    
    .loading-dot:nth-child(1) {
        animation-delay: -0.32s;
    }
    
    .loading-dot:nth-child(2) {
        animation-delay: -0.16s;
    }
    
    @keyframes loading-bounce {
        0%, 80%, 100% { 
            transform: scale(0);
        } 40% { 
            transform: scale(1.0);
        }
    }
    
    .nav-dot.empty {
        background: rgba(255, 255, 255, 0.1);
        border: 2px dashed var(--event-horizon);
    }
    
    .nav-dot.empty .nav-tooltip {
        background: rgba(26, 26, 46, 0.95);
        color: var(--starlight);
    }
`;
document.head.appendChild(style);

// Инициализация при загрузке страницы
document.addEventListener('DOMContentLoaded', () => {
    window.problemManager = new CosmicProblemManager();
    
    // Глобальные методы для интеграции с C++
    window.loadProblems = function(problemsData) {
        problemManager.loadProblemsFromServer(problemsData);
    };
    
    window.addProblem = function(problemData) {
        problemManager.addProblemFromServer(problemData);
    };
    
    window.clearProblems = function() {
        problemManager.problems = [];
        problemManager.renderProblems();
        problemManager.showNotification('Все задачи очищены', 'info');
    };
});