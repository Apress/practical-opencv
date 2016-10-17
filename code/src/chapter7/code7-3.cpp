// Program to count the number of objects using morphology operations and the watershed transform
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

class objectCounter {
    private:
        Mat image, gray, markers, output;
        int count;
    public:
        objectCounter(Mat); //constructor
        void get_markers(); //function to get markers for watershed segmentation
        int count_objects(); //function to implement watershed segmentation and count catchment basins
};

objectCounter::objectCounter(Mat _image) {
    image = _image.clone();
    cvtColor(image, gray, CV_BGR2GRAY);
    imshow("image", image);
}

void objectCounter::get_markers() {
    // equalize histogram of image to improve contrast
    Mat im_e; equalizeHist(gray, im_e);
    //imshow("im_e", im_e);

    // dilate to remove small black spots
    Mat strel = getStructuringElement(MORPH_ELLIPSE, Size(9, 9));
    Mat im_d; dilate(im_e, im_d, strel);
    //imshow("im_d", im_d);

    // open and close to highlight objects
    strel = getStructuringElement(MORPH_ELLIPSE, Size(19, 19));
    Mat im_oc; morphologyEx(im_d, im_oc, MORPH_OPEN, strel);
    morphologyEx(im_oc, im_oc, MORPH_CLOSE, strel);
    //imshow("im_oc", im_oc);

    // adaptive threshold to create binary image
    Mat th_a; adaptiveThreshold(im_oc, th_a, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 105, 0);
    //imshow("th_a", th_a);

    // erode binary image twice to separate regions
    Mat th_e; erode(th_a, th_e, strel, Point(-1, -1), 2);
    //imshow("th_e", th_e);

    vector<vector<Point> > c, contours;
    vector<Vec4i> heirarchy;
    findContours(th_e, c, heirarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

    // remove very small contours
    for(int idx = 0; idx >= 0; idx = heirarchy[idx][0])
        if(contourArea(c[idx]) > 20) contours.push_back(c[idx]);        

    cout << "Extracted " << contours.size() << " contours" << endl;

    count = contours.size();
    markers.create(image.rows, image.cols, CV_32SC1);
    for(int idx = 0; idx < contours.size(); idx++)
        drawContours(markers, contours, idx, Scalar::all(idx + 1), -1, 8);
}

int objectCounter::count_objects() {
    watershed(image, markers);

    // colors generated randomly to make the output look pretty
    vector<Vec3b> colorTab;
    for(int i = 0; i < count; i++) {
        int b = theRNG().uniform(0, 255);
        int g = theRNG().uniform(0, 255);
        int r = theRNG().uniform(0, 255);

        colorTab.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }

    // watershed output image
    Mat wshed(markers.size(), CV_8UC3);

    // paint the watershed output image
    for(int i = 0; i < markers.rows; i++)
        for(int j = 0; j < markers.cols; j++) {
            int index = markers.at<int>(i, j);
            if(index == -1)
                wshed.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
            else if(index <= 0 || index > count)
                wshed.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            else
                wshed.at<Vec3b>(i, j) = colorTab[index - 1];
        }

    // superimpose the watershed image with 50% transparence on the grayscale original image
    Mat imgGray; cvtColor(gray, imgGray, CV_GRAY2BGR); 
    wshed = wshed*0.5 + imgGray*0.5;
    imshow("Segmentation", wshed);

    return count;
}

int main() {
    Mat im = imread("fruit.jpg");
    
    objectCounter oc(im);
    oc.get_markers();

    int count = oc.count_objects();

    cout << "Counted " << count << " fruits." << endl;

    while(char(waitKey(1)) != 'q') {}

    return 0;
}
