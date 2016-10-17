// Program to change between color and grayscale representations of an image using a GUI trackbar
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

// Global variables
// Maximum slider value
const int slider_max = 1;
// Constantly updated slider value
int slider;
// Original image
Mat img;

// Callback function for trackbar event
void on_trackbar(int pos, void *)
{
	// Holds the image processed acording to value of slider
	Mat img_converted;
	// Convert color-spaces according to value of slider
	if(pos > 0) cvtColor(img, img_converted, CV_BGR2GRAY);
	else img_converted = img;

	imshow("Trackbar app", img_converted);
}

int main()
{
	img = imread("image.jpg");

	namedWindow("Trackbar app");
	imshow("Trackbar app", img);

	// Initial value of slider
	slider = 0;

	// Create the trackbar
	createTrackbar("RGB <-> Grayscale", "Trackbar app", &slider, slider_max, on_trackbar);
	
	// Press 'q' to exit
	while(char(waitKey(1)) != 'q') {}

	return 0;
}
