class ImageButtonList extends Container {
  ArrayList<ImageButton> icons = new ArrayList<ImageButton>();
  ArrayList<ImageButton> displayed = new ArrayList<ImageButton>();
  ImageButton leftButton, rightButton;
  int startIndex;
  int iconNum;

  String id;

  ImageButtonList(float _x, float _y, float _w, float _h, String path, float ratio) {
    super(_x, _y, _w, _h);
    id = path;
    leftButton = new ImageButton("buttons/arrow-left.png", _x, _y+10) {
      void mousePressed() {
        moveLeft();
      }
    };
    rightButton = new ImageButton("buttons/arrow-right.png", _x+_w-leftButton.getWidth(), _y+10) {
      void mousePressed() {
        moveRight();
      }
    };
    addChild(leftButton);
    addChild(rightButton);
    String[] files = listFileNames(dataPath(path), true, "png");
    float icony = _y;
    for (int i = 0; i < files.length; i++) {
      if (debug)
        println("loaded "+files[i]);
      ImageButton icon = new IconButton(dataPath(path)+"/"+files[i], 0, _y+15, ratio);
      icons.add(icon);
      addChild(icon);
    }

    startIndex = 0;
    iconNum = 8;
    updateDisplayedIcons();
  }

  void moveRight() {
    if (startIndex < icons.size() - iconNum)
      startIndex++;
    updateDisplayedIcons();
  }

  void moveLeft() {
    if (startIndex > 0)
      startIndex--;
    updateDisplayedIcons();
  }

  void setEnabled(boolean val) {
    super.setEnabled(val);
    
    if (val) {
      updateDisplayedIcons();
    }
  }

  void render() {
    pushStyle();
    fill(255);
    //stroke(128);
    noStroke();
    rect(x + leftButton.getWidth() + 10, y, w - 2 * (leftButton.getWidth()+10), h);
    popStyle();
  }

  void updateDisplayedIcons() {
    int ELEMENT_GAP = 10;
    displayed.clear();
    float iconx = x + leftButton.getWidth() + 10;
    for (int i = 0; i < icons.size(); i++) {
      ImageButton icon = icons.get(i); 
      if ((i >= startIndex) && (i < startIndex + iconNum)) {
        icon.setEnabled(true);
        icon.setX(iconx);
        iconx += icon.w + ELEMENT_GAP;
        displayed.add(icon);
      }
      else {
        icon.setEnabled(false);
      }
    }

    leftButton.setEnabled(!(startIndex == 0));
    rightButton.setEnabled(!(startIndex + iconNum >= icons.size()));
  }
}

