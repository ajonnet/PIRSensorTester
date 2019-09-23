//
//  PIRSensorTesterApp.cpp
//  PIRSensorTester
//
//  Created by Amit Jain on 19/09/19.
//  Copyright © 2019 ajonnet. All rights reserved.
//

#include "PIRSensorTesterApp.hpp"
#include "utils.hpp"
#include <fstream>
#include <cstdlib>

#ifdef USING_PI
//#define WIRINGPI_ENABLED
#define PIGPIO_ENABLED
#endif

#ifdef WIRINGPI_ENABLED
#include <wiringPi.h>
#endif

#ifdef PIGPIO_ENABLED
#include <pigpio.h>
#inlcude "irslinger.h"  //For sending IR AC_OFF signal
#endif


#define CAM_Width 320
#define CAM_Height 240
#define VID_FPS 10
#define VID_LengthMin 30
#define StorePath "/home/pi/data/"
#define IRCodesFPath "/home/pi/data/irCodes.txt"

#define PIR_PIN 14
#define IR_TX_PIN 4

vector<vector<int>> irCodes;
//int codes[] = {1036, 540, 1012, 564, 1036, 2516, 1012, 2536, 1012, 564, 1012, 564, 1036, 2516, 1012, 2536, 1012, 2540, 1012, 2540, 1036, 2516, 1012, 564, 1008, 2544, 1008, 568, 1012, 560, 1036, 544, 1036, 540, 1012, 564, 1012, 564, 1036, 540, 1036, 2516, 1008, 568, 1012, 564, 1012, 564, 1012, 564, 1012, 564, 1012, 564, 1012, 2540, 1012, 2536, 1012, 2540, 1012, 2540, 1012, 564, 1012, 564, 1012, 564, 1012, 2540, 1012, 2536, 1012, 2540, 1012, 564, 1012, 2540, 1012, 2540, 1012, 564, 1012, 568, 1008, 2540, 1012, 2540, 1012, 2540, 1012, 592, 984, 2540, 1012, 2536, 1016, 588, 1012, 564, 988, 2540, 1012, 2536, 1016, 2536, 1040, 560, 1020, 2508, 1044, 2504, 1076, 528, 1048, 528, 1024, 556, 1020, 2504, 1076, 528, 1048, 528, 1048, 528, 1048, 2476, 1076, 528, 1048, 528, 1048, 2476, 1076, 528, 1048, 528, 1052, 524, 1052, 524, 1052, 524, 1052, 2472, 1076, 2476, 1072, 532, 1048, 2476, 1068, 2484, 1040, 2508, 1040, 540, 1036, 540, 1036};

using namespace std;

