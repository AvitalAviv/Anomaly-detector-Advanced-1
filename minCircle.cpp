/*
 * minCircle
 * Author: Avital Aviv 316125855 and Noa Benita 209281344
 */

#include "minCircle.h"
#include <algorithm>
#include <math.h>
#include <vector>

using namespace std;

float findDistance(Point a, Point b) {
    float distance = sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
    return distance;
}

Circle circleFromTwoPoints(Point a, Point b) {
    Point midPoint = Point((a.x + b.x)/2, (a.y + b.y)/2);
    return Circle(midPoint, findDistance(a,b)/2);
}

Circle circleFromThreePoints(Point a, Point b, Point c) {
    float bx = b.x - a.x;
    float by = b.y - a.y;
    float cx = c.x - a.x;
    float cy = c.y - a.y;
    float B = bx * bx + by * by;
    float C = cx * cx + cy * cy;
    float D = bx * cy - by * cx;
    float x = (cy * B - by * C) / (2 * D);
    float y = (bx * C - cx * B) / (2 * D);
    Point p = Point(x,y);
    p.x += a.x;
    p.y += a.y;
    return Circle(p, findDistance(p, a));
}

int isCircleEncloses(vector<Point> points, Circle circle) {
    for (Point p : points) {
        float dist = findDistance(circle.center, p);
        if (dist > circle.radius) {
            return 0;
        }
    }
    return 1;
}


Circle buildCircle (vector<Point> R){
    if (R.empty()) {
        return Circle(Point(0,0),0);
    }
    else if (R.size() == 1){
        return Circle(R[0],0);
    }
    else if (R.size() == 2){
        Circle circle = circleFromTwoPoints(R[0],R[1]);
        return circle;
    }
    if (R.size() == 3){
        for(int i=0; i < 3; i++) {
            for(int j=i+1; j < 3; j++) {
                Circle c = circleFromTwoPoints(R[i],R[j]);
                if (isCircleEncloses(R, c)==1) {
                    return c;
                }
            }
        }
    }
    Circle c = circleFromThreePoints(R[0],R[1],R[2]);
    return c;
}


Circle circleMade(vector<Point> points, vector<Point> R, int size) {
    if ((size == 0 || R.size() == 3)) {
        return buildCircle(R);
    }
    int i=rand()%size;
    Point p = Point(points[i].x,points[i].y);
    swap(points[i],points[size-1]);
    Circle C = circleMade(points, R, size-1);
    if (findDistance(C.center, p) <= C.radius) {
        return C;
    }
    R.push_back(p);
    return circleMade(points, R, size-1);
}

Circle findMinCircle(Point** points,size_t size) {
    vector<Point> copyPoints;
    for (int i=0; i < size; i++) {
        copyPoints.push_back(*points[i]);
    }
    vector<Point> R;
    Circle circle = circleMade(copyPoints, R, size);
    return circle;
}
