// Program to illustrate BOW object categorization
// Author: Samarth Manoj Brahmbhatt, University of Pennsylvania

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/ml/ml.hpp>
#include <boost/filesystem.hpp>
#include "Config.h"

using namespace cv;
using namespace std;
using namespace boost::filesystem3;

class categorizer {
    private:
        map<string, Mat> templates, objects, positive_data, negative_data; //maps from category names to data
        multimap<string, Mat> train_set; //training images, mapped by category name
        map<string, CvSVM> svms; //trained SVMs, mapped by category name
        vector<string> category_names; //names of the categories found in TRAIN_FOLDER
        int categories; //number of categories
        int clusters; //number of clusters for SURF features to build vocabulary
        Mat vocab; //vocabulary
    
        // Feature detectors and descriptor extractors
        Ptr<FeatureDetector> featureDetector;
        Ptr<DescriptorExtractor> descriptorExtractor;
        Ptr<BOWKMeansTrainer> bowtrainer;
        Ptr<BOWImgDescriptorExtractor> bowDescriptorExtractor;
        Ptr<FlannBasedMatcher> descriptorMatcher;

        void make_train_set(); //function to build the training set multimap
        void make_pos_neg(); //function to extract BOW features from training images and organize them into positive and negative samples 
        string remove_extension(string); //function to remove extension from file name, used for organizing templates into categories
    public:
        categorizer(int); //constructor
        void build_vocab(); //function to build the BOW vocabulary
        void train_classifiers(); //function to train the one-vs-all SVM classifiers for all categories
        void categorize(VideoCapture); //function to perform real-time object categorization on camera frames
};

string categorizer::remove_extension(string full) {
    int last_idx = full.find_last_of(".");
    string name = full.substr(0, last_idx);
    return name;
}

categorizer::categorizer(int _clusters) {
    clusters = _clusters;
    // Initialize pointers to all the feature detectors and descriptor extractors
    featureDetector = (new SurfFeatureDetector());
    descriptorExtractor = (new SurfDescriptorExtractor());
    bowtrainer = (new BOWKMeansTrainer(clusters));
    descriptorMatcher = (new FlannBasedMatcher());
    bowDescriptorExtractor = (new BOWImgDescriptorExtractor(descriptorExtractor, descriptorMatcher));

    // Organize the object templates by category
    // Boost::filesystem directory iterator
    for(directory_iterator i(TEMPLATE_FOLDER), end_iter; i != end_iter; i++) {
        // Prepend full path to the file name so we can imread() it
        string filename = string(TEMPLATE_FOLDER) + i->path().filename().string();
        // Get category name by removing extension from name of file
        string category = remove_extension(i->path().filename().string());
        Mat im = imread(filename), templ_im;
        objects[category] = im;
        cvtColor(im, templ_im, CV_BGR2GRAY);
        templates[category] = templ_im;
    }
    cout << "Initialized" << endl;

    // Organize training images by category
    make_train_set();
}

void categorizer::make_train_set() {
    string category;
    // Boost::filesystem recursive directory iterator to go through all contents of TRAIN_FOLDER
    for(recursive_directory_iterator i(TRAIN_FOLDER), end_iter; i != end_iter; i++) {
        // Level 0 means a folder, since there are only folders in TRAIN_FOLDER at the zeroth level
        if(i.level() == 0) {
            // Get category name from name of the folder
            category = (i -> path()).filename().string();
            category_names.push_back(category);
        }
        // Level 1 means a training image, map that by the current category
        else {
            // File name with path
            string filename = string(TRAIN_FOLDER) + category + string("/") + (i -> path()).filename().string();
            // Make a pair of string and Mat to insert into multimap
            pair<string, Mat> p(category, imread(filename, CV_LOAD_IMAGE_GRAYSCALE));
            train_set.insert(p);
        }
    }
    // Number of categories
    categories = category_names.size();
    cout << "Discovered " << categories << " categories of objects" << endl;
}

void categorizer::make_pos_neg() {
    // Iterate through the whole training set of images
    for(multimap<string, Mat>::iterator i = train_set.begin(); i != train_set.end(); i++) {
        // Category name is the first element of each entry in train_set
        string category = (*i).first;
        // Training image is the second elemnt
        Mat im = (*i).second, feat;
        
        // Detect keypoints, get the image BOW descriptor
        vector<KeyPoint> kp;
        featureDetector -> detect(im, kp);
        bowDescriptorExtractor -> compute(im, kp, feat);

        // Mats to hold the positive and negative training data for current category
        Mat pos, neg;
        for(int cat_index = 0; cat_index < categories; cat_index++) {
            string check_category = category_names[cat_index];
            // Add BOW feature as positive sample for current category ...
            if(check_category.compare(category) == 0)
                positive_data[check_category].push_back(feat);
            //... and negative sample for all other categories
            else
                negative_data[check_category].push_back(feat);
        }
    }
    
    // Debug message
    for(int i = 0; i < categories; i++) {
        string category = category_names[i];
        cout << "Category " << category << ": " << positive_data[category].rows << " Positives, " << negative_data[category].rows << " Negatives" << endl;
    }
}
            
void categorizer::build_vocab() {
    // Mat to hold SURF descriptors for all templates
    Mat vocab_descriptors;
    // For each template, extract SURF descriptors and pool them into vocab_descriptors
    for(map<string, Mat>::iterator i = templates.begin(); i != templates.end(); i++) {
        vector<KeyPoint> kp; Mat templ = (*i).second, desc;
        featureDetector -> detect(templ, kp);
        descriptorExtractor -> compute(templ, kp, desc);
        vocab_descriptors.push_back(desc);
    }
    
    // Add the descriptors to the BOW trainer to cluster
    bowtrainer -> add(vocab_descriptors);
    // cluster the SURF descriptors
    vocab = bowtrainer->cluster();

    // Save the vocabulary
    FileStorage fs(DATA_FOLDER "vocab.xml", FileStorage::WRITE);
    fs << "vocabulary" << vocab;
    fs.release();

    cout << "Built vocabulary" << endl;
} 

void categorizer::train_classifiers() {
    // Set the vocabulary for the BOW descriptor extractor
    bowDescriptorExtractor -> setVocabulary(vocab);
    // Extract BOW descriptors for all training images and organize them into positive and negative samples for each category
    make_pos_neg();

    for(int i = 0; i < categories; i++) {
        string category = category_names[i];
        
        // Postive training data has labels 1
        Mat train_data = positive_data[category], train_labels = Mat::ones(train_data.rows, 1, CV_32S);
        // Negative training data has labels 0
        train_data.push_back(negative_data[category]);
        Mat m = Mat::zeros(negative_data[category].rows, 1, CV_32S);
        train_labels.push_back(m);
        
        // Train SVM!
        svms[category].train(train_data, train_labels);

        // Save SVM to file for possible reuse
        string svm_filename = string(DATA_FOLDER) + category + string("SVM.xml");
        svms[category].save(svm_filename.c_str());

        cout << "Trained and saved SVM for category " << category << endl;
    }
    
}

void categorizer::categorize(VideoCapture cap) {
    cout << "Starting to categorize objects" << endl;
    namedWindow("Image");

    while(char(waitKey(1)) != 'q') {
        Mat frame, frame_g;
        cap >> frame;
        imshow("Image", frame);

        cvtColor(frame, frame_g, CV_BGR2GRAY);

        // Extract frame BOW descriptor
        vector<KeyPoint> kp;
        Mat test;
        featureDetector -> detect(frame_g, kp);
        bowDescriptorExtractor -> compute(frame_g, kp, test);
        
        // Predict using SVMs for all catgories, choose the prediction with the most negative signed distance measure
        float best_score = 777;
        string predicted_category;
        for(int i = 0; i < categories; i++) {
            string category = category_names[i];
            float prediction = svms[category].predict(test, true);
            //cout << category << " " << prediction << " ";
            if(prediction < best_score) {
                best_score = prediction;
                predicted_category = category;
            }
        }
        //cout << endl;

        // Pull up the object template for the detected category and show it in a separate window
        imshow("Detected object", objects[predicted_category]);
    }
}

int main() {
    // Number of clusters for building BOW vocabulary from SURF features
    int clusters = 1000;    
    categorizer c(clusters);
    c.build_vocab();
    c.train_classifiers();
    
    VideoCapture cap(0);
    namedWindow("Detected object");
    c.categorize(cap);
    return 0;
}
