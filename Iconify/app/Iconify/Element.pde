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
