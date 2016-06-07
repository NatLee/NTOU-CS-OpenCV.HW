#include "TextDetection.hpp"
#include "TEXTRecognizer.h"
#include "opencv2/opencv.hpp"
int main()
{
    VideoCapture cap(0);
    if (!cap.isOpened()) return -1;
    for (;;) {
        Mat frame, input;
        cap >> frame;
        //flip(frame, frame, 1);
        frame.copyTo(input);
        imshow("image_show", input);
        if ((waitKey(1)) !=-1) {
            textDetection(input);
            imshow("image_show", input);
            waitKey(0);
            destroyAllWindows();
        }
    }
    return 0;
}

