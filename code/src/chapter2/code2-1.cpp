//Code to check the OpenCV installation
//Author: Samarth Manoj Brahmbhatt, University of Pennsyalvania

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

int main()
{
	Mat im = imread("image.jpg", CV_LOAD_IMAGE_COLOR);
	namedWindow("Hello");
	imshow("Hello", im);

  	cout << "Press 'q' or ESC to quit..." << endl;
  	int key;
  	while(1)
	{
		if(char(waitKey(1)) == 'q') break;
  	}
	destroyAllWindows();
	cout << "Got here" << endl;
 	return 0;
}
