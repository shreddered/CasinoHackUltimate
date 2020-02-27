# Взлом казино на С++

Легковесный бот, предназначенный для накрутки VK Coin.
Принцип работы достаточно прост: бот сам разводит другого бота ([Coin Math](https://vk.com/coinmath)) на бабки, после чего владелец бота (вы)
можете выкачивать деньги. Эдакий игрушечный майнинг.

Почему взлом казино? Хороший вопрос. Поскольку примеры на картинках, вероятно, были созданы против ботоводов,
то можно назвать это взломом. Казино? Coin Math иногда лагает и отвечает полную чушь, то есть рандом присутствует.


## Зависимости

Из зависимостей только tesseract (отвечает за обработку сообщений), 
leptonica (зависимость tesseract) и curl (работа с VK API).


## Запуск

*В первую очередь*, необходимо начать играть с ботом Coin Math. 
Можно не доходить до шестого уровня, но в таком случае бота потребуется пересобирать для каждого уровня.

Для сборки необходимо наличие утилиты _make_.
_Makefile_ следует отредактировать: указать, где расположены _tesseract_, _leptonica_ и _curl_
(в случае, если они не в стандартных директориях).
```bash
$ make
$ ./casino_hack "token"
```


## Further Development

Из целей на будущее:
* допилить поддержку 12 лвл (дроби).
    Файлы для дробей уже прописаны, осталось только прописать кадрирование картинки;
* допилить поддержку уравнений (7-11 (?) лвл);
* сделать бинаризацию изображения, чтобы уменьшить количество ошибок.
* допилить поддержку 12 лвл (дроби). Скоро будет реализовано!
* сделать поддержку уравнений (7-11 (?) лвл);
* сделать бинаризацию изображения, чтобы уменьшить количество ошибок.