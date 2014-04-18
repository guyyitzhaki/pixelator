class Dialog extends Component {
  String msg;
  TextButton ok;
  
  Dialog(String msg, float w, float h) {
    super(width / 2 - w / 2,height / 2 - h / 2, w, h);
    this.msg = msg;
    ok = new TextButton("OK", getX() + w / 2 - 30, getY() + h - 40, 60,30) {
      void mousePressed() {
        close();
      }
    };
  }
  
  void render() {
    pushStyle();
    fill(255);
    stroke(0);
    strokeWeight(5);
    rect(x,y,w,h);
    fill(0);
    text(msg, x + w / 2 - 20, y  + h  /  3);
    rect(x, y + h - 40, w, 40);
    popStyle();
    ok.render();
  }
  
  void close() {
    // remember to remove children too
    components.remove(this);
    components.remove(ok);
  }
  
  
  
  
  
}
