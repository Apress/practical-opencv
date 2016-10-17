// Program to find the largest ellipse using RANSAC
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <Eigen/Dense>
#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include <math.h>

#define PI 3.14159265

using namespace std;
using namespace cv;
using namespace Eigen;

// Class to hold RANSAC parameters
class RANSACparams {
    private:
        //number of iterations 
        int iter;
        //minimum number of inliers to further process a model
        int min_inliers;
        //distance threshold to be conted as inlier
        float dist_thresh;
        //number of points to select randomly at each iteration
        int N;
    public:
        RANSACparams(int _iter, int _min_inliers, float _dist_thresh, int _N) { //constructor
            iter = _iter;
            min_inliers = _min_inliers;
            dist_thresh = _dist_thresh;
            N = _N;
        }

        int get_iter() {return iter;}
        int get_min_inliers() {return min_inliers;}
        float get_dist_thresh() {return dist_thresh;}
        int get_N() {return N;}
};

// Class that deals with fitting an ellipse, RANSAC and drawing the ellipse in the image
class ellipseFinder {
    private:
        Mat img; // input image
        vector<vector<Point> > contours; // contours in image
        Mat Q; // Matrix representing conic section of detected ellipse
        Mat fit_ellipse(vector<Point>); // function to fit ellipse to a contour
        Mat RANSACellipse(vector<vector<Point> >); // function to find ellipse in contours using RANSAC
        bool is_good_ellipse(Mat); // function that determines whether given conic section represents a valid ellipse
        vector<vector<Point> > choose_random(vector<Point>); //function to choose points at random from contour
        vector<float> distance(Mat, vector<Point>); //function to return distance of points from the ellipse
        float distance(Mat, Point); //overloaded function to return signed distance of point from ellipse 
        void draw_ellipse(Mat); //function to draw ellipse in an image
        vector<Point> ellipse_contour(Mat); //function to convert equation of ellipse to a contour of points 
        void draw_inliers(Mat, vector<Point>); //function to debug inliers

        // RANSAC parameters
        int iter, min_inliers, N;
        float dist_thresh;

