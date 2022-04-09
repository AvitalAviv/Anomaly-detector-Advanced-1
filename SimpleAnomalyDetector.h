#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct correlatedFeatures{
    string feature1,feature2;  // names of the correlated features
    float corrlation;
    Line lin_reg;
    float threshold;
    Point centerCircle = {0,0};
};


class SimpleAnomalyDetector: public TimeSeriesAnomalyDetector{

public:
    float lowThreshlod;
    float highThreshold;
    vector<correlatedFeatures> cf;
    SimpleAnomalyDetector();
    virtual ~SimpleAnomalyDetector();

    virtual void learnNormal(const TimeSeries& ts);
    virtual vector<AnomalyReport> detect(const TimeSeries& ts);

    vector<correlatedFeatures> getNormalModel(){
        return cf;
    }
    float* fromVectorToArray(vector<float> v);
    float maxDev(Point** points, Line line, int size);
    Point** fromArrayToPoints(float x[], float y[], int size);
    virtual void insertNewCf(map<string, vector<float>> &copyTs, string sub1, string sub2, int vectorIndex, float* arr1, float max);
    virtual bool checkDeviation(Point point, correlatedFeatures cf);
};

#endif /* SIMPLEANOMALYDETECTOR_H_ */