#include "ofApp.h"

#define GRID_SIZE 15

#define MODE_LINE 0
#define MODE_POLYGON 1

bool drawGrid = true;
bool drawGui = true;
vector<CPoint> tempPoints;

//--------------------------------------------------------------
void ofApp::setup(){
    maskColor = ofColor::blue;
    shapeColor = ofColor::black;
    gridColor = ofColor::cyan;

    ofEnableSmoothing();
    ofBackground(ofColor::white);
    ofHideCursor();
    mask = false;
    mode = MODE_POLYGON;

    load();


}

void ofApp::load() {
    ofBuffer buffer = ofBufferFromFile("shapes.txt");
    while (!buffer.isLastLine())
    {
        string line = buffer.getNextLine();
        cout << line;
        cout << "\n";
        //Split line into strings
        vector<string> tokens = ofSplitString(line, " ");
        string id = tokens[0];
        string type = tokens[1];
        bool mask = ofToBool(tokens[2]);
        if (mask) {
            ofSetColor(maskColor);
        }
        else {
            ofSetColor(shapeColor);
        }
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
            for (int i = 3; i < tokens.size()-1; i+=2) {
                CPoint p = getPoint(tokens[i], tokens[i+1]);
                ((CPolygon *)newShape)->add(p);
            }
        }
        newShape->setMask(mask);
        shapes.push_back(newShape);
    }

}


//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(gridColor);
    for (int x = 0; x < ofGetWindowWidth(); x+= GRID_SIZE)
    {
        ofLine(x, 0, x, ofGetWindowHeight());
    }
    for (int y = 0; y < ofGetWindowHeight(); y+= GRID_SIZE)
    {
        ofLine(0, y, ofGetWindowWidth(), y);
    }


    ofSetColor(ofColor::red);
    ofLine(cursorX-10, cursorY, cursorX+10, cursorY);
    ofLine(cursorX, cursorY+10, cursorX, cursorY-10);


    for (unsigned int i = 0; i < shapes.size(); i++)
    {
        ofSetColor(shapes[i]->isMask() ? maskColor : shapeColor);
        shapes[i]->draw();
    }

    ofSetColor(ofColor::red);
    if (curShape)
        curShape->draw();

    if (drawGui) {
        ofSetColor(ofColor::black);
        char mouseStr[255]; // an array of chars
        sprintf(mouseStr, "(%d,%d) ", cursorX, cursorY);
        ofDrawBitmapString(mouseStr, 50,50);
        ofDrawBitmapString(getModeStr(), 50,25);
    }

}

string ofApp::getModeStr() {
    string s;
    switch (mode) {
        case MODE_LINE:
            s = "line";
            break;
        case MODE_POLYGON:
            s = "polygon";
            break;
        default:
            s = "unknown";

    }
    if (mask)
        s += " (mask)";
    return s;
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
   if(key == 'f')
    {

        fullscreen = !fullscreen;

        if(!fullscreen)
        {
            ofSetWindowShape(1024,768);
            ofSetFullscreen(false);
            // figure out how to put the window in the center:
            int screenW = ofGetScreenWidth();
            int screenH = ofGetScreenHeight();
            ofSetWindowPosition(screenW/2-300/2, screenH/2-300/2);
        }
        else if(fullscreen == 1)
        {
            ofSetFullscreen(true);
        }
    }
    else if (key == 'd')
    {
        shapes.pop_back();
    }
    else if (key == 'm')
    {
        mask = !mask;
    }
    else if (key == 'g')
    {
        drawGui = !drawGui;
    }
    else if (key == 'l')
    {
        mode = MODE_LINE;
        cout << "LINE mode\n";
    }
    else if (key == 'p')
    {
        mode = MODE_POLYGON;
        cout << "POLYGON mode\n";
    }
    else if (key == 's')
    {
        string buffer;
        for (unsigned int i = 0; i < shapes.size(); i++)
        {
            buffer.append(shapes[i]->serialize());
            buffer.append("\n");
        }
        ofBuffer msg(buffer.c_str(), buffer.length());
        ofBufferToFile("shapes.txt", msg);
        cout << buffer;

    }
    else if (key == 'a')
    {
        if (mode == MODE_POLYGON) {
            CPoint p;
            p.x = cursorX;
            p.y = cursorY;
            cout << "adding point at " << p.x << "," << p.y << "\n";
            ((CPolygon *) curShape)->add(p);

            tempPoints.push_back(p);
        }
    }
    else if (key == 'x')
    {
        if (mode == MODE_POLYGON) {
            tempPoints.pop_back();
            ((CPolygon *) curShape)->pop();

        }
    }
    else if (key == 'e' || key == OF_KEY_RETURN)
    {
        if (mode == MODE_POLYGON) {
            CPolygon *added = new CPolygon(false);
            added->setMask(mask);
            for (int i = 0; i < tempPoints.size(); i++) {
                added->add(tempPoints[i]);
            }
            shapes.push_back(added);
            delete curShape;
            tempPoints.clear();
            curShape = 0;
        }
    }

    else if (key == OF_KEY_UP)   // UP
    {
#ifdef WINDOWS
        POINT pt;
        GetCursorPos(&pt);
        SetCursorPos(pt.x, pt.y-1);
#endif
        cursorY--;
    }
    else if (key == OF_KEY_DOWN)   // DOWN
    {
#ifdef WINDOWS
        POINT pt;
        GetCursorPos(&pt);
        SetCursorPos(pt.x, pt.y+1);
#endif
        cursorY++;
    }
    else if (key == OF_KEY_RIGHT)   // RIGHT
    {
#ifdef WINDOWS
        POINT pt;
        GetCursorPos(&pt);
        SetCursorPos(pt.x+1, pt.y);
#endif
        cursorX++;
    }
    else if (key == OF_KEY_LEFT)   // LEFT
    {
#ifdef WINDOWS
        POINT pt;
        GetCursorPos(&pt);
        SetCursorPos(pt.x-1, pt.y);
#endif
        cursorX--;
    }


}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    cursorX = x;
    cursorY = y;

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
  cursorX = x;
    cursorY = y;

    CPoint drag;
    drag.x = x;
    drag.y = y;
    if (mode == MODE_LINE)
    {
        delete curShape;
        curShape = new CLine(press, drag, true);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    press.x = cursorX;
    press.y = cursorY;

    if (mode == MODE_LINE)
    {
        curShape = new CLine(press,press,true);
    }
    else if (mode == MODE_POLYGON) {
        tempPoints.clear();
        cout << "starting polygon at " << press.x << "," << press.y << "\n";

        curShape = new CPolygon(true);
        ((CPolygon *) curShape)->add(press);
        tempPoints.push_back(press);

    }

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
   CPoint p2;
    p2.x = cursorX;
    p2.y = cursorY;

    if (mode == MODE_LINE) {
        CLine *added = new CLine(press,p2,false);
        added->setMask(mask);

        shapes.push_back(added);
        delete curShape;
        curShape = 0;
    }


}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
