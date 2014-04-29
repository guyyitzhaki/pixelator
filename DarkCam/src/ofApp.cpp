#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    showControls = true;
    fullscreen = false;
    setFullScreen();
    ofBackground(ofColor::black);

    // enable depth->video image calibration?
    kinect.setRegistration(false);

    // turn off video cam to save cpu
    kinect.init(false, true);

    kinect.open();

    depthImage.allocate(kinect.width, kinect.height);

    ofSetFrameRate(60);

    loadSettings();


}

void ofApp::setFullScreen()
{
    ofSetFullscreen(fullscreen);
    if (fullscreen)
        ofHideCursor();
    else
        ofShowCursor();
}

//--------------------------------------------------------------
void ofApp::update()
{

    kinect.update();
    if(kinect.isFrameNew())
    {

        // load grayscale depth image from the kinect source
        depthImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);

        // we do two thresholds - one for the far plane and one for the near plane
        // we then do a cvAnd to get the pixels which are a union of the two thresholds
        grayThreshNear = depthImage;
        grayThreshFar = depthImage;
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), depthImage.getCvImage(), NULL);

        // update the cv images
        depthImage.flagImageChanged();

        contourFinder.findContours(depthImage, 50, (kinect.width*kinect.height)/2, 2, false);
    }


}

//--------------------------------------------------------------
void ofApp::draw()
{
    if (showControls)
    {
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
    }
    else
    {
        for(int i = 0; i < contourFinder.nBlobs; i++)
        {
            ofRectangle r = contourFinder.blobs.at(i).boundingRect;
            r.x = ofMap(r.x, 0, kinect.width, 0, ofGetWidth());
            r.y = ofMap(r.y, 0, kinect.height, 0, ofGetHeight());
            r.setWidth(ofMap(r.getWidth(), 0, kinect.width, 0, ofGetWidth()));
            r.setHeight(ofMap(r.getHeight(), 0, kinect.height, 0, ofGetHeight()));
            ofNoFill();
            ofSetColor(ofColor::white);
            ofRect(r);
            float centerX = r.x + r.getWidth()/2;
            float centerY = r.y + r.getHeight()/2;

            ofSetColor(ofColor::red);
            ofLine(centerX - 15, centerY, centerX + 15, centerY);
            ofLine(centerX, centerY - 15, centerX, centerY + 15);
            ofCircle(centerX, centerY, 10);
        }
    }

}

void ofApp::exit()
{
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();
}

void ofApp::saveSettings()
{
    ofxXmlSettings settings;
    settings.setValue("settings:nearThreshold", nearThreshold);
    settings.setValue("settings:farThreshold", farThreshold);
    settings.setValue("settings:angle", angle);
    settings.setValue("settings:led", ledOn);
    settings.saveFile("settings.xml");
}

void ofApp::loadSettings()
{
    ofxXmlSettings settings;
    settings.loadFile("settings.xml");
    nearThreshold = settings.getValue("settings:nearThreshold", 230);
    farThreshold = settings.getValue("settings:farThreshold", 70);
    angle = settings.getValue("settings:angle", 0);
    kinect.setCameraTiltAngle(angle);
    bool on = settings.getValue("settings:led", false);
    if (!on)
        kinect.setLed(ofxKinect::LED_OFF);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    switch (key)
    {
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
