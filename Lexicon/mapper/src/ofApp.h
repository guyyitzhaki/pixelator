#pragma once

#include "ofMain.h"
#include "ofMain.h"
#include "CPoint.h"
#include "CShape.h"
#include "CLine.h"
#include "CPolygon.h"

#define WINDOWS

class ofApp : public ofBaseApp
{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void load();
    string getModeStr();

    bool fullscreen;
    vector<CShape *> shapes;
    CShape *curShape;
    CPoint press;
    unsigned int mode;
    bool mask;

    ofColor maskColor, shapeColor, gridColor;
    int cursorX, cursorY;



};
