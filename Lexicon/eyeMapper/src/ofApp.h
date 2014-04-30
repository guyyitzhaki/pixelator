#pragma once

#include "ofMain.h"
#include "ofxMtlMapping2D.h"
#include "ofxImageSequence.h"

#define NUM_VIDEOS 256


class ofApp : public ofBaseApp
{

public:
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    vector<string> getVideos();
    string getRandomVideo();
    void loadVideos();
    void switchVideo();
private:
    bool loaded;
    ofxMtlMapping2D* _mapping;
    ofVideoPlayer	videos[NUM_VIDEOS];
    int sliceWidth, sliceHeight;
    int lastSwitch;
    int switchEvery;

    void setupVideo(int idx, string path);


};
