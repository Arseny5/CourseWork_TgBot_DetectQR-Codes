# Telegram-бот на С++ для генерации и считывания QR-кодов

Данный Telegram-bot позволяет взаимодействовать с
штрих-кодами разных типов. Telegram-бот предназначен для генерации и детекции
матричных штриховых кодов без использования программ для обработки изображений и
наличия специальных навыков для этого. С помощью данного бота пользователь может
распознавать на картинке/скриншоте штрих коды, а также генерировать новые QR-коды
по определенному тексту/ссылке/номеру телефона в удобном Telegram-боте с user-friendly
интерфейсом. Результатом служит сгенерированный QR-код или информация с
распознанных на изображении штрих-кодов.

First             |  Second          | Third                    |    Fourth
:-------------------------:|:-------------------------:|:-------------------------:|:-------------------------:
![alt text](1.jpg "UI")  |  ![alt text](2.jpg "UI")  |  ![alt text](3.jpg "UI")  |  ![alt text](4.jpg "UI")

Для детекции бот может принимать на вход изображения с
расширением .jpeg .jpg .png .svg. Для генерации бот принимает на вход текст и
преобразует его в QR-код с расширением .jpeg в 4 размерах: 300x300, 40x40, 1080x1080,
1024x1024 – в пикселях.

<b>Детекция штрих-кодов:</b>

  При обнаружении знака бот отправит сообщение с расшифрованной
  информацией, а также напишет, какого конкретно типа данный штрих-код.
  Поддерживаются линейные и двумерные: UPC/EAN-128; EAN-13; UPC-E;
  Code39; UPC-A; EAN-8; «Interleaved 2of5»; QR-codes

<b>Генерация штрих-кода:</b>

Генерируются только QR-коды (поскольку это самый распространённый вид
штрих кодов и используется в 95% случаев) и происходит это по введенному
тексту пользователя. Во время компиляции проекта внутри директории
~/Documents/CourseWork/coursework-main/build создается буферное
изображение example1.jpeg, которое обновляется на новый QR-код при
каждом запросе на генерацию и после чего выводится пользователю.


