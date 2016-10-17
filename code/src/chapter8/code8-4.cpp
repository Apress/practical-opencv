// Program to illustrate ORB keypoint and descriptor extraction, and matching using FLANN-LSH
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace cv;
using namespace std;

int main() {
    Mat train = imread("template.jpg"), train_g;
    cvtColor(train, train_g, CV_BGR2GRAY);

    //detect SIFT keypoints and extract descriptors in the train image
    vector<KeyPoint> train_kp;
    Mat train_desc;

    OrbFeatureDetector featureDetector;
    featureDetector.detect(train_g, train_kp);
    OrbDescriptorExtractor featureExtractor;
    featureExtractor.compute(train_g, train_kp, train_desc);

    cout << "Descriptor depth " << train_desc.depth() << endl;

    // FLANN based descriptor matcher object
    flann::Index flannIndex(train_desc, flann::LshIndexParams(12, 20, 2), cvflann::FLANN_DIST_HAMMING);

    // VideoCapture object
    VideoCapture cap(0);
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

    unsigned int frame_count = 0;

    while(char(waitKey(1)) != 'q') {
        double t0 = getTickCount();
        Mat test, test_g;
        cap >> test;
        if(test.empty())
            continue;

        cvtColor(test, test_g, CV_BGR2GRAY);

        //detect SIFT keypoints and extract descriptors in the test image
        vector<KeyPoint> test_kp;
        Mat test_desc;
        featureDetector.detect(test_g, test_kp);
        featureExtractor.compute(test_g, test_kp, test_desc);

        // match train and test descriptors, getting 2 nearest neighbors for all test descriptors
        Mat match_idx(test_desc.rows, 2, CV_32SC1), match_dist(test_desc.rows, 2, CV_32FC1);
        flannIndex.knnSearch(test_desc, match_idx, match_dist, 2, flann::SearchParams());

        // filter for good matches according to Lowe's algorithm
        vector<DMatch> good_matches;
        for(int i = 0; i < match_dist.rows; i++) {
            if(match_dist.at<float>(i, 0) < 0.6 * match_dist.at<float>(i, 1)) {
                DMatch dm(i, match_idx.at<int>(i, 0), match_dist.at<float>(i, 0));
                good_matches.push_back(dm);
            }
        }

        Mat img_show;
        drawMatches(test, test_kp, train, train_kp, good_matches, img_show);
        imshow("Matches", img_show);

        cout << "Frame rate = " << getTickFrequency() / (getTickCount() - t0) << endl;
    }

    return 0;
}
