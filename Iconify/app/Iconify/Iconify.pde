import java.util.*;
import java.awt.print.*;
import java.io.FilenameFilter;

int APP_WIDTH = 1024;
int APP_HEIGHT = 600;

int ELEMENT_START_X = 10;
int ELEMENT_START_Y = 10;
int ELEMENT_GAP = 10;
int ELEMENT_RATIO = 2;

int maxElementHeight;
int elementWidth;

int ICON_WIDTH = 128;
int ICON_HEIGHT = 128;

int CANVAS_WIDTH = 400;
int CANVAS_HEIGHT = 400;
int CANVAS_X = APP_WIDTH / 2  - CANVAS_WIDTH / 2;
int CANVAS_Y = APP_HEIGHT - 10 - CANVAS_HEIGHT;

ArrayList<Element> elements = new ArrayList<Element>();
Canvas canvas;
Element dragged;
Layer current;
int moveX, moveY;
boolean moving = false;

void setup() {
  size(APP_WIDTH, APP_HEIGHT);
  String[] files = listFileNames(dataPath("icons"), true, "png");
  maxElementHeight = 0;
  for (int i = 0; i < files.length; i++) {
    println("loaded "+files[i]);
    Element element = new Element(dataPath("icons")+"/"+files[i]);
    elements.add(element);
    if (element.getHeight() > maxElementHeight)
      maxElementHeight = element.getHeight();
  }
  canvas = new Canvas(CANVAS_X, CANVAS_Y, CANVAS_WIDTH, CANVAS_HEIGHT);
}

void draw() {
  background(255, 100, 100);
  int x = ELEMENT_START_X;
  int y = ELEMENT_START_Y;
  
  for (Element e : elements) {
    e.render(x, y, e.getWidth()/ELEMENT_RATIO, e.getHeight()/ELEMENT_RATIO);
    x += e.getWidth()/ELEMENT_RATIO + ELEMENT_GAP;
  }
  canvas.render();

  if (dragged != null) {
    pushStyle();
    tint(255, 128);
    int w = dragged.getWidth();
    int h = dragged.getHeight();
    dragged.render(mouseX-w/2, mouseY-h/2, w, h);
    popStyle();
  }
  
  if (moving) {
    int changeX = mouseX - moveX;
    int changeY = mouseY - moveY;
    if (current == null)
      return;
    current.place(changeX-CANVAS_X, changeY-CANVAS_Y);  
  }
}

String[] listFileNames(String dir, boolean filter, final String ext) {
  File file = new File(dir);
  if (file.isDirectory()) {
    if (!filter)
      return file.list();
    String names[] = file.list(new FilenameFilter() {
      public boolean accept(File dir, String name) {
        return name.endsWith(ext);
      }
    }
    );
    return names;
  } 
  else {
    // If it's not a directory
    return null;
  }
}

void mousePressed() {
  if ((mouseY > ELEMENT_START_Y) && (mouseY < ELEMENT_START_Y + maxElementHeight/ELEMENT_RATIO)) {
    if (mouseX > ELEMENT_START_X) {
      int startx = ELEMENT_START_X; 
      for (Element e : elements) {
        int endx = startx + e.getWidth()/ELEMENT_RATIO;
        if (mouseX >= startx && mouseX <= endx) {
          dragged = e;
          println("found!");
        }
        startx  = endx + ELEMENT_GAP;
      }
    }
  }
  else if (mouseY > CANVAS_Y && mouseY < CANVAS_Y + CANVAS_HEIGHT) {
    if (mouseX > CANVAS_X && mouseX < CANVAS_X + CANVAS_WIDTH) {
      Layer l = canvas.getLayerAt(mouseX - CANVAS_X, mouseY - CANVAS_Y);
      current = l;
     // Layer l = canvas.topLayer(); 
      if (l == null)
        return;
      moveX = mouseX - CANVAS_X-l.x;
      moveY = mouseY - CANVAS_Y-l.y;
      println("gap = "+moveX);
      moving = true;
    }
  }
}

void mouseReleased() {
  if (dragged != null) {
    if (mouseX > CANVAS_X && mouseX < CANVAS_X + CANVAS_WIDTH) {
      if (mouseY > CANVAS_Y && mouseY < CANVAS_Y + CANVAS_HEIGHT) {
        canvas.addLayer(dragged, mouseX - CANVAS_X - dragged.getWidth()/2, mouseY - CANVAS_Y - dragged.getHeight()/2);
      }
    }
    dragged = null;
    
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


