#pragma once

#include "ofMain.h"
#include "ofxMtlMapping2D.h"
#include "ofxImageSequence.h"

#define NUM_VIDEOS 256

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
private:
    void play(string path);
    ofVideoPlayer player;
	int x, y, w, h;
	int targetx, targety;
	int sourcex, sourcey;
	float time;
	bool migrate;

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
private:
	bool loaded;
	ofxMtlMapping2D* _mapping;
	VideoPart	parts[NUM_VIDEOS];
	int sliceWidth, sliceHeight;
	int lastSwitch;
	int switchEvery;
	bool config;
	void setupVideo(int idx, string path);


};

