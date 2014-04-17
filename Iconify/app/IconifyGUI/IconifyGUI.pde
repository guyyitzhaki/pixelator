import java.util.*;
import java.awt.print.*;
import java.io.FilenameFilter;

int ELEMENT_START_X = 145;
int ELEMENT_START_Y = 147;
int ELEMENT_GAP = 10;
int ELEMENT_RATIO = 6;

int CANVAS_WIDTH = 450;
int CANVAS_HEIGHT = 450;
int CANVAS_X, CANVAS_Y;

PImage bgImage;
ImageButtonList iconList;
ImageButtonList bgList;

Canvas canvas;
PImage dragged;

Layer current;
float moveX, moveY;
boolean moving = false;
PFont engFont;

void setup() {
  size(1024, 768);
  bgImage = loadImage("main.png");
  CANVAS_X = width / 2  - CANVAS_WIDTH / 2;
  CANVAS_Y = height - 40 - CANVAS_HEIGHT;

  engFont = loadFont("Arial-Black-16.vlw");
  textFont(engFont);
  float listWidth = width - 300;
  iconList = new ImageButtonList(ELEMENT_START_X, ELEMENT_START_Y, listWidth, 90, "icons", ELEMENT_RATIO);  
  bgList = new ImageButtonList(ELEMENT_START_X, ELEMENT_START_Y, listWidth, 90, "backgrounds", ELEMENT_RATIO);  
  canvas = new Canvas(CANVAS_X, CANVAS_Y, CANVAS_WIDTH, CANVAS_HEIGHT);
  
  ImageButton zoomIn = new ImageButton("plus.png", 215, CANVAS_Y + 5) {
      public void mousePressed() {
        if (current != null)
          current.zoomIn();
      }
    };
  ImageButton zoomOut = new ImageButton("minus.png", 215, CANVAS_Y + zoomIn.getHeight() + 15) {
      public void mousePressed() {
        if (current != null)
          current.zoomOut();
      }
    };
    
   ImageButton backgrounds = new ImageButton("minus.png", 910, 120) {
      public void mousePressed() {
        iconList.setEnabled(false);
        bgList.setEnabled(true);
      }
    }; 
    ImageButton elements = new ImageButton("minus.png", 910, 180) {
      public void mousePressed() {
        iconList.setEnabled(true);
        bgList.setEnabled(false);
      }
    }; 
    ImageButton text = new ImageButton("minus.png", 910, 240) {
      public void mousePressed() {
        iconList.setEnabled(false);
        bgList.setEnabled(false);
      }
    }; 
  iconList.setEnabled(false);

}

boolean sketchFullScreen() {
  return true;
} 

void draw() {
  image(bgImage, 0, 0);
  
  for (Component c : components) {
    if (c.isEnabled())
      c.render();
  }

  if (dragged != null) {
    pushStyle();
    tint(255, 128);
    image(dragged, mouseX-dragged.width/2, mouseY-dragged.height/2); 
    popStyle();
  }

  if (moving) {
    float changeX = mouseX - moveX;
    float changeY = mouseY - moveY;
    if (current == null)
      return;
    current.place(changeX-CANVAS_X, changeY-CANVAS_Y);
  }
  
  text(mouseX + "," + mouseY, 10,10);
}

void mousePressed() {
  for (Component component : components) {
    if (component.isEnabled() && component.isInside(mouseX, mouseY)) {
      component.mousePressed();
    }
  }
}

void mouseReleased() {
  for (Component component : components) {
    if (component.isEnabled() && component.isInside(mouseX, mouseY)) {
      component.mouseReleased();
    }
  }

  if (moving)
    moving = false;
} 

void keyPressed() {
  if (key == 'x') {
    canvas.clear();
  }
  else if (key == 'd') {
    canvas.removeLast();
  }
  else if (key == 's') {
    canvas.save();
  }
  else if (key == 'a') {
    if (current != null)
      current.zoomIn();
  }
  else if (key == 'z') {
    if (current != null)
      current.zoomOut();
  }
  else if (key == 'F') { // DEBUG
    canvas.addLayer(new TextLayer("Hi", mouseX - CANVAS_X, mouseY - CANVAS_Y));
  }
  else if (key == 'p') {
    PGraphics img = canvas.getImage();
    handlePrint(img);
  }
  else if (key == CODED) {
    switch (keyCode) {
    case UP:
      if (!canvas.isEmpty()) {
        canvas.moveLayerUp();
      }
      break;
    case DOWN:
      if (!canvas.isEmpty()) {
        canvas.moveLayerDown();
      }
      break;
    case LEFT:
      if (!canvas.isEmpty()) {
        canvas.moveLayerLeft();
      }
      break;
    case RIGHT:
      if (!canvas.isEmpty()) {
        canvas.moveLayerRight();
      }
      break;
    }
  }
}

