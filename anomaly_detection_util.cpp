/*
 * animaly_detection_util.cpp
 * Author: Avital Aviv 316125855 and Noa Benita 209281344
 */

#include <math.h>
#include "anomaly_detection_util.h"

float avg(float* x, int size){
    float sum = 0;
    for(int i=0; i < size; i++) {
        sum += x[i];
    }
    float average = sum / size;
    return average;
}

// returns the variance of X and Y
float var(float* x, int size){
    float mew = avg(x, size);
    float variance = 0;
    float sum = 0;
    for(int i=0; i < size; i++) {
        sum += pow(x[i], 2);
    }
    variance = (sum/size) - pow(mew, 2);
    return variance;
}

// returns the covariance of X and Y
float cov(float* x, float* y, int size){
    float z[size];
    for(int i=0; i < size; i++) {
        z[i] = x[i] * y[i];
    }
    float cov = avg(z, size) - (avg(x,size) * avg(y,size));
    return cov;
}


// returns the Pearson correlation coefficient of X and Y
float pearson(float* x, float* y, int size){
    float pearson = cov(x, y, size)/(sqrt(var(x, size)) * sqrt(var(y, size)));
    return pearson;
}

// performs a linear regression and returns the line equation
Line linear_reg(Point** points, int size){
    float x[size];
    float y[size];
    for(int i=0; i < size; i++){
        x[i] = points[i]->x;
        y[i] = points[i]->y;
    }
    float a = cov(x, y, size)/var(x, size);
    float b = avg(y, size) - (a * avg(x, size));
    Line line = Line(a,b);
    return line;
}

// returns the deviation between point p and the line equation of the points
float dev(Point p,Point** points, int size){
    Line line = linear_reg(points, size);
    return dev(p, line);
}

// returns the deviation between point p and the line
float dev(Point p, Line l){
    float yFromLine = l.a * p.x + l.b;
    float yFromPoint = p.y;
    float deviation = yFromLine - yFromPoint;
    if (deviation < 0) {
        return -1*deviation;
    }
    return deviation;
}