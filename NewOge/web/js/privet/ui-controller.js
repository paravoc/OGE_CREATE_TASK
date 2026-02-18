// Элементы UI
let menu, leftContent, rightContent;

// Инициализация UI элементов
function initUI() {
    menu = document.getElementById('mainMenu');
    leftContent = document.querySelector('.left-content');
    rightContent = document.getElementById('tarifPanel');
}

// Функция активации меню и тарифа
function showElements(callback) {
    menu.classList.add('active');
    leftContent.classList.add('shrink');
    rightContent.classList.add('visible');
    
    if (callback) callback();
}

// Функция скрытия/показа для теста
function toggleElements() {
    menu.classList.toggle('active');
    leftContent.classList.toggle('shrink');
    rightContent.classList.toggle('visible');
}

// Получить элементы (геттеры)
function getUIElements() {
    return { menu, leftContent, rightContent };
}