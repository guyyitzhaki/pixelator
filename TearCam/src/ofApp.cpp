#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup() {
	frameW = 640;
	frameH = 480;
//    ofSetVerticalSync(true);
	ofSetBackgroundAuto(false);
	ofSetFrameRate(30);
	configure = false;
#ifdef USE_VIDEO
	player.loadMovie("test.mov");
	player.setSpeed(1.0);
	player.play();
	tempImage.allocate(frameW, frameH);
#endif
#ifdef USE_KINECT
	kinect.setRegistration(false);
	// turn off video cam and texture to save cpu
	kinect.init(false, true);
	kinect.open();
	kinect.setLed(ofxKinect::LED_OFF);
#endif
#ifdef LIVE_CAM
	grabber.initGrabber(player.width, player.height);
#endif

	depthImage.allocate(frameW, frameH);
	img.allocate(ofGetWidth(), ofGetHeight());
	loadSettings();
	loadVideoData();

	bg.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
	slit.allocate(frameW, frameH, GL_RGB);
	pix.allocate(ofGetWidth(), ofGetHeight(), OF_PIXELS_RGB);
	slitpix.allocate(frameW, frameH, OF_PIXELS_RGB);
	colDepth.allocate(frameW, frameH);
	frame.allocate(frameW, frameH);


	ofHideCursor();
#ifdef __APPLE__
//        CGDisplayHideCursor(NULL); // <- OK
#endif

	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);
	curData = 1;
	code = "TEST";
	phase = PHASE_IDLE;
	if (debug)
        play();

}

void ofApp::setupBackground() {
	bg.begin();
	ofClear(0,0,0);
	bg.end();

	nLines = 50;
	lines.resize(nLines);

	noiseFactor = 2.0;

	int linePos = 0;
	int lineHeight = 10;
	float noiseStep = 0.064;
	for (int i=0; i<nLines; i++) {
		lines[i].x = 0;
		lines[i].y = linePos;
		lines[i].width = ofGetWidth();
		lines[i].height = lineHeight;
		lines[i].count = 0;
		lines[i].timer = 0;
		lines[i].active = false;
		lines[i].rtol = (ofRandom(1) > 0.5);
		noiseStep = ofRandom(0.128, 0.256);
		lines[i].noiseStep = noiseStep;
		for (int j=0; j < LINE_DATA_LENGTH; j++) {
			lines[i].data[j] = 1.0;
		}
		linePos += lineHeight + 6;
	}

	lines[int(ofRandom(nLines))].active = true;
	nextActivate = 4000;
	nextCount = 2;
	lastActivate = ofGetElapsedTimeMillis();

	for (int i = 0; i < nLines; i++) {
		float noiseStep = lines[i].noiseStep;
		for (int j = 0; j < LINE_DATA_LENGTH; j++) {
			float t = lines[i].timer * noiseStep;
			lines[i].timer++;
			lines[i].count++;
			lines[i].data[j] = ofNoise(t) * noiseFactor;
		}
	}

}

void ofApp::setupDelay() {
	timeDelay = 0;

	ofImage distortionMap;
	distortionMap.loadImage("delay.png");

	//set up a slit scan with a maximum capacity of frames
	// in the distortion buffer
	slitScan.setup(640, 480, 120);

	slitScan.setDelayMap(distortionMap);
	//blending means the edges between the scans are feathered
	slitScan.setBlending(true);

	//time delay is the deepest in history the delay can go
	//and width is the number of frames the distortion will encompass
	//note that the delay cannot be more than the total capacity
	slitScan.setTimeDelayAndWidth(timeDelay, 0);

}

void ofApp::play() {
	if (phase != PHASE_IDLE)
		stop();
	string name = videoData[curData].name + ".mov";
	tearname =  videoData[curData].name;
	cout << "loading " << name << endl;
	movie.loadMovie("movies/" + name);
	setupBackground();
	phase = PHASE_WAITING;
	sound.loadSound("sounds/" + videoData[curData].name + ".wav");
	sound.setMultiPlay(false);
	saved = false;
}

void ofApp::stop() {
	teardownMovie();
	if (sound.getIsPlaying()) {
		sound.stop();
	}
	phase = PHASE_IDLE;
}

