#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	showControls = true;
	fullscreen = true;
    recorded = true;
    startTime = 0;
	setFullScreen();
	ofBackground(ofColor::black);
	// enable depth->video image calibration?
	kinect.setRegistration(false);

	// turn off video cam and texture to save cpu
	kinect.init(false, true);

	kinect.open();

	depthImage.allocate(kinect.width, kinect.height);

	ofSetFrameRate(60);

	loadCameraCalibration();
	loadSettings();

	grayFrame.allocate(kinect.width, kinect.height);
	frame.allocate(kinect.width, kinect.height);
	masked.allocate(kinect.width, kinect.height);

	scan = false;
	scanY = ofGetHeight();
	scanGap = 50;

	sound.loadSound("breathing.wav");
	sound.setMultiPlay(false);

	vector<string> movies;
	ofDirectory dir("movies");
	dir.allowExt("mov");
	dir.allowExt("mp4");
	dir.listDir();
	for(int i = 0; i < dir.numFiles(); i++)
    {
        movies.push_back(dir.getFile(i).getFileName());
    }
    switchMovie(movies[0]);

	gui = new ofxUICanvas();
	gui->setColorBack(ofColor::white);
	gui->setColorFill(ofColor::black);
	gui->setColorOutline(ofColor::black);
	gui->setColorFillHighlight(ofColor::black);
	gui->setColorOutlineHighlight(ofColor::black);
	gui->addToggle("FULLSCREEN", false);
	gui->addToggle("SCAN",scan);
	gui->addSlider("GAP SIZE",0.0, 300.0, scanGap);
	gui->addRadio("MOVIE",movies, OFX_UI_ORIENTATION_VERTICAL);
	gui->addButton("SOUND", false);
	ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
	gui->loadSettings("settings.xml");

	receiver.setup(PORT);


}

void ofApp::setFullScreen() {
	ofSetFullscreen(fullscreen);
	/*   if (fullscreen)
	       ofHideCursor();
	   else
	       ofShowCursor();
	*/
}

//--------------------------------------------------------------
void ofApp::update() {
	movie.update();
	kinect.update();
	if(kinect.isFrameNew()) {

		// load grayscale depth image from the kinect source
		depthImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		//depthImage.mirror(false,true);
		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
		grayThreshNear = depthImage;
		grayThreshFar = depthImage;
		grayThreshNear.threshold(nearThreshold, true);
		grayThreshFar.threshold(farThreshold);
		cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), depthImage.getCvImage(), NULL);

		// update the cv images
		depthImage.flagImageChanged();

		contourFinder.findContours(depthImage, 100, (kinect.width*kinect.height)/2, 2, false);
	}

	updateReceiver();
	if (recorded == false) {
        if (ofGetElapsedTimef() - startTime > RECORD_AFTER) {
            recorded = true;
            ofImage frame;
            frame.setFromPixels(depthImage.getPixels(), kinect.width, kinect.height, OF_IMAGE_COLOR);
            frame.saveImage("frame.png");
            cout < "saved\n";
        }
	}


}

