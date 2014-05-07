#include "ofApp.h"

GLdouble modelviewMatrix[16], projectionMatrix[16];
GLint viewport[4];
void updateProjectionState()
{
    glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
}

ofVec3f ofWorldToScreen(ofVec3f world)
{
    updateProjectionState();
    GLdouble x, y, z;
    gluProject(world.x, world.y, world.z, modelviewMatrix, projectionMatrix, viewport, &x, &y, &z);
    ofVec3f screen(x, y, z);
    screen.y = ofGetHeight() - screen.y;
    return screen;
}

ofMesh getProjectedMesh(const ofMesh& mesh)
{
    ofMesh projected = mesh;
    for(int i = 0; i < mesh.getNumVertices(); i++)
    {
        ofVec3f cur = ofWorldToScreen(mesh.getVerticesPointer()[i]);
        cur.z = 0;
        projected.setVertex(i, cur);
    }
    return projected;
}

template <class T>
void addTexCoords(ofMesh& to, const vector<T>& from)
{
    for(int i = 0; i < from.size(); i++)
    {
        to.addTexCoord(from[i]);
    }
}


using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetVerticalSync(true);
    gui = new ofxUICanvas();
    gui->addLabel("FACES", OFX_UI_FONT_LARGE);
    gui->addButton("BIBI", false, 64, 64);
    gui->addImageButton("BIBI2", "faces/bibi.jpg", false, 64, 64);

    loadSettings();

    camX = ofGetWidth()/2 - camWidth / 2;
    camY = ofGetHeight() / 2 - camHeight / 2;

    vector<ofVideoDevice> devices = cam.listDevices();
    for(int i = 0; i < devices.size(); i++)
    {
        cout << devices[i].id << ": " << devices[i].deviceName;
        if( devices[i].bAvailable )
        {
            cout << endl;
        }
        else
        {
            cout << " - unavailable " << endl;
        }
    }

    cam.setDeviceID(deviceId);
    cam.initGrabber(camWidth,camHeight);

    loaded = false;
    recording = true;
    frameCount = 0;

    cleanup();
    setupRecorder();

    tracker.setup();
    eyeFbo.allocate(sliceWidth, sliceHeight, GL_RGB);

    cloneReady = false;
    clone.setup(cam.getWidth(), cam.getHeight());
    ofFbo::Settings settings;
    settings.width = cam.getWidth();
    settings.height = cam.getHeight();
    maskFbo.allocate(settings);
    srcFbo.allocate(settings);

    srcTracker.setup();
    srcTracker.setIterations(25);
    srcTracker.setAttempts(4);

    faces.allowExt("jpg");
    faces.allowExt("png");
    faces.listDir("faces");
    currentFace = 0;
    if(faces.size()!=0)
    {
        loadFace(faces.getPath(currentFace));
    }

    ofBackground(0);

}

void ofApp::cleanup()
{
    ofDirectory dir("");
    dir.allowExt("mov");
    dir.listDir();
    ofFile doomed;
    for(int i = 0; i < dir.numFiles(); i++)
    {
        doomed.removeFile(dir.getPath(i));
    }
}

void ofApp::loadFace(string face)
{
    src.loadImage(face);
    if(src.getWidth() > 0)
    {
        srcTracker.update(toCv(src));
        srcPoints = srcTracker.getImagePoints();
    }
    cout << "loaded " << face << endl;
}

void ofApp::setupRecorder()
{
    left = !left;
    recorder = new ofxQtVideoSaver();
    recorder->setCodecQualityLevel(OF_QT_SAVER_CODEC_QUALITY_NORMAL);
    filename = ofGetTimestampString("%m%d%H%M%S") + ".mov";
    recorder->setup(sliceWidth, sliceHeight, filename);
}

