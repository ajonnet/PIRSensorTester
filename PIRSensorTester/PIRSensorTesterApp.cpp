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
#include "irslinger.h"  //For sending IR AC_OFF signal
#endif


#define CAM_Width 320
#define CAM_Height 240
#define VID_FPS 10
#define VID_LengthMin 30
#define StorePath "/home/pi/data/"
#define IRCodesFPath "/home/pi/data/irCodes.txt"

#define PIR1_PIN 23 //Gray
#define PIR2_PIN 24  //Purple
#define PIR_PIN 14
#define IR_TX_PIN 3  //4

#define ITER 6

//int codes[] = {1036, 540, 1012, 564, 1036, 2516, 1012, 2536, 1012, 564, 1012, 564, 1036, 2516, 1012, 2536, 1012, 2540, 1012, 2540, 1036, 2516, 1012, 564, 1008, 2544, 1008, 568, 1012, 560, 1036, 544, 1036, 540, 1012, 564, 1012, 564, 1036, 540, 1036, 2516, 1008, 568, 1012, 564, 1012, 564, 1012, 564, 1012, 564, 1012, 564, 1012, 2540, 1012, 2536, 1012, 2540, 1012, 2540, 1012, 564, 1012, 564, 1012, 564, 1012, 2540, 1012, 2536, 1012, 2540, 1012, 564, 1012, 2540, 1012, 2540, 1012, 564, 1012, 568, 1008, 2540, 1012, 2540, 1012, 2540, 1012, 592, 984, 2540, 1012, 2536, 1016, 588, 1012, 564, 988, 2540, 1012, 2536, 1016, 2536, 1040, 560, 1020, 2508, 1044, 2504, 1076, 528, 1048, 528, 1024, 556, 1020, 2504, 1076, 528, 1048, 528, 1048, 528, 1048, 2476, 1076, 528, 1048, 528, 1048, 2476, 1076, 528, 1048, 528, 1052, 524, 1052, 524, 1052, 524, 1052, 2472, 1076, 2476, 1072, 532, 1048, 2476, 1068, 2484, 1040, 2508, 1040, 540, 1036, 540, 1036};

using namespace std;

