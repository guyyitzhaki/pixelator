#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "ofxSlitScan.h"
#include "ofxKinect.h"

// either USE_KINECT or USE_VIDEO must be defined
//#define USE_VIDEO 1
#define USE_KINECT 1

#define PORT 12001
#define LINE_DATA_LENGTH 500

#define END_LINES_PHASE 1
#define END_MOVIE_PHASE 30
#define END_DELAY_PHASE 60

#define SAVE_AFTER 40

#define PHASE_WAITING 0
#define PHASE_LINES 1
#define PHASE_MOVIE 2
#define PHASE_DELAY 3
#define PHASE_IDLE 4



struct LineData {
	float data[LINE_DATA_LENGTH];
	float x;
	float y;
	float width;
	float height;
	float noiseStep;
    bool active;
    int count;
    int timer;
    bool rtol;
};

struct VideoData {
    string name;
    int end_line_phase;
    int end_movie_phase;
    int end_delay_phase;
};

class ofApp : public ofBaseApp{

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

    void saveSettings();
    void loadSettings();

    void loadVideoData();

    void setupBackground();
    void setupMovie();
    void setupDelay();
    void updateBackground();
    void teardownMovie();
    void renderLine(float x, float y, float width, float height, float dt, float *data, int count, bool rtol);

    void checkTime();
    void checkMessages();
    void play();
    void stop();
    void exit();
    void startRunning();
    void saveImage(string name);

    bool debug = false;
#ifdef USE_KINECT
    ofxKinect kinect;
#endif

#ifdef USE_VIDEO
    ofVideoPlayer player;
    ofxCvColorImage tempImage;
#endif
#ifdef LIVE_CAM
    ofVideoGrabber grabber;
#endif
    int frameW, frameH;

    ofxCvGrayscaleImage depthImage; // grayscale depth image
    ofxCvContourFinder contourFinder;

    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image

    bool configure;
    int nearThreshold;
    int farThreshold;
    int angle;

    ofFbo bg;
    ofFbo slit;
    ofxCvColorImage img;
    ofPixels pix;
    ofPixels slitpix;
    ofxCvColorImage colDepth;

    ofxOscReceiver receiver;

    int		nLines;
    vector<LineData> lines;
    float noiseFactor;

    float nextActivate, lastActivate;
    int nextCount;


    vector<VideoData> videoData;
    int curData;

    ofVideoPlayer movie;
    ofxCvColorImage frame;
    ofSoundPlayer  sound;
    ofxSlitScan slitScan;
    int timeDelay;

    int phase;
    float startRun;

    string code;
    string imgdir;
    string tearname;
    bool saved;


};
