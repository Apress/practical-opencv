// Program to display a video from attached default camera device
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	// Create a VideoCapture object to read from video file
	// 0 is the ID of the built-in laptop camera, change if you want to use other camera
	VideoCapture cap(0);
	
	//check if the file was opened properly
	if(!cap.isOpened())
	{
		cout << "Capture could not be opened succesfully" << endl;
		return -1;
	}

	namedWindow("Video");

	// Play the video in a loop till it ends
	while(char(waitKey(1)) != 'q' && cap.isOpened())
	{
		Mat frame;
		cap >> frame;
		// Check if the video is over
		if(frame.empty())
		{
			cout << "Video over" << endl;
			break;
		}
		imshow("Video", frame);
	}

	return 0;
}
