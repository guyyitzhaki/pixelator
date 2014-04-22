abstract class Layer extends Component {
  int angle = 0;

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

  void rotateRight() {
    angle += 3;
  }

  void rotateLeft() {
    angle -= 3;
  }

  void zoomIn() {
    w *= 1.1;
    h *= 1.1;
  }

  void zoomOut() {
    w *= 0.9;
    h *= 0.9;
  }

  boolean isInside(float mx, float my) {
    return mx >= x && mx <= x + w && my >= y && my <= y + h;
  }

  abstract void render(PGraphics gr);

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


  void render(PGraphics gr) { 
    gr.pushMatrix();
    gr.translate(x+w/2, y+h/2);
    gr.rotate(radians(angle));
    if (debug) {
      gr.fill(255, 0, 0); 
      gr.rect(-w/2, -h/2, w, h);
    }
    gr.image(img, -w/2, -h/2, w, h);
    gr.rotate(radians(-angle));
    gr.popMatrix();
  }
}

class TextLayer extends Layer {
  String str;
  int textSize;

  TextLayer(String s, float x, float y, int textSize) {
    super(x, y, s.length() * (textSize - 2), textSize);
    this.str = s;
    this.textSize = textSize;
    setDimensions();
  }

  void zoomIn() {
    textSize += 4;
    setDimensions();
  }

  void setDimensions() {
    canvas.img.pushStyle();
    canvas.img.textSize(textSize);
    setWidth(canvas.img.textWidth(str));
    setHeight(textSize);
    canvas.img.popStyle();
  }

  void zoomOut() {
    if (textSize > 4) {
      textSize -= 4;
      setDimensions();
    }
  }


  void render(PGraphics gr) {
    gr.pushStyle();
    gr.textSize(textSize);
    gr.pushMatrix();
    gr.translate(x+w/2, y+h/2);
    gr.rotate(radians(angle));   
    if (debug) {
      gr.fill(0, 0, 255);
      gr.rect(-w/2, -h/2, getWidth(), getHeight());
    }
    gr.fill(0);
    gr.text(str, -w/2, -h/2);
    gr.popMatrix();
    gr.popStyle();
  }
}

