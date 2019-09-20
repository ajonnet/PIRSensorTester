//
//  main.cpp
//  PIRSensorTester
//
//  Created by Amit Jain on 19/09/19.
//  Copyright © 2019 ajonnet. All rights reserved.
//

#include <iostream>
#include "PIRSensorTesterApp.hpp"

int main(int argc, const char * argv[]) {
    
    try {
        PIRSensorTesterApp app;
        app.run(argc, argv);
    }catch(...) {
        std::cout<<"Exiting app, Unkown exception"<<std::endl;
    }
    
    return 0;
}
