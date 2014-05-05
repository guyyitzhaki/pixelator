#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofBackground(ofColor::white);
    kinect.setRegistration(false);

    // turn off video cam and texture to save cpu
    kinect.init(false, true);

    kinect.open();
    kinect.setCameraTiltAngle(6);
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
    ofxXmlSettings calibration;
    calibration.setValue("calibration:CAM.ANGLE", angle);
    calibration.setValue("calibration:CAM.X", camX);
    calibration.setValue("calibration:CAM.Y", camY);
    calibration.setValue("calibration:CAM.W", camW);
    calibration.setValue("calibration:CAM.H", camH);
    calibration.setValue("calibration:TL.X", points[TL].x);
    calibration.setValue("calibration:TL.Y", points[TL].y);
    calibration.setValue("calibration:TR.X", points[TR].x);
    calibration.setValue("calibration:TR.Y", points[TR].y);
    calibration.setValue("calibration:BR.X", points[BR].x);
    calibration.setValue("calibration:BR.Y", points[BR].y);
    calibration.setValue("calibration:BL.X", points[BL].x);
    calibration.setValue("calibration:BL.Y", points[BL].y);
    calibration.saveFile("calibration.xml");
}

void testApp::loadSettings()
{
    ofxXmlSettings calibration;
    calibration.loadFile("calibration.xml");
    int x = calibration.getValue("calibration:TL.X", points[TL].x);
    int y = calibration.getValue("calibration:TL.Y", points[TL].y);
    points[TL].x = x;
    points[TL].y = y;
    x = calibration.getValue("calibration:TR.X", points[TR].x);
    y = calibration.getValue("calibration:TR.Y", points[TR].y);
    points[TR].x = x;
    points[TR].y = y;
    x = calibration.getValue("calibration:BR.X", points[BR].x);
    y = calibration.getValue("calibration:BR.Y", points[BR].y);
    points[BR].x = x;
    points[BR].y = y;
    x = calibration.getValue("calibration:BL.X", points[BL].x);
    y = calibration.getValue("calibration:BL.Y", points[BL].y);
    points[BL].x = x;
    points[BL].y = y;

    angle = calibration.getValue("calibration:CAM.ANGLE", 0);
    kinect.setCameraTiltAngle(angle);

}



//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    switch(key) {
    case 's':
        saveSettings();
        break;
    case 'l':
        loadSettings();
        break;
    case OF_KEY_UP:
        if (current != -1) {
            points[current].y--;
        }
        break;
    case OF_KEY_DOWN:
        if (current != -1) {
            points[current].y++;
        }
        break;
    case OF_KEY_LEFT:
        if (current != -1) {
            points[current].x--;
        }
        break;
    case OF_KEY_RIGHT:
        if (current != -1) {
            points[current].x++;
        }
        break;
    case '+':
    case '=':
        angle++;
        if(angle>30) angle=30;
        kinect.setCameraTiltAngle(angle);
        break;
    case '-':
        angle--;
        if(angle<-30) angle=-30;
        kinect.setCameraTiltAngle(angle);
        break;
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
