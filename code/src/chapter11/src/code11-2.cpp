//Program to illustrate a simple affine transform
//Author: Samarth Manoj Brahmbhatt, University of Pennsyalvania

#include <opencv2/opencv.hpp>
#include <opencv2/stitching/stitcher.hpp>
#include <opencv2/stitching/warpers.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Config.h"

using namespace std;
using namespace cv;

// Mouse callback function
void on_mouse(int event, int x, int y, int, void* _p) {
    Point2f* p = (Point2f *)_p;
    if (event == CV_EVENT_LBUTTONUP) {
        p->x = x;
        p->y = y;
    }
}

class affine_transformer {
    private:
        Mat im, im_transformed, im_affine_transformed, im_show, im_transformed_show;
        vector<Point2f> points, points_transformed; 
        Mat M; // Estimated Affine transformation matrix
        Point2f get_click(string, Mat);
    public:
        affine_transformer(); //constructor
        void estimate_affine();
        void show_diff();
};

affine_transformer::affine_transformer() {
    im = imread(DATA_FOLDER_2 + string("/image.jpg"));
    im_transformed = imread(DATA_FOLDER_2 + string("/transformed.jpg"));
}

// Function to get location clicked by user on a specific window
Point2f affine_transformer::get_click(string window_name, Mat im) {
    Point2f p(-1, -1);
    setMouseCallback(window_name, on_mouse, (void *)&p);
    while(p.x == -1 && p.y == -1) {
        imshow(window_name, im);
        waitKey(20);
    }
    return p;
}

void affine_transformer::estimate_affine() {
    imshow("Original", im);
    imshow("Transformed", im_transformed);

    cout << "To estimate the Affine transform between the original and transformed images you will have to click on 3 matching pairs of points" << endl;
 
    im_show = im.clone();
    im_transformed_show = im_transformed.clone();
    Point2f p;

    // Get 3 pairs of matching points from user
    for(int i = 0; i < 3; i++) {
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

    // Estimate Affine transform
    M = getAffineTransform(points, points_transformed);
    cout << "Estimated Affine transform = " << M << endl;

    // Apply estimates Affine transfrom to check its correctness
    warpAffine(im, im_affine_transformed, M, im.size());
    imshow("Estimated Affine transform", im_affine_transformed);
}

void affine_transformer::show_diff() {
    imshow("Difference", im_transformed - im_affine_transformed);
}

int main() {
    affine_transformer a;
    a.estimate_affine();
    cout << "Press 'd' to show difference, 'q' to end" << endl;
    if(char(waitKey(-1)) == 'd') {
        a.show_diff();
        cout << "Press 'q' to end" << endl;
        if(char(waitKey(-1)) == 'q') return 0;
    }
    else
        return 0;
}
