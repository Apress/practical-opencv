// Program to interactvely blur an image using a Gaussian kernel of varying size
// Author: Samarth Manoj Brahmbhatt, University of Pennyslvania

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

Mat image, image_blurred;
int slider = 5;
float sigma = 0.3 * ((slider - 1) * 0.5 - 1) + 0.8;

void on_trackbar(int, void *) {
    int k_size = max(1, slider);
    k_size = k_size % 2 == 0 ? k_size + 1 : k_size;
    setTrackbarPos("Kernel Size", "Blurred image", k_size);
    sigma = 0.3 * ((k_size - 1) * 0.5 - 1) + 0.8;
    GaussianBlur(image, image_blurred, Size(k_size, k_size), sigma);
    imshow("Blurred image", image_blurred);
}

int main() {
    image = imread("baboon.jpg");
    
    namedWindow("Original image");
    namedWindow("Blurred image");

    imshow("Original image", image);
    sigma = 0.3 * ((slider - 1) * 0.5 - 1) + 0.8;
    GaussianBlur(image, image_blurred, Size(slider, slider), sigma);
    imshow("Blurred image", image_blurred);

    createTrackbar("Kernel Size", "Blurred image", &slider, 21, on_trackbar);

    while(char(waitKey(1) != 'q')) {}

    return 0;
}
