#pragma once

#include "ofMain.h"
#include "ofxMtlMapping2D.h"
#include "ofxImageSequence.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"

#define NUM_VIDEOS  324 // for 1024X768: sliceSize 60X40
#define PORT 27900

class VideoPart {
public:
    VideoPart();
    ~VideoPart();
    void init(string path, int _w, int _h);
	void draw();
	void update();
	void setTarget(int x, int y);
	void place(int x, int y);
    void switchTo(string path);
    void setPaused(bool b);
    void pause();
    void unpause();
    bool isPaused();


private:
    void play(string path);
    ofVideoPlayer player;
	int x, y, w, h;
	int targetx, targety;
	int sourcex, sourcey;
	float time;
	bool migrate;
	bool paused;

};

class ofApp : public ofBaseApp {

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
	void loadSettings();
	void shuffle();
	void handleMessage(string msg);
	void pausePart();



private:
    string videoPath;
	bool loaded;
	ofxMtlMapping2D* _mapping;
	VideoPart	parts[NUM_VIDEOS];
	int sliceWidth, sliceHeight;
	int lastSwitch;
	int switchEvery;
	bool config;
	void setupVideo(int idx, string path);
    ofxOscReceiver receiver;

	bool pauseMode;
	float lastPause, pauseGap, minPause, maxPause;


};

