// Program illustrate disparity calculation from a calibrated stereo camera
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "Config.h"

using namespace cv;
using namespace std;

class disparity {
    private:
        Mat map_l1, map_l2, map_r1, map_r2; // rectification pixel maps
        StereoSGBM stereo; // stereo matching object for disparity computation
        int min_disp, num_disp; // parameters of StereoSGBM
    public:
        disparity(string, Size); //constructor
        void set_minDisp(int minDisp) { stereo.minDisparity = minDisp; }
        void set_numDisp(int numDisp) { stereo.numberOfDisparities = numDisp; }
        void show_disparity(Size); // show live disparity by processing stereo camera feed
};

// Callback functions for minDisparity and numberOfDisparities trackbars
void on_minDisp(int min_disp, void * _disp_obj) {
    disparity * disp_obj = (disparity *) _disp_obj;
    disp_obj -> set_minDisp(min_disp - 30);
}

void on_numDisp(int num_disp, void * _disp_obj) {
    disparity * disp_obj = (disparity *) _disp_obj;
    num_disp = (num_disp / 16) * 16;
    setTrackbarPos("numDisparity", "Disparity", num_disp);
    disp_obj -> set_numDisp(num_disp);
}

disparity::disparity(string filename, Size image_size) {
    // Read pixel maps from XML file
    FileStorage fs(filename, FileStorage::READ);
    fs["map_l1"] >> map_l1;
    fs["map_l2"] >> map_l2;
    fs["map_r1"] >> map_r1;
    fs["map_r2"] >> map_r2;

    if(map_l1.empty() || map_l2.empty() || map_r1.empty() || map_r2.empty())
        cout << "WARNING: Loading of mapping matrices not successful" << endl;

    // Set SGBM parameters (from OpenCV stereo_match.cpp demo)
    stereo.preFilterCap = 63;
    stereo.SADWindowSize = 3;
    stereo.P1 = 8 * 3 * stereo.SADWindowSize * stereo.SADWindowSize;
    stereo.P2 = 32 * 3 * stereo.SADWindowSize * stereo.SADWindowSize;
    stereo.uniquenessRatio = 10;
    stereo.speckleWindowSize = 100;
    stereo.speckleRange = 32;
    stereo.disp12MaxDiff = 1;
    stereo.fullDP = true;
}

void disparity::show_disparity(Size image_size) {
    VideoCapture capr(1), capl(2);
    //reduce frame size
    capl.set(CV_CAP_PROP_FRAME_HEIGHT, image_size.height);
    capl.set(CV_CAP_PROP_FRAME_WIDTH, image_size.width);
    capr.set(CV_CAP_PROP_FRAME_HEIGHT, image_size.height);
    capr.set(CV_CAP_PROP_FRAME_WIDTH, image_size.width);

    min_disp = 30;
    num_disp = ((image_size.width / 8) + 15) & -16;
    
    namedWindow("Disparity", CV_WINDOW_NORMAL);
    namedWindow("Left", CV_WINDOW_NORMAL);
    createTrackbar("minDisparity + 30", "Disparity", &min_disp, 60, on_minDisp, (void *)this);
    createTrackbar("numDisparity", "Disparity", &num_disp, 150, on_numDisp, (void *)this);

    on_minDisp(min_disp, this);
    on_numDisp(num_disp, this);

    while(char(waitKey(1)) != 'q') {
        //grab raw frames first
        capl.grab();
        capr.grab();
        //decode later so the grabbed frames are less apart in time
        Mat framel, framel_rect, framer, framer_rect;
        capl.retrieve(framel);
        capr.retrieve(framer);

        if(framel.empty() || framer.empty()) break;

        remap(framel, framel_rect, map_l1, map_l2, INTER_LINEAR);
        remap(framer, framer_rect, map_r1, map_r2, INTER_LINEAR);
        
        // Calculate disparity
        Mat disp, disp_show;
        stereo(framel_rect, framer_rect, disp);
        // Convert disparity to a form easy for visualization
        disp.convertTo(disp_show, CV_8U, 255/(stereo.numberOfDisparities * 16.));
        imshow("Disparity", disp_show);
        imshow("Left", framel);
    }
    capl.release();
    capr.release();
}

int main() {
    string filename = DATA_FOLDER + string("stereo_calib.xml");
   
    Size image_size(320, 240);
    disparity disp(filename, image_size);
    disp.show_disparity(image_size);

    return 0;
}
