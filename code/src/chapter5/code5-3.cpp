// Program to examine erosion and dilation of images
// Author: Samarth Manoj Brahmbhatt, University of Pennyslvania

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

Mat image, image_processed;
int choice_slider = 0, size_slider = 5; // 0 - erode, 1 - dilate

void process() {
    Mat st_elem = getStructuringElement(MORPH_RECT, Size(size_slider, size_slider));

    if(choice_slider == 0) {    
        erode(image, image_processed, st_elem);
    }
    else {
        dilate(image, image_processed, st_elem);
    }
    imshow("Processed image", image_processed);
}

void on_choice_slider(int, void *) {
    process();
}

void on_size_slider(int, void *) {
    int size = max(1, size_slider);
    size = size % 2 == 0 ? size + 1 : size;
    setTrackbarPos("Kernel Size", "Processed image", size);
    process();
}

int main() {
    image = imread("j.png");

    namedWindow("Original image");
    namedWindow("Processed image");

    imshow("Original image", image);
    Mat st_elem = getStructuringElement(MORPH_RECT, Size(size_slider, size_slider));
    erode(image, image_processed, st_elem);
    imshow("Processed image", image_processed);

    createTrackbar("Erode/Dilate", "Processed image", &choice_slider, 1, on_choice_slider);
    createTrackbar("Kernel Size", "Processed image", &size_slider, 21, on_size_slider);

    while(char(waitKey(1) != 'q')) {}

    return 0;
}
