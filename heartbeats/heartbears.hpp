//
//  heartbears.hpp
//  opencvtest
//
//  Created by 張語航 on 2016/5/9.
//  Copyright © 2016年 張語航. All rights reserved.
//

#ifndef heartbears_hpp
#define heartbears_hpp

#include "skin_color.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#define red_thres 0.07
double heartBeat(Mat&);
Mat findMaxContours(Mat skin, Mat frame, Rect &temp);
#endif /* heartbears_hpp */
