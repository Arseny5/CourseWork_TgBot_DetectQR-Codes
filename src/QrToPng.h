#ifndef QR_TO_PNG_H
#define QR_TO_PNG_H

#if defined(__GNUC__) && __GNUC__ < 9
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#include "QrCode.hpp"
#include "TinyPngOut.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

class QrToPng {
public:
    /**
     * Создает объект, содержащий все данные для создания QR-кода. Когда вызывается @writeToPNG(),
     * данный файл создается и записывается.
     * Изображение масштабируется, чтобы максимально соответствовать заданному размеру относительно размера QR-кода
     * @param fileName - относительное или абсолютное имя файла для записи изображения. 
     * @param imgSize - Высота и ширина изображения. Изображение квадратное, поэтому будет ширина и высота.
     * @param minModulePixelSize - Сколько пикселей должен быть QR-code (белая или черная пиксель)
     * @param text - Текст для кодирования в QR-код.
     * @param overwriteExistingFile - Перезаписывает, если файл с @fileName уже существует
     * @param исправление ошибок ecc (low,mid,high).
     */
    QrToPng(std::string fileName, int imgSize, int minModulePixelSize, std::string text,
            bool overwriteExistingFile, qrcodegen::QrCode::Ecc ecc);

    /** Записывает объект QrToPng в файл png по адресу @_fileName.
     *  @return true, если файл может быть записан, и false, если файл не может быть записан */
    bool writeToPNG();

private:
    std::string _fileName;
    int _size;
    int _minModulePixelSize;
    std::string _text;
    bool _overwriteExistingFile;
    qrcodegen::QrCode::Ecc _ecc;

    /** Записывает файл PNG. Создает вектор с
     * каждым элементом представляющим собой строку пикселей RGB 8.8.8.
     * Формат ориентирован на библиотеку tinypngoutput.
     * @param qrData код, возвращаемый библиотекой qrcodegen
     * @return true, если файл может быть записан, и false, если файл не может быть записан */
    [[nodiscard]] bool _writeToPNG(const qrcodegen::QrCode &qrData) const;

    /* Возвращает ширину/высоту изображения на основе максимального размера изображения
    * и ширины QR-кода. Если максимальный размер изображения равен 90, размер QR-кода равен 29.
    * Размер пикселя QR-кода будет 3, размер изображения будет 3*29=87. */
    [[nodiscard]] uint32_t _imgSize(const qrcodegen::QrCode &qrData) const;

    [[nodiscard]] uint32_t _imgSizeWithBorder(const qrcodegen::QrCode &qrData) const;
};

#endif //QR_TO_PNG_H
