/**
 * \file      QRcapture.h
 * \author    lue
 * \version   0.1
 * \date      13/02/2019
 * \brief     video and QR capture
 *
 * \details   capture video frame and QR code
 */
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <zbar.h>
using namespace cv;
using namespace zbar;

/**
 * \class QRcapture
 * \brief Class of QR capture
 *
 * It captures the frame and the related QRs
*/
class QRcapture:public VideoCapture
{
private:
    ImageScanner scanner;
public :
    QRcapture(char * videoFile):VideoCapture(videoFile) {}
    QRcapture(int dev):VideoCapture(dev) {}
    int init();
    const SymbolSet getQR(Mat &frame);
};
