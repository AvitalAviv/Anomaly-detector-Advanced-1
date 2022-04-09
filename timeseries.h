#include <string.h>
#include <sstream>
#include <vector>
#include "map"

#ifndef TIMESERIES_H_
#define TIMESERIES_H_

using namespace std;
class TimeSeries{
    string fileName;
    map<string, vector<float>> data;
    vector<string> subjects;
    int rowsNumber;
public:
    TimeSeries(string CSVfileName);
    void ReadFile();
    vector<string> getSubjects() const;
    map<string, vector<float>> getData() const;
    vector<string> parseLine(const string &line);
    int getNumberOfRows();
};

#endif /* TIMESERIES_H_ */