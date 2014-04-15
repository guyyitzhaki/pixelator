class Layer extends Component{
  Element element;
  
  Layer(Element e, int x, int y) {
    super(x, y, e.getWidth(), e.getHeight());
    element = e;
    
  }
  
  void place(int newx, int newy) {
    x = newx;
    y = newy;
  }
  
  void moveUp() {
    y--;
  }

  void moveDown() {
    y++;
  }

  void moveRight() {
    x++;
  }

  void moveLeft() {
    x--;
  }
  
  void render(int cx, int cy) {
    render(null, cx, cy);
  }
  
    void render(PGraphics gr,int cx, int cy) {
    element.render(gr, cx+x, cx+y, element.getWidth(), element.getHeight());
  }
}