void PIRSensorTesterApp::run(int argc, const char * argv[]) {
#ifdef USING_PI
    //Initialize Camera
    piCam = new raspicam::RaspiCam_Cv();
    piCam->set( CV_CAP_PROP_FRAME_WIDTH, CAM_Width );
    piCam->set( CV_CAP_PROP_FRAME_HEIGHT, CAM_Height );
    piCam->set( CV_CAP_PROP_FORMAT, CV_8UC3 );
    piCam->set( CV_CAP_PROP_FPS, VID_FPS);
    piCam->set( CV_CAP_PROP_EXPOSURE, -1);
    piCam->set( CV_CAP_PROP_WHITE_BALANCE_RED_V, -1);
    piCam->set( CV_CAP_PROP_WHITE_BALANCE_BLUE_U, -1);
    
    //Open camera
    cout<<"Opening Camera..."<<endl;
    if (!piCam->open()) {cerr<<"Error opening the camera"<<endl; return;}
#endif
    
#ifdef WIRINGPI_ENABLED
    //Initialize PIRSensor
    wiringPiSetupGpio();
    pinMode(PIR_PIN,INPUT) ;
#endif
    
#ifdef PIGPIO_ENABLED
    //Initialize PIRSensor
    if(gpioInitialise() < 0) {
        cout<<"Failed to initialize pigpio"<<endl;
        std::exit(1);
    }
    gpioSetMode(PIR_PIN, PI_INPUT);
#endif
    
    //Load IR Codes
    loadIRCodes(IRCodesFPath);
    cout<<"Number of IRCodes loaded: "<<irCodes.size()<<endl;
    
    //Initialize Logfile
    time_t now = time(0);
    string logFName = "log_" + utils::strForTime(now) + ".txt";
    string logFPath = StorePath + logFName;
    logFile.open(logFPath);
    
    //Initialize Recorder
    recorder = new Recorder();
    recorder->setup(CAM_Width, CAM_Height, VID_FPS, StorePath);
    
#ifdef USING_PI
    cout<<"PI mode"<<endl;
#endif

    //Loop
    cout<<"Loop started"<<endl;
    int lastStatePirSensor = 0;
    int pirSensorState = 0;
    time_t lastSplitTime = time(0);
    time_t lastIRSignalTxTime = time(0);
    int irTxIntervalTimeMin = 0;
    cv::Mat inpFrame;
    while(true) {
#ifdef USING_PI
        //Grab frame from camera
        if(!piCam->grab()) {
            cout<<"Failed to grab frame"<<endl;
            continue;
        }
        piCam->retrieve(inpFrame);
#endif

#ifdef WIRINGPI_ENABLED
        //Check state of PIR Sensor
        pirSensorState = digitalRead (PIR_PIN);
#endif

#ifdef PIGPIO_ENABLED
        //Check state of PIR Sensor
        pirSensorState = gpioRead (PIR_PIN);
#endif
        
        now = time(0);
        
        //PIR sensor state check
        bool eventOccured = false;
        string eventType = "";
        if(lastStatePirSensor != pirSensorState) {
            eventOccured = true;
            eventType = (pirSensorState == 1)? "MotionStarted":"MotionEnded";
            cout<<eventType<<endl;
            lastStatePirSensor = pirSensorState;
        }
        
        //Log event to log file
        if(eventOccured) {
            time_t offsetTime = recorder->frmCount / (double) VID_FPS;
            string eventStr = utils::strForTime(now) + ", " + eventType + ", " + recorder->vidFPName + ", " + to_string(offsetTime) + ", " + to_string(recorder->frmCount) + "\n";
            logFile<<eventStr;
            logFile.flush();
        }
        
        //Update inpFrame to reflect PIR status
        if(pirSensorState == 1) {
            Rect rect(0,0,20,20);
            cv::Mat subMat(inpFrame,rect);
            subMat.setTo(cv::Scalar(0,0,200));
        }
        
        //Show current time on inpFrame
        string msg = utils::strForTime(now,"%H:%M:%S");
        int baseline=0;
        double fontScale = 0.3;
        int thickness = 1;
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        cv::Scalar color = cv::Scalar(0,200,200);
        cv::Size text_size = cv::getTextSize(msg,fontFace,fontScale,thickness,&baseline);
        cv::Point textPos = cv::Point(inpFrame.cols - text_size.width ,10 + (text_size.height/2));
        cv::putText(inpFrame, msg, textPos, fontFace, fontScale, color, 1);
    
        //Record frame
        recorder->write(inpFrame);
        
        //Send IR Signal to turn off AC, if no motion found
        if(pirSensorState == 0 &&
           (now - lastIRSignalTxTime) > (irTxIntervalTimeMin * 60)) {
            int freq = 38000;
            double dutyCycle = 0.5;
            
            cout<<"Sending IR Signal"<<endl;
            for(int i=0; i< irCodes.size(); i++) {
                irSlingRaw(IR_TX_PIN, freq, dutyCycle, irCodes[i].data(), irCodes[i].size());
                cout<<"IR Code "<<i+1<<" sent"<<endl;
            }
            cout<<"IR Signal transmitted"<<endl;
            lastIRSignalTxTime = now;
        }
        
        //Split video file check
        if(now - lastSplitTime > VID_LengthMin * 60) {
            lastSplitTime = now;
            recorder->splitVideo();
        }
        
        //Loop breaker
        int ch = cv::waitKey(33);
        if(ch == 27) {
            break;
        }
    }
    
#ifdef PIGPIO_ENABLED
    gpioTerminate();
#endif
}

PIRSensorTesterApp::~PIRSensorTesterApp() {
#ifdef USING_PI
    piCam->release();
#endif
    
    logFile.flush();
    logFile.close();
    recorder->close();
}


vector<vector<int>> PIRSensorTesterApp::loadIRCodes(string fPath) {
    cout<<"Loading irCodes from file: "<<fPath<<endl;
    
    ifstream irCodesFile(fPath);
    string entry;
    while(getline(irCodesFile,entry)) {
        
        //Skip invalid entries
        if(entry.size() < 10)  {
            cout<<"Skipping entry(length too small): "<<entry<<endl;
            continue;
        }
        
        //String to int
        vector<string> pulsesStr = utils::Split(entry, ",");
        vector<int> irCode;
        for(int i=0; i<pulsesStr.size(); i++) {
            if(pulsesStr[i].length() > 0) {
                string irCodeStr = utils::trim(pulsesStr[i]);
                irCode.push_back(stoi(irCodeStr));
            }
        }
        
        //Logging IRCode to console
        cout<<"IR_CODE("<<irCode.size()<<"): ";
        for(int i=0; i<irCode.size(); i++) cout<<irCode[i]<<", ";
        
        irCodes.push_back(irCode);
    }
}
