#include "TextDetection.hpp"
#include "TEXTRecognizer.h"
int main()
{
    //read the image
    Mat input = imread("abc.jpg", 1);
	
    textDetection(input);
	namedWindow("image_show", WINDOW_AUTOSIZE);
	imshow("image_show", input);
    waitKey(0);
    return 0;
}