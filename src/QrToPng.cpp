#include "QrToPng.h"

QrToPng::QrToPng(std::string fileName, int imgSize, int minModulePixelSize, std::string text,
                 bool overwriteExistingFile, qrcodegen::QrCode::Ecc ecc) :
        _fileName(std::move(fileName)), _size(imgSize), _minModulePixelSize(minModulePixelSize), _text(std::move(text)),
        _overwriteExistingFile(overwriteExistingFile), _ecc(ecc) {
}

bool QrToPng::writeToPNG() {
    /* Наличие текста необходимо */
    if (_text.empty())
        return false;


    if (!_overwriteExistingFile and fs::exists(_fileName))
        return false;

    auto _qr = qrcodegen::QrCode::encodeText("", _ecc);
    try {
        _qr = qrcodegen::QrCode::encodeText(_text.c_str(), _ecc);
    }
    catch (const std::length_error &e) {
        std::cerr << "Failed to generate QR code, too much data. Decrease _ecc, enlarge size or give less text."
                  << std::endl;
        std::cerr << "e.what(): " << e.what() << std::endl;
        return false;
    }

    if (_overwriteExistingFile and fs::exists(_fileName))
        if (!fs::copy_file(_fileName, _fileName + ".tmp", fs::copy_options::overwrite_existing))
            return false;

    auto result = _writeToPNG(_qr);

    if (result)
        fs::remove(_fileName + ".tmp");

    return result;

}

bool QrToPng::_writeToPNG(const qrcodegen::QrCode &qrData) const {
    std::ofstream out(_fileName.c_str(), std::ios::binary);
    int pngWH = _imgSizeWithBorder(qrData);
    TinyPngOut pngout(pngWH, pngWH, out);

    auto qrSize = qrData.getSize();
    auto qrSizeWithBorder = qrData.getSize() + 2;
    if (qrSizeWithBorder > _size)
        return false; // qrcode doesn't fit

    int qrSizeFitsInMaxImgSizeTimes = _size / qrSizeWithBorder;
    int pixelsWHPerModule = qrSizeFitsInMaxImgSizeTimes;

    if (qrSizeFitsInMaxImgSizeTimes < _minModulePixelSize)
        return false; // image would be to small to scan

    std::vector<uint8_t> tmpData;
    const uint8_t blackPixel = 0x00;
    const uint8_t whitePixel = 0xFF;

    /* Приведенный ниже цикл преобразует qrData в пиксели RGB 8.8.8 и записывает их с помощью
     * библиотеки tinyPNGputput. Так как мы, вероятно, запросили больший
     * размер QR-кода в пикселях, мы должны преобразовать модули qrData, чтобы они имели больше
     * пикселей (чем просто 1x1). */

    // Верхняя граница
    for (int i = 0; i < pngWH; i++) // row
        for (int j = 0; j < pixelsWHPerModule; j++) // module pixel (height)
            tmpData.insert(tmpData.end(), {whitePixel, whitePixel, whitePixel});

    pngout.write(tmpData.data(), static_cast<size_t>(tmpData.size() / 3));
    tmpData.clear();

    for (int qrModuleAtY = 0; qrModuleAtY < qrSize; qrModuleAtY++) {
        for (int col = 0; col < pixelsWHPerModule; col++) {
            // Граница слева
            for (int i = 0; i < qrSizeFitsInMaxImgSizeTimes; ++i)
                tmpData.insert(tmpData.end(), {whitePixel, whitePixel, whitePixel});

            // QR модуль в пиксель
            for (int qrModuleAtX = 0; qrModuleAtX < (qrSize); qrModuleAtX++) {
            for (int row = 0; row < qrSizeFitsInMaxImgSizeTimes; ++row) {
                    if (qrData.getModule(qrModuleAtX, qrModuleAtY)) {
                        // insert saves us a for loop or 3 times the same line.
                        tmpData.insert(tmpData.end(), {blackPixel, blackPixel, blackPixel});
                    } else {
                        tmpData.insert(tmpData.end(), {whitePixel, whitePixel, whitePixel});
                    }
                }
            }
            // Граница справа
            for (int i = 0; i < qrSizeFitsInMaxImgSizeTimes; ++i)
                tmpData.insert(tmpData.end(), {whitePixel, whitePixel, whitePixel});

            // Запись всего ряда
            pngout.write(tmpData.data(), static_cast<size_t>(tmpData.size() / 3));
            tmpData.clear();
        }
    }

    // Нижняя граница
    for (int i = 0; i < pngWH; i++) // row
        for (int j = 0; j < pixelsWHPerModule; j++) // module pixel (высота)
            tmpData.insert(tmpData.end(), {whitePixel, whitePixel, whitePixel});

    pngout.write(tmpData.data(), static_cast<size_t>(tmpData.size() / 3));
    tmpData.clear();

    return fs::exists(_fileName);
}


uint32_t QrToPng::_imgSize(const qrcodegen::QrCode &qrData) const {
    return (_size / qrData.getSize()) * qrData.getSize();
}

uint32_t QrToPng::_imgSizeWithBorder(const qrcodegen::QrCode &qrData) const {
    return (_size / (qrData.getSize() + 2)) * (qrData.getSize() + 2);
}
