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


//детект куар кодов с помощью опенсв
//генерирование куар кодов на введенное слово/видео/линку

ImageScanner scanner;
int max_size = 20480; // 20 Mbytes
int max_duration = 30; // 30 seconds
std::string token = "5030969867:AAH3jiMXDnHmj6Twa9uym_yGONjUbleR2Q0";




size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    std::vector<uchar> *stream = (std::vector<uchar>*)userdata;
    size_t count = size * nmemb;
    stream->insert(stream->end(), ptr, ptr + count);
    return count;
}

//function to retrieve the image as cv::Mat data type
cv::Mat curlImg(const char *img_url, int timeout=10)
{
    std::vector<uchar> stream;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, img_url); //the img url
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); // pass the writefunction
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream); // pass the stream ptr to the writefunction
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout); // timeout if curl_easy hangs, 
    CURLcode res = curl_easy_perform(curl); // start curl
    curl_easy_cleanup(curl); // cleanup
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

void QRGen(std::string &_data){
    std::string qrText = _data;
    std::string fileName = "example1.png";

    int imgSize = 300;
    int minModulePixelSize = 3;
    auto exampleQrPng1 = QrToPng(fileName, imgSize, minModulePixelSize, qrText, true, qrcodegen::QrCode::Ecc::MEDIUM);

    std::cout << "Writing Example QR code 1 (normal) to " << fileName << " with text: '" << qrText << "', size: " <<
              imgSize << "x" << imgSize << ", qr module pixel size: " << minModulePixelSize << ". " << std::endl;
    if (exampleQrPng1.writeToPNG())
        std::cout << "Success!" << std::endl;
    else
        std::cerr << "Failure..." << std::endl;
}

int main() {
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
    // getQR();
    TgBot::Bot bot(token);
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Привет <b>" + message->chat->firstName + "</b> \n\n➟ Этот бот шифрует Ваши текстовые сообщения в QR-коды; \n➟ Бот берет ссылки и текст из изображений с QR-кодами; \n\nОн также работает в встроенном режиме)", false, 0, std::make_shared<TgBot::GenericReply>(), "html");
        //bot.getApi().sendMessage(message->chat->id, "Hello <b>" + message->chat->firstName + "</b> \n\n➟ This bot encrypts your text messages into a QR Code; \n➟ The bot takes links and texts from QR coded images; \n\nIt also works in inline mode)", false, 0, std::make_shared<TgBot::GenericReply>(), "html");
        //bot.getApi().sendMessage(message->chat->id, "<b>Type text or send QR Coded picture:</b>", false, 0, std::make_shared<TgBot::GenericReply>(), "html");
        bot.getApi().sendMessage(message->chat->id, "<b>Напиши текст или отправь изображение с QR-кодом:</b>", false, 0, std::make_shared<TgBot::GenericReply>(), "html");
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

    /*
    Отправка видео файлов

    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {

            if (!message->photo.empty()) {
                auto file_id = message->photo[0]->fileId;
                auto file_info = bot.getApi().getFile(message->photo[message->photo.size() - 1]->fileId);
                auto downloaded = bot.getApi().downloadFile(file_info->filePath);

                std::ofstream out(message->photo[0]->fileId + ".jpg", std::ios::binary);
                out << downloaded;
            }

            if (message->video) {
                auto file_info = bot.getApi().getFile(message->video->fileId);
                std::string file_id = message->video->fileId;
                float file_size = file_info->fileSize / 1024.0;
                int32_t duration = message->video->duration;
                std::string file_name = file_info->filePath;
                bool should_save = (file_size <= max_size) && (duration <= max_duration);
                printf("\x1B[34mVideo:\x1B[33m\n\tfile id: %s\n\tsize: %f Bytes\n\tduration: %d sec\n\tfile path: %s\n\tshould_download: %s\x1B[0m\n", file_id.c_str(), file_size, duration, file_name.c_str(), (should_save ? "true" : "false"));
                if (should_save) {
                    std::ofstream out(file_id + ".mp4", std::ios::binary);
                    auto downloaded = bot.getApi().downloadFile(file_info->filePath);
                    out << downloaded;
                } else {
                    bot.getApi().sendMessage(message->chat->id, "Too long video ...");
                }
            }
            else if (message->replyToMessage) {
                if (message->replyToMessage->video) {
                    std::string file_id = message->replyToMessage->video->fileId;
                    printf("\x1B[34mReply:\n\t\x1B[33mtext: %s\n\tfile id: %s\x1B[0m\n", message->text.c_str(), file_id.c_str());
                }
            }
        });

    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
    });
    */


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