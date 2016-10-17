// Program to detect edges in an image using the thresholded Scharr operator
// Author: Samarth Manoj Brahmbhatt, University of Pennyslvania

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

Mat edges, edges_thresholded;
int slider = 50;

void on_slider(int, void *) {
    if(!edges.empty()) {
        Mat edges_thresholded;
        threshold(edges, edges_thresholded, slider, 255, THRESH_TOZERO);
        imshow("Thresholded Scharr edges", edges_thresholded);
    }
}

int main() {
    //Mat image = imread("lena.jpg"), image_blurred;
    Mat image = imread("lena.jpg"), image_blurred;

    // Blur image with a Gaussian kernel to remove edge noise
    GaussianBlur(image, image_blurred, Size(3, 3), 0, 0);

    // Convert to gray
    Mat image_gray;
    cvtColor(image_blurred, image_gray, CV_BGR2GRAY);

    // Gradients in X and Y directions
    Mat grad_x, grad_y;

    Scharr(image_gray, grad_x, CV_32F, 1, 0);
    Scharr(image_gray, grad_y, CV_32F, 0, 1);

    // Calculate overall gradient
    pow(grad_x, 2, grad_x);
    pow(grad_y, 2, grad_y);

    Mat grad = grad_x + grad_y;
    sqrt(grad, grad);

    // Display
    namedWindow("Original image");
    namedWindow("Thresholded Scharr edges");

    // Convert to 8 bit depth for displaying
    grad.convertTo(edges, CV_8U);
    threshold(edges, edges_thresholded, slider, 255, THRESH_TOZERO);

    imshow("Original image", image);
    imshow("Thresholded Scharr edges", edges_thresholded);

    createTrackbar("Threshold", "Thresholded Scharr edges", &slider, 255, on_slider);

    while(char(waitKey(1)) != 'q') {}

    return 0;
}
