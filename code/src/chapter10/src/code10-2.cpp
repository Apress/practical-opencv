//Code to check the OpenCV installation on Raspberry Pi and mesaure frame rate
//Author: Samarth Manoj Brahmbhatt, University of Pennsyalvania

#include "cap.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main() {
    namedWindow("Hello");

    PiCapture cap(320, 240, false);

    Mat im;
    double time = 0;
    unsigned int frames = 0;
    while(char(waitKey(1)) != 'q') {
        double t0 = getTickCount();
        im = cap.grab();
        frames++;
        if(!im.empty()) imshow("Hello", im);
        else cout << "Frame dropped" << endl;

        time += (getTickCount() - t0) / getTickFrequency();
        cout << frames / time << " fps" << endl;
    }

    return 0;
}
