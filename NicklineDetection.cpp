#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <cstdio>

#define long_max 120
#define lineLengthMax 200

int iLowH = 0;
int iHighH = 35;

int iLowS = 58;
int iHighS =174;

int iLowV = 0;
int iHighV = 255;


using namespace std;
using namespace cv;

Mat color(Mat& src) {
    cvtColor(src, src, CV_BGR2HSV);
    inRange(src, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), src);
    
    //cvtColor( src, src, cv::COLOR_BGR2YCrCb );
    //inRange( src, cv::Scalar(80, 135, 85), cv::Scalar(255, 180, 135), src );
    
    erode(src, src, Mat(), Point(-1, -1), 6);
    dilate(src, src, Mat(), Point(-1, -1), 6);
    return src;
}

bool color(double b){
    return b != 0;
}

int main(){
    VideoCapture cap(0);
    if (!cap.isOpened()) return -1;
    namedWindow("Original", WINDOW_AUTOSIZE);
    namedWindow("Skin", WINDOW_AUTOSIZE);
    namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"
    //Create trackbars in "Control" window
    cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
    cvCreateTrackbar("HighH", "Control", &iHighH, 179);
    cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
    cvCreateTrackbar("HighS", "Control", &iHighS, 255);
    cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
    cvCreateTrackbar("HighV", "Control", &iHighV, 255);
    
    int now_row = 87;
    int breath = 0;
    bool change = false;
    int row_max = 0;
    int row_min = 8787;
    
    for (;;) {
        Mat frame, skin;
        cap >> frame;
        flip(frame, frame, 1);
        frame.copyTo(skin);
        color(skin);
        
        for (int row = frame.rows - 2; row >= 0; row--){//NO FIRST
            for (int col = frame.cols - 2; col >= 0; col--){
                int count = 0;
                for (int k = col; k > col - long_max && col>long_max && k>0; k--) {
                    if (color(skin.ptr<uchar>(row, k)[0])){
                        if (!color(skin.ptr<uchar>(row + 1, col)[0])) break;
                        count++;
                    }
                    else break;
                }
                if (count == long_max) {
                    for (int lineLength = col; lineLength > col - lineLengthMax && col > lineLengthMax; lineLength--) {
                        for (int lineWidth = row; lineWidth > row - 5 && row > 5; lineWidth--) {
                            frame.ptr<uchar>(lineWidth, lineLength)[0] = 0;
                            frame.ptr<uchar>(lineWidth, lineLength)[1] = 255;
                            frame.ptr<uchar>(lineWidth, lineLength)[2] = 0;
                        }
                    }
                    //cout << now_row << " " << row << endl;
                    row_max = max(row_max, row);
                    row_min = min(row_min, row);
                    if(now_row!=row){
                        if(now_row>row?!change:change){
                            if (row_max - row_min > 5){
                                breath++;
                                cout << "呼吸次數: " << breath / 2 << endl;
                            }
                            row_max = 0;
                            row_min = 8787;
                            change = now_row>row?true:false;
                        }
                    }
                    else cout << "NO MOVE" << endl;
                    now_row = row;
                    goto imgshow;
                }
            }
        }
    imgshow:
        imshow("Original", frame);
        imshow("Skin", skin);
        if (waitKey(30) >= 0) break;
    }
    return 0;
}
