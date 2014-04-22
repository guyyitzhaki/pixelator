class Label extends Component {
  String text;
  
  Label(String txt, float x, float y) {
    super(x, y, 0, 0);
    setId(txt);
    text = txt;
  }
  
  void render() {
    pushStyle();
    fill(0);
    noStroke();
    text(text, x, y);
    popStyle();
  }
  
}
