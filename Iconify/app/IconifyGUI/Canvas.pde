class Canvas extends Container {
  ArrayList<Layer> layers = new ArrayList<Layer>();
  PGraphics img;
  color bg;

  Canvas(int x, int y, int w, int h) {
    super(x, y, w, h);
    img = createGraphics(w, h);
    PFont font = loadFont("Monospaced-48.vlw");
    img.beginDraw();
    img.background(color(255));
    img.textAlign(LEFT, TOP);
    img.textFont(font);
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
      l.render(img);
    }
    img.endDraw();
    image(img, x, y);
    /*pushStyle();
    noStroke();
    noFill();
    rect(x, y, w, h);
    popStyle();
    */
    
  }

  PGraphics getImage() {
    return img;
  }

  Layer addLayer(Layer l) {
    layers.add(l); 
    addChild(l);
    return l;
  }

  void clear() {
    for (Layer l : layers) {
      l.dispose();
    }
    layers.clear();
    
  }
  
  void delete(Layer l) {
    l.dispose();
    layers.remove(l);
    children.remove(l);
  }

  void removeLast() {
    if (!layers.isEmpty()) {
      Layer l = layers.remove(layers.size() - 1);
      l.dispose();
    }
  }

  Layer topLayer() {
    if (layers.isEmpty())
      return null;
    return layers.get(layers.size()-1);
  }

  Layer getLayerAt(float x, float y) {
    for (int i = layers.size()-1; i >= 0; i--) {
      Layer l = layers.get(i);
      if (l.isInside(x, y)) {
        return l;
      }
    }
    return null;
  }

  String save() {
    render();
    String filename =  year() + nf(month(), 2) + nf(day(),2) + nf(hour(),2) + nf(minute(),2) + nf(second(),2) + ".png";
    img.save("output/" + filename);  
    return filename;
  }

  void mousePressed() {
    Layer l = getLayerAt(mouseX - x, mouseY - y);
    current = l;
    if (l == null)
      return;
    moveX = mouseX - x - l.x;
    moveY = mouseY - y - l.y;
    moving = true;
  }

  void mouseReleased() {
    if (dragged != null) {
      current = addLayer(new ImageLayer(dragged, mouseX - x - dragged.width/2, mouseY - y - dragged.height/2));
    }
    dragged = null;
  }
  
  int getCursor() {
    return REG_CURSOR;
  }
}