void PIRSensorTesterApp::run(int argc, const char * argv[]) {
    cout<<"v0.0."<<ITER<<endl;
    
    debugMode = false;
    int maxIdleDelayMins = 10;
    if(argc >1 ) {
        if(strcmp(argv[1], "d") == 0 ||
           strcmp(argv[1], "D") == 0) {
            debugMode = true;
            cout<<"[DEBUG MODE]"<<endl;
        }
        
        if(argc >2) {
            maxIdleDelayMins = stoi(argv[2]);
        }
    }
    cout<<"MaxIdleDelay: "<<maxIdleDelayMins<<" mins"<<endl;
    
    
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
    pinMode(PIR_PIN,INPUT);
    pinMode(PIR1_PIN,INPUT);
    pinMode(PIR2_PIN,INPUT);
#endif
    
#ifdef PIGPIO_ENABLED
    //Initialize PIRSensor
    if(gpioInitialise() < 0) {
        cout<<"Failed to initialize pigpio"<<endl;
        std::exit(1);
    }
    gpioSetMode(PIR_PIN, PI_INPUT);
    gpioSetPullUpDown(PIR1_PIN, PI_PUD_DOWN);
    gpioSetPullUpDown(PIR2_PIN, PI_PUD_DOWN);
#endif
    
    //Load IR Codes
    irCodes = loadIRCodes(IRCodesFPath);
    cout<<"Number of IRCodes loaded: "<<irCodes.size()<<endl;

#ifdef PIGPIO_ENABLED
    if(debugMode) {
        txIRSignal();
        gpioTerminate();
        cout<<"Exiting"<<endl;
        std::exit(0);
    }
#endif
    
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

    bool turnOffAC = false;
    double occupancyCheckIntervalMin = 1;
    
    //Loop
    cout<<"Loop started"<<endl;
    int pir0SensorState,pir1SensorState,pir2SensorState;
    pir0SensorState = pir1SensorState = pir2SensorState = 0;
    int lastStatePirSensor = 0;
    int pirSensorState = 0;
    time_t lastSplitTime = time(0);
    time_t lastIRSignalTxTime = time(0);
    time_t lastMotionDetectedTime = -1;
    double irTxIntervalTimeMin = 2;
    time_t lastOccupancyCheckTime=time(0);
    int countMotionStartDetected = 0;
    time_t lastOccupiedDetectedTime = time(0);
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
        pir0SensorState = digitalRead (PIR_PIN);
        pir1SensorState = digitalRead (PIR1_PIN);
        pir2SensorState = digitalRead (PIR2_PIN);
        pirSensorState = pir1SensorState || pir2SensorState;
#endif

#ifdef PIGPIO_ENABLED
        //Check state of PIR Sensor
        pir0SensorState = gpioRead (PIR_PIN);
        pir1SensorState = gpioRead (PIR1_PIN);
        pir2SensorState = gpioRead (PIR2_PIN);
        pirSensorState =  pir1SensorState || pir2SensorState;
#endif
        
        now = time(0);
        
        cout<<"PIR_State [0]: "<<pir0SensorState<<", [1]:"<<pir1SensorState<<", [2]:"<<pir2SensorState<<endl;
        
        //PIR sensor state check
        bool eventOccured = false;
        string eventType = "";
        if(lastStatePirSensor != pirSensorState) {
            eventOccured = true;
            eventType = (pirSensorState == 1)? "MotionStarted":"MotionEnded";
            cout<<eventType<<endl;
            lastStatePirSensor = pirSensorState;
            if(pirSensorState == 1) countMotionStartDetected++;
        }
        
        
        //Check for occupancy behaviour
        if(now - lastOccupancyCheckTime > occupancyCheckIntervalMin*60) {
            
            //Occupancy check logic
            bool roomOccupied = false;
            if(countMotionStartDetected > 1) roomOccupied = true;
            
            cout<<"Occupancy check: "<<((roomOccupied)?"Occupied":"Not Occupied")<<", "<<countMotionStartDetected<<endl;
            if(roomOccupied) lastOccupiedDetectedTime = now;
            
            lastOccupancyCheckTime = now;
            countMotionStartDetected = 0;
        
            
            //Check if AC is supposed turn off
            if((now - lastOccupiedDetectedTime) > maxIdleDelayMins*60) {
                if(turnOffAC == false) cout<<"Turning AC off..."<<endl;
                turnOffAC = true;
            }else {
                turnOffAC = false;
            }
            
            //Send IR Signal to turn off AC, if no motion found
            if(turnOffAC) {
                time_t timeElapsed = now - lastIRSignalTxTime;
                //cout<<"TimeElapsed: "<<timeElapsed<<" / "<<irTxIntervalTimeMin * 60<<endl;
                if(timeElapsed > (irTxIntervalTimeMin * 60)) {
                    txIRSignal();
                    lastIRSignalTxTime = now;
                }
            }
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

void PIRSensorTesterApp::txIRSignal() {
    int freq = 38000;
    double dutyCycle = 0.5;
    
    cout<<"Sending IR Signal"<<endl;
    for(int i=0; i< irCodes.size(); i++) {
        irSlingRaw(IR_TX_PIN, freq, dutyCycle, irCodes[i].data(), irCodes[i].size());
        cout<<"IR Code "<<i+1<<" sent"<<endl;
    }
    cout<<"IR Signal transmitted"<<endl;
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
    
    vector<vector<int>> _irCodes;
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
        for(int i=0; i<irCode.size() - 1; i++) cout<<irCode[i]<<", ";
        cout<<irCode[irCode.size()-1]<<endl;
        
        _irCodes.push_back(irCode);
    }
    irCodesFile.close();
    return _irCodes;
}
