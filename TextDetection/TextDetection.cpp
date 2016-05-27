#include "TextDetection.hpp"
#include "TEXTRecognizer.h"
#include <iomanip>
static Mat on_trackbar(int, void *, Mat &src, Mat &input)
{
    TEXTRecognizer textHandler;
    textHandler.initialize();
    double confidence = 0.0;
    string str;
    imshow("binary", src);
    
    Mat labelImage(src.size(), CV_32SC1);
    int nLabels = connectedComponents(src, labelImage, 8);
    cout<<nLabels;

    //Mat dst(src.size(), CV_8UC3);//For connected component image
    for (int r = 0; r < labelImage.rows; ++r) {
        for (int c = 0; c < labelImage.cols; ++c) {
            int label = labelImage.at<int>(r, c);
            //cout<<setfill(' ')<<setw(3)<<label;
        }
        cout<<endl;
    }
    Mat gray;//For gray image to do the findcontours
    labelImage.copyTo(gray);
    //cvtColor(gray, gray, COLOR_BGR2GRAY);
    
    //imshow("binary", gray);
    
    Rect temp;
    vector<vector<Point>> contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    findContours(src, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); // Find the contours in the images
    string tstring="roia";
    for (int i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i], false);
        //if (area >= 200 && area <= 3000) {//set the area,don't take if exceed the region
            
            temp = boundingRect(contours[i]);
            
            tstring[3]++;
            cout<<tstring;
            Mat roi  (input, temp);
            imshow(tstring, roi);
            textHandler.charDecode(roi, str, confidence);
            cout << "Character = " << str << ", Confidence = " << confidence << std::endl;
            rectangle(src, temp, Scalar(255, 0, 0), 1, 8, 0);
        //}
    }
    return src;
}

void textDetection(Mat &input) {
    Mat src;
    input.copyTo(src);
    cvtColor(src, src, COLOR_BGR2GRAY);
    double maxVal = 255;
    adaptiveThreshold(src, src, maxVal, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2);
    
    Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
    //erode(src, src, element, cv::Point(-1, -1), 2);
    //dilate(src, src, element, cv::Point(-1, -1), 1);
    input = on_trackbar(100, 0, src, input);
}
