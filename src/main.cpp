#include <iostream>
#include <chrono>

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
    //camera.WaitForNewFrame();
    auto lastTime = chrono::high_resolution_clock::now();
    while(true) {
        imshow("My Window", camera.GetFrame().first);
        auto currentTime = chrono::high_resolution_clock::now();
        auto time = currentTime - lastTime;
        cout << "FPS: " << 1000000.0/std::chrono::duration_cast<std::chrono::microseconds>(time).count() << endl;
        lastTime = currentTime;
        if(waitKey(1) >= 0) break;
    }
    return 0;
}