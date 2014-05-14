#include "ofApp.h"

VideoPart::VideoPart() {
    paused = false;
    migrate = false;
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
    if (migrate) {
        time += 0.05;
        x = (1.0 - time) * sourcex + time * targetx;
        y = (1.0 - time) * sourcey + time * targety;
        if (time >= 1.0)
        {
            migrate = false;
            x = targetx;
            y = targety;
            sourcex = x;
            sourcey = y;
        }

    }
    player.update();
}

void VideoPart::draw()  {
    if (paused)
        return;
    player.draw(x, y, w, h);
}

void VideoPart::place(int _x, int _y) {
    x = _x;
    y = _y;
    targetx = x;
    targety = y;
    sourcex = x;
    sourcey = y;
}

void VideoPart::switchTo(string path) {
    player.closeMovie();
    play(path);
}

void VideoPart::setTarget(int _x, int _y) {
    sourcex = x;
    sourcey = y;
    targetx = _x;
    targety = _y;
    migrate = true;
    time = 0.0;
}

void VideoPart::setPaused(bool b) {
    paused = b;
}

void VideoPart::pause() {
    setPaused(true);
}

void VideoPart::unpause() {
    setPaused(false);
}

bool VideoPart::isPaused() {
    return paused;
}

//--------------------------------------------------------------
void ofApp::setup()
{
    config = false;
    ofHideCursor();
    ofSetFrameRate(60);
    ofBackground(50);
    lastSwitch = ofGetElapsedTimef();
    loadSettings();
    loadVideos();
    pauseMode = false;

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
    if (pauseMode) {
        int idx = ofRandom(0,NUM_VIDEOS);
        parts[idx].pause();
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
    ofDirectory dir(videoPath);
    cout << dir.exists() << endl;
    dir.allowExt("mp4");
    dir.allowExt("mov");
    dir.listDir();
    string prefix = "eye";
    for(int i = 0; i < dir.numFiles(); i++)
    {
        cout << dir.getName(i) << endl;
        if (dir.getName(i).substr(0,prefix.size()) == prefix)
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
    case 'P':
        for (int i = 0; i < NUM_VIDEOS; i++) {
            parts[i].setPaused(!parts[i].isPaused());
        }
        break;

    case 'p':
        pauseMode = !pauseMode;
        break;

    case 'x': {
        vector<int> nums;
        for (int i = 0; i < NUM_VIDEOS; i++)
        {
            nums.push_back(i);
        }
        random_shuffle(nums.begin(), nums.end());


        int xOffset = 0;
        int yOffset = 0;
        for (int i = 0; i < NUM_VIDEOS; i++)
        {
            parts[nums[i]].setTarget(xOffset,yOffset);
            xOffset += sliceWidth;
            if (xOffset >= ofGetWidth())
            {
                xOffset = 0;
                yOffset += sliceHeight;
            }
        }
        break;

     }
    }
}

void ofApp::loadSettings() {
    ofxXmlSettings settings;
    settings.load("settings.xml");
    videoPath = settings.getValue("settings:videoPath", "c:/temp");
    minPause = settings.getValue("settings:minPause", 2);
    maxPause = settings.getValue("settings:maxPause", 10);
    switchEvery = settings.getValue("settings:switchEvery", 10);
    sliceWidth = settings.getValue("settings:sliceWidth", 64);
    sliceHeight = settings.getValue("settings:sliceHeight", 48);
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
