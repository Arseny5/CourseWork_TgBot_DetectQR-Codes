#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <tgbot/tgbot.h>
#include "curl/curl.h"
#include "QrToPng.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <zbar.h>

using namespace cv;
using namespace zbar;

ImageScanner scanner;
std::string token = "YOUR_PERSONAL_TOKEN"; //Персональный токен от Tg-bot


//-------------------РАСПОЗНОВАНИЕ QR-кодов НА ИЗОБРАЖЕНИИ-------------------
size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    std::vector<uchar> *stream = (std::vector<uchar>*)userdata;
    size_t count = size * nmemb;
    stream->insert(stream->end(), ptr, ptr + count);
    return count;
}

//Функция для получения изображения как типа данных cv::Mat
cv::Mat curlImg(const char *img_url, int timeout=10)
{
    std::vector<uchar> stream;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, img_url); //url изображения
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); // передать функцию записи
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream); // передать поток ptr в функцию записи
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout); // тайм-аут, если curl_easy зависает,
    CURLcode res = curl_easy_perform(curl); // старт curl
    curl_easy_cleanup(curl); // уборка
    return imdecode(stream, -1); // 'keep-as-is'
}

const SymbolSet getQR(Mat &frame) 
{
    // convert image to greyscale
    Mat grey;
    cvtColor(frame,grey,COLOR_BGR2GRAY);
    int width = frame.cols;
    int height = frame.rows;
    uchar *raw = (uchar *)grey.data;
    // wrap image data
    Image image(width, height, "Y800", raw, width * height);
    // scan the image for barcodes
    scanner.scan(image);
    return image.get_symbols();
}



//----------------------ГЕНЕРАЦИЯ QR-кодов----------------------
//Функция для генерации QR-кодов по введенному тексту пользователя. Аргументом в функцию QRGen() передается текст юзера. Генерация QR-кода происходит 
//с помощью функции QrToPng() вызываемой из заголовочного файла QrToPng.h. Любой ново-сгенерированный QR-код хранится в изображении example1.png.
//Это изображение все время обновляется. Бот работает в потоке для одного юзера.

void QRGen(std::string &_data){
    std::string qrText = _data;
    std::string fileName = "example1.png";

//  Вариант 1 QR-кода с размером 300x300
    int imgSize = 300;
    int minModulePixelSize = 3;
    auto exampleQrPng1 = QrToPng(fileName, imgSize, minModulePixelSize, qrText, true, qrcodegen::QrCode::Ecc::MEDIUM);
    //Специально для просмотра всех запросов поступающих от пользователей
    std::cout << "Writing Example QR code 1 (normal) to " << fileName << " with text: '" << qrText << "', size: " <<
              imgSize << "x" << imgSize << ", qr module pixel size: " << minModulePixelSize << ". " << std::endl;
    if (exampleQrPng1.writeToPNG())
        std::cout << "Success!" << std::endl;
    else
        std::cerr << "Failure..." << std::endl;

//  Вариант 2 QR-кода с размером 40x40
//  imgSize = 40;
//  minModulePixelSize = 1;
//  auto exampleQrPng2 = QrToPng(fileName, imgSize, minModulePixelSize, qrText, true, qrcodegen::QrCode::Ecc::LOW);

//  Вариант 3 QR-кода с размером 1080x1080
//     imgSize = 1080;
//     minModulePixelSize = 20;
//     auto exampleQrPng3 = QrToPng(fileName, imgSize, minModulePixelSize, qrText, true, qrcodegen::QrCode::Ecc::HIGH);

//  Вариант 4 QR-кода с размером 1024x1024
//     imgSize = 1024;
//     minModulePixelSize = 1;
//     auto exampleQrPng4 = QrToPng(fileName, imgSize, minModulePixelSize, qrText, true, qrcodegen::QrCode::Ecc::HIGH);
}

int main() {
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
    TgBot::Bot bot(token);
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Привет <b>" + message->chat->firstName + "</b> \n\n➟ Этот бот шифрует Ваши текстовые сообщения в QR-коды; \n➟ Бот берет ссылки и текст из изображений с QR-кодами; \n\nОн также работает в встроенном режиме)", false, 0, std::make_shared<TgBot::GenericReply>(), "html");
        //bot.getApi().sendMessage(message->chat->id, "Hello <b>" + message->chat->firstName + "</b> \n\n➟ This bot encrypts your text messages into a QR Code; \n➟ The bot takes links and texts from QR coded images; \n\nIt also works in inline mode)", false, 0, std::make_shared<TgBot::GenericReply>(), "html");
        bot.getApi().sendMessage(message->chat->id, "<b>Напиши текст или отправь изображение с QR-кодом:</b>", false, 0, std::make_shared<TgBot::GenericReply>(), "html");
        //bot.getApi().sendMessage(message->chat->id, "<b>Type text or send QR Coded picture:</b>", false, 0, std::make_shared<TgBot::GenericReply>(), "html");
    });

    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }     
        if (!message->photo.empty()) {
            auto file_id = message->photo[0]->fileId;
            auto file_info = bot.getApi().getFile(message->photo[message->photo.size() - 1]->fileId);
            // auto downloaded = bot.getApi().downloadFile(file_info->filePath);
            // std::cout << file_info->filePath << std::endl;
            // std::ofstream out("image.jpg", std::ios::binary);
            // out << downloaded;
            Mat img;
            std::cout << file_info->filePath << std::endl;
            img = curlImg(("https://api.telegram.org/file/bot" + token + "/" + file_info->filePath).c_str());
            const SymbolSet data = getQR(img);
            std::string reply_mes = "";
            for(SymbolIterator symbol = data.symbol_begin(); symbol != data.symbol_end(); ++symbol){
                // display results on standard output
                reply_mes += "✅ Распознан " + symbol->get_type_name() + ": \n"+ symbol->get_data() + "\n";
                // reply_mes += "Decoded " + symbol->get_type_name() + " "+ symbol->get_data() + "\n";
                // std::cout << "decoded " << symbol->get_type_name() << " symbol " << symbol->get_data() << '"' <<" "<< std::endl;
            }
            if(reply_mes.size()){
                bot.getApi().sendMessage(message->chat->id, reply_mes, false, message->messageId);
            }
            else{
                bot.getApi().sendMessage(message->chat->id, "❌ На этом фото нет QR-кодов", false, message->messageId);
            }
        } 

        else if(!message->text.empty()){
            if(message->text.size() < 500){
                QRGen(message->text);
                bot.getApi().sendPhoto(message->chat->id, TgBot::InputFile::fromFile("example1.png", "/Documents/CourseWork/coursework-main/build"), "✅ Готово к использованию)", message->messageId);
            }
            else{
                bot.getApi().sendMessage(message->chat->id, "❌ Сообщение слишком большое :(");
            }
        } 
        else{
            bot.getApi().sendMessage(message->chat->id, "Прости, я не понимаю :(");
        }     
    });
    
    
   

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (std::exception& e) {
        printf("error: %s\n", e.what());
    }
    

    return 0;
}
