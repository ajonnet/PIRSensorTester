//
//  utils.cpp
//  PIRSensorTester
//
//  Created by Amit Jain on 19/09/19.
//  Copyright © 2019 ajonnet. All rights reserved.
//

#include "utils.hpp"

using namespace std;

string utils::strForTime(time_t timeVal, string frmt) {
    struct tm * timeinfo;
    timeinfo = localtime(&timeVal);
    
    //Generate File Name
    string timeStr = utils::strForTime(*timeinfo, frmt);
    return timeStr;
}

string utils::strForTime(struct std::tm timeVal, string frmt) {
    
    //Generate File Name
    char buf[80];
    strftime(buf, sizeof(buf), frmt.c_str(), &timeVal);
    string timeStr = string(buf);
    return timeStr;
}

bool utils::parseTimeStr(string timeStr, time_t &t, string frmt) {
    
    time_t _t = 0;
    bool timeParsingSuccess = false;
    int crossCheckCount = 0;
    while(true) {
        //Prepare TM structure for current time
        std::time_t _now = std::time(0);
        std::tm* _nowTM = std::localtime(&_now);
        _nowTM->tm_isdst = 0;
        
        //Parse time from string
        struct std::tm _TM;
        _TM = *_nowTM;
        bool tpSuccess = utils::parseTimeStr(timeStr,_TM,frmt);
        if(!tpSuccess) {
            cout<<"Parsing failed for time string. "<<timeStr<<endl;
            break;
        }
        
        _t = mktime(&_TM);
        
        //Regenerate time string
        string _timeStr = utils::strForTime(_t,frmt);
        
        //Cross check
        //cout<<timeStr<<"\t"<<_timeStr<<endl;
        if(timeStr.compare(_timeStr) == 0){
            timeParsingSuccess = true;
            break;
        }
        
        if(crossCheckCount++ > 5) {
            break;
        }
    }
    //cout<<"-->"<<crossCheckCount<<endl;
    if(timeParsingSuccess) t = _t;
    
    return timeParsingSuccess;
}

bool utils::parseTimeStr(string timeStr, struct std::tm &TM, string frmt) {
#ifdef TARGET_WIN32
    bool parsingSucceded = true;
    
    std::time_t _now = std::time(0);
    std::tm* _nowTM = std::localtime(&_now);
    if(frmt.compare("%Y%m%d_%H%M%S") == 0) {
        unsigned int year, month, date, hour, min, sec;
        sscanf(timeStr.c_str(), "%4u%2u%2u_%2u%2u%2u",&year,&month,&date,&hour,&min,&sec);
        TM = *_nowTM;
        TM.tm_year = year - 1900;
        TM.tm_mon = month -1;
        TM.tm_mday = date;
        TM.tm_hour = hour;
        TM.tm_min = min;
        TM.tm_sec = sec;
        
        //Cross check
        std::time_t tepoch = mktime(&TM);
        string opStr = utils::strForTime(tepoch, frmt);
        assert(timeStr == opStr);
        
    }else if(frmt.compare("%H:%M") == 0) {
        unsigned int hour, min;
        sscanf(timeStr.c_str(), "%2u:%2u",&hour,&min);
        TM = *_nowTM;
        TM.tm_hour = hour;
        TM.tm_min = min;
        
        //Cross check
        std::time_t tepoch = mktime(&TM);
        string opStr = utils::strForTime(tepoch, frmt);
        assert(timeStr == opStr);
        
    }else {
        cout<<"Unsuported dateTime format: "<<frmt<<endl;
        parsingSucceded = false;
    }
    
    return parsingSucceded;
#else
    bool timeParsingSuccess = true;
    
    //Prepare TM structure for current time
    std::time_t _now = std::time(0);
    std::tm* _nowTM = std::localtime(&_now);
    _nowTM->tm_isdst = 0;
    
    //Parse time from string
    TM = *_nowTM;
    char *tpSuccess = strptime(timeStr.c_str(), frmt.c_str(), &TM);
    if(!tpSuccess) {
        //cout<<"Parsing failed for time string. "<<timeStr<<endl;
        timeParsingSuccess = false;
    }
    
    return timeParsingSuccess;
#endif
}


std::vector<std::string> utils::Split(std::string mystring, std::string delimiter)
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

std::string utils::trim(std::string &line){
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
