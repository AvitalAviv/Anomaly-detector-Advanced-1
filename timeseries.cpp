/*
 * TimeSeries
 * Author: Avital Aviv 316125855 works with Noa Benita 209281344
 */
#include "timeseries.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "string"
#include <map>

using namespace std;

/**
 * constructor.
 * @param CSVfileName
 */
TimeSeries::TimeSeries(string CSVfileName) {
    this->fileName = CSVfileName;
    ReadFile();
}

/**
 * ReadFile method.
 * @param fileName
 * the function that gets the file, read line by line and create the data base.
 */
void TimeSeries::ReadFile() {
    this->rowsNumber = 0;
    fstream file;
    file.open(fileName);
    vector<string> row(0);
    string line, word;
    /* getting the first line - the subjects */
    getline(file, line);
    /* parsing the subject line */
    subjects = parseLine(line);
    if (file.is_open()) {
        while(file.good()) {
            while(getline(file, line)) {
                /* creating vector of rows */
                row.push_back(line);
            }
        }
    }
    file.close();
    int subjectsSize = subjects.size();
    vector<vector<string>> rows;
    for (auto r : row) {
        /* creating vectors from the rows */
        vector<string> temp = parseLine(r);
        rows.push_back(temp);
        this->rowsNumber++;
    }
    for (int i = 0; i < subjectsSize; i++) {
        vector<float> col;
        for (auto r : rows) {
            /* converting the string to floats numbers and push to col vector */
            col.push_back(stof(r[i]));
        }
        /* creating the map */
        data.insert(pair<string,vector<float>> (subjects[i], col));
    }
}

/**
 * parsing function.
 * @param line
 * @return vector of the parsing strings.
 */
vector<string> TimeSeries::parseLine(const string &line) {
    string word;
    stringstream stringstream1(line);
    vector<string> row;
    while(getline(stringstream1, word, ',')){
        row.push_back (word);
    }
    return row;
}

/**
 * getSubjects - getter function.
 * @return vector of string subjects.
 */
vector<string> TimeSeries::getSubjects() const {
    return this->subjects;
}

/**
 * getter of the data - the map <key=subjects, value=vectors of float>
 * @return the map.
 */
map<string, vector<float>> TimeSeries::getData() const{
    return this->data;
}

int TimeSeries::getNumberOfRows() {
    return this->rowsNumber;
}