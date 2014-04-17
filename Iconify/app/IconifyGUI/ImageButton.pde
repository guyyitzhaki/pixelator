class ImageButton extends Component {
  PImage img;
  String id;
  
  ImageButton(String path, float x, float y) {
    this(path, x, y, 1);
  }
  
  ImageButton(String path, float x, float y, float ratio) {
    super(x,y,0,0);
    id = path;
    img = loadImage(path);
    setWidth(img.width/ratio);
    setHeight(img.height/ratio);
  } 
  
  void render() {
    image(img, x, y, w ,h);
  }
  

 
}






