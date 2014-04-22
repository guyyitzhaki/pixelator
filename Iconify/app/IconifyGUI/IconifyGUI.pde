import java.util.*;
import java.awt.print.*;
import java.io.FilenameFilter;

boolean debug = false;

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
PImage dragCursor, regCursor, buttonCursor;


void setup() {
  size(1024, 768);
  noCursor();
  bgImage = loadImage("main.png");
  
  dragCursor = loadImage("dragcursor.png");
  regCursor = loadImage("regcursor.png");
  buttonCursor = loadImage("buttoncursor.png");
  
  PFont font = loadFont("Monospaced-16.vlw");
  textFont(font);
  float listWidth = width - 300;
  int listX = 150;
  int listY = 150;
  int elementRatio = 6;

  iconList = new ImageButtonList(listX, listY, listWidth, 90, "elements", elementRatio);  
  bgList = new ImageButtonList(listX, listY, listWidth, 90, "backgrounds", elementRatio);  
  textInput = new TextInput(listX + 60, listY, listWidth, 90);

  int canvasWidth = 450;
  int canvasHeight = 450;
  canvasX = width / 2  - canvasWidth / 2;
  canvasY = height - 40 - canvasHeight;
  canvas = new Canvas(canvasX, canvasY, canvasWidth, canvasHeight);

  float btnY = canvasY;
  ImageButton zoomIn = new ImageButton("buttons/plus.png", 211, btnY) {
    public void mousePressed() {
      if (current != null)
        current.zoomIn();
    }
  };
  btnY += zoomIn.getHeight() + 10;

  ImageButton zoomOut = new ImageButton("buttons/minus.png", 211, btnY) {
    public void mousePressed() {
      if (current != null)
        current.zoomOut();
    }
  };
  btnY += zoomIn.getHeight() + 10;
  
  ImageButton rotateRight = new ImageButton("buttons/turnright.png", 211, btnY) {
    public void mousePressed() {
      if (current != null)
        current.rotateRight();
    }
  };
  btnY += zoomIn.getHeight() + 10;

  ImageButton rotateLeft = new ImageButton("buttons/turnleft.png", 211, btnY) {
    public void mousePressed() {
      if (current != null)
        current.rotateLeft();
    }
  };
  btnY += zoomIn.getHeight() + 10;

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
  ImageButton del = new ImageButton("buttons/trash.png", canvasX+canvasWidth+25, height - 90) {
    public void mousePressed() {
      if (current != null)
        canvas.delete(current);
        current = canvas.topLayer();
    }
  }; 
  ImageButton print = new ImageButton("buttons/print.png", canvasX-75, height - 160) {
    public void mousePressed() {
      if (!canvas.isEmpty()) {
        Dialog printing = new Dialog("Printing...", 400, 300, false);
        PGraphics img = canvas.getImage();
        handlePrint(img);
      }
    }
  }; 
  ImageButton save = new ImageButton("buttons/save.png", canvasX-75, height - 90) {
    public void mousePressed() {
      if (!canvas.isEmpty()) {
        SaveDialog save = new SaveDialog();
        //canvas.save();
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
    if (c.isEnabled() && c.isTopLevel())
      c.render();
  }

  if (dragged != null) {
    pushStyle();
    tint(255, 128);
    image(dragged, mouseX-dragged.width/2, mouseY-dragged.height/2); 
    popStyle();
  }
  
  drawCursor();

  if (moving) {
    float changeX = mouseX - moveX;
    float changeY = mouseY - moveY;
    if (current == null)
      return;
    current.place(changeX-canvasX, changeY-canvasY);
  }
  if (debug) {
    text(mouseX + "," + mouseY, 10, 10);
  }
}

void drawCursor() {
  PImage cursorImg = getCursor(REG_CURSOR);
  ArrayList<Component> active = getActiveComponents();
  for (Component component : active) {
    if (component.isEnabled() && component.isInside(mouseX, mouseY) && !(component instanceof Container)) {
      int cursor = component.getCursor();
      cursorImg = getCursor(cursor);
    }
  }
  image(cursorImg, mouseX, mouseY); 

}

ArrayList<Component> getActiveComponents() {
  ArrayList<Component> active = new ArrayList<Component>();
  if (modal != null) {
    active.add(modal);
    active.addAll(modal.getChildren());
  }
  else {
    active.addAll(components);
  }
  return active;
}

void mousePressed() {
  ArrayList<Component> active = getActiveComponents();
  for (Component component : active) {
    if (component.isEnabled() && component.isInside(mouseX, mouseY)) {
      component.mousePressed();
    }
  }
}

void mouseReleased() {
  ArrayList<Component> active = getActiveComponents();
  for (Component component : active) {
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
  else if (key == '[') {
    if (current != null)
      current.rotateRight();
  }
  else if (key == ']') {
    if (current != null)
      current.rotateLeft();
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

PImage getCursor(int type) {
  switch (type) {
     case REG_CURSOR:
       return regCursor;
     case BUTTON_CURSOR:
       return buttonCursor;
     case DRAG_CURSOR:
       return dragCursor;
  }
  return null;
}


