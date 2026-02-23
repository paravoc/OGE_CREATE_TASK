// API клиент для работы с сервером
var API = {
    // Базовый URL
    baseURL: '',
    
    // GET запрос
    get: async function(endpoint) {
        try {
            var response = await fetch(this.baseURL + endpoint, {
                credentials: 'include',
                headers: {
                    'Accept': 'application/json'
                }
            });
            return await response.json();
        } catch (error) {
            console.error('API GET Error:', error);
            throw error;
        }
    },

    // POST запрос
    post: async function(endpoint, data) {
        try {
            var response = await fetch(this.baseURL + endpoint, {
                method: 'POST',
                credentials: 'include',
                headers: {
                    'Content-Type': 'application/json',
                    'Accept': 'application/json'
                },
                body: JSON.stringify(data)
            });
            return await response.json();
        } catch (error) {
            console.error('API POST Error:', error);
            throw error;
        }
    },

    // Получение данных пользователя
    getUser: function() {
        return this.get('/api/user/me');
    },

    // Получение вариантов пользователя
    getVariants: function(limit, offset) {
        limit = limit || 10;
        offset = offset || 0;
        return this.get('/api/user/variants?limit=' + limit + '&offset=' + offset);
    },

    // Получение конкретного варианта
    getVariant: function(id) {
        return this.get('/api/variant/' + id);
    },

    // Генерация нового варианта
    generateVariant: function(params) {
        return this.post('/api/generate', params);
    },

    // Проверка статуса генерации
    checkGeneration: function(taskId) {
        return this.get('/api/generation-status/' + taskId);
    },

    // Отметка о решении задания
    markTaskSolved: function(variantId, taskId, correct) {
        return this.post('/api/task-solved', {
            variant_id: variantId,
            task_id: taskId,
            correct: correct
        });
    },

    // Получение статистики
    getStats: function() {
        return this.get('/api/user/stats');
    },

    // Покупка кредитов
    buyCredits: function(amount) {
        return this.post('/api/buy-credits', { amount: amount });
    },

    // Обновление до премиум
    upgradeToPremium: function() {
        return this.post('/api/upgrade', {});
    },

    // Выход
    logout: function() {
        return this.get('/logout');
    },

    // Установка согласия на куки
    setCookieConsent: function(consent) {
        return this.post('/api/cookie-consent', { consent: consent });
    }
};

// Экспортируем для использования в других файлах
window.API = API;