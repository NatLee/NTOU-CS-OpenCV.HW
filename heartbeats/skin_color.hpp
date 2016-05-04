//
//  skin_color.hpp
//  opencvtest
//
//  Created by 張語航 on 2016/5/3.
//  Copyright © 2016年 張語航. All rights reserved.
//

#ifndef skin_color_hpp
#define skin_color_hpp
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;

Mat skincolor(const Mat&);
void CreateTrackbar();
#endif /* skin_color_hpp */
