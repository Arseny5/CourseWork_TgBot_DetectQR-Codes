#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <tgbot/tgbot.h>

//детект куар кодов с помощью опенсв
//генерирование куар кодов на введенное слово/видео/линку


int max_size = 20480; // 20 Mbytes
int max_duration = 30; // 30 seconds
std::string token = "5030969867:AAH3jiMXDnHmj6Twa9uym_yGONjUbleR2Q0";

int main() {
    TgBot::Bot bot(token);
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi " + message->chat->firstName + "!");
    });

    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);


        if (!message->photo.empty()) {
                auto file_id = message->photo[0]->fileId;
                auto file_info = bot.getApi().getFile(message->photo[message->photo.size() - 1]->fileId);
                auto downloaded = bot.getApi().downloadFile(file_info->filePath);

                std::ofstream out("image.jpg", std::ios::binary);
                out << downloaded;
            }
    });

    /*
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
    */

    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
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