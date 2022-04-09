/*
 * CLI
 * Author: Avital Aviv 316125855 and Noa Benita 209281344
 */

#include "CLI.h"

CLI::CLI(DefaultIO* dio) {
    this->dio = dio;
    this->data = new DataFromHybrid();
    this->commandArray[0] = new UploadCSV(this->dio, this->data);
    this->commandArray[1] = new algorithmSettings(this->dio, this->data);
    this->commandArray[2] = new detectAnomalies(this->dio, this->data);
    this->commandArray[3] = new displayResults(this->dio, this->data);
    this->commandArray[4] = new uploadAndAnalyze(this->dio, this->data);
    this->commandArray[5] = new endCLI(this->dio, this->data);
}

void CLI::start(){
    // printing the menu
    string printingMenu = "Welcome to the Anomaly Detection Server.\nPlease choose an option:\n" +
            this->commandArray[0]->title + this->commandArray[1]->title + this->commandArray[2]->title
                          + this->commandArray[3]->title + this->commandArray[4]->title + this->commandArray[5]->title;
    string userInput = "0";

    // while user not entered 6 - it means that it continues to print the menu and choose option.
    while (userInput != "6") {
        this->dio->write(printingMenu);
        userInput = this->dio->read();
        int userInputToInt = stoi(userInput.c_str());
        if (userInputToInt >=1 && userInputToInt <= 5) {
            this->commandArray[userInputToInt-1]->execute();
        }
    }
}


CLI::~CLI() {
    for (int i=0; i<6; i++) {
        delete this->commandArray[i];
    }
}

