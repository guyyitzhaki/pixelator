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
    camWidth 	= 640;
    camHeight 	= 480;
    sliceWidth  = 72;
    sliceHeight = 64;
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

    cam.setDeviceID(0);
    cam.initGrabber(camWidth,camHeight);

    loaded = false;
    recording = true;
    frameCount = 0;

    cleanup();
    setupRecorder();

    tracker.setup();
    eyeFbo.allocate(sliceWidth, sliceHeight, GL_RGB);
    ofSetFrameRate(30);
    ofBackground(0);

}

void ofApp::cleanup() {
    ofDirectory dir("");
    dir.allowExt("mov");
    dir.listDir();
    ofFile doomed;
    for(int i = 0; i < dir.numFiles(); i++){
        doomed.removeFile(dir.getPath(i));
    }

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
        if(tracker.getFound())
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

            if (recording) {
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

                if (frameCount == 0) {
                    blinkOn = true;
                }
                recorder->addFrame(eyeImage.getPixels(), 1/16.0f);
                frameCount++;
                //cout << frameCount << " ";

                if (ofGetElapsedTimeMillis() - blinkTime > 500) {
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
    }

    float now = ofGetElapsedTimef();
    for (int i = finished.size() - 1; i >= 0; i--) {
        if (now - finished[i]->time > WRITE_DELAY) {
            ofFile f(finished[i]->filename);
            f.copyTo("done\\eye" + finished[i]->filename);
            delete finished[i];
            finished.erase(finished.begin() + i);
        }
    }

}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetColor(ofColor::white);
    int x = ofGetWidth()/2 - camWidth / 2;
    int y = ofGetHeight() / 2 - camHeight / 2;
    ofTranslate(x,y);
    cam.draw(0,0);
    tracker.draw();
    if (blinkOn) {
        ofSetColor(ofColor::red);
        ofFill();
        ofCircle(40,40,10,10);
    }
    ofTranslate(-x,-y);

    ofSetColor(ofColor::white);
    stringstream reportStream;
    reportStream << "fps " << ofGetFrameRate() << endl;
    reportStream << "file " << filename << endl;
    ofDrawBitmapString(reportStream.str(), 20,20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == 'r')
    {
        recording = !recording;
        delete recorder;
        setupRecorder();

    }
    if (key == 's')
    {
        string fname;
        fname = "screen" + ofGetTimestampString("%n%e%H%M%S") + ".png";
        ofSaveScreen(fname);
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