//--------------------------------------------------------------
void ofApp::update() {
	checkTime();

#ifdef LIVE_CAM
	grabber.update();
#endif

	if (movie.isPlaying())
		movie.update();
#ifdef USE_VIDEO
	player.update();
	if (player.isFrameNew()) {
		tempImage.setFromPixels(player.getPixels(),player.width, player.height);
		depthImage = tempImage;
#endif
#ifdef USE_KINECT
		kinect.update();
		if(kinect.isFrameNew()) {
			depthImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
#endif
			grayThreshNear = depthImage;
			grayThreshFar = depthImage;
			grayThreshNear.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), depthImage.getCvImage(), NULL);

			switch (phase) {
			case PHASE_WAITING:
                contourFinder.findContours(depthImage, 1000, frameW * frameH / 2, 1, false);
    			if (contourFinder.nBlobs > 0) {
                    startRunning();
    			}
				break;
			case PHASE_LINES: {
				updateBackground();
				depthImage.invert();
				// update the cv images
				//depthImage.flagImageChanged();

				// convert background to image
				bg.readToPixels(pix);
				img.setFromPixels(pix);
				colDepth.setFromGrayscalePlanarImages(depthImage, depthImage, depthImage);
				colDepth.resize(ofGetWidth(), ofGetHeight());
				// mask background with depth image
				cvAnd(colDepth.getCvImage(), img.getCvImage(), img.getCvImage(), NULL);
				colDepth.resize(frameW, frameH);
			}
			break;
			case PHASE_MOVIE: {
				img.resize(frameW, frameH);
				ofSetColor(ofColor::white);
				frame.setFromPixels(movie.getPixels(), movie.getWidth(), movie.getHeight());
				colDepth.setFromGrayscalePlanarImages(depthImage, depthImage, depthImage);
				cvAnd(colDepth.getCvImage(), frame.getCvImage(), img.getCvImage(), NULL);
				img.resize(ofGetWidth(), ofGetHeight());
			}
			break;
			case PHASE_DELAY: {
				if (timeDelay < 120)
					timeDelay++;
				slitScan.setTimeDelayAndWidth(0, timeDelay);

				// for contour
				contourFinder.findContours(depthImage, 1000, frameW * frameH / 2, 1, false);
				slit.begin();
				ofClear(0,0,0);
				ofNoFill();
				ofSetColor(ofColor::whiteSmoke);
				for(int i = 0; i < contourFinder.nBlobs; i++) {
					ofBeginShape();
					for (int j = 0; j < contourFinder.blobs[i].nPts; j++) {
						ofVertex(contourFinder.blobs[i].pts[j].x, contourFinder.blobs[i].pts[j].y);
					}
					ofEndShape(true);
				}
				slit.end();

				slit.readToPixels(slitpix);
				slitScan.addImage(slitpix);

				// for depthimage
				//slitScan.addImage(player.getPixels());

				// for live cam
				//if (grabber.isFrameNew()) {
				//    slitScan.addImage(grabber);
				//}

			}
			break;
			}
			img.flagImageChanged();
		}
		if (!saved && (ofGetElapsedTimef() - startRun > SAVE_AFTER)) {
            saveImage(imgdir+"\\"+code+".png");
			saved = true;
		}
		checkMessages();
	}

	void ofApp::startRunning() {
        phase = PHASE_LINES;
        startRun = ofGetElapsedTimef();
        sound.play();
    }


	void ofApp::setupMovie() {
		movie.play();

	}

	void ofApp::teardownMovie() {
		if (movie.isPlaying()) {
			movie.stop();
			movie.close();
		}
	}

	void ofApp::checkTime() {
		float now = ofGetElapsedTimef();

		switch(phase) {
		case PHASE_LINES:
			if (now - startRun > videoData[curData].end_line_phase) {
				phase = PHASE_MOVIE;
				setupMovie();
				cout << "switching to movie\n";
			}
			break;
		case PHASE_MOVIE:
			if (now - startRun > videoData[curData].end_movie_phase) {
				teardownMovie();
				setupDelay();
				phase = PHASE_DELAY;
				cout << "switching to delay\n";
			}
			break;
		case PHASE_DELAY:
			if (now - startRun > videoData[curData].end_delay_phase) {
				phase = PHASE_IDLE;
				cout << "switching to idle\n";
			}
			break;
		}


	}

	void ofApp::updateBackground() {
		if (ofGetElapsedTimeMillis() - lastActivate > nextActivate) {
			for (int i = 0; i < nextCount; i++) {
				lines[int(ofRandom(0,nLines))].active = true;
			}
			nextActivate = ofRandom(500,3000);
			lastActivate = ofGetElapsedTimeMillis();
			nextCount = int(ofRandom(3,10));
		}

		for (int i=0; i<nLines; i++) {
			if (lines[i].active) {
				for (int k = 0; k < 15; k++) {
					// Push the older data to the end of the array
					float *data = (float *)lines[i].data;
					for (int j=(LINE_DATA_LENGTH-1); j>0; j--) {
						data[j] = data[j-1];
					}

					// Note how ofNoise() requires an argument ('t'); this is
					// the coordinate (on a one-dimensional axis) whose
					// corresponding noise value we wish to obtain.
					float noiseStep = lines[i].noiseStep;
					float t = lines[i].timer * noiseStep;
					lines[i].timer++;
					//data[0] = ofNoise(t)*2.0;
					data[0] = ofNoise(t)*noiseFactor;
					if (lines[i].count < LINE_DATA_LENGTH)
						lines[i].count++;


				}
			}
		}


		bg.begin();
		ofClear(0,0,0);
		for (int i=0; i<nLines; i++) {
			if (lines[i].active) {
				float x = lines[i].x;
				float y = lines[i].y;
				float w = lines[i].width;
				float h = lines[i].height;
				int count = lines[i].count;
				float noiseStep = lines[i].noiseStep;
				float *data = (float *) (lines[i].data);
				renderLine (x,y, w,h, noiseStep, data, count, lines[i].rtol);
			}
		}
		bg.end();

	}

	void ofApp::renderLine(float x, float y, float width, float height, float dt, float *data, int count, bool rtol) {
		ofPushMatrix();
		ofSetLineWidth(1);
		ofTranslate(x, y, 0);
		ofEnableSmoothing();
		// Draw the black line of the noise waveform
		ofNoFill();
		ofSetColor(ofColor::white);
		ofBeginShape();

		for (int i=0; i<count; i++) {
			float startx = rtol ? width : 0;
			float endx = rtol ? 0 : width;
			float px = ofMap(i, 0,(LINE_DATA_LENGTH-1), startx,endx);
			float py = height * data[i];
			ofVertex(px,py);
		}
		ofEndShape(false);
		ofSetLineWidth(1);

		ofPopMatrix();

	}

	void ofApp::checkMessages() {
		while(receiver.hasWaitingMessages()) {
			ofxOscMessage m;
			receiver.getNextMessage(&m);

			cout << "received" << m.getAddress() << endl;
			string playCmd = "/play";

			if (m.getAddress() == "/stop") {
				stop();
			} else if (m.getAddress().compare(0, playCmd.size(), playCmd) == 0) {
				string name = m.getArgAsString(0);
				bool found = false;
				for (int i = 0; i < videoData.size(); i++) {
					if (name == videoData[i].name) {
						found = true;
						curData = i;
					}
				}
				if (!found) {
					curData = -1;
					cout << "could not find ..."<<name<<"\n";
				}
				cout << "playing " << name << "\n";
				code = m.getArgAsString(1);
				play();
			}

		}
	}

	void ofApp::saveSettings() {
		ofxXmlSettings settings;
		settings.setValue("settings:nearThreshold", nearThreshold);
		settings.setValue("settings:farThreshold", farThreshold);
		settings.setValue("settings:angle", angle);
		settings.setValue("settings:imgdir", imgdir);
		settings.saveFile("settings.xml");
	}

	void ofApp::loadSettings() {
		ofxXmlSettings settings;
		settings.loadFile("settings.xml");
		imgdir = settings.getValue("settings:imgdir", "");
		nearThreshold = settings.getValue("settings:nearThreshold", 230);
		farThreshold = settings.getValue("settings:farThreshold", 70);
		angle = settings.getValue("settings:angle", 0);
#ifdef USE_KINECT
		kinect.setCameraTiltAngle(angle);
#endif

	}

	void ofApp::saveImage(string name) {
        ofImage screen;
        screen.setFromPixels(img.getPixelsRef());
        screen.saveImage(name);
	}

	void ofApp::loadVideoData() {
		ofxXmlSettings videos;
		videos.loadFile("videos.xml");
		videos.pushTag("videos");
		int nVideos = videos.getNumTags("video");
		for(int i = 0; i < nVideos; i++) {
			videos.pushTag("video", i);

			VideoData data;
			data.name = videos.getValue("name", "");
			data.end_line_phase = videos.getValue("end_line_phase", END_LINES_PHASE);
			data.end_movie_phase = videos.getValue("end_movie_phase", END_MOVIE_PHASE);
			data.end_delay_phase = videos.getValue("end_delay_phase", END_DELAY_PHASE);

			videoData.push_back(data);
			videos.popTag();
		}

		videos.popTag();

		cout << "Video Data\n";
		for (int i = 0; i < videoData.size(); i++) {
			cout << videoData[i].name << ": " << videoData[i].end_line_phase << "," << videoData[i].end_movie_phase << "," << videoData[i].end_delay_phase << endl;
		}
	}


