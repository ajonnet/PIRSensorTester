//
//  Recorder.hpp
//  PIRSensorTester
//
//  Created by Amit Jain on 19/09/19.
//  Copyright © 2019 ajonnet. All rights reserved.
//

#ifndef Recorder_hpp
#define Recorder_hpp

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Recorder
{
public:
    
    //! Setup
    void setup(int width, int height, int fps, string storePath);
    
    //! Update
    void write(Mat img);
    void splitVideo();

    //! Shutdown
    void close();
    
    string vidFPName;
    unsigned int frmCount;
    
private:
    string genFileNameForTime(time_t timeVal);
    VideoWriter genVideoWriter(string fPName);
    
    //Recording parameters
    int recordingFPS;
    int recdFrameWidth,recdFrameHeight;
    string recdDirPath;
    
    //Video Writer Constructs
    time_t time_OVidFCreated;
    VideoWriter vidWriter;
};

#endif /* Recorder_hpp */