//--------------------------------------------------------------
void ofApp::update()
{
    cam.update();
    if(cam.isFrameNew())
    {
        tracker.update(toCv(cam));
        position = tracker.getPosition();
        scale = tracker.getScale();
        rotationMatrix = tracker.getRotationMatrix();

        cloneReady = tracker.getFound();
        if(cloneReady)
        {
            if (recording)
            {
                captureEye();
            }
            substituteFace();
        }
    }

    float now = ofGetElapsedTimef();
    for (int i = finished.size() - 1; i >= 0; i--)
    {
        if (now - finished[i]->time > WRITE_DELAY)
        {
            ofFile f(finished[i]->filename);
            f.copyTo("done\\eye" + finished[i]->filename);
            delete finished[i];
            finished.erase(finished.begin() + i);
        }
    }

}

void ofApp::substituteFace()
{
    ofMesh camMesh = tracker.getImageMesh();
    camMesh.clearTexCoords();
    camMesh.addTexCoords(srcPoints);

    maskFbo.begin();
    ofClear(0, 255);
    camMesh.draw();
    maskFbo.end();

    srcFbo.begin();
    ofClear(0, 255);
    src.bind();
    camMesh.draw();
    src.unbind();
    srcFbo.end();

    clone.setStrength(16);
    clone.update(srcFbo.getTextureReference(), cam.getTextureReference(), maskFbo.getTextureReference());

}

void ofApp::captureEye()
{
    ofVec2f
    leftOuter = tracker.getImagePoint(36),
    leftInner = tracker.getImagePoint(39),
    rightInner = tracker.getImagePoint(42),
    rightOuter = tracker.getImagePoint(45);

    ofPolyline leftEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
    ofPolyline rightEye = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE);

    ofVec2f leftCenter = leftEye.getBoundingBox().getCenter();
    ofVec2f rightCenter = rightEye.getBoundingBox().getCenter();

    float leftRadius = (leftCenter.distance(leftInner) + leftCenter.distance(leftOuter)) / 2;
    float rightRadius = (rightCenter.distance(rightInner) + rightCenter.distance(rightOuter)) / 2;

    ofVec2f
    leftOuterObj = tracker.getObjectPoint(36),
    leftInnerObj = tracker.getObjectPoint(39),
    rightInnerObj = tracker.getObjectPoint(42),
    rightOuterObj = tracker.getObjectPoint(45);

    ofVec3f upperBorder(0, -3.5, 0), lowerBorder(0, 2.5, 0);
    ofVec3f leftDirection(-1, 0, 0), rightDirection(+1, 0, 0);
    float innerBorder = 1.5, outerBorder = 2.5;

    ofMesh leftRect, rightRect;
    leftRect.setMode(OF_PRIMITIVE_LINE_LOOP);
    leftRect.addVertex(leftOuterObj + upperBorder + leftDirection * outerBorder);
    leftRect.addVertex(leftInnerObj + upperBorder + rightDirection * innerBorder);
    leftRect.addVertex(leftInnerObj + lowerBorder + rightDirection * innerBorder);
    leftRect.addVertex(leftOuterObj + lowerBorder + leftDirection * outerBorder);
    rightRect.setMode(OF_PRIMITIVE_LINE_LOOP);
    rightRect.addVertex(rightInnerObj+ upperBorder + leftDirection * innerBorder);
    rightRect.addVertex(rightOuterObj + upperBorder + rightDirection * outerBorder);
    rightRect.addVertex(rightOuterObj + lowerBorder + rightDirection * outerBorder);
    rightRect.addVertex(rightInnerObj + lowerBorder + leftDirection * innerBorder);

    ofPushMatrix();
