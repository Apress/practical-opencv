// Program to automate the color-based object detector using floodfill
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

Mat frame_hsv, frame, mask;

int low_diff = 10, high_diff = 10, conn = 4, val = 255, flags = conn + (val << 8) + CV_FLOODFILL_MASK_ONLY;
double h_h = 0, l_h = 0, h_s = 0, l_s = 0;

bool selected = false;

void on_low_diff_trackbar(int, void *) {}

void on_high_diff_trackbar(int, void *) {}

void on_mouse(int event, int x, int y, int, void *) {
    if(event != EVENT_LBUTTONDOWN) return;

    selected = true;
    
    //seed point
    Point p(x, y);
    
    // make mask using floodFill
    mask = Scalar::all(0);
    floodFill(frame, mask, p, Scalar(255, 255, 255), 0, Scalar(low_diff, low_diff, low_diff), Scalar(high_diff, high_diff, high_diff), flags);
    
    // find the H and S range of piexels selected by floodFill
    Mat channels[3];
    split(frame_hsv, channels);
    minMaxLoc(channels[0], &l_h, &h_h, NULL, NULL, mask.rowRange(1, mask.rows-1).colRange(1, mask.cols-1));
    minMaxLoc(channels[1], &l_s, &h_s, NULL, NULL, mask.rowRange(1, mask.rows-1).colRange(1, mask.cols-1));
}

int main() {
    // Create a VideoCapture object to read from video file
    // 0 is the ID of the built-in laptop camera, change if you want to use other camera
    VideoCapture cap(0);
    
    //check if the file was opened properly
    if(!cap.isOpened()) {
        cout << "Capture could not be opened succesfully" << endl;
        return -1;
    }

    namedWindow("Video");
    namedWindow("Segmentation");

    createTrackbar("Low Diff", "Segmentation", &low_diff, 50, on_low_diff_trackbar);
    createTrackbar("High Diff ", "Segmentation", &high_diff, 50, on_high_diff_trackbar);

    setMouseCallback("Video", on_mouse);

    while(char(waitKey(1)) != 'q' && cap.isOpened()) {
        cap >> frame;
        if(!selected) mask.create(frame.rows+2, frame.cols+2, CV_8UC1);
        // Check if the video is over
        if(frame.empty()) {
            cout << "Video over" << endl;
            break;
        }
        cvtColor(frame, frame_hsv, CV_BGR2HSV);
        
        // extract the hue and saturation channels
        int from_to[] = {0,0, 1,1};
        Mat hs(frame.size(), CV_8UC2);
        mixChannels(&frame_hsv, 1, &hs, 1, from_to, 2);

        // check for the range of H and S obtained from floodFill
        Mat frame_thresholded;
        inRange(hs, Scalar(l_h, l_s), Scalar(h_h, h_s), frame_thresholded);

        // open and close to remove noise
        Mat str_el = getStructuringElement(MORPH_RECT, Size(5, 5));
        morphologyEx(frame_thresholded, frame_thresholded, MORPH_OPEN, str_el);
        morphologyEx(frame_thresholded, frame_thresholded, MORPH_CLOSE, str_el);
        
        imshow("Video", frame);
        imshow("Segmentation", frame_thresholded);
    }

    return 0;
}
