

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>

#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO{
public:
	virtual string read()=0;
	virtual void write(string text)=0;
	virtual void write(float f)=0;
	virtual void read(float* f)=0;
	virtual ~DefaultIO(){}

	// you may add additional methods here
};

class DataFromHybrid{
public:
    vector<AnomalyReport> reports;
    HybridAnomalyDetector* hybridAnomalyDetector;
    int numberOfLines;
    DataFromHybrid() {
        this->hybridAnomalyDetector = new HybridAnomalyDetector();
    }
    HybridAnomalyDetector* getHybridDetector() {
        return this->hybridAnomalyDetector;
    }
    vector<AnomalyReport> getReports() {
        return this->reports;
    }
    void setReports(vector<AnomalyReport> report) {
        this->reports = std::move(report);
    }
    void setNumberOfLines(int lines) {
        this->numberOfLines = lines;
    }
    int getNumberOfLines() {
        return this->numberOfLines;
    }
};


class Command{
public:
	DefaultIO* dio;
	DataFromHybrid* dataFromHybrid;
	string title;
public:
	Command(DefaultIO *dio, DataFromHybrid* data) {
	    this->dio = dio;
	    this->dataFromHybrid = data;
	}
	virtual void execute()=0;
	virtual ~Command(){}
};

class UploadCSV: public Command {
public:
    UploadCSV(DefaultIO *dio, DataFromHybrid *dataFromHybrid) : Command(dio, dataFromHybrid) {
        this->title = "1.upload a time series csv file\n";
    }
    void execute() override {
        ofstream trainFile, testFile;
        dio->write("Please upload your local train CSV file.\n");

        // uploading the train data
        trainFile.open("anomalyTrain.csv");
        string input = "";
        input = dio->read();
        // reading line by line from dio until it got "done" message.
        while (input != "done") {
            trainFile << input << endl;
            input = dio->read();
        }

        dio->write("Upload complete.\n");

        dio->write("Please upload your local test CSV file.\n");
        // uploading the test data
        testFile.open("anomalyTest.csv");
        input = dio->read();
        while (input != "done") {
            testFile << input << endl;
            input = dio->read();
        }
        dio->write("Upload complete.\n");
        trainFile.close();
        testFile.close();
    }
};

class algorithmSettings : public Command {
public:
    algorithmSettings(DefaultIO *dio, DataFromHybrid *dataFromHybrid): Command(dio, dataFromHybrid) {
        this->title = "2.algorithm settings\n";
    }
    void execute() override {
        HybridAnomalyDetector *hybridAnomalyDetector = this->dataFromHybrid->getHybridDetector();
        float currentTreshold = hybridAnomalyDetector->highThreshold;
        dio->write("The current correlation threshold is ");
        dio->write(currentTreshold);
        dio->write("\n");
        dio->write("Type a new threshold\n");
        float newThreshold = stof(dio->read());
        while (newThreshold >= 1 || newThreshold <= 0) {
            dio->write("please choose a value between 0 and 1.\n");
            newThreshold = stof(dio->read());
        }
        this->dataFromHybrid->hybridAnomalyDetector->highThreshold = newThreshold;
    }
};

class detectAnomalies : public Command {
public:
    detectAnomalies(DefaultIO *dio, DataFromHybrid *dataFromHybrid): Command(dio, dataFromHybrid) {
        this->title = "3.detect anomalies\n";
    }
    void execute() override {
        TimeSeries trainFileTimeSeries("anomalyTrain.csv");
        TimeSeries testFileTimeSeries("anomalyTest.csv");
        HybridAnomalyDetector *hybridAnomalyDetector = this->dataFromHybrid->getHybridDetector();
        hybridAnomalyDetector->learnNormal(trainFileTimeSeries);
        vector<AnomalyReport> vectorReports = hybridAnomalyDetector->detect(testFileTimeSeries);
        this->dataFromHybrid->setReports(vectorReports);
        dio->write("anomaly detection complete.\n");
        this->dataFromHybrid->setNumberOfLines(trainFileTimeSeries.getNumberOfRows());
        hybridAnomalyDetector->cf.clear();
    }
};

