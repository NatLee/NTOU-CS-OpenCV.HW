#include "TextDetection.hpp"
#include "TEXTRecognizer.h"
int main()
{
    TEXTRecognizer textHandler;
    textHandler.initialize();
    
//    //use the camera
//    VideoCapture cap(0);
//    if (!cap.isOpened()) return -1;
//    Mat input;
//    for (;;) {
//    	cap >> input;
//    	textDetection(input);
//    	imshow("text", input);
//    	if (waitKey(30) >= 0) break;
//    }
//    
    //read the image
    Mat input = imread("ada_threshold.jpg", 1);

    std::string str;
    double confidence;
    textHandler.charDecode( input, str, confidence);
    std::cout << "Character = " << str << ", Confidence = " << confidence << std::endl;
    imshow("test", input);
    //textDetection(input);
    waitKey(0);
    return 0;
}