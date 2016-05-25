//
//  TextDetection.cpp
//  Chapter11_AdvancedCameraControl
//
//  Created by 張語航 on 2016/5/24.
//  Copyright © 2016年 Alexander Shishkov & Kirill Kornyakov. All rights reserved.
//

#include "TextDetection.hpp"
long long int count_thres(Mat in1)
{
    long long int temp = 0;
    for (int y = 0; y < in1.rows; y++) {
        for (int x = 0; x < in1.cols; x++) {
            temp += in1.ptr<uchar>(y, x)[0];
        }
    }
    return temp / (in1.rows*in1.cols);
}

static Mat on_trackbar(int, void *,Mat &src,Mat &input)
{
    cvtColor(src, src, COLOR_BGR2GRAY);
    
    Mat labelImage(src.size(), CV_32S);
    int nLabels = connectedComponents(input, labelImage, 8);
    std::vector<Vec3b> colors(nLabels);
    colors[0] = Vec3b(0, 0, 0);//background
    for (int label = 1; label < nLabels; ++label) {
        colors[label] = Vec3b((255), (255), (255));
    }
    Mat dst(src.size(), CV_8UC3);//For connected component image
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {
            int label = labelImage.at<int>(r, c);
            Vec3b &pixel = dst.at<Vec3b>(r, c);
            pixel = colors[label];
        }
    }
    
    Mat gray;//For gray image to do the findcontours
    dst.copyTo(gray);
    cvtColor(gray, gray, COLOR_BGR2GRAY);
    threshold(gray, gray, 90, 255, THRESH_BINARY_INV);
    
    cv::Rect temp;
    vector<vector<cv::Point>> contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    findContours(gray, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the images
    
    for (int i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i], false);
        if (area >= 30 && area <= 1000) {//set the area, if bigger or lower don't take
            temp = boundingRect(contours[i]);
            rectangle(dst, temp, Scalar(255, 0, 0), 1, 8, 0);
        }
    }
    return dst;
}

void textDetection(Mat &input){
    Mat src;
    input.copyTo(src);
    cvtColor(input, input, COLOR_BGR2GRAY);
    
    long long int thres = 0;
    thres = count_thres(input);//do the average of the threshold
    threshold(input, input, thres, 255, THRESH_BINARY_INV);
    
    for (int y = 0; y < input.rows; y++)//clear noise
        for (int x = 0; x < input.cols; x++)
            if (input.ptr<uchar>(y, x)[0] == 255) {
                int count = 0;
                for (int i = y - 2; i <= y + 2 && i >= 0 && i < input.rows; i++) {
                    for (int j = x - 2; j <= x + 2 && j >= 0 && j < input.cols; j++) {
                        if (input.ptr<uchar>(i, j)[0] == 255)
                            count++;
                    }
                }
                if (count <= 4)
                    input.ptr<uchar>(y, x)[0] = 0;
            }
    
    Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
    dilate(input, input, element, cv::Point(-1, -1), 1);
    input=on_trackbar(100, 0, src, input);
}
