class ImageButtonList extends Component {
  ArrayList<ImageButton> icons = new ArrayList<ImageButton>();
  ArrayList<ImageButton> displayed = new ArrayList<ImageButton>();
  ImageButton leftButton, rightButton;
  int startIndex;
  int iconNum;
  
  String id;

  ImageButtonList(float _x, float _y, float _w, float _h, String path, float ratio) {
    super(_x, _y, _w, _h);
    id = path;
    leftButton = new ImageButton("left.jpg", _x, _y+15) {
      void mousePressed() {
        moveLeft();
      }
    };
    rightButton = new ImageButton("right.jpg", _x+_w-leftButton.getWidth(), _y+15) {
      void mousePressed() {
        moveRight();
      }
    };
    String[] files = listFileNames(dataPath(path), true, "png");
    float icony = _y;
    for (int i = 0; i < files.length; i++) {
      println("loaded "+files[i]);
      ImageButton icon = new IconButton(dataPath(path)+"/"+files[i], 0, _y+15, ratio);
      icons.add(icon);
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
    leftButton.setEnabled(val);
    rightButton.setEnabled(val);
    for(ImageButton b : icons) {
      b.setEnabled(val);
    }
    if (val) {
      updateDisplayedIcons();
    }
  }

  void render() {
    pushStyle();
    fill(255);
    stroke(128);
    rect(x + leftButton.getWidth() + 10, y, w - 2 * (leftButton.getWidth()+10), h);
    popStyle();
  }

  void updateDisplayedIcons() {
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
  }
}

