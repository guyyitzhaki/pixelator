
// The next line is needed if running in JavaScript Mode with Processing.js
/*@pjs preload="icons/lock.png";*/  
String[] files = new String[] {"1342.png","1764.png","rsz_hands.png","lock.png","logoff.png","add.png"};

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
  size(1024, 600);
  maxElementHeight = 0;

  for (int i = 0; i < files.length; i++) {
    Element element = new Element("icons/"+files[i]);
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
 //   tint(255, 128);
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

void mousePressed() {
  if ((mouseY > ELEMENT_START_Y) && (mouseY < ELEMENT_START_Y + maxElementHeight/ELEMENT_RATIO)) {
    if (mouseX > ELEMENT_START_X) {
      int startx = ELEMENT_START_X; 
      for (Element e : elements) {
        int endx = startx + e.getWidth()/ELEMENT_RATIO;
        if (mouseX >= startx && mouseX <= endx) {
          dragged = e;
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

void saveImage() {
  canvas.save();
}


  class Canvas extends Component {
  ArrayList<Layer> layers = new ArrayList<Layer>();
  PGraphics img;
  color bg;
  boolean select;
  
  Canvas(int x, int y, int w, int h) {
    super(x, y, w, h);
    img = createGraphics(w, h);
    img.beginDraw();
    img.background(color(255));
    img.endDraw();
    
  }
  
  void setBackground(color c) {
    bg = c;
  }
  
  boolean isEmpty() {
    return layers.isEmpty();
  }
  
  void moveLayerUp() {
    layers.get(layers.size()-1).moveUp();
  }

  void moveLayerDown() {
    layers.get(layers.size()-1).moveDown();
  }

  void moveLayerRight() {
    layers.get(layers.size()-1).moveRight();
  }

  void moveLayerLeft() {
    layers.get(layers.size()-1).moveLeft();
  }
  
  void render() {
    img.beginDraw();
    img.background(255);
    for (Layer l : layers) {
      l.render(img, 0,0);
    }
    if (select && !layers.isEmpty()) {
      Layer l = topLayer();
      img.stroke(0);
      img.noFill();
      img.rect(l.x,l.y,l.w,l.h);
    }
    img.endDraw();
    image(img, x,y);
    pushStyle();
    stroke(0);
    noFill();
    rect(x,y,w,h);
    popStyle();    
  }
  
  void addLayer(Element e, int ex, int ey) {
    Layer l = new Layer(e, ex, ey);
    layers.add(l); 
  }
  
  void clear() {
    layers.clear();
  }
  
  void removeLast() {
    if (!layers.isEmpty()) {
      layers.remove(layers.size() - 1);
    }
  }
  
  Layer topLayer() {
    if (layers.isEmpty())
      return null;
    return layers.get(layers.size()-1);  
  }
  
  Layer getLayerAt(int x, int y) {
    for (int i = layers.size()-1; i >= 0; i--) {
      Layer l = layers.get(i);
      if (x >= l.x && x <= l.x + l.w && y >= l.y && y<= l.y + l.h) {
        return l;
      }  
    }
    return null;
  }
  
  void save() {
 //   select = false;
    render();
    img.save("output.png");  
//    select = true;  
  }
  
  
  
  
}

class Component {
  int x,y,w,h;
  
  Component(int _x, int _y, int _w, int _h) {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
  }
}
class Element {
  PImage img;
  
  Element(String path) {
    img = loadImage(path);
  }
  
  int getWidth() {
    return img.width;
  }
  
  int getHeight() {
    return img.height;
  }
  
  
  void render(int x, int y, int w, int h) {
/*    fill(0);
    rect(x,y,w,h);
*/    
    render(null, x, y, w ,h);
  }
  
  void render(PGraphics gr, int x, int y, int w, int h) {
    if (gr == null)
      image(img, x, y, w ,h);
    else  
      gr.image(img, x, y, w ,h);
  }
}
class Layer extends Component{
  Element element;
  
  Layer(Element e, int x, int y) {
    super(x, y, e.getWidth(), e.getHeight());
    element = e;
    
  }
  
  void place(int newx, int newy) {
    x = newx;
    y = newy;
  }
  
  void moveUp() {
    y--;
  }

  void moveDown() {
    y++;
  }

  void moveRight() {
    x++;
  }

  void moveLeft() {
    x--;
  }
  
  void render(int cx, int cy) {
    render(null, cx, cy);
  }
  
    void render(PGraphics gr,int cx, int cy) {
    element.render(gr, cx+x, cx+y, element.getWidth(), element.getHeight());
  }
}

