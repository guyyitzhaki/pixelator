abstract class Button extends Component {
  Button(float x, float y, float w, float h) {
    super(x, y, w, h);
  }

  int getCursor() {
    return BUTTON_CURSOR;
  }
}

class TextButton extends Button {
  String text;
  int vgap = 5;

  TextButton(String txt, float x, float y, float w, float h) {
    super(x, y, w, h);
    setId(txt);
    text = txt;
  } 

  void setVGap(int val) {
    vgap = val;
  }   

  void render() {
    pushStyle();
    fill(255);
    noStroke();
    rect(x, y, w, h);
    fill(0); 
    text(text, x + w/2-textWidth(text)/2, y + h/2 + vgap);
    popStyle();
  }

  void setText(String txt) {
    text = txt;
  }

  String getText() {
    return text;
  }
}


class ImageButton extends Button {
  PImage img;

  ImageButton(String path, float x, float y) {
    this(path, x, y, 1);
  }

  ImageButton(String path, float x, float y, float ratio) {
    super(x, y, 0, 0);
    setId(path);
    img = loadImage(path);
    setWidth(img.width/ratio);
    setHeight(img.height/ratio);
  } 

  void render() {
    image(img, x, y, w, h);
  }
}

class TextArea extends Component {
  String text;
  int hgap = 15;
  int vgap = 5;

  TextArea(String txt, float x, float y, float w, float h) {
    super(x, y, w, h);
    setId(txt);
    text = txt;
  } 

  void setHGap(int val) {
    hgap = val;
  }

  void setVGap(int val) {
    vgap = val;
  }   

  void render() {
    pushStyle();
    fill(255);
    noStroke();
    rect(x, y, w, h);
    fill(0);
    String displayed = text;
    int charNum = int((w - hgap) / textWidth("A"));
    if (text.length() > charNum) {
      displayed = "..." + text.substring(text.length()-charNum+3);
    }
    text(displayed, x + hgap, y + h/2 + vgap);
    float sw = textWidth(displayed);
    stroke(0);
    line(x + hgap + sw, y, x + hgap + sw, y+h);
    popStyle();
  }

  void setText(String txt) {
    text = txt;
  }

  String getText() {
    return text;
  }

  int getCursor() {
    return REG_CURSOR;
  }
}





