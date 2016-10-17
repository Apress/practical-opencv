// Program to collect stereo snapshots for calibration
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Config.h"
#include <iomanip>

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

    cout << "Press 'c' to capture ..." << endl;

    char choice = 'z';
    int count = 0;
    while(choice != 'q') {
        //grab frames quickly in succession
        capl.grab();
        capr.grab();
        //execute the heavier decoding operations
        Mat framel, framer;
        capl.retrieve(framel);
        capr.retrieve(framer);

        if(framel.empty() || framer.empty()) break;

        imshow("Left", framel);
        imshow("Right", framer);
        if(choice == 'c') {
            //save files at proper locations if user presses 'c'
            stringstream l_name, r_name;
            l_name << "left" << setw(4) << setfill('0') << count << ".jpg";
            r_name << "right" << setw(4) << setfill('0') << count << ".jpg";
            imwrite(string(LEFT_FOLDER) + l_name.str(), framel);
            imwrite(string(RIGHT_FOLDER) + r_name.str(), framer);
            cout << "Saved set " << count << endl;
            count++;
        }
        choice = char(waitKey(1));
    }
    capl.release();
    capr.release();
    return 0;
}
