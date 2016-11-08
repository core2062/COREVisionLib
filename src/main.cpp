#include <iostream>

#include "opencv2/opencv.hpp"
#include "CORECapture.h"

using namespace std;
using namespace cv;
using namespace CORE;

int main ( int argc, char **argv )
{
    CORECapture camera(0, 640, 480);
    cvNamedWindow( "My Window", CV_WINDOW_AUTOSIZE);
    //camera.SetCrop(0,0,100,100);
    while(true) {
        imshow("My Window", camera.GetFrame().first);
        if(waitKey(30) >= 0) break;
    }
    return 0;
}