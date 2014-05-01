#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxXmlSettings.h"

class testApp : public ofBaseApp{

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
        ofxKinect kinect;
        ofPoint points[4] ;
        const int TL = 0;
        const int TR = 1;
        const int BR = 2;
        const int BL = 3;
        bool dragging;
        int current = -1;

        int camX, camY;
        int camW, camH;



};
