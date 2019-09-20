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
#include <opencv2/opencv.hpp>
#include "Recorder.hpp"

#ifdef USING_PI
#include <raspicam/raspicam_cv.h>
#include <wiringPi.h>
#endif

class PIRSensorTesterApp {
public:
    void run(int argc, const char * argv[]);
    ~PIRSensorTesterApp();
private:
#ifdef USING_PI
    cv::Ptr<raspicam::RaspiCam_Cv> piCam;
#endif
    cv::Ptr<Recorder> recorder;
    ofstream logFile;
};

#endif /* PIRSensorTesterApp_hpp */
