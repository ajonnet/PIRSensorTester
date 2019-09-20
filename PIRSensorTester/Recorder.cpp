//
//  Recorder.cpp
//  PIRSensorTester
//
//  Created by Amit Jain on 19/09/19.
//  Copyright © 2019 ajonnet. All rights reserved.
//

#include "Recorder.hpp"
#include "utils.hpp"


//--------------------------------------------------------------
void Recorder::setup(int width, int height, int fps, string storePath)
{
    cout<<"Initilaizing Recorder"<<endl;
    
    //Recording params
    recordingFPS = fps;
    recdFrameWidth = width;
    recdFrameHeight = height;
    recdDirPath = storePath;
    
    //Initialize videoWriter constructs
    time(&time_OVidFCreated);
    frmCount = 0;
    vidFPName = genFileNameForTime(time_OVidFCreated);
    vidWriter = genVideoWriter(vidFPName);
    cout<<"New video file created: "<<vidFPName<<endl;
}

//--------------------------------------------------------------
void Recorder::write(Mat img)
{
    if((img.rows * img.cols > 0) && vidWriter.isOpened()) {
        
        //Resize image
        Mat img_resized;
        resize(img,img_resized,cv::Size(recdFrameWidth,recdFrameHeight));
        
        //Write to Video
        vidWriter.write(img_resized);
        frmCount++;
    }else {
        cout<<"Frame not written"<<endl;
        cout<<"Rows: "<<img.rows<<", Cols: "<<img.cols<<endl;
    }
}

void Recorder::splitVideo() {
    
    time_t now;
    time(&now);
    double secs_elapsed = difftime(now, time_OVidFCreated);
    
    //Log Current Video file details
    cout<<"Closing Video file with name: "<<vidFPName<<endl;
    cout<<"Time Elapsed: "<<secs_elapsed<<" secs"<<endl;
    cout<<"Number of Frames Written: "<<frmCount<<endl;
    cout<<"Avg recording FPS: "<<frmCount/(double)secs_elapsed<<endl;
    
    //Close the current video file
    frmCount = 0;
    vidWriter.release();
    cout<<"Video File closed"<<endl<<endl;
    
    //Spawning new video writer for new file
    time(&time_OVidFCreated);
    vidFPName = genFileNameForTime(time_OVidFCreated);
    vidWriter = genVideoWriter(vidFPName);
    cout<<"New video file created: "<<vidFPName<<endl;
}

//--------------------------------------------------------------
void Recorder::close()
{
    if(vidWriter.isOpened()) {
        vidWriter.release();
    }
}

//--------------------------------------------------------------
string Recorder::genFileNameForTime(time_t timeVal) {
    struct tm * timeinfo;
    timeinfo = localtime(&timeVal);
    
    //Generate File Name
    string frmt = "%Y%m%d_%H%M%S";
    string dateTimeStr = utils::strForTime(*timeinfo,frmt);
    string fileName = "vid_" + dateTimeStr + ".avi";
    return fileName;
}

//--------------------------------------------------------------
VideoWriter Recorder::genVideoWriter(string fPName) {
    string vidFName = recdDirPath + fPName;
    VideoWriter vw(vidFName,CV_FOURCC('H','2','6','4'),recordingFPS,cv::Size(recdFrameWidth,recdFrameHeight),true);
    return vw;
}
