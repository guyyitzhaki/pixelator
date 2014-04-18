class TextButton extends Component {
  String text;
  

  TextButton(String txt, float x, float y, float w, float h) {
    super(x,y,w,h);
    setId(txt);
    text= txt;
   } 
  
  void render() {
    pushStyle();
    fill(0);
    stroke(255);
    rect(x,y,w,h);
    fill(255);
    text(text, x + 15, y + h/2+5);
    popStyle();
  }
}


class ImageButton extends Component {
  PImage img;

  ImageButton(String path, float x, float y) {
    this(path, x, y, 1);
  }
  
  ImageButton(String path, float x, float y, float ratio) {
    super(x,y,0,0);
    setId(path);
    img = loadImage(path);
    setWidth(img.width/ratio);
    setHeight(img.height/ratio);
  } 
  
  void render() {
    image(img, x, y, w ,h);
  }
  

 
}






