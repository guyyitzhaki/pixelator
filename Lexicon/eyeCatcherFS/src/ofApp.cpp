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
    gui->addFPS();
    loadSettings();

    camX = ofGetWidth()/2 - camWidth / 2;
    camY = ofGetHeight() / 2 - camHeight / 2;
    cloneW = 800;
    cloneH = 600;

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
    tracking = false;
    frameCount = 0;

    cleanup();
    setupRecorder();

    tracker.setup();
    tracker.setIterations(NOT_FOUND_ITERATIONS);
    tracker.setAttempts(1);
    leftEyeFbo.allocate(sliceWidth, sliceHeight, GL_RGB);
    rightEyeFbo.allocate(sliceWidth, sliceHeight, GL_RGB);

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
    gui->toggleVisible();

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
    lastSwitch = ofGetElapsedTimef();

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
            if (!tracking)
            {
                tracker.setIterations(FOUND_ITERATIONS);
                tracking = true;
                cout << "setting iterations to " << FOUND_ITERATIONS << endl;
            }
            captureEye();
            substituteFace();
        }
        else
        {
            blinkOn = false;
            if (tracking)
            {
                tracker.setIterations(NOT_FOUND_ITERATIONS);
                tracking = false;
                cout << "setting iterations to " << NOT_FOUND_ITERATIONS << endl;
            }
        }
    }

    float now = ofGetElapsedTimef();
    for (int i = finished.size() - 1; i >= 0; i--)
    {
        if (now - finished[i]->time > WRITE_DELAY)
        {
            ofFile f(finished[i]->filename);
            f.copyTo(outputDir + "/eye" + finished[i]->filename);
            cout << "copied to " << outputDir << "/eye" << finished[i]->filename << endl;
            delete finished[i];
            finished.erase(finished.begin() + i);
        }
    }



    if ((now - lastCaptureEnded > captureEvery) && (!recording))
    {
        cout << "starting at " << now << endl;
        startRecording();
    }

    if (now - lastSwitch > switchEvery && (!recording))
    {
        nextFace();
    }

}

void ofApp::nextFace()
{
    currentFace++;
    if (currentFace >= faces.size())
        currentFace = 0;
    updateFace();

}

void ofApp::prevFace()
{
    currentFace--;
    if (currentFace < 0)
        currentFace = faces.size() - 1;
    updateFace();
}


void ofApp::startRecording()
{
    recording = true;
    delete recorder;
    setupRecorder();
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

    leftEyeFbo.begin();
    ofSetColor(255);
    ofFill();
    cam.getTextureReference().bind();
    normLeft.draw();
    cam.getTextureReference().unbind();
    leftEyeFbo.end();

    rightEyeFbo.begin();
    ofSetColor(255);
    ofFill();
    cam.getTextureReference().bind();
    normRight.draw();
    cam.getTextureReference().unbind();
    rightEyeFbo.end();

    if (recording)
    {

        ofPixels pixels;
        ofImage eyeImage;
        eyeImage.allocate(sliceWidth, sliceHeight, OF_IMAGE_COLOR);
        if (left)
            leftEyeFbo.readToPixels(pixels);
        else
            rightEyeFbo.readToPixels(pixels);
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
            float now = ofGetElapsedTimef();
            recording = false;
            blinkOn = false;
            recorder->finishMovie();
            frameCount = 0;
            video_data *vd = new video_data();
            vd->filename = filename;
            vd->time = now;
            finished.push_back(vd);
            filename = "";
            lastCaptureEnded = now;
            cout <<  "done at " << now << "\n";
        }
    }

}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetColor(ofColor::white);


    //cam.draw(0,0);
    //tracker.draw(true);
    float imgX = ofGetWidth() / 2 - cloneW / 2;
    float imgY = ofGetHeight() / 2 - cloneH / 2;
    if(src.getWidth() > 0 && cloneReady)
    {
        clone.draw(imgX, imgY, cloneW, cloneH);
    }
    else
    {
        cam.draw(imgX, imgY, cloneW, cloneH);
    }

    if (blinkOn)
    {
        ofSetColor(ofColor::red);
        ofFill();
        ofCircle(imgX + 40, imgY + 40, 7, 7);
    }

    int gap = 10;
    ofSetColor(255);
    src.draw(imgX + cloneW + gap,imgY ,100, 100);
    if (cloneReady)
    {
        rightEyeFbo.draw(imgX + cloneW + gap, imgY + cloneH - sliceHeight);
        leftEyeFbo.draw(imgX - sliceWidth - gap, imgY + cloneH - sliceHeight);
    }

    /*   ofSetColor(ofColor::white);
       stringstream reportStream;
       reportStream << "fps " << ofGetFrameRate() << endl;
       reportStream << "file " << filename << endl;
       ofDrawBitmapString(reportStream.str(), 20,20);
       */
}

void ofApp::updateFace()
{
    currentFace = ofClamp(currentFace,0,faces.size()-1);
    if(faces.size()!=0)
    {
        cout << "loading " << faces.getPath(currentFace) << endl;
        loadFace(faces.getPath(currentFace));
    }

}

void ofApp::captureImage()
{
    ofImage capture;
    capture.setFromPixels(cam.getPixels(), camWidth, camHeight, OF_IMAGE_COLOR);
    capture.saveImage("face"+ofGetTimestampString("%n%e%H%M%S") + ".png");
}

void ofApp::saveSettings()
{
    ofxXmlSettings settings;
    settings.setValue("settings:deviceId", deviceId);
    settings.setValue("settings:camWidth", camWidth);
    settings.setValue("settings:camHeight", camHeight);
    settings.setValue("settings:sliceWidth", sliceWidth);
    settings.setValue("settings:sliceHeight", sliceHeight);
    settings.setValue("settings:captureEvery", captureEvery);
    settings.setValue("settings:switchEvery", switchEvery);
    settings.setValue("settings:outputDir", outputDir);
    settings.saveFile("settings.xml");

}

void ofApp::loadSettings()
{
    lastCaptureEnded = 0;
    ofxXmlSettings settings;
    settings.loadFile("settings.xml");
    deviceId = settings.getValue("settings:deviceId", 0);
    camWidth = settings.getValue("settings:camWidth", 800);
    camHeight = settings.getValue("settings:camHeight", 600);
    sliceWidth = settings.getValue("settings:sliceWidth", 80);
    sliceHeight = settings.getValue("settings:sliceHeight", 60);
    captureEvery = settings.getValue("settings:captureEvery", 60);
    switchEvery = settings.getValue("settings:switchEvery", 70);
    outputDir = settings.getValue("settings:outputDir", "c:/temp");
    cout << "cam=" << camWidth << ", " << camHeight << endl;
    cout << "slice=" << sliceWidth << ", " << sliceHeight << endl;
    cout << "captureEvery=" << captureEvery << ", switchEvery=" << switchEvery << endl;
    cout << "outputDir=" << outputDir << endl;
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
    case 'f':
        ofToggleFullscreen();
        break;
    case 'r':
        startRecording();
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
        nextFace();
        break;
    case OF_KEY_DOWN:
        prevFace();
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


