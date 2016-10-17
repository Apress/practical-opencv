//Code to create a panorama from a collection of images
//Author: Samarth Manoj Brahmbhatt, University of Pennsyalvania

#include <opencv2/opencv.hpp>
#include <opencv2/stitching/stitcher.hpp>
#include <opencv2/stitching/warpers.hpp>
#include "Config.h"
#include <boost/filesystem.hpp>

using namespace std;
using namespace cv;
using namespace boost::filesystem;

int main() {
    vector<Mat> images;

    // Read images 
    for(directory_iterator i(DATA_FOLDER_5), end_iter; i != end_iter; i++) {
        string im_name = i->path().filename().string();
        string filename = string(DATA_FOLDER_5) + im_name;
        Mat im = imread(filename);
        if(!im.empty()) 
            images.push_back(im);
    }

    cout << "Read " << images.size() << " images" << endl << "Now making panorama..." << endl;

    Mat panorama;

    Stitcher stitcher = Stitcher::createDefault();
    stitcher.stitch(images, panorama);

    namedWindow("Panorama", CV_WINDOW_NORMAL);
    imshow("Panorama", panorama);

    while(char(waitKey(1)) != 'q') {}

    return 0;
}
