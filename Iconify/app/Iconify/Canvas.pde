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
  
  PGraphics getImage() {
    return img;
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

