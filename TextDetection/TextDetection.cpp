#include "TextDetection.hpp"
#include "TEXTRecognizer.h"
#include <iomanip>
static Mat on_trackbar(int, void *, Mat &src, Mat &input)
{
    TEXTRecognizer textHandler;
    textHandler.initialize();
    double confidence = 0.0;
    string str;
    //cvtColor(src, src, COLOR_BGR2GRAY);
    
    //threshold(src, src, 125, 255, THRESH_BINARY_INV);
    imshow("binary", src);
    
    Mat labelImage(src.size(), CV_32S);
    int nLabels = connectedComponents(src, labelImage, 8);
    cout<<nLabels;
    std::vector<Vec3b> colors(nLabels);
    colors[0] = Vec3b(0, 0, 0);//background
    for (int label = 1; label < nLabels; ++label) {
        colors[label] = Vec3b((label*3+50), (label*3+50), (label*3+50));
    }
    Mat dst(src.size(), CV_8UC3);//For connected component image
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {
            int label = labelImage.at<int>(r, c);
            cout<<setfill(' ')<<setw(2)<<label;
            Vec3b &pixel = dst.at<Vec3b>(r, c);
            pixel = colors[label];
        }
        cout<<endl;
    }
    Mat gray;//For gray image to do the findcontours
    dst.copyTo(gray);
    cvtColor(gray, gray, COLOR_BGR2GRAY);
    
    //imshow("binary", gray);
    
    Rect temp;
    vector<vector<Point>> contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    findContours(gray, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the images
    string tstring="roia";
    for (int i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i], false);
        if (area >= 100 && area <= 2000) {//set the area,don't take if exceed the region
            
            temp = boundingRect(contours[i]);
            
            tstring[3]++;
            cout<<tstring;
            Mat roi  (input, temp);
            imshow(tstring, roi);
            textHandler.charDecode(roi, str, confidence);
            cout << "Character = " << str << ", Confidence = " << confidence << std::endl;
            rectangle(dst, temp, Scalar(255, 0, 0), 1, 8, 0);
        }
    }
    return dst;
}

void textDetection(Mat &input) {
    Mat src;
    input.copyTo(src);
    cvtColor(src, src, COLOR_BGR2GRAY);
    double maxVal = 255;
    adaptiveThreshold(src, src, maxVal, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2);
    
    Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
    erode(src, src, element, cv::Point(-1, -1), 1);
    dilate(src, src, element, cv::Point(-1, -1), 1);
    imshow("二值化", src);
    input = on_trackbar(100, 0, src, input);
}
