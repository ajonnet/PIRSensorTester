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
        if(trim(entry).length() == 0) continue;
        
        vector<string> parts = Split(entry, ",");
        string timeStr = trim(parts[0]);
        string eventType = trim(parts[1]);
        
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

vector<string> Split(string mystring, string delimiter)
{
    vector<string> subStringList;
    string token;
    while (true)
    {
        size_t findfirst = mystring.find_first_of(delimiter);
        if (findfirst == string::npos) //find_first_of returns npos if it couldn't find the delimiter anymore
        {
            subStringList.push_back(mystring); //push back the final piece of mystring
            return subStringList;
        }
        token = mystring.substr(0, mystring.find_first_of(delimiter));
        mystring = mystring.substr(mystring.find_first_of(delimiter) + 1);
        subStringList.push_back(token);
    }
    return subStringList;
}

std::string trim(std::string &line){
    if(line.length() == 0) return line;
    
    auto val = line.find_last_not_of(" \n\r\t") + 1;
    
    if(val == line.size() || val == std::string::npos){
        val = line.find_first_not_of(" \n\r\t");
        line = line.substr(val);
    }
    else
        line.erase(val);
    
    return line;
}
