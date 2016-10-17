// Program to illustrate contour extraction and heirarchy of contours
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

Mat img;
vector<vector<Point> > contours;
vector<Vec4i> heirarchy;
int levels = 0;

void on_trackbar(int, void *) {
    Mat img_show = img.clone();
    drawContours(img_show, contours, -1, Scalar(0, 0, 255), 3, 8, heirarchy, levels);
    imshow("Contours", img_show);
}

int main() {
    img = imread("bullseye.jpg");

    Mat edges;
    Canny(img, edges, 50, 100);

    findContours(edges, contours, heirarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

    namedWindow("Contours", CV_WINDOW_AUTOSIZE);

    createTrackbar("Levels", "Contours", &levels, 15, on_trackbar);

    on_trackbar(0, 0);

    while(char(waitKey(1)) != 'q') {}

    return 0;
}
