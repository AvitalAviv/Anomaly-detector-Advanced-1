/*
 * HybridAnomalyDetector
 * Author: Avital Aviv 316125855 and Noa Benita 209281344
 */
#include "HybridAnomalyDetector.h"
#include "minCircle.h"


HybridAnomalyDetector::HybridAnomalyDetector() {
}

HybridAnomalyDetector::~HybridAnomalyDetector() {
}

void HybridAnomalyDetector::insertNewCf(map<string, vector<float>> &copyTs, string sub1, string sub2, int vectorIndex,
                                        float* arr1, float max){
    if (max >= this->highThreshold) {
        SimpleAnomalyDetector::insertNewCf(copyTs,sub1,sub2,vectorIndex,arr1,max);
    }
    if (max > this->lowThreshlod && max < this->highThreshold) {
        float* arr2;
        arr2 = fromVectorToArray(copyTs.find(sub2)->second);
        int sizeSub1 = copyTs.find(sub1)->second.size();
        Point** arrayOfPoints = fromArrayToPoints(arr1, arr2, sizeSub1);
        Circle circle = findMinCircle(arrayOfPoints, sizeSub1);
        auto *pair = new correlatedFeatures();
        pair->feature1 = sub1;
        pair->feature2 = sub2;
        pair->corrlation = max;
        pair->lin_reg = Line(0,0);
        pair->threshold = float(1.1)*circle.radius;
        pair->centerCircle = circle.center;
        cf.push_back(*pair);
    }
}

bool HybridAnomalyDetector::checkDeviation(Point point, correlatedFeatures cf) {
    if ((cf.corrlation > this->lowThreshlod && cf.corrlation < this->highThreshold && (findDistance(cf.centerCircle, point) > cf.threshold)) ||
            (cf.corrlation >= this->highThreshold && SimpleAnomalyDetector::checkDeviation(point, cf))) {
        return true;
    }
    return false;
}


