#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "ofxQtVideoSaver.h"

#define MAX_FRAMES 60
#define NUM_VIDEOS 256
#define WRITE_DELAY 10

struct video_data {
    string filename;
    float time;
};

class ofApp : public ofBaseApp
{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void setupRecorder();

    int camWidth, camHeight;
    int sliceWidth, sliceHeight;

    ofxFaceTracker tracker;

    ofxQtVideoSaver *recorder;
    ofVideoGrabber cam;

    ofFbo eyeFbo;
    ofMesh leftRectImg, rightRectImg;

    ofVec2f position;
    float scale;
    ofMatrix4x4 rotationMatrix;

    string filename;
    vector<video_data *> finished;

    bool recording;
    int frameCount;
    bool left;
    bool loaded;

    float blinkTime;
    bool blinkOn;

};
