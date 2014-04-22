class Dialog extends Container {
  Label msgLabel;
  TextButton ok, cancel;
  color bg;
  
  Dialog(String msg, float w, float h, boolean showCancel) {
    super(width / 2 - w / 2,height / 2 - h / 2, w, h);
    int gap = showCancel ? 90 : 30;
    ok = new TextButton("OK", getX() + w / 2 - gap, getY() + h - 35, 80,30) {
      void mousePressed() {
        okPressed();
      }
    };
    addChild(ok);
    
    if (showCancel) {
      cancel = new TextButton("CANCEL", getX() + w / 2 + gap, getY() + h - 35, 80, 30) {
        void mousePressed() {
          cancelPressed();
        }
      };
      addChild(cancel);
    }
    float msgX = w / 2 - msg.length()/2 * 10;
    float msgY = h  /  3;
    msgLabel = new Label(msg, x + msgX, y + msgY);
    addChild(msgLabel);
    modal = this;
  }
  
  void setMsgX(float msgx) {
    msgLabel.setX(x + msgx);
  }
  
  void setMsgY(float msgy) {
    msgLabel.setY(y + msgy);
  }
  
  void render() {
    pushStyle();
    fill(240);
    stroke(0);
    strokeWeight(5);
    rect(x,y,w,h);
    fill(0);
//    text(msg, x + msgX, y  + msgY);
    rect(x, y + h - 40, w, 40);
    popStyle();
    super.render();
  }
  
  void okPressed() {
    close();
  }
  
  void cancelPressed() {
    close();
  }

  void close() {
    modal = null;
    dispose();
  }
  
  
  
  
  
}