//--------------------------------------------------------------
	void ofApp::draw() {
		if (configure) {
			ofSetColor(ofColor::white);
			depthImage.draw(0,0,ofGetWidth(), ofGetHeight());
			ofSetColor(ofColor::red);
			ofDrawBitmapString("near:" + ofToString(nearThreshold) + "(- / +)", 10, 10);
			ofDrawBitmapString("far:" + ofToString(farThreshold) + "(< / >)", 10, 30);
			ofDrawBitmapString("angle:" + ofToString(angle) + "(UP / DOWN)", 10, 50);
			return;
		}

		if ((phase == PHASE_IDLE) || (phase == PHASE_WAITING))  {
			ofSetColor(ofColor::black);
			ofFill();
			ofRect(0,0,ofGetWidth(), ofGetHeight());
			ofSetColor(ofColor::red);
			ofCircle(ofGetWidth()/2, ofGetHeight()/2, 15);
		} else if (phase == PHASE_DELAY) {
			slitScan.getOutputImage().draw(0, 0,ofGetWidth(), ofGetHeight());
		} else {
			img.draw(0,0, ofGetWidth(), ofGetHeight());
		}

		if (debug) {
			ofSetColor(ofColor::white);
			ofDrawBitmapString("total:" + ofToString(ofGetElapsedTimef(), 2)+"s", 10, 10);
			if (phase == PHASE_IDLE)
				ofDrawBitmapString("IDLE", 10,30);
            else if (phase == PHASE_WAITING)
				ofDrawBitmapString("WAITING", 10,30);
            else
				ofDrawBitmapString("current("+tearname+"):"+ ofToString(ofGetElapsedTimef()-startRun, 2)+"s, phase:" + ofToString(phase), 10, 30);
		}
	}

//--------------------------------------------------------------
	void ofApp::keyPressed(int key) {
		switch (key) {
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255)
				farThreshold = 255;
			break;

		case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0)
				farThreshold = 0;
			break;

		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255)
				nearThreshold = 255;
			break;
		case '-':
			nearThreshold --;
			if (nearThreshold < 0)
				nearThreshold = 0;
			break;
		case OF_KEY_UP:
			angle++;
			if(angle>30)
				angle=30;
#ifdef USE_KINECT
			kinect.setCameraTiltAngle(angle);
#endif
			break;
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30)
				angle=-30;
#ifdef USE_KINECT
			kinect.setCameraTiltAngle(angle);
#endif
			break;
		case 's':
			saveSettings();
			break;
		case 'l':
			loadSettings();
			break;
		case 'p':
			play();
			break;
		case 'x':
			stop();
			break;
		case 'i':
			saveImage("capture"+ofToString(ofGetElapsedTimef())+".png");
			break;
		case 'c':
			configure = !configure;
			break;
		case 'd':
			debug = !debug;
			break;

		}

	}

	void ofApp::exit() {
#ifdef USE_KINECT
		kinect.setCameraTiltAngle(0); // zero the tilt on exit
		kinect.close();
#endif
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
