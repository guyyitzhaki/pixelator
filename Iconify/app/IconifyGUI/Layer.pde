abstract class Layer extends Component {


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



  abstract void render(PGraphics gr, float cx, float cy);
  
  int getCursor() {
    return DRAG_CURSOR;
  }
}


class ImageLayer extends Layer {
  PImage img;
  ImageLayer(PImage i, float x, float y) {
    super(x, y, i.width, i.height);
    img = i;
  }


  void render(PGraphics gr, float cx, float cy) { 
    gr.image(img, cx+x, cy+y, w, h);
  }
}

class TextLayer extends Layer {
  String str;
  int textSize;

  TextLayer(String s, float x, float y, int textSize) {
    super(x, y, s.length() * (textSize - 2), textSize);
    this.str = s;
    this.textSize = textSize;
  }
  
  void zoomIn() {
    textSize += 4;
    println(textSize);
    setWidth(str.length() * (textSize - 2));
    setHeight(textSize);
  }

  void zoomOut() {
    if (textSize > 4) {
      textSize -= 4;
      println(textSize);
      setWidth(str.length() * (textSize - 2));
      setHeight(textSize);
    }
  }


  void render(PGraphics gr, float cx, float cy) {
    gr.pushStyle();
    gr.fill(0);
    gr.textSize(textSize);
    gr.text(str, cx+x, cy+y);
    gr.popStyle();
  }
}

