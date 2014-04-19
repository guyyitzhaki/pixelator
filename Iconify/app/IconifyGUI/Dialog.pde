class Dialog extends Container {
  String msg;
  float msgX, msgY;
  TextButton ok;
  
  Dialog(String msg, float w, float h) {
    super(width / 2 - w / 2,height / 2 - h / 2, w, h);
    this.msg = msg;
    ok = new TextButton("OK", getX() + w / 2 - 30, getY() + h - 40, 60,30) {
      void mousePressed() {
        okPressed();
      }
    };
    ok.setHGap(20);
    msgX = w / 2 - msg.length()/2 * 10;
    msgY = h  /  3;
    modal = this;
    addChild(ok);
  }
  
  void setMsgX(float msgx) {
    msgX = msgx;
  }
  
  void setMsgY(float msgy) {
    msgY = msgy;
  }
  
  void render() {
    pushStyle();
    fill(255);
    stroke(0);
    strokeWeight(5);
    rect(x,y,w,h);
    fill(0);
    text(msg, x + msgX, y  + msgY);
    rect(x, y + h - 40, w, 40);
    popStyle();
    ok.render();
  }
  
  void okPressed() {
    close();
  }
  
  void close() {
    modal = null;
    dispose();
  }
  
  
  
  
  
}
