#include "TextDetection.hpp"
#include "TEXTRecognizer.h"
int main()
{
    //TEXTRecognizer textHandler;
    //textHandler.initialize();
    
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
    Mat input = imread("13285567_1183160171734202_1669539971_n.jpg", 1);

    //string str;
    //double confidence;
    //textHandler.charDecode( input, str, confidence);
    //cout << "Character = " << str << ", Confidence = " << confidence << std::endl;
    imshow("test", input);
    textDetection(input);
	imshow("image_show", input);
    waitKey(0);
    return 0;
}