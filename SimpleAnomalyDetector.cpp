/*
 * SimpleAnomalyDetector
 * Author: Avital Aviv 316125855 and Noa Benita 209281344
 */
#include "anomaly_detection_util.h"
#include "SimpleAnomalyDetector.h"
#include <vector>
using namespace std;

/**
 * constructor
 */
SimpleAnomalyDetector::SimpleAnomalyDetector() {
    this->highThreshold = 0.9;
    this->lowThreshlod = 0.5;
}

/**
 * destructor
 */
SimpleAnomalyDetector::~SimpleAnomalyDetector() {
    return;
}

/**
 * converting from array to points
 * @param x - first array = arr1
 * @param y - second array = arr2
 * @param size - size of the vector or the arrays
 * @return returning array of points.
 */
Point** SimpleAnomalyDetector::fromArrayToPoints(float x[], float y[], int size) {
    Point** arr = new Point*[size];
    for (int i = 0; i < size; i++) {
        arr[i] = new Point(x[i], y[i]);
    }
    return arr;
}

/**
 * finding the max deviation value.
 * @param points
 * @param line - linear regression.
 * @param size of the vector - number of values.
 * @return returning the max deviation value.
 */
float SimpleAnomalyDetector::maxDev(Point** points, Line line, int size) {
    float maxSize = 0;
    float dev1;
    for (int i = 0 ; i < size ; ++i) {
        dev1 = dev(*points[i] , line);
        if (dev1 > maxSize) {
            maxSize = dev1;
        }
    }
    return maxSize;
}

/**
 * getting vector and returning array of floats.
 * @param v - vector of floats.
 * @return returning pointer to the first element in the array.
 */
float* SimpleAnomalyDetector::fromVectorToArray(vector<float> v) {
    float * arr = (float*) malloc(sizeof(float)*v.size());
    for(int i = 0; i < v.size(); i++) {
        arr[i] = (v[i]);
    }
    return arr;
}

/**
 * function that creates the correlated feautres data structor.
 * going over all the possible pairs of vectors, and find the correlated ones.
 * @param ts - the data from the files.
 */
void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts){
    map<string, vector<float>> copyTs = ts.getData();
    int sizeFeatures = ts.getSubjects().size();
    vector<float> v1;
    vector<float> v2;
    string sub1, sub2;
    float* arr1;
    float* arr2;
    for (int i=0; i < sizeFeatures; i++) {
        float max=0;
        int vectorIndex=-1;
        sub1 = ts.getSubjects()[i];
        arr1 = fromVectorToArray(ts.getData().find(sub1)->second);
        for (int j=i+1; j < sizeFeatures; j++) {
            arr2 = fromVectorToArray(ts.getData().find(ts.getSubjects()[j])->second);
            float pear = fabsf( pearson(arr1, arr2, ts.getData().find(ts.getSubjects()[j])->second.size()));
            if (pear > max) {
                max = pear;
                vectorIndex = j;
                sub2 = ts.getSubjects()[j];
            }
        }
        if (vectorIndex != -1) {
            insertNewCf(copyTs, sub1, sub2, vectorIndex, arr1, max);
        }
    }
    delete [] arr1;
    delete [] arr2;
}


/**
 * insertNewCf - inserting nre correlated feature after finding
 * @param copyTs - copied timeseries data
 * @param sub1 - first subject from the ts
 * @param sub2 - second subject from the ts
 * @param vectorIndex - the correlated index
 * @param arr1 - first subjects turned to array instead of vector
 * @param max - correlation value
 */
void SimpleAnomalyDetector::insertNewCf(map<string, vector<float>> &copyTs, string sub1, string sub2, int vectorIndex, float* arr1,
                                        float max){
    if (max >= this->highThreshold) {
        float *arr2;
        arr2 = fromVectorToArray(copyTs.find(sub2)->second);
        int vectorSize = copyTs.find(sub1)->second.size();
        Point** arrayOfPoints = fromArrayToPoints(arr1, arr2, vectorSize);
        Line lin_reg = linear_reg((arrayOfPoints), vectorSize);
        auto *pair = new correlatedFeatures();
        pair->feature1 = sub1;
        pair->feature2 = sub2;
        pair->corrlation = max;
        pair->threshold = float(1.1)*maxDev((arrayOfPoints),lin_reg, vectorSize);;
        pair->lin_reg = lin_reg;
        cf.push_back(*pair);
    }
}

/**
 * detecting the alarm reports.
 * @param ts - data base.
 * @return returning the reports of the time there was a alarm.
 */
vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts){
    vector<AnomalyReport> reports;
    for (auto pair : cf) {
        vector<float> feature1 = ts.getData().find(pair.feature1)->second;
        vector<float> feature2 = ts.getData().find(pair.feature2)->second;
        for (int i = 0; i < feature1.size(); i++) {
            Point point = Point(feature1[i], feature2[i]);
            if (checkDeviation(point, pair)) {
                string description = pair.feature1 + "-" + pair.feature2;
                AnomalyReport *report = new AnomalyReport(description, i+1);
                reports.push_back(*report);
            }
        }
    }
    return reports;
}

/**
 * checkDeviation - check dev in a line regression
 * @param point
 * @param cf - correlated feature
 * @return true - if there is a report, false - if there is no report
 */
bool SimpleAnomalyDetector::checkDeviation(Point point, correlatedFeatures cf) {
    return cf.threshold < dev(point, cf.lin_reg);
}
