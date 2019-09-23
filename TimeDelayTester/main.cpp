//
//  main.cpp
//  TimeDelayTester
//
//  Created by Amit Jain on 21/09/19.
//  Copyright © 2019 ajonnet. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include "utils.hpp"

using namespace std;

void timeDelayEval();
vector<string> Split(string mystring, string delimiter);
std::string trim(std::string &line);

int main(int argc, const char * argv[]) {
    timeDelayEval();
    return 0;
}


void timeDelayEval() {
    string logFPath = "/Users/Amit/Desktop/AC_Smart/log_20190921_142339.txt";
    
    int timeDelayMin = 1;
    
    ifstream logFile(logFPath);
    string entry;
    int i=0;
    vector<string> eventsNew;
    time_t endTS = 0;
    int skippedMSEventCount = 0;
    while(getline(logFile,entry)) {
        if(utils::trim(entry).length() == 0) continue;
        
        vector<string> parts = utils::Split(entry, ",");
        string timeStr = utils::trim(parts[0]);
        string eventType = utils::trim(parts[1]);
        
        time_t tSecs = 0;
        utils::parseTimeStr(timeStr, tSecs);
        
        cout<<++i<<": "<<timeStr<<", "<<eventType<<", "<<tSecs<<endl;
        
        if(eventType == "MotionStarted") {
            if(endTS == 0) {
                eventsNew.push_back(entry + ", " + to_string(tSecs));
                endTS = tSecs + (timeDelayMin*60);
            }else {
                if(tSecs < endTS) {
                    endTS = tSecs + (timeDelayMin*60);
                    skippedMSEventCount++;
                }else {
                    //Log motionEnded event
                    string eventStr = utils::strForTime(endTS) + ", MotionEnded, " + to_string(endTS) + ", " + to_string(skippedMSEventCount);
                    eventsNew.push_back(eventStr);
                    
                    //Log motionStarted event
                    eventsNew.push_back(entry + ", " + to_string(tSecs));
                    endTS = tSecs + (timeDelayMin*60);
                    skippedMSEventCount = 0;
                }
            }
        }
    }
    
    cout<<endl;
    cout<<"Timdelay: "<<timeDelayMin<<endl;
    i=0;
    for(auto entry:eventsNew) {
        cout<<++i<<"-"<<entry<<endl;
    }
}
