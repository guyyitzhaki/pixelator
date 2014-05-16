#include "ofApp.h"

VideoPart::VideoPart()
{
    paused = false;
    migrate = false;
}

VideoPart::~VideoPart()
{
}

void VideoPart::init(string path, int _w, int _h)
{
    w = _w;
    h = _h;
    play(path);
}

void VideoPart::play(string path)
{
    player.loadMovie(path);
    player.setSpeed(1.0);
    player.play();
}

void VideoPart::update()
{
    if (migrate)
    {
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

void VideoPart::draw()
{
    if (paused)
        return;
    player.draw(x, y, w, h);
}

void VideoPart::place(int _x, int _y)
{
    x = _x;
    y = _y;
    targetx = x;
    targety = y;
    sourcex = x;
    sourcey = y;
}

void VideoPart::switchTo(string path)
{
    player.closeMovie();
    play(path);
}

void VideoPart::setTarget(int _x, int _y)
{
    sourcex = x;
    sourcey = y;
    targetx = _x;
    targety = _y;
    migrate = true;
    time = 0.0;
}

void VideoPart::setPaused(bool b)
{
    paused = b;
}

void VideoPart::pause()
{
    setPaused(true);
}

void VideoPart::unpause()
{
    setPaused(false);
}

bool VideoPart::isPaused()
{
    return paused;
}

//--------------------------------------------------------------
void ofApp::setup()
{
    useMtlMapping = USE_MTL_MAPPING;
    config = false;
    ofHideCursor();
    ofBackground(50);
    lastSwitch = ofGetElapsedTimef();
    loadSettings();
    loadMasks();
    loadVideos();
    receiver.setup(PORT);

    pauseMode = false;

    if (useMtlMapping)
    {
        _mapping = new ofxMtlMapping2D();
        _mapping->init(ofGetWidth(), ofGetHeight(), "mapping/xml/shapes.xml", "mapping/controls/mapping.xml");
    }

}

void ofApp::loadMasks()
{
    ofBuffer buffer = ofBufferFromFile("shapes.txt");
    while (!buffer.isLastLine())
    {
        string line = buffer.getNextLine();
        cout << line << endl;
        vector<string> tokens = ofSplitString(line, " ");
        string id = tokens[0];
        string type = tokens[1];
        bool mask = ofToBool(tokens[2]);
        CShape *newShape;
        if (type == "line")
        {
            CPoint p1 = getPoint(tokens[3], tokens[4]);
            CPoint p2 = getPoint(tokens[5], tokens[6]);
            newShape = new CLine(p1,p2,false);
        }
        if (type == "polygon")
        {
            newShape = new CPolygon(false);
            for (int i = 3; i < tokens.size()-1; i+=2)
            {
                CPoint p = getPoint(tokens[i], tokens[i+1]);
                ((CPolygon *)newShape)->add(p);
            }
        }
        newShape->setMask(mask);
        if (newShape->isMask())
            masks.push_back(newShape);
        else
        {
            cout << "ignoring shape" << newShape->getId();
        }
    }
}

void ofApp::pausePart()
{
    lastPause = ofGetElapsedTimeMillis();
    pauseGap = ofRandom(minPause, maxPause);

    vector<int> nums;
    for (int i = 0; i < NUM_VIDEOS; i++)
    {
        nums.push_back(i);
    }
    random_shuffle(nums.begin(), nums.end());
    bool found = false;
    for (int  i = 0; i < NUM_VIDEOS; i++)
    {
        int idx = nums[i];
        if (!parts[idx].isPaused())
        {
            parts[idx].pause();
            found = true;

            cout << "pausing "<< idx << endl;
            break;
        }

    }
    if (!found)
    {
        pauseMode = false;
        for (int i = 0; i < NUM_VIDEOS; i++)
        {
            parts[i].unpause();
        }
    }



}

//--------------------------------------------------------------
void ofApp::update()
{
    if (useMtlMapping)
    {
        _mapping->update();
    }
    for (int i = 0; i < NUM_VIDEOS; i++)
    {
        parts[i].update();
    }
    if (pauseMode)
    {
        if (ofGetElapsedTimeMillis() - lastPause > pauseGap)
        {
            pausePart();
        }
    }
    if (ofGetElapsedTimef() - lastSwitch > switchEvery)
    {
        lastSwitch = ofGetElapsedTimef();
        switchVideo();
    }

    while(receiver.hasWaitingMessages())
    {
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        handleMessage(m.getAddress());
        cout << m.getAddress() << endl;
    }
}

void ofApp::handleMessage(string msg)
{
    if (msg == "/shuffle")
    {
        shuffle();
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    if (loaded)
    {
        if (useMtlMapping)
        {
            _mapping->bind();
        }
        ofBackground(0);
        ofSetColor(ofColor::white);

        for (int i = 0; i < NUM_VIDEOS; i++)
        {
            parts[i].draw();
        }

        if (useMtlMapping)
        {
            _mapping->unbind();
            _mapping->draw();
        }
        else
        {
            ofSetColor(ofColor::black);
            for (unsigned int i = 0; i < masks.size(); i++)
            {
                masks[i]->draw();
            }
        }
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

vector<string> ofApp::getVideos()
{
    vector<string> eyes;
    ofDirectory dir(videoPath);
    dir.allowExt("mp4");
    dir.allowExt("mov");
    dir.listDir();
    string prefix = "eye";
    for(int i = 0; i < dir.numFiles(); i++)
    {
        if (dir.getName(i).substr(0,prefix.size()) == prefix)
        {
            eyes.push_back(dir.getPath(i));
        }
    }
    return eyes;
}

string ofApp::getRandomVideo()
{
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
//	cout << "[" << idx << "] " << path << endl;
    parts[idx].init(path, sliceWidth, sliceHeight);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

    switch(key)
    {
    case 'm':
        config = !config;
        if (config)
        {
            ofShowCursor();
        }
        else
        {
            ofHideCursor();
        }
        break;
    case 'P':
        for (int i = 0; i < NUM_VIDEOS; i++)
        {
            parts[i].setPaused(!parts[i].isPaused());
        }
        break;

    case 'p':
        pauseMode = !pauseMode;
        if (pauseMode)
        {
            pausePart();
        }
        break;

    case 'x':
    {
        shuffle();
        break;

    }
    }
}

void ofApp::shuffle()
{
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

}

void ofApp::loadSettings()
{
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
