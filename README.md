# Взлом казино на С++

Легковесный бот, предназначенный для накрутки VK Coin.
Принцип работы достаточно прост: бот сам разводит другого бота (Coin Math) на бабки, после чего владелец бота (вы)
можете выкачивать деньги


## Зависимости
Из зависимостей только tesseract (отвечает за обработку сообщений) и curl (работа с VK API)

## Further Development
Из целей на будущее:
* допилить поддержку 12 лвл (дроби).
    Файлы для дробей уже прописаны, осталось только прописать кадрирование картинки
* допилить поддержку уравнений (7-11 (?) лвл)
* сделать бинаризацию изображения, чтобы уменьшить количество ошибок
