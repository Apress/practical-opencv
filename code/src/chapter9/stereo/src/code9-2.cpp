// Program to illustrate frame capture from a USB stereo camera
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main() {
    VideoCapture capr(1), capl(2);
    //reduce frame size
    capl.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    capl.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    capr.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    capr.set(CV_CAP_PROP_FRAME_WIDTH, 320);

    namedWindow("Left");
    namedWindow("Right");

    while(char(waitKey(1)) != 'q') {
        //grab raw frames first
        capl.grab();
        capr.grab();
        //decode later so the grabbed frames are less apart in time
        Mat framel, framer;
        capl.retrieve(framel);
        capr.retrieve(framer);

        if(framel.empty() || framer.empty()) break;

        imshow("Left", framel);
        imshow("Right", framer);
    }
    capl.release();
    capr.release();
    return 0;
}
