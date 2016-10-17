// Program to display a video from a file
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania
// Video from: http://ftp.nluug.nl/ftp/graphics/blender/apricot/trailer/sintel_trailer-480p.mp4

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	// Create a VideoCapture object to read from video file
	VideoCapture cap("video.mp4");
	
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
