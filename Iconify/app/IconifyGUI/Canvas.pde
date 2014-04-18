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
    img.textAlign(LEFT, TOP);
    img.textFont(engFont);
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
    stroke(128);
    noFill();
    rect(x,y,w,h);
    popStyle();    
  }
  
  PGraphics getImage() {
    return img;
  }
  
  Layer addLayer(Layer l) {
    layers.add(l); 
    return l;
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
  
  Layer getLayerAt(float x, float y) {
    for (int i = layers.size()-1; i >= 0; i--) {
      Layer l = layers.get(i);
      if (l.isInside(x,y)) {
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
  
  void mousePressed() {
      Layer l = getLayerAt(mouseX - x, mouseY - y);
      current = l;
     // Layer l = canvas.topLayer(); 
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
  
  
  
  
}

