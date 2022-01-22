/**
 * \file      QRcapture.cpp
 * \author    lue
 * \version   0.1
 * \date      13/02/2019
 * \brief     video and QR capture
 *
 * \details   methods for capturing QR code
 */
#include <iostream>
#include "QRcapture.h"
using namespace std;

/**
 * \brief   initialize QRcature
 * \details check capture device and configure scan
 * \return  0:init ok - 1:init nok
 */
int QRcapture::init() 
{
    // check capture device is open
    if (!isOpened())
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }

    // configure the scanner
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
    return 0;
}

/**
 * \brief   capture QR
 * \details capture and analyze frame and recover all QR inside
 * \param   frame the image captured
 */
const SymbolSet QRcapture::getQR(Mat &frame) 
{
    // get the frame from video
    bool bSuccess = read(frame);
    if (!bSuccess)
    {
        cout << "Cannot read a frame from video stream" << endl;
        return 0;
    }
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