class displayResults : public Command {
public:
    displayResults(DefaultIO *dio, DataFromHybrid *dataFromHybrid): Command(dio, dataFromHybrid) {
        this->title = "4.display results\n";
    }
    void execute() override {
        vector<AnomalyReport> reports = this->dataFromHybrid->getReports();
        for (auto report : reports) {
            string str = to_string(report.timeStep) + "\t" + report.description + "\n";
            dio->write(str);
        }
        dio->write("Done.\n");
    }
};

class uploadAndAnalyze : public Command {
public:
    uploadAndAnalyze(DefaultIO *dio, DataFromHybrid *dataFromHybrid): Command(dio, dataFromHybrid) {
        this->title = "5.upload anomalies and analyze results\n";
    }
    void execute() override {
        vector<pair<int,int>> reportsFromHybrid;
        vector<pair<int,int>> reportsFromClient;

        //arrangement of the data from hybrid
        int sizeOfReports = this->dataFromHybrid->getReports().size()-1;
        vector<AnomalyReport> vectorData = this->dataFromHybrid->getReports();
        for (int i = 0; i < sizeOfReports; i++) {
            int getFirst = vectorData[i].timeStep;
            while ((i < sizeOfReports) && (vectorData[i].description == vectorData[i+1].description)
            && ((vectorData[i+1].timeStep - vectorData[i].timeStep) == 1)) {
                i++;
            }
            reportsFromHybrid.emplace_back(getFirst, vectorData[i].timeStep);
        }

        dio->write("Please upload your local anomalies file.\n");
        float positive = 0;
        float negative = this->dataFromHybrid->getNumberOfLines();
        string input = dio->read();
        while (input != "done") {
            int delimiterIndex = input.find(",");
            int firstNum = stoi(input.substr(0,delimiterIndex));
            int secondNum = stoi(input.substr(delimiterIndex+1));
            reportsFromClient.emplace_back(firstNum, secondNum);
            negative -= (secondNum - firstNum);
            positive++;
            input = dio->read();
        }
        dio->write("Upload complete.\n");
        float TP = 0;
        float FP = 0;
        for (int i=0; i < reportsFromHybrid.size(); i++) {
            int flag = 0;
            for (int j=0; j < reportsFromClient.size(); j++) {
                if ((reportsFromClient[j].second >= reportsFromHybrid[i].first
                     && reportsFromHybrid[i].second >= reportsFromClient[j].second)
                    ||(reportsFromClient[j].first >= reportsFromHybrid[i].first
                       && reportsFromHybrid[i].second >= reportsFromClient[j].first)
                    ||(reportsFromClient[j].first <= reportsFromHybrid[i].first
                       && reportsFromHybrid[i].second <= reportsFromClient[j].second)) {
                    TP++;
                    flag = 1;
                }
            }
            if (flag == 0) {
                FP++;
            }
        }
        float truePositiveRate = TP/positive;
        float falseAlarmsRate = FP/negative;
        truePositiveRate = (floor(truePositiveRate*1000))/(1000);
        falseAlarmsRate = (floor(falseAlarmsRate*1000))/(1000);

        // printings
        dio->write("True Positive Rate: ");
        dio->write(truePositiveRate);
        dio->write("\n");
        dio->write("False Positive Rate: ");
        dio->write(falseAlarmsRate);
        dio->write("\n");
    }
};

/**
 * exit command
 */
class endCLI : public Command {
public:
    endCLI(DefaultIO *dio, DataFromHybrid *dataFromHybrid): Command(dio, dataFromHybrid){
        this->title = "6.exit\n";
    }
    void execute() override {
    }
};

#endif /* COMMANDS_H_ */
