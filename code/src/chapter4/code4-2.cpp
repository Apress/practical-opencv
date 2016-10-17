// Program to crop images using GUI mouse callbacks
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

// Global variables
// Flags updated according to left mouse button activity
bool ldown = false, lup = false;
// Original image
Mat img;
// Starting and ending points of the user's selection
Point corner1, corner2;
// ROI
Rect box;

// Callback function for mouse events
static void mouse_callback(int event, int x, int y, int, void *)
{
	// When the left mouse button is pressed, record its position and save it in corner1 
	if(event == EVENT_LBUTTONDOWN)
	{	
		ldown = true;
		corner1.x = x;
		corner1.y = y;
		cout << "Corner 1 recorded at " << corner1 << endl;
	}
	
	// When the left mouse button is released, record its position and save it in corner2 
	if(event == EVENT_LBUTTONUP)
	{	
		// Also check if user selection is bigger than 20 pixels (jut for fun!)
		if(abs(x - corner1.x) > 20 && abs(y - corner1.y) > 20)
		{
			lup = true;
			corner2.x = x;
			corner2.y = y;
			cout << "Corner 2 recorded at " << corner2 << endl << endl;
		}
		else
		{
			cout << "Please select a bigger region" << endl;
			ldown = false;
		}
	}

	// Update the box showing the selected region as the user drags the mouse
	if(ldown == true && lup == false)
	{
		Point pt;
		pt.x = x;
		pt.y = y;
		Mat local_img = img.clone();
		rectangle(local_img, corner1, pt, Scalar(0, 0, 255));
		imshow("Cropping app", local_img);
	}
	
	// Define ROI and crop it out when both corners have been selected	
	if(ldown == true && lup == true)
	{
		box.width = abs(corner1.x - corner2.x);
		box.height = abs(corner1.y - corner2.y);
		box.x = min(corner1.x, corner2.x);
		box.y = min(corner1.y, corner2.y);

		// Make an image out of just the selected ROI and display it in a new window
		Mat crop(img, box);
		namedWindow("Crop");
		imshow("Crop", crop);
		
		ldown = false;
		lup = false;
	}
}
	

int main()
{
	img = imread("image.jpg");

	namedWindow("Cropping app");
	imshow("Cropping app", img);

	// Set the mouse event callback function
	setMouseCallback("Cropping app", mouse_callback);

	// Exit by pressing 'q'
	while(char(waitKey(1)) != 'q') {}

	return 0;
}