void ofApp::updateReceiver() {
	while(receiver.hasWaitingMessages()){
		ofxOscMessage m;
		receiver.getNextMessage(&m);
		if(m.getAddress() == "/start"){
			int arg = m.getArgAsInt32(0);
            cout << "received arg:" << arg << "\n";
            startTime = ofGetElapsedTimef();
            recorded = false;
            //sound.play();
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	if (showControls) {
		ofNoFill();

		ofSetColor(ofColor::white);
		kinect.drawDepth(10, 10, 400, 300);

		ofSetColor(ofColor::red);
		ofRect(10,10,400,300);

		ofSetColor(ofColor::white);
		ofDrawBitmapString("Kinect depth image", 10,10);

		kinect.draw(420, 10, 400, 300);
		ofSetColor(ofColor::red);
		ofRect(420,10,400,300);
		ofSetColor(ofColor::white);
		ofDrawBitmapString("Kinect video image", 420,10);

		depthImage.draw(10, 320, 400, 300);
		ofSetColor(ofColor::red);
		ofRect(10,320,400,300);
		ofSetColor(ofColor::white);
		ofDrawBitmapString("Threshold depth image", 10,320);
		contourFinder.draw(10, 320, 400, 300);

		stringstream reportStream;

		reportStream << "near threshold " << nearThreshold << " (press: + -)" << endl
		             << "far threshold " << farThreshold << " (press: < >)" << endl
		             << "num blobs found " << contourFinder.nBlobs << endl
		             << "fps: " << ofGetFrameRate() << endl
		             << "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl;

		reportStream << "angle: " << angle << " degrees (UP and DOWN)" << endl
		             << "0-5 to change the led mode" << endl;

		ofDrawBitmapString(reportStream.str(), 20, 652);
	} else {
		ofFill();
		ofSetColor(ofColor::white);
		ofRect(0,0,ofGetWidth(), ofGetHeight());

		for(int i = 0; i < contourFinder.nBlobs; i++) {
			ofRectangle r = contourFinder.blobs.at(i).boundingRect;
			ofRectangle scr = kinect2screen(r);
			ofRectangle translated = screen2projector(scr);
			ofFill();
			ofSetColor(ofColor::lightGreen);
			ofRect(translated);
			float centerX = translated.x + translated.getWidth()/2;
			float centerY = translated.y + translated.getHeight()/2;

			ofSetColor(ofColor::red);
			ofLine(centerX - 15, centerY, centerX + 15, centerY);
			ofLine(centerX, centerY - 15, centerX, centerY + 15);
			ofCircle(centerX, centerY, 10);
		}

		ofSetColor(ofColor::white);
		ofxCvGrayscaleImage trg = depthImage;
		frame.setFromPixels(movie.getPixels(), kinect.width, kinect.height);
		grayFrame = frame;
		cvAnd(depthImage.getCvImage(), grayFrame.getCvImage(), trg.getCvImage(), NULL);
		trg.flagImageChanged();
		trg.draw(0,0, ofGetWidth(), ofGetHeight());

		if (scan) {
            ofSetColor(ofColor::black);
            ofFill();
            ofRect(0,0,ofGetWidth(), scanY);
            ofRect(0,scanY + scanGap ,ofGetWidth(), ofGetHeight());
            scanY--;

            if (scanY <= -scanGap) {
                scanY = ofGetHeight();
            }
		}


	}

}

ofRectangle ofApp::kinect2screen(ofRectangle r) {
	float x = mapXtoW(r.x, kinect.width);
	float y = mapYtoH(r.y, kinect.height);
	float w = mapXtoW(r.getWidth(), kinect.width);
	float h = mapYtoH(r.getHeight(), kinect.height);
	return ofRectangle(x,y,w,h);
}

float ofApp::mapXtoW(float val, float srcW) {
	return ofMap(val, 0,  srcW, 0, ofGetWidth(), true);
}

float ofApp::mapYtoH(float val, float srcH) {
	return ofMap(val, 0,  srcH, 0, ofGetHeight(), true);
}

ofRectangle ofApp::screen2projector(ofRectangle r) {
	int tx = r.x - projectionArea.x;
	int ty = r.y - projectionArea.y;
	float x = mapXtoW(tx, projectionArea.getWidth());
	float y = mapYtoH(ty, projectionArea.getHeight());
	float w = mapXtoW(r.getWidth(),  projectionArea.getWidth());
	float h = mapYtoH(r.getHeight(),  projectionArea.getHeight());
	return ofRectangle(x,y,w,h);
}

void ofApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	gui->saveSettings("settings.xml");
	delete gui;

}

void ofApp::saveSettings() {
	ofxXmlSettings settings;
	settings.setValue("settings:nearThreshold", nearThreshold);
	settings.setValue("settings:farThreshold", farThreshold);
	settings.setValue("settings:led", ledOn);
	settings.saveFile("settings.xml");
}

void ofApp::loadSettings() {
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");
	nearThreshold = settings.getValue("settings:nearThreshold", 230);
	farThreshold = settings.getValue("settings:farThreshold", 70);
	bool on = settings.getValue("settings:led", false);
	if (!on)
		kinect.setLed(ofxKinect::LED_OFF);
}

void ofApp::loadCameraCalibration() {
	ofxXmlSettings calibration;
	calibration.loadFile("calibration.xml");

	angle = calibration.getValue("calibration:CAM.ANGLE", 0);
	kinect.setCameraTiltAngle(angle);

	int TL = 0, TR = 1, BR = 2, BL = 3;
	ofPoint points[4];
	int maxx = -1, maxy = -1, minx = ofGetWidth(), miny = ofGetHeight();
	int x = calibration.getValue("calibration:TL.X", points[TL].x);
	int y = calibration.getValue("calibration:TL.Y", points[TL].y);
	maxx = max(x, maxx);
	maxy = max(y, maxy);
	minx = min(x, minx);
	miny = min(y, miny);
	x = calibration.getValue("calibration:TR.X", points[TR].x);
	y = calibration.getValue("calibration:TR.Y", points[TR].y);
	maxx = max(x, maxx);
	maxy = max(y, maxy);
	minx = min(x, minx);
	miny = min(y, miny);
	x = calibration.getValue("calibration:BR.X", points[BR].x);
	y = calibration.getValue("calibration:BR.Y", points[BR].y);
	maxx = max(x, maxx);
	maxy = max(y, maxy);
	minx = min(x, minx);
	miny = min(y, miny);
	x = calibration.getValue("calibration:BL.X", points[BL].x);
	y = calibration.getValue("calibration:BL.Y", points[BL].y);
	maxx = max(x, maxx);
	maxy = max(y, maxy);
	minx = min(x, minx);
	miny = min(y, miny);

	int camx = calibration.getValue("calibration:CAM.X", 0);
	int camy = calibration.getValue("calibration:CAM.Y", 0);
	minx -= camx;
	maxx -= camx;
	miny -= camy;
	maxy -= camy;

	cout << "(" << minx << "," << miny << ") - (" << maxx << "," << maxy << ")\n";
	projectionArea.set(minx, miny, maxx-minx, maxy-miny);
}

void ofApp::switchMovie(string name) {
    if (movie.isPlaying()) {
        movie.stop();
        movie.close();
    }
	movie.loadMovie("movies/" + name);
	movie.play();
}


void ofApp::guiEvent(ofxUIEventArgs &e) {
	if(e.getName() == "SCAN") {
		scan = !scan;
	}
	else if (e.getName() == "GAP SIZE") {
	    ofxUISlider *slider = e.getSlider();
	    scanGap = slider->getScaledValue();
	}
	else if(e.getName() == "FULLSCREEN") {
		ofxUIToggle *toggle = e.getToggle();
		ofSetFullscreen(toggle->getValue());
	}
	else if (e.getName() == "SOUND") {
        sound.play();
	}
	else if((e.getName().find(".mov") != -1) || (e.getName().find(".mp4") != -1)) {
	    switchMovie(e.getName());
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case '>':
	case '.':
		farThreshold ++;
		if (farThreshold > 255) farThreshold = 255;
		break;

	case '<':
	case ',':
		farThreshold --;
		if (farThreshold < 0) farThreshold = 0;
		break;

	case '+':
	case '=':
		nearThreshold ++;
		if (nearThreshold > 255) nearThreshold = 255;
		break;

	case '-':
		nearThreshold --;
		if (nearThreshold < 0) nearThreshold = 0;
		break;

	case 'w':
		kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
		break;

	case 's':
		saveSettings();
		break;

	case 'l':
		loadSettings();
		break;

	case '1':
		kinect.setLed(ofxKinect::LED_GREEN);
		break;

	case '2':
		kinect.setLed(ofxKinect::LED_YELLOW);
		break;

	case '3':
		kinect.setLed(ofxKinect::LED_RED);
		break;

	case '4':
		kinect.setLed(ofxKinect::LED_BLINK_GREEN);
		break;

	case '5':
		kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
		break;

	case '0':
		kinect.setLed(ofxKinect::LED_OFF);
		break;

	case 'f':
		fullscreen = !fullscreen;
		setFullScreen();
		break;

	case OF_KEY_UP:
		angle++;
		if(angle>30) angle=30;
		kinect.setCameraTiltAngle(angle);
		break;

	case OF_KEY_DOWN:
		angle--;
		if(angle<-30) angle=-30;
		kinect.setCameraTiltAngle(angle);
		break;
	case 'c':
		showControls = !showControls;
		break;
	case ' ':
		gui->toggleVisible();
		break;

	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
