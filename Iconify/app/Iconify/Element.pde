abstract class Element {
  abstract int getWidth();
  
  abstract int getHeight();
  
  void render(int x, int y, int w, int h) {
/*    fill(0);
    rect(x,y,w,h);
*/   
    render(null, x, y, w ,h);
  }
  
    abstract void render(PGraphics gr, int x, int y, int w, int h);
}

class TextElement extends Element {
  String txt;
  
  TextElement(String s) {
    txt = s;
    println("added "+txt);
  }
  
  int getWidth() {
    return CANVAS_WIDTH;
  }
  
  int getHeight() {
    return CANVAS_WIDTH;
  }
  
  void render(PGraphics gr, int x, int y, int w, int h) {
    if (gr == null) {
      pushStyle();
      fill(0);
      text(txt, x, y, w ,h);
      popStyle();
    }
    else {
      gr.pushStyle();

      gr.fill(0);
      gr.text(txt, x, y, w ,h);
      gr.popStyle();
    }
  }
    
  

}

class ImageElement extends Element {
  PImage img;
  
  ImageElement(String path) {
    img = loadImage(path);
  }
  
  int getWidth() {
    return img.width;
  }
  
  int getHeight() {
    return img.height;
  }
  
  
  
  void render(PGraphics gr, int x, int y, int w, int h) {
    if (gr == null)
      image(img, x, y, w ,h);
    else  
      gr.image(img, x, y, w ,h);
  }
}
