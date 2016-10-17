// Program to display a video from attached default camera device and detect colored blobs using simple R G and B thresholding
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include "cap.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

Mat frame, frame_thresholded;
int rgb_slider = 0, low_slider = 30, high_slider = 100;
int low_r = 30, low_g = 30, low_b = 30, high_r = 100, high_g = 100, high_b = 100;

void on_rgb_trackbar(int, void *) {
    switch(rgb_slider) {
        case 0:
            setTrackbarPos("Low threshold", "Segmentation", low_r);
            setTrackbarPos("High threshold", "Segmentation", high_r);
            break;
        case 1:
            setTrackbarPos("Low threshold", "Segmentation", low_g);
            setTrackbarPos("High threshold", "Segmentation", high_g);
            break;
        case 2:
            setTrackbarPos("Low threshold", "Segmentation", low_b);
            setTrackbarPos("High threshold", "Segmentation", high_b);
            break;
    }
}


void on_low_thresh_trackbar(int, void *) {
    switch(rgb_slider) {
        case 0:
            low_r = min(high_slider - 1, low_slider);
            setTrackbarPos("Low threshold", "Segmentation", low_r);
            break;
        case 1:
            low_g = min(high_slider - 1, low_slider);
            setTrackbarPos("Low threshold", "Segmentation", low_g);
            break;
        case 2:
            low_b = min(high_slider - 1, low_slider);
            setTrackbarPos("Low threshold", "Segmentation", low_b);
            break;
    }
}

void on_high_thresh_trackbar(int, void *) {
    switch(rgb_slider) {
        case 0:
            high_r = max(low_slider + 1, high_slider);
            setTrackbarPos("High threshold", "Segmentation", high_r);
            break;
        case 1:
            high_g = max(low_slider + 1, high_slider);
            setTrackbarPos("High threshold", "Segmentation", high_g);
            break;
        case 2:
            high_b = max(low_slider + 1, high_slider);
            setTrackbarPos("High threshold", "Segmentation", high_b);
            break;
    }
}

int main()
{
	// Create a VideoCapture object to read from video file
	// 0 is the ID of the built-in laptop camera, change if you want to use other camera
	PiCapture cap(320, 240, true);

	namedWindow("Video");
	namedWindow("Segmentation");

        createTrackbar("0. R\n1. G\n2.B", "Segmentation", &rgb_slider, 2, on_rgb_trackbar);
        createTrackbar("Low threshold", "Segmentation", &low_slider, 255, on_low_thresh_trackbar);
        createTrackbar("High threshold", "Segmentation", &high_slider, 255, on_high_thresh_trackbar);

        double time = 0;
        unsigned int frames = 0;
	while(char(waitKey(1)) != 'q')
	{
                frames++;
                double t0 = getTickCount();
		frame = cap.grab();
		// Check if the video is over
		if(frame.empty())
		{
			cout << "Frame dropped" << endl;
			continue;
		}
                
                inRange(frame, Scalar(low_b, low_g, low_r), Scalar(high_b, high_g, high_r), frame_thresholded);
		
                imshow("Video", frame);
                imshow("Segmentation", frame_thresholded);
                time += (getTickCount() - t0) / getTickFrequency();
                cout << frames / time << " fps" << endl;
	}

	return 0;
}
