#include "TextDetection.hpp"
#include "TEXTRecognizer.h"
int main()
{
    //read the image
    Mat input = imread("digit1.jpg", 1);
    textDetection(input);
	imshow("image_show", input);
    waitKey(0);
    return 0;
}