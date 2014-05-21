#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxCv.h"
#include "Clone.h"
#include "ofxFaceTracker.h"
#include "ofxQtVideoSaver.h"
#include "ofxXmlSettings.h"

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
    void startRecording();
    void cleanup();
    void loadFace(string face);
    void captureEye();
    void substituteFace();
    void updateFace();
    void captureImage();

    void nextFace();
    void prevFace();

    void saveSettings();
    void loadSettings();

    int camWidth, camHeight;
    int camX, camY;
    int sliceWidth, sliceHeight;

    ofxFaceTracker tracker;

    ofxQtVideoSaver *recorder;
    ofVideoGrabber cam;

    ofFbo leftEyeFbo, rightEyeFbo;
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

    int deviceId;

	ofxUICanvas *gui;
	void guiEvent(ofxUIEventArgs &e);

    // FaceSubstitution
    ofxFaceTracker srcTracker;
	ofImage src;
	vector<ofVec2f> srcPoints;


    bool cloneReady;
	Clone clone;
	ofFbo srcFbo, maskFbo;

    ofDirectory faces;
	int currentFace;

	float captureEvery;
	float lastCaptureEnded;
	float switchEvery;
	float lastSwitch;
    string outputDir;

    float cloneW, cloneH;
    bool tracking;
    int FOUND_ITERATIONS = 2;
    int NOT_FOUND_ITERATIONS = 10;


};
