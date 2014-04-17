ArrayList<Component> components = new ArrayList<Component>();

class Component {
  float x,y,w,h;
  boolean enabled = true;
  
  Component(float _x, float _y, float _w, float _h) {
    this(_x, _y,_w,_h,true);
  }
  
  
  Component(float _x, float _y, float _w, float _h, boolean manage) {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    if (manage)
      components.add(this);
  }
  
  void setX(float x){
    this.x = x;
  }

  void setY(float y){
    this.y = y;
  }

  void setWidth(float w) {
    this.w = w;
  }
  
  void setHeight(float h) {
    this.h = h;
  }
  
  float getHeight() {
    return h;
  }
  
  float getWidth() {
    return w;
  }
  
  boolean isInside(float mx, float my) {
    return mx >= x && mx <= x + w && my >= y && my <= y + h;
  }
  
  boolean isEnabled() {
    return enabled;
  }
  
  void setEnabled(boolean val) {
    enabled = val;
  }
  
  void render() {
  }
  
  void mousePressed() {
    // default: do nothing
  }
  
  void mouseReleased() {
  }

}
