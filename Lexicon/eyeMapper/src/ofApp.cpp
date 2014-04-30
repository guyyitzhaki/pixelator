#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    config = false;
    ofHideCursor();
    ofSetFrameRate(60);
    ofBackground(50);
    lastSwitch = ofGetElapsedTimef();
    switchEvery = 10;
    sliceWidth  = 72;
    sliceHeight = 64;
    loadVideos();

    _mapping = new ofxMtlMapping2D();
    _mapping->init(ofGetWidth(), ofGetHeight(), "mapping/xml/shapes.xml", "mapping/controls/mapping.xml");

}

//--------------------------------------------------------------
void ofApp::update()
{
    _mapping->update();
    for (int i = 0; i < NUM_VIDEOS; i++) {
        videos[i].update();
    }
    if (ofGetElapsedTimef() - lastSwitch > switchEvery)
    {
        lastSwitch = ofGetElapsedTimef();
        switchVideo();
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    // ----
    if (loaded)
    {
        _mapping->bind();
        ofBackground(0);
        ofSetColor(255);

        int xOffset = 0;
        int yOffset = 0;

        for (int i = 0; i < NUM_VIDEOS; i++)
        {
            videos[i].draw(xOffset,yOffset, sliceWidth,sliceHeight);
            xOffset += sliceWidth;
            if (xOffset >= ofGetWidth())
            {
                xOffset = 0;
                yOffset += sliceHeight;
            }
        }

        _mapping->unbind();
        _mapping->draw();
    }
}

void ofApp::loadVideos()
{
    vector<string> eyes = getVideos();
    loaded = !(eyes.size() == 0);
    if (loaded)
    {
        for (int i = 0; i < NUM_VIDEOS; i++)
        {
            int idx = (int) ofRandom(eyes.size());
            setupVideo(i, eyes[idx]);
        }
    }
}

vector<string> ofApp::getVideos() {
    vector<string> eyes;
    string path = "";
    ofDirectory dir(path);
    dir.listDir();
    string prefix = "eye";
    for(int i = 0; i < dir.numFiles(); i++)
    {
        if (dir.getPath(i).substr(0,prefix.size()) == prefix)
        {
            eyes.push_back(dir.getPath(i));
        }
    }
    return eyes;
}

string ofApp::getRandomVideo() {
    vector<string> eyes = getVideos();
    int idx = (int) ofRandom(eyes.size());
    return eyes[idx];
}


void ofApp::switchVideo()
{
    int playeridx = (int) ofRandom(NUM_VIDEOS);
    string path = getRandomVideo();
    videos[playeridx].closeMovie();
    setupVideo(playeridx, path);
}

void ofApp::setupVideo(int idx, string path)
{
    cout << "[" << idx << "] " << path << endl;
    videos[idx].loadMovie(path);
    videos[idx].setSpeed(1.0);
    videos[idx].play();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == 'm') {
        config = !config;
        if (config) {
            ofShowCursor();
        }
        else {
            ofHideCursor();
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}
