import java.util.*;
import java.awt.print.*;
import java.io.FilenameFilter;

boolean debug = true;

PImage bgImage;
ImageButtonList iconList;
ImageButtonList bgList;
TextInput textInput;

int canvasX, canvasY;
Canvas canvas;
PImage dragged;

Layer current;
float moveX, moveY;
boolean moving = false;
PFont engFont;

void setup() {
  size(1024, 768);
  bgImage = loadImage("main.png");

  engFont = loadFont("Arial-Black-16.vlw");
  textFont(engFont);
  float listWidth = width - 300;
  int listX = 150;
  int listY = 150;
  int elementRatio = 6;

  iconList = new ImageButtonList(listX, listY, listWidth, 90, "elements", elementRatio);  
  bgList = new ImageButtonList(listX, listY, listWidth, 90, "backgrounds", elementRatio);  
  textInput = new TextInput(listX + 40, listY, listWidth, 90);

  int canvasWidth = 450;
  int canvasHeight = 450;
  canvasX = width / 2  - canvasWidth / 2;
  canvasY = height - 40 - canvasHeight;
  canvas = new Canvas(canvasX, canvasY, canvasWidth, canvasHeight);

  ImageButton zoomIn = new ImageButton("buttons/plus.png", 211, canvasY) {
    public void mousePressed() {
      if (current != null)
        current.zoomIn();
    }
  };
  ImageButton zoomOut = new ImageButton("buttons/minus.png", 211, canvasY + zoomIn.getHeight() + 10) {
    public void mousePressed() {
      if (current != null)
        current.zoomOut();
    }
  };

  ImageButton backgrounds = new ImageButton("buttons/background.png", 910, 120) {
    public void mousePressed() {
      iconList.setEnabled(false);
      bgList.setEnabled(true);
      textInput.setEnabled(false);
    }
  }; 
  ImageButton elements = new ImageButton("buttons/elements.png", 910, 175) {
    public void mousePressed() {
      iconList.setEnabled(true);
      bgList.setEnabled(false);
      textInput.setEnabled(false);
    }
  }; 
  ImageButton text = new ImageButton("buttons/text.png", 910, 230) {
    public void mousePressed() {
      iconList.setEnabled(false);
      bgList.setEnabled(false);
      textInput.setEnabled(true);
    }
  }; 
  ImageButton clear = new ImageButton("buttons/trash.png", canvasX+canvasWidth+25, height - 90) {
    public void mousePressed() {
      canvas.clear();
    }
  }; 
  ImageButton print = new ImageButton("buttons/print.png", canvasX-75, height - 160) {
    public void mousePressed() {
      if (!canvas.isEmpty()) {
        Dialog printing = new Dialog("Printing...", 400, 300);
        PGraphics img = canvas.getImage();
        handlePrint(img);
      }
    }
  }; 
  ImageButton save = new ImageButton("buttons/save.png", canvasX-75, height - 90) {
    public void mousePressed() {
      if (!canvas.isEmpty()) {
        Dialog save = new Dialog("Save", 400, 300);
        canvas.save();
      }
    }
  }; 
  iconList.setEnabled(false);
  textInput.setEnabled(false);
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
    current.place(changeX-canvasX, changeY-canvasY);
  }
  if (debug)
    text(mouseX + "," + mouseY, 10, 10);
}

void mousePressed() {
  ArrayList<Component> all = new ArrayList<Component>(components);
  for (Component component : all) {
    if (component.isEnabled() && component.isInside(mouseX, mouseY)) {
      component.mousePressed();
    }
  }
}

void mouseReleased() {
  ArrayList<Component> all = new ArrayList<Component>(components);
  for (Component component : all) {
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
    canvas.addLayer(new TextLayer("Hi", mouseX - canvasX, mouseY - canvasY, 16));
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

