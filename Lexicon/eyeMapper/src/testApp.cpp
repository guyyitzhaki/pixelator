#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetFrameRate(30);
	ofBackground(50);
	lastSwitch = ofGetElapsedTimef();
	switchEvery = 10;
	sliceWidth  = 64;
	sliceHeight = 48;
	loadImageSequences();
	// ----
	_mapping = new ofxMtlMapping2D();
	_mapping->init(ofGetWidth(), ofGetHeight(), "mapping/xml/shapes.xml", "mapping/controls/mapping.xml");
}

//--------------------------------------------------------------
void testApp::update() {
	_mapping->update();
	if (ofGetElapsedTimef() - lastSwitch > switchEvery) {
		lastSwitch = ofGetElapsedTimef();
		switchSequence();
		cout << "switching";
	}
}

//--------------------------------------------------------------
void testApp::draw() {
	// ----
	_mapping->bind();
	if (loaded) {
		int xOffset = 0;
		int yOffset = 0;

		for (int i = 0; i < NUM_VIDEOS; i++) {
			sequences[i].getFrameForTime(ofGetElapsedTimef())->draw(xOffset,yOffset, sliceWidth,sliceHeight);
			xOffset += sliceWidth;
			if (xOffset >= ofGetWidth()) {
				xOffset = 0;
				yOffset += sliceHeight;
			}

		}
		/*   int gridSize = 64;
		    for (int x = 0; x < ofGetWidth(); x += gridSize) {
		        for (int y = 0; y < ofGetHeight(); y += gridSize) {
		            ofFill();
		            ofSetColor(ofColor::red);
		            ofRect(x, y, gridSize, gridSize);
		            ofNoFill();
		            ofSetColor(ofColor::black);
		            ofRect(x, y, gridSize, gridSize);
		        }
		    }
		*/
	}
	_mapping->unbind();
	_mapping->draw();
}

void testApp::loadImageSequences() {
	vector<string> directories;
	string path = "";
	ofDirectory dir(path);
	dir.listDir();
	int maxIndex = 0;
	string prefix = "recording";
	for(int i = 0; i < dir.numFiles(); i++) {
		if (dir.getPath(i).substr(0,prefix.size()) == prefix) {
			directories.push_back(dir.getPath(i));
			string num = dir.getPath(i).substr(prefix.size());
			int idx = ofToInt(num);
			if (idx > maxIndex)
				maxIndex = idx;
			loaded = true;
		}
	}
	cout << "max index = " << maxIndex << "\n";
	if (loaded) {
		for (int i = 0; i < NUM_VIDEOS; i++) {
			int idx = (int) ofRandom(directories.size());
			sequences[i].loadSequence(directories[idx]);
			sequences[i].preloadAllFrames();
			sequences[i].setFrameRate(15);
		}
	}
	index = maxIndex + 1;

}

void testApp::switchSequence() {
	int seqidx = (int) ofRandom(NUM_VIDEOS);
	int vididx = (int) ofRandom(1,index-2);
	char path[100];
	sprintf(path,  "%s%.4i" , "recording", vididx);
	cout << "loading " << path << " to " << seqidx << loaded << "\n";
	sequences[seqidx].unloadSequence();
	sequences[seqidx].loadSequence(path);
	sequences[seqidx].preloadAllFrames();

}


//--------------------------------------------------------------
void testApp::keyPressed(int key) {
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo) {

}
