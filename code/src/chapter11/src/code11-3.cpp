//Program to illustrate a simple perspective transform
//Author: Samarth Manoj Brahmbhatt, University of Pennsyalvania

#include <opencv2/opencv.hpp>
#include <opencv2/stitching/stitcher.hpp>
#include <opencv2/stitching/warpers.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "Config.h"

using namespace std;
using namespace cv;

void on_mouse(int event, int x, int y, int, void* _p) {
    Point2f* p = (Point2f *)_p;
    if (event == CV_EVENT_LBUTTONUP) {
        p->x = x;
        p->y = y;
    }
}

class perspective_transformer {
    private:
        Mat im, im_transformed, im_perspective_transformed, im_show, im_transformed_show;
        vector<Point2f> points, points_transformed;
        Mat M;
        Point2f get_click(string, Mat);
    public:
        perspective_transformer();
        void estimate_perspective();
        void show_diff();
};

perspective_transformer::perspective_transformer() {
    im = imread(DATA_FOLDER_3 + string("/image.jpg"));
    im_transformed = imread(DATA_FOLDER_3 + string("/transformed.jpg"));
    cout << DATA_FOLDER_3 + string("/transformed.jpg") << endl;
}

Point2f perspective_transformer::get_click(string window_name, Mat im) {
    Point2f p(-1, -1);
    setMouseCallback(window_name, on_mouse, (void *)&p);
    while(p.x == -1 && p.y == -1) {
        imshow(window_name, im);
        waitKey(20);
    }
    return p;
}

void perspective_transformer::estimate_perspective() {
    imshow("Original", im);
    imshow("Transformed", im_transformed);

    cout << "To estimate the Perspective transform between the original and transformed images you will have to click on 8 matching pairs of points" << endl;
 
    im_show = im.clone();
    im_transformed_show = im_transformed.clone();
    Point2f p;
    for(int i = 0; i < 8; i++) {
        cout << "POINT " << i << endl;

        cout << "Click on a distinguished point in the ORIGINAL image" << endl;
        p = get_click("Original", im_show);
        cout << p << endl;
        points.push_back(p);
        circle(im_show, p, 2, Scalar(0, 0, 255), -1);
        imshow("Original", im_show);

        cout << "Click on a distinguished point in the TRANSFORMED image" << endl;
        p = get_click("Transformed", im_transformed_show);
        cout << p << endl;
        points_transformed.push_back(p);
        circle(im_transformed_show, p, 2, Scalar(0, 0, 255), -1);
        imshow("Transformed", im_transformed_show);
    }

    // Estimate perspective transform
    M = findHomography(points, points_transformed, CV_RANSAC, 2);
    cout << "Estimated Perspective transform = " << M << endl;

    // Apply estimated perspecive trasnform
    warpPerspective(im, im_perspective_transformed, M, im.size());
    imshow("Estimated Perspective transform", im_perspective_transformed);
}

void perspective_transformer::show_diff() {
    imshow("Difference", im_transformed - im_perspective_transformed);
}

int main() {
    perspective_transformer a;
    a.estimate_perspective();
    cout << "Press 'd' to show difference, 'q' to end" << endl;
    if(char(waitKey(-1)) == 'd') {
        a.show_diff();
        cout << "Press 'q' to end" << endl;
        if(char(waitKey(-1)) == 'q') return 0;
    }
    else
        return 0;
}
