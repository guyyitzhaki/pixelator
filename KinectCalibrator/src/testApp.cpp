#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofBackground(ofColor::white);
    kinect.setRegistration(false);

    // turn off video cam and texture to save cpu
    kinect.init(false, true);

    kinect.open();
    ofSetFullscreen(true);
    int gap = 20;
    points[TL] = ofPoint(gap,gap);
    points[TR] = ofPoint(ofGetWidth() - gap,gap);
    points[BR] = ofPoint(ofGetWidth() - gap, ofGetHeight() - gap);
    points[BL] = ofPoint(gap, ofGetHeight() - gap);

    camW = ofGetWidth();
    camH = ofGetHeight();
    camX = 0;
    camY = 0;

}

//--------------------------------------------------------------
void testApp::update()
{
    kinect.update();
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofSetColor(ofColor::white);
    kinect.draw(camX, camY, camW, camH);

    int mx = ofGetMouseX();
    int my = ofGetMouseY();

    ofSetColor(ofColor::black);
    stringstream str;
    str << mx << "," << my << endl;
    str << (mx - camX) << "," << (my - camY) << endl;
    ofDrawBitmapString(str.str(), 10,10);

    ofPolyline area;
    for (int i = 0; i < 4; i++)
    {
        area.addVertex(points[i]);
    }
    area.close();

    area.draw();

    ofNoFill();
    for (int i = 0; i < 4; i++)
    {
        if (i == current)
            ofSetColor(ofColor::red);
        else
            ofSetColor(ofColor::gray);


        ofCircle(points[i], 10);
    }

}

void testApp::saveSettings()
{
    ofxXmlSettings settings;
    settings.setValue("settings:CAM.X", camX);
    settings.setValue("settings:CAM.Y", camY);
    settings.setValue("settings:CAM.W", camW);
    settings.setValue("settings:CAM.H", camH);
    settings.setValue("settings:TL.X", points[TL].x);
    settings.setValue("settings:TL.Y", points[TL].y);
    settings.setValue("settings:TR.X", points[TR].x);
    settings.setValue("settings:TR.Y", points[TR].y);
    settings.setValue("settings:BR.X", points[BR].x);
    settings.setValue("settings:BR.Y", points[BR].y);
    settings.setValue("settings:BL.X", points[BL].x);
    settings.setValue("settings:BL.Y", points[BL].y);
    settings.saveFile("kinect.xml");
}

void testApp::loadSettings()
{
    ofxXmlSettings settings;
    settings.loadFile("kinect.xml");
    int x = settings.getValue("settings:TL.X", points[TL].x);
    int y = settings.getValue("settings:TL.Y", points[TL].y);
    points[TL].x = x;
    points[TL].y = y;
    x = settings.getValue("settings:TR.X", points[TR].x);
    y = settings.getValue("settings:TR.Y", points[TR].y);
    points[TR].x = x;
    points[TR].y = y;
    x = settings.getValue("settings:BR.X", points[BR].x);
    y = settings.getValue("settings:BR.Y", points[BR].y);
    points[BR].x = x;
    points[BR].y = y;
    x = settings.getValue("settings:BL.X", points[BL].x);
    y = settings.getValue("settings:BL.Y", points[BL].y);
    points[BL].x = x;
    points[BL].y = y;
}



//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    if (key == 's') {
        saveSettings();
    }
    else if (key == 'l') {
        loadSettings();
    }
    else if (key == OF_KEY_UP) {
        if (current != -1) {
            points[current].y--;
        }
    }
    else if (key == OF_KEY_DOWN) {
        if (current != -1) {
            points[current].y++;
        }
    }
    else if (key == OF_KEY_LEFT) {
        if (current != -1) {
            points[current].x--;
        }
    }
    else if (key == OF_KEY_RIGHT) {
        if (current != -1) {
            points[current].x++;
        }
    }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
    if (dragging)
    {
        if (current != -1)
        {
            points[current].x = x;
            points[current].y = y;
        }
    }

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    for (int i = 0; i < 4; i++)
    {
        if ((abs(x - points[i].x)) < 10 && (abs(y - points[i].y) < 10))
        {
            current = i;
            dragging = true;
        }
    }

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
    dragging = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}