//    ofSetOrientation(OF_ORIENTATION_UNKNOWN, true);
//    ofSetupScreenPerspective(640,480,60, -1000,1000);
    ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
    ofScale(1, 1, -1);
    ofTranslate(position);
    applyMatrix(rotationMatrix);
    ofScale(scale, scale, scale);
    leftRectImg = getProjectedMesh(leftRect);
    rightRectImg = getProjectedMesh(rightRect);
    ofPopMatrix();

    ofMesh normRect, normLeft, normRight;
    normRect.addVertex(ofVec2f(0, 0));
    normRect.addVertex(ofVec2f(sliceWidth, 0));
    normRect.addVertex(ofVec2f(sliceWidth, sliceHeight));
    normRect.addVertex(ofVec2f(0, sliceHeight));
    normLeft.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    normRight.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    normLeft.addVertices(normRect.getVertices());
    normRight.addVertices(normRect.getVertices());
    addTexCoords(normLeft, leftRectImg.getVertices());
    addTexCoords(normRight, rightRectImg.getVertices());

    if (recording)
    {
        eyeFbo.begin();
        ofSetColor(255);
        ofFill();
        cam.getTextureReference().bind();
        if (left)
        {
            normLeft.draw();
        }
        else
        {
            normRight.draw();
        }
        cam.getTextureReference().unbind();
        eyeFbo.end();

        ofPixels pixels;
        ofImage eyeImage;
        eyeImage.allocate(sliceWidth, sliceHeight, OF_IMAGE_COLOR);
        eyeFbo.readToPixels(pixels);
        eyeImage.setFromPixels(pixels);

        if (frameCount == 0)
        {
            blinkOn = true;
        }
        recorder->addFrame(eyeImage.getPixels(), 1/16.0f);
        frameCount++;
        //cout << frameCount << " ";

        if (ofGetElapsedTimeMillis() - blinkTime > 500)
        {
            blinkOn = !blinkOn;
            blinkTime = ofGetElapsedTimeMillis();
        }
        if (frameCount == MAX_FRAMES)
        {
            recording = false;
            blinkOn = false;
            recorder->finishMovie();
            frameCount = 0;
            video_data *vd = new video_data();
            vd->filename = filename;
            vd->time = ofGetElapsedTimef();
            finished.push_back(vd);
            filename = "";
            cout <<  "done at " << ofGetElapsedTimef() << "\n";
        }
    }

}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetColor(ofColor::white);
    ofTranslate(camX,camY);
    //cam.draw(0,0);
    //tracker.draw(true);

    if(src.getWidth() > 0 && cloneReady)
    {
        clone.draw(0, 0);
    }
    else
    {
        cam.draw(0, 0);
    }

    if (blinkOn)
    {
        ofSetColor(ofColor::red);
        ofFill();
        ofCircle(40,40,10,10);
    }
    ofTranslate(-camX,-camY);


    ofSetColor(ofColor::white);
    stringstream reportStream;
    reportStream << "fps " << ofGetFrameRate() << endl;
    reportStream << "file " << filename << endl;
    ofDrawBitmapString(reportStream.str(), 20,20);
}

void ofApp::updateFace()
{
    currentFace = ofClamp(currentFace,0,faces.size()-1);
    if(faces.size()!=0)
    {
        loadFace(faces.getPath(currentFace));
    }

}

void ofApp::captureImage() {
    ofImage capture;
    capture.setFromPixels(cam.getPixels(), camWidth, camHeight, OF_IMAGE_COLOR);
    capture.saveImage("face"+ofGetTimestampString("%n%e%H%M%S") + ".png");
}

void ofApp::saveSettings() {
    ofxXmlSettings settings;
    settings.setValue("settings:deviceId", deviceId);
    settings.setValue("settings:camwidth", camWidth);
    settings.setValue("settings:camheight", camHeight);
    settings.setValue("settings:slicewidth", sliceWidth);
    settings.setValue("settings:sliceheight", sliceHeight);
    settings.saveFile("settings.xml");

}

void ofApp::loadSettings() {
    ofxXmlSettings settings;
    settings.loadFile("settings.xml");
    deviceId = settings.getValue("settings:deviceId", 0);
    camWidth = settings.getValue("settings:camwidth", 1024);
    camHeight = settings.getValue("settings:camheight", 768);
    sliceWidth = settings.getValue("settings:slicewidth", 800);
    sliceHeight = settings.getValue("settings:sliceheight", 600);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    string fname;
    switch (key)
    {
    case ' ':
        gui->toggleVisible();
        break;
    case 'r':
        recording = !recording;
        delete recorder;
        setupRecorder();
        break;
    case 's':
        fname = "screen" + ofGetTimestampString("%n%e%H%M%S") + ".png";
        ofSaveScreen(fname);
        break;
    case 'c':
        captureImage();
        break;
    case 'q':
        saveSettings();
        break;
    case OF_KEY_UP:
        currentFace++;
        updateFace();
        break;
    case OF_KEY_DOWN:
        currentFace--;
        updateFace();
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


