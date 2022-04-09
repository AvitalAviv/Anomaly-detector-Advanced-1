
/*
 * HybridAnomalyDetector
 * Author: Avital Aviv 316125855 and Noa Benita 209281344
 */
#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector: public SimpleAnomalyDetector {
public:
	HybridAnomalyDetector();
	virtual ~HybridAnomalyDetector();
    virtual void insertNewCf(map<string, vector<float>> &copyTs, string sub1, string sub2, int vectorIndex, float* arr1,
    float max);
    virtual bool checkDeviation(Point point, correlatedFeatures cf);
};

#endif /* HYBRIDANOMALYDETECTOR_H_ */
