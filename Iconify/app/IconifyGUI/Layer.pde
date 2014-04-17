abstract class Layer extends Component{

  
  Layer(float x, float y, float w, float h) {
    super(x, y, w, h, false);
  }
  
  void place(float newx, float newy) {
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
  
  void zoomIn() {
    w *= 1.1;
    h *= 1.1;  
  }
  
  void zoomOut() {
    w *= 0.9;
    h *= 0.9;  
  }

  void render(int cx, int cy) {
    render(null, cx, cy);
  }
  
  abstract void render(PGraphics gr,float cx, float cy);

}


class ImageLayer extends Layer {
    PImage img;
   ImageLayer(PImage i, float x, float y) {
    super(x, y, i.width, i.height);
    img = i;
    
  }

  
  void render(PGraphics gr,float cx, float cy) {
    if (gr == null)
      image(img, cx+x, cy+y, w ,h);
    else  
      gr.image(img, cx+x, cy+y, w ,h);
  }
}

class TextLayer extends Layer {
  String str;

   TextLayer(String s, float x, float y) {
     super(x, y, s.length() * 10, 12);
     this.str = s;
     
   }
  
  void render(PGraphics gr,float cx, float cy) {
    
    if (gr == null) {
      pushStyle();
      fill(0);
      text(str, cx+x, cy+y);
      popStyle();
    }
    else  {
      gr.pushStyle();
      gr.fill(0);
      gr.text(str, cx+x, cy+y);
      gr.popStyle();
    }
    
  }

}