    public:
        ellipseFinder(Mat _img, int l_canny, int h_canny, RANSACparams rp) { // constructor
            img = _img.clone();
            Mat img_b;
            GaussianBlur(img, img_b, Size(7, 7), 0);

            // Edge detection and contour extraction
            Mat edges; Canny(img_b, edges, l_canny, h_canny);
            vector<vector<Point> > c;
            findContours(edges, c, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
            // Remove small spurious short contours
            for(int i = 0; i < c.size(); i++) {
                bool is_closed = false;
                
                vector<Point> _c = c[i];
                
                Point p1 = _c.front(), p2 = _c.back();
                float d = sqrt((p1.x - p2.x)^2 + (p1.y - p2.y)^2);
                if(d <= 0.5) is_closed = true;

                d = arcLength(_c, is_closed);

                if(d > 50) contours.push_back(_c);
            }

            iter = rp.get_iter();
            min_inliers = rp.get_min_inliers();
            N = rp.get_N();
            dist_thresh = rp.get_dist_thresh();

            Q = Mat::eye(6, 1, CV_32F);

            /*
            //for debug
            Mat img_show = img.clone();
            drawContours(img_show, contours, -1, Scalar(0, 0, 255));
            imshow("Contours", img_show);
            //imshow("Edges", edges);
            */
            cout << "No. of Contours = " << contours.size() << endl;
        }

        void detect_ellipse(); //final wrapper function
        void debug(); //debug function
};

vector<float> ellipseFinder::distance(Mat Q, vector<Point> c) {
    vector<Point> ellipse = ellipse_contour(Q);
    vector<float> distances;
    for(int i = 0; i < c.size(); i++) {
        distances.push_back(float(pointPolygonTest(ellipse, c[i], true)));
    }

    return distances;
}

float ellipseFinder::distance(Mat Q, Point p) {
    vector<Point> ellipse = ellipse_contour(Q);
    return float(pointPolygonTest(ellipse, p, true));
}

vector<vector<Point> > ellipseFinder::choose_random(vector<Point> c) {
    vector<vector<Point> > cr;
    vector<Point> cr0, cr1;
    // Randomly shuffle all elements of contour 
    std::random_shuffle(c.begin(), c.end());
    // Put the first N elements into cr[0] as consensus set (see Wikipedia RANSAC algorithm) and
    // the rest in cr[1] to check for inliers
    for(int i = 0; i < c.size(); i++) {
        if(i < N) cr0.push_back(c[i]);
        else cr1.push_back(c[i]);
    }
    cr.push_back(cr0);
    cr.push_back(cr1);

    return cr;    
}

Mat ellipseFinder::fit_ellipse(vector<Point> c) {
    /*
    // for debug
    Mat img_show = img.clone();
    vector<vector<Point> > cr;
    cr.push_back(c);
    drawContours(img_show, cr, -1, Scalar(0, 0, 255), 2);
    imshow("Debug fitEllipse", img_show);
    */
    int N = c.size();
    
    Mat D;
    for(int i = 0; i < N; i++) {
        Point p = c[i];
        Mat r(1, 6, CV_32FC1);
        r = (Mat_<float>(1, 6) << (p.x)*(p.x), (p.x)*(p.y), (p.y)*(p.y), p.x, p.y, 1.f);
        D.push_back(r);
    }
    Mat S = D.t() * D, _S;

    double d = invert(S, _S);
    //if(d < 0.001) cout << "S matrix is singular" << endl;

    Mat C = Mat::zeros(6, 6, CV_32F);
    C.at<float>(2, 0) = 2;
    C.at<float>(1, 1) = -1;
    C.at<float>(0, 2) = 2;

    // Using EIGEN to calculate eigenvalues and eigenvectors
    Mat prod = _S * C;
    Eigen::MatrixXd prod_e;
    cv2eigen(prod, prod_e);
    EigenSolver<Eigen::MatrixXd> es(prod_e);

    Mat evec, eval, vec(6, 6, CV_32FC1), val(6, 1, CV_32FC1);

    eigen2cv(es.eigenvectors(), evec);
    eigen2cv(es.eigenvalues(), eval);

    evec.convertTo(evec, CV_32F);
    eval.convertTo(eval, CV_32F);

    // Eigen returns complex parts in the second channel (which are all 0 here) so select just the first channel
    int from_to[] = {0, 0};
    mixChannels(&evec, 1, &vec, 1, from_to, 1);
    mixChannels(&eval, 1, &val, 1, from_to, 1);

    Point maxLoc;
    minMaxLoc(val, NULL, NULL, NULL, &maxLoc);
        
    return vec.col(maxLoc.y);
}

bool ellipseFinder::is_good_ellipse(Mat Q) {
    float a = Q.at<float>(0, 0),
          b = (Q.at<float>(1, 0))/2,
          c = Q.at<float>(2, 0),
          d = (Q.at<float>(3, 0))/2,
          f = (Q.at<float>(4, 0))/2,
          g = Q.at<float>(5, 0);

    if(b*b - a*c == 0) return false;

    float thresh = 0.09,
        num = 2 * (a*f*f + c*d*d + g*b*b - 2*b*d*f - a*c*g),
        den1 = (b*b - a*c) * (sqrt((a-c)*(a-c) + 4*b*b) - (a + c)),
        den2 = (b*b - a*c) * (-sqrt((a-c)*(a-c) + 4*b*b) - (a + c)),
        a_len = sqrt(num / den1),
        b_len = sqrt(num / den2),
        major_axis = max(a_len, b_len),
        minor_axis = min(a_len, b_len);

    if(minor_axis < thresh*major_axis || num/den1 < 0.f || num/den2 < 0.f || major_axis > max(img.rows, img.cols)) return false;
    else return true;
}

Mat ellipseFinder::RANSACellipse(vector<vector<Point> > contours) {
    int best_overall_inlier_score = 0;
    Mat Q_best = 777 * Mat::ones(6, 1, CV_32FC1);
    int idx_best = -1;

    //for each contour...
    for(int i = 0; i < contours.size(); i++) {
        vector<Point> c = contours[i];
        if(c.size() < min_inliers) continue;
        
        Mat Q;
        int best_inlier_score = 0;
        for(int j = 0; j < iter; j++) {
            // ...choose points at random...
            vector<vector<Point> > cr = choose_random(c);
            vector<Point> consensus_set = cr[0], rest = cr[1];
            // ...fit ellipse to those points...
            Mat Q_maybe = fit_ellipse(consensus_set);
            // ...check for inliers...
            vector<float> d = distance(Q_maybe, rest);
            for(int k = 0; k < d.size(); k++)
                if(abs(d[k]) < dist_thresh) consensus_set.push_back(rest[k]);
            // ...and find the random set with the most number of inliers
            if(consensus_set.size() > min_inliers && consensus_set.size() > best_inlier_score) {
                Q = fit_ellipse(consensus_set);
                best_inlier_score = consensus_set.size();
            }
        }
        // find cotour with ellipse that has the most number of inliers
        if(best_inlier_score > best_overall_inlier_score && is_good_ellipse(Q)) {
            best_overall_inlier_score = best_inlier_score;
            Q_best = Q.clone();
            if(Q_best.at<float>(5, 0) < 0) Q_best *= -1.f;
            idx_best = i;
        }
    }
    
    /*
    //for debug
    Mat img_show = img.clone();
    drawContours(img_show, contours, idx_best, Scalar(0, 0, 255), 2);
    imshow("Best Contour", img_show);

    cout << "inliers " << best_overall_inlier_score << endl;
    */
    if(idx_best >= 0) draw_inliers(Q_best, contours[idx_best]);
    return Q_best;
}

vector<Point> ellipseFinder::ellipse_contour(Mat Q) {
    float a = Q.at<float>(0, 0),
          b = (Q.at<float>(1, 0))/2,
          c = Q.at<float>(2, 0),
          d = (Q.at<float>(3, 0))/2,
          f = (Q.at<float>(4, 0))/2,
          g = Q.at<float>(5, 0);

    vector<Point> ellipse;
    if(b*b - a*c == 0) {
        ellipse.push_back(Point(0, 0));
        return ellipse;
    }

    Point2f center((c*d - b*f)/(b*b - a*c), (a*f - b*d)/(b*b - a*c));

    float num = 2 * (a*f*f + c*d*d + g*b*b - 2*b*d*f - a*c*g),
        den1 = (b*b - a*c) * (sqrt((a-c)*(a-c) + 4*b*b) - (a + c)),
        den2 = (b*b - a*c) * (-sqrt((a-c)*(a-c) + 4*b*b) - (a + c)),
        a_len = sqrt(num / den1),
        b_len = sqrt(num / den2),
        major_axis = max(a_len, b_len),
        minor_axis = min(a_len, b_len);
    
    //angle of rotation of ellipse
    float alpha = 0.f;
    if(b == 0.f && a == c) alpha = PI/2;
    else if(b != 0.f && a > c) alpha = 0.5 * atan2(2*b, a-c);
    else if(b != 0.f && a < c) alpha = PI/2 - 0.5 * atan2(2*b, a-c);

    // 'draw' the ellipse and put it into a STL Point vector so you can use drawContours()
    int N = 200;
    float theta = 0.f;
    for(int i = 0; i < N; i++, theta += 2*PI/N) {
        float x = center.x + major_axis*cos(theta)*cos(alpha) + minor_axis*sin(theta)*sin(alpha);
        float y = center.y - major_axis*cos(theta)*sin(alpha) + minor_axis*sin(theta)*cos(alpha);
        Point p(x, y);
        if(x < img.cols && y < img.rows) ellipse.push_back(p);
    }
    if(ellipse.size() == 0) ellipse.push_back(Point(0, 0));

    return ellipse;
}

void ellipseFinder::detect_ellipse() {
    Q = RANSACellipse(contours);
    cout << "Q" << Q << endl;
    draw_ellipse(Q);
}

void ellipseFinder::debug() {
    int i = 1; //index of contour you want to debug
    cout << "No. of points in contour " << contours[i].size() << endl;
    Mat a = fit_ellipse(contours[i]);
    Mat img_show = img.clone();
    drawContours(img_show, contours, i, Scalar(0, 0, 255), 3);
    imshow("Debug contour", img_show);
    draw_inliers(a, contours[i]);
    draw_ellipse(a);
}

void ellipseFinder::draw_ellipse(Mat Q) {
    vector<Point> ellipse = ellipse_contour(Q);
    vector<vector<Point> > c;
    c.push_back(ellipse);
    Mat img_show = img.clone();
    drawContours(img_show, c, -1, Scalar(0, 0, 255), 3);
    imshow("Ellipse", img_show);
}

void ellipseFinder::draw_inliers(Mat Q, vector<Point> c) {
    vector<Point> ellipse = ellipse_contour(Q);
    vector<vector<Point> > cs;
    cs.push_back(ellipse);
    Mat img_show = img.clone();
    // draw all contours in thin red 
    drawContours(img_show, contours, -1, Scalar(0, 0, 255));
    // draw ellipse in thin blue
    drawContours(img_show, cs, 0, Scalar(255, 0, 0), 3);
    int count = 0;
    // draw inliers as green points
    for(int i = 0; i < c.size(); i++) {
        double d = pointPolygonTest(ellipse, c[i], true);
        float d1 = float(d);
        if(abs(d1) < dist_thresh) {
            circle(img_show, c[i], 3, Scalar(0, 255, 0), -1);
            count ++;
        }
    }
    imshow("Debug inliers", img_show);
    cout << "inliers " << count << endl;
}
    
int main() {
    Mat img = imread("eye.jpg");
    namedWindow("Ellipse");

    // object holding RANSAC parameters, initialized using the constructor
    RANSACparams rp(400, 100, 1, 5);

    // Canny thresholds
    int canny_l = 50, canny_h = 150;
    // Ellipse finder object, initialized using the constructor
    ellipseFinder ef(img, canny_l, canny_h, rp);
    ef.detect_ellipse();
    //ef.debug();

    while(char(waitKey(1)) != 'q') {}

    return 0;
}
