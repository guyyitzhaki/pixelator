#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp
{

public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void loadCameraCalibration();
    ofRectangle screen2projector(ofRectangle r);
    ofRectangle kinect2screen(ofRectangle r);
    void saveSettings();
    void loadSettings();
    void setFullScreen();
    float mapXtoW(float val, float srcW);
    float mapYtoH(float val, float srcH);
    ofxKinect kinect;
    ofxCvGrayscaleImage depthImage; // grayscale depth image
    ofxCvContourFinder contourFinder;

    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image


    int nearThreshold;
    int farThreshold;

    int angle;
    bool ledOn;

    bool showControls;
    bool fullscreen;

    ofRectangle projectionArea;

    ofVideoPlayer movie;
    ofxCvGrayscaleImage grayFrame;
    ofxCvColorImage frame;
    ofxCvGrayscaleImage masked;




};
