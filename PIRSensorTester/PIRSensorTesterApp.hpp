//
//  PIRSensorTesterApp.hpp
//  PIRSensorTester
//
//  Created by Amit Jain on 19/09/19.
//  Copyright © 2019 ajonnet. All rights reserved.
//

#ifndef PIRSensorTesterApp_hpp
#define PIRSensorTesterApp_hpp

#ifndef USING_XCODE
    #define USING_PI
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Recorder.hpp"

#ifdef USING_PI
#include <raspicam/raspicam_cv.h>
#endif

class PIRSensorTesterApp {
public:
    void run(int argc, const char * argv[]);
    vector<vector<int>> loadIRCodes(string fPath);
    void txIRSignal();
    ~PIRSensorTesterApp();
private:
#ifdef USING_PI
    cv::Ptr<raspicam::RaspiCam_Cv> piCam;
#endif
    vector<vector<int>> irCodes;
    cv::Ptr<Recorder> recorder;
    ofstream logFile;
    bool debugMode;
};

#endif /* PIRSensorTesterApp_hpp */
