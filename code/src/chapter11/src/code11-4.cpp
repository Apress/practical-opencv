//Program to illustrate a simple perspective transform recovery by matching ORB features
//Author: Samarth Manoj Brahmbhatt, University of Pennsyalvania

#include <opencv2/opencv.hpp>
#include <opencv2/stitching/stitcher.hpp>
#include <opencv2/stitching/warpers.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "Config.h"

using namespace std;
using namespace cv;

class perspective_transformer {
    private:
        Mat im, im_transformed, im_perspective_transformed;
        vector<Point2f> points, points_transformed;
        Mat M;
    public:
        perspective_transformer();
        void estimate_perspective();
        void show_diff();
};

perspective_transformer::perspective_transformer() {
    im = imread(DATA_FOLDER_3 + string("/transformed-0.jpg"));
    im_transformed = imread(DATA_FOLDER_3 + string("/transformed-1.jpg"));
}

void perspective_transformer::estimate_perspective() {
    // Match ORB features to point correspondences between the images
    vector<KeyPoint> kp, t_kp;
    Mat desc, t_desc, im_g, t_im_g;

    cvtColor(im, im_g, CV_BGR2GRAY);
    cvtColor(im_transformed, t_im_g, CV_BGR2GRAY);

    OrbFeatureDetector featureDetector;
    OrbDescriptorExtractor featureExtractor;

    featureDetector.detect(im_g, kp);
    featureDetector.detect(t_im_g, t_kp);

    featureExtractor.compute(im_g, kp, desc);
    featureExtractor.compute(t_im_g, t_kp, t_desc);

    flann::Index flannIndex(desc, flann::LshIndexParams(12, 20, 2), cvflann::FLANN_DIST_HAMMING);
    Mat match_idx(t_desc.rows, 2, CV_32SC1), match_dist(t_desc.rows, 2, CV_32FC1);
    flannIndex.knnSearch(t_desc, match_idx, match_dist, 2, flann::SearchParams());

    vector<DMatch> good_matches;
    for(int i = 0; i < match_dist.rows; i++) {
        if(match_dist.at<float>(i, 0) < 0.6 * match_dist.at<float>(i, 1)) {
            DMatch dm(i, match_idx.at<int>(i, 0), match_dist.at<float>(i, 0));
            good_matches.push_back(dm);
            points.push_back((kp[dm.trainIdx]).pt);
            points_transformed.push_back((t_kp[dm.queryIdx]).pt);
        }
    }

    Mat im_show;
    drawMatches(im_transformed, t_kp, im, kp, good_matches, im_show, Scalar(0, 0, 255), Scalar(255, 0, 255));
    imshow("ORB matches", im_show);

    M = findHomography(points, points_transformed, CV_RANSAC, 2);
    cout << "Estimated Perspective transform = " << M << endl;

    warpPerspective(im, im_perspective_transformed, M, im_transformed.size());
    imshow("Estimated Perspective transform", im_perspective_transformed);

    cout << "original " << im_transformed.size() << endl;
    cout << "transformed " << im_perspective_transformed.size() << endl;
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
