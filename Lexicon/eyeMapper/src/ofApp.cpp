#include "ofApp.h"

VideoPart::VideoPart() {
}

VideoPart::~VideoPart() {
}

void VideoPart::init(string path, int _w, int _h) {
    w = _w;
    h = _h;
    play(path);
}

void VideoPart::play(string path) {
    player.loadMovie(path);
    player.setSpeed(1.0);
    player.play();
}

void VideoPart::update()  {
    if (targetx != x) {
        if (targetx > x)
            x++;
        else
            x--;
    }
    if (targety != y) {
        if (targety > y)
            y++;
        else
            y--;
    }
    player.update();
}

void VideoPart::draw()  {
    player.draw(x, y, w, h);
}

void VideoPart::place(int _x, int _y) {
    x = _x;
    y = _y;
    targetx = x;
    targety = y;
}

void VideoPart::switchTo(string path) {
    player.closeMovie();
    play(path);
}

void VideoPart::setTarget(int _x, int _y) {
    targetx = _x;
    targety = _y;
}

//--------------------------------------------------------------
void ofApp::setup()
{
    config = false;
    ofHideCursor();
    ofSetFrameRate(60);
    ofBackground(50);
    lastSwitch = ofGetElapsedTimef();
    switchEvery = 10;
    sliceWidth  = 64;
    sliceHeight = 48;
    loadVideos();

    _mapping = new ofxMtlMapping2D();
    _mapping->init(ofGetWidth(), ofGetHeight(), "mapping/xml/shapes.xml", "mapping/controls/mapping.xml");

}

//--------------------------------------------------------------
void ofApp::update()
{
    _mapping->update();
    for (int i = 0; i < NUM_VIDEOS; i++) {
  //      videos[i].update();
        parts[i].update();
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

        for (int i = 0; i < NUM_VIDEOS; i++) {
            parts[i].draw();
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
        int xOffset = 0;
        int yOffset = 0;
        for (int i = 0; i < NUM_VIDEOS; i++)
        {
            int idx = (int) ofRandom(eyes.size());
            setupVideo(i, eyes[idx]);
            parts[i].place(xOffset,yOffset);
            xOffset += sliceWidth;
            if (xOffset >= ofGetWidth())
            {
                xOffset = 0;
                yOffset += sliceHeight;
            }
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

    int idx = (int) ofRandom(NUM_VIDEOS);
    string path = getRandomVideo();
    parts[idx].switchTo(path);
    cout << "switched " << idx << " to " << path << endl;
}

void ofApp::setupVideo(int idx, string path)
{
    cout << "[" << idx << "] " << path << endl;
    parts[idx].init(path, sliceWidth, sliceHeight);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

    switch(key) {
    case 'm':
        config = !config;
        if (config) {
            ofShowCursor();
        }
        else {
            ofHideCursor();
        }
        break;
     case 'x': {
        for (int i = 0; i < NUM_VIDEOS; i++) {
            parts[i].setTarget(ofRandom(ofGetWidth()),ofRandom(ofGetHeight()));
        }
        break;

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
