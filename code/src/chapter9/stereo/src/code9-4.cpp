// Program illustrate stereo camera calibration
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <boost/filesystem.hpp>
#include "Config.h"

using namespace cv;
using namespace std;
using namespace boost::filesystem3;

class calibrator {
    private:
        string l_path, r_path; //path for folders containing left and right checkerboard images
        vector<Mat> l_images, r_images; //left and right checkerboard images
        Mat l_cameraMatrix, l_distCoeffs, r_cameraMatrix, r_distCoeffs; //Mats for holding individual camera calibration information
        bool show_chess_corners; //visualize checkerboard corner detections?
        float side_length; //side length of checkerboard squares
        int width, height; //number of internal corners in checkerboard along width and height
        vector<vector<Point2f> > l_image_points, r_image_points; //left and right image points
        vector<vector<Point3f> > object_points; //object points (grid)
        Mat R, T, E, F; //stereo calibration information

    public:
        calibrator(string, string, float, int, int); //constructor
        bool calibrate(); //function to calibrate stereo camera
        void calc_image_points(bool); //function to calculae image points by detecting checkerboard corners
};

calibrator::calibrator(string _l_path, string _r_path, float _side_length, int _width, int _height) {
    side_length = _side_length;
    width = _width;
    height = _height;

    l_path = _l_path;
    r_path = _r_path;

    // Read images    
    for(directory_iterator i(l_path), end_iter; i != end_iter; i++) {
        string im_name = i->path().filename().string();
        string l_filename = l_path + im_name;
        im_name.replace(im_name.begin(), im_name.begin() + 4, string("right"));
        string r_filename = r_path + im_name;
        Mat lim = imread(l_filename), rim = imread(r_filename);
        if(!lim.empty() && !rim.empty()) {
            l_images.push_back(lim);
            r_images.push_back(rim);
        }
    }
}

void calibrator::calc_image_points(bool show) {
    // Calculate the object points in the object co-ordinate system (origin at top left corner)
    vector<Point3f> ob_p;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            ob_p.push_back(Point3f(j * side_length, i * side_length, 0.f));
        }
    }

    if(show) {
        namedWindow("Left Chessboard corners");
        namedWindow("Right Chessboard corners");
    }

    for(int i = 0; i < l_images.size(); i++) {
        Mat lim = l_images[i], rim = r_images[i];
        vector<Point2f> l_im_p, r_im_p;
        bool l_pattern_found = findChessboardCorners(lim, Size(width, height), l_im_p, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE+ CALIB_CB_FAST_CHECK);
        bool r_pattern_found = findChessboardCorners(rim, Size(width, height), r_im_p, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE+ CALIB_CB_FAST_CHECK);
        if(l_pattern_found && r_pattern_found) {
            object_points.push_back(ob_p);
            Mat gray;
            cvtColor(lim, gray, CV_BGR2GRAY);
            cornerSubPix(gray, l_im_p, Size(5, 5), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
            cvtColor(rim, gray, CV_BGR2GRAY);
            cornerSubPix(gray, r_im_p, Size(5, 5), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
            l_image_points.push_back(l_im_p);
            r_image_points.push_back(r_im_p);
            if(show) {
                Mat im_show = lim.clone();
                drawChessboardCorners(im_show, Size(width, height), l_im_p, true);
                imshow("Left Chessboard corners", im_show);
                im_show = rim.clone();
                drawChessboardCorners(im_show, Size(width, height), r_im_p, true);
                imshow("Right Chessboard corners", im_show);
                while(char(waitKey(1)) != ' ') {}
            }
        }
        else {
            l_images.erase(l_images.begin() + i);
            r_images.erase(r_images.begin() + i);
        }
    }
}

bool calibrator::calibrate() {
    string filename = DATA_FOLDER + string("left_cam_calib.xml");
    FileStorage fs(filename, FileStorage::READ);
    fs["cameraMatrix"] >> l_cameraMatrix;
    fs["distCoeffs"] >> l_distCoeffs;
    fs.release();

    filename = DATA_FOLDER + string("right_cam_calib.xml");
    fs.open(filename, FileStorage::READ);
    fs["cameraMatrix"] >> r_cameraMatrix;
    fs["distCoeffs"] >> r_distCoeffs;
    fs.release();

    if(!l_cameraMatrix.empty() && !l_distCoeffs.empty() && !r_cameraMatrix.empty() && !r_distCoeffs.empty()) {
        double rms = stereoCalibrate(object_points, l_image_points, r_image_points, l_cameraMatrix, l_distCoeffs, r_cameraMatrix, r_distCoeffs, l_images[0].size(), R, T, E, F);
        cout << "Calibrated stereo camera with a RMS error of " << rms << endl;
        filename = DATA_FOLDER + string("stereo_calib.xml");
        fs.open(filename, FileStorage::WRITE);
        fs << "l_cameraMatrix" << l_cameraMatrix;
        fs << "r_cameraMatrix" << r_cameraMatrix;
        fs << "l_distCoeffs" << l_distCoeffs;
        fs << "r_distCoeffs" << r_distCoeffs;
        fs << "R" << R;
        fs << "T" << T;
        fs << "E" << E;
        fs << "F" << F;
        cout << "Calibration parameters saved to " << filename << endl;
        return true;
    }
    else return false;
}

int main() {
    calibrator calib(LEFT_FOLDER, RIGHT_FOLDER, 1.f, 5, 4);
    calib.calc_image_points(true);
    bool done = calib.calibrate();

    if(!done) cout << "Stereo Calibration not successful because individial calibration matrices could not be read" << endl;

    return 0;
}
