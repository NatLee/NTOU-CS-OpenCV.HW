#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <ctype.h>


int LY = 0;
int HY = 255;

int LCr = 137;
int HCr = 177;

int LCb = 77;
int HCb = 127;

using namespace cv;
using namespace std;

#define UNKNOWN_FLOW_THRESH 1e9

static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step,double, const Scalar& color)
{
    for (int y = 0; y < cflowmap.rows; y += step)
        for (int x = 0; x < cflowmap.cols; x += step)
        {
            const Point2f& fxy = flow.at<Point2f>(y, x);
            line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)),
                 color);
            circle(cflowmap, Point(x, y), 2, color, -1);
        }
}

Mat color(Mat& src) {
    //cvtColor(src, src, CV_BGR2HSV);
    //inRange(src, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), src);
    cvtColor(src, src, cv::COLOR_BGR2YCrCb);
    inRange(src, Scalar(LY, LCr, LCb), Scalar(HY, HCr, HCb), src);
    //inRange( src, Scalar(80, 135, 85), Scalar(255, 180, 135), src );
    
    erode(src, src, Mat(), Point(-1, -1), 6);
    dilate(src, src, Mat(), Point(-1, -1), 6);
    return src;
}

void contourFilter(Mat image)
{
    Mat temp;
    
    Mat flow, cflow;
    UMat gray, prevgray, uflow;
    
    RNG rng(12345);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(image, contours, hierarchy,
                 CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    //在threshold_img影像中尋找所有的輪廓
    
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    
    for (int i = 0; i < contours.size(); i++)
    {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        //計算可以包含輪廓的最小長方形區域
        
        boundRect[i] = boundingRect(Mat(contours_poly[i]));
        
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        //隨機給一個顏色
        
        if (boundRect[i].area() > 50000)
        {//長方形區域面積超過50000，則畫在影像上
            drawContours(image, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
            boundRect[i].y += 50;
            rectangle(image, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
            
            int x1= boundRect[i].tl().x, y1= boundRect[i].tl().y, x2= boundRect[i].br().x, y2= boundRect[i].br().y;
            if (boundRect[i].tl().x >= image.cols)x1 = image.cols - 2;
            if (boundRect[i].tl().y >= image.rows)y1 = image.rows - 2;
            if (boundRect[i].br().x >= image.cols)x2 = image.cols - 2;
            if (boundRect[i].br().y >= image.rows)y2 = image.rows - 2;
            
            Rect r(x1, y1, x2-x1, y2-y1);
            image(r).copyTo(temp);
            
            /*temp.copyTo(gray);
             
             if (!prevgray.empty())
             {
             calcOpticalFlowFarneback(prevgray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
             cvtColor(prevgray, cflow, COLOR_GRAY2BGR);
             uflow.copyTo(flow);
             drawOptFlowMap(flow, cflow, 16, 1.5, Scalar(0, 255, 0));
             imshow("flow", cflow);
             }
             
             std::swap(prevgray, gray);*/
            
        }
    }
    
    namedWindow("tmp", CV_WINDOW_AUTOSIZE);
    imshow("tmp", temp);
    
    /*namedWindow("coutour", CV_WINDOW_AUTOSIZE);
     imshow("coutour", image);*/
}
int main()
{
    VideoCapture cap;
    cap.open(0);
    
    Mat image;
    Mat flow, cflow;
    UMat gray, prevgray, uflow;
    namedWindow("flow", 1);
    
    for (;;)
    {
        Mat frame;
        cap >> frame;
        if (frame.empty())
            break;
        
        frame.copyTo(image);
        color(image);
        contourFilter(image);

        frame.copyTo(gray);
        cvtColor(gray, gray, COLOR_BGRA2GRAY);
        
        if (!prevgray.empty())
        {
            calcOpticalFlowFarneback(prevgray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
            cvtColor(prevgray, cflow, COLOR_GRAY2BGR);
            uflow.copyTo(flow);
            drawOptFlowMap(flow, cflow, 8, 1.5, Scalar(0, 255, 0));
            imshow("flow", cflow);
        }
        
        std::swap(prevgray, gray);
        
        if (waitKey(30) >= 0) break;
    }
    return 0;
}