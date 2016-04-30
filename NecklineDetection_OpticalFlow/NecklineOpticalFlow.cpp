#include "opencv2/video/tracking.hpp"
#include "opencv2/videoio/videoio.hpp"

#include <iostream>
#include <ctype.h>
#include<algorithm>
#include "find_collar.hpp"
//#include "opticalflow.hpp"
using namespace std;

static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step, double, const Scalar& color,Rect temp)
{
    //cout << temp.br().y <<" "<< temp.br().x<< endl;
    for (int y = temp.tl().y; y < temp.br().y; y += step)
        for (int x = temp.tl().x; x < temp.br().x; x += step)
        {
            //cout << x << " " << y << endl;
            const Point2f& fxy = flow.at<Point2f>(y, x);
            line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)),color);
            circle(cflowmap, Point(x, y), 2, color, -1);
        }
}

Mat findMaxContours(Mat const skin, Mat frame,Rect &temp) {
    
    int largest_area = 0;
    int largest_contour_index = 0;
    Rect bounding_rect;
    
    //threshold(skin, skin, 25, 255, THRESH_BINARY); //Threshold the gray
    
    vector<vector<Point>> contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    findContours(skin, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
    
    for (int i = 0; i < contours.size(); i++) // iterate through each contour.
    {
        double a = contourArea(contours[i], false);  //  Find the area of contour
        if (a > largest_area) {
            largest_area = a;
            largest_contour_index = i;                //Store the index of largest contour
            bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
        }
        
    }
    Mat black(skin.rows, skin.cols, CV_8UC1, Scalar::all(0));//bulid black Mat
    
    Scalar color(255, 255, 255);
    // Draw the largest contour using previously stored index.
    drawContours(black, contours, largest_contour_index, color, CV_FILLED, 8, hierarchy);
    rectangle(frame, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);
    temp = bounding_rect;
    
    imshow("src", frame);
    imshow("largest Contour", black);
    return black;
}


int main() {
    VideoCapture cap(0);
    if (!cap.isOpened()) return -1;
    
    namedWindow("flow", 1);
    CreateTrackbar();
    Mat flow, cflow;
    UMat gray, prevgray, uflow;

    for (;;) {
        Mat frame, skin;
        cap >> frame;
        flip(frame, frame, 1);
        frame.copyTo(skin);
        //skincolor(skin);
        find_collar(skin);
        Rect temp;
        skin = findMaxContours(skin, frame, temp);
        frame.copyTo(gray);
        cvtColor(gray, gray, COLOR_BGRA2GRAY);
        if (!prevgray.empty())
        {
            calcOpticalFlowFarneback(prevgray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
            cvtColor(prevgray, cflow, COLOR_GRAY2BGR);
            uflow.copyTo(flow);
            drawOptFlowMap(flow, cflow, 8, 1.5, Scalar(0, 255, 0), temp);
            imshow("flow", cflow);
        }
        swap(prevgray, gray);
        
        if (waitKey(30) >= 0) break;
    }
    return 0;
}