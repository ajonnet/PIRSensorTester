//
//  PIRSensorTesterApp.cpp
//  PIRSensorTester
//
//  Created by Amit Jain on 19/09/19.
//  Copyright © 2019 ajonnet. All rights reserved.
//

#include "PIRSensorTesterApp.hpp"
#include "utils.hpp"

#define CAM_Width 160
#define CAM_Height 120
#define VID_FPS 10
#define VID_LengthMin 30
#define StorePath "data/"

#define PIR_PIN 15

using namespace std;

void PIRSensorTesterApp::run(int argc, const char * argv[]) {

#ifdef USING_PI
    //Initialize Camera
    piCam = new raspicam::RaspiCam_Cv();
    piCam->set( CV_CAP_PROP_FRAME_WIDTH, CAM_Width );
    piCam->set( CV_CAP_PROP_FRAME_HEIGHT, CAM_Height );
    piCam->set( CV_CAP_PROP_FORMAT, CV_8UC3 );
    piCam->set( CV_CAP_PROP_FPS, VID_FPS);
    
    //Open camera
    cout<<"Opening Camera..."<<endl;
    if (!piCam->open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    
    //Initialize PIRSensor
    wiringPiSetup();
    pinMode(PIR_PIN,INPUT) ;
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

    //Loop
    cout<<"Loop started"<<endl;
    int lastStatePirSensor = 0;
    int pirSensorState = 0;
    time_t lastSplitTime = time(0);
    cv::Mat inpFrame;
    while(true) {
#ifdef USING_PI
        //Grab frame from camera
        if(!piCam->grab()) {
            cout<<"Failed to grab frame"<<endl;
            continue;
        }
        piCam->retrieve(inpFrame);

        //Check state of PIR Sensor
        pirSensorState = digitalRead (PIR_PIN);
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
            string eventStr = utils::strForTime(now) + ", " + eventType + ", " + recorder->vidFPName + ", " + utils::strForTime(offsetTime,"%H:%M:%S") + ", " + to_string(recorder->frmCount) + "\n";
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
        double fontScale = 0.4;
        int thickness = 1;
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        cv::Scalar color = cv::Scalar(0,200,200);
        cv::Size text_size = cv::getTextSize(msg,fontFace,fontScale,thickness,&baseline);
        cv::Point textPos = cv::Point(inpFrame.cols - text_size.width ,2);
        cv::putText(inpFrame, msg, textPos, fontFace, fontScale, color, 1);
    
        //Record frame
        recorder->write(inpFrame);
        
        //Split video file check
        if(now - lastSplitTime > VID_LengthMin * 60) {
            lastSplitTime = now;
            recorder->splitVideo();
        }
        
        //Loop breaker
        char ch = (char) cv::waitKey(20);
        if(ch == 'q') {
            break;
        }
    }
}

PIRSensorTesterApp::~PIRSensorTesterApp() {
#ifdef USING_PI
    piCam->release();
#endif
    
    logFile.flush();
    logFile.close();
    recorder->close();
}
