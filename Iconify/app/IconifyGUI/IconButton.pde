class IconButton extends ImageButton {
    IconButton(String path, float x, float y, float ratio) {
      super(path, x, y, ratio);
    }
    
   void mousePressed() {
    dragged = img;
  }
   void mouseReleased() {
    dragged = null;
  }
  
  int getCursor() {
    return DRAG_CURSOR;
  }

}
