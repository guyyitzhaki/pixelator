ArrayList<Component> components = new ArrayList<Component>();
Component modal = null;

class Component {
  float x, y, w, h;
  boolean enabled = true;
  String id;


  Component(float _x, float _y, float _w, float _h) {
    this(_x, _y, _w, _h, true);
  }


  Component(float _x, float _y, float _w, float _h, boolean manage) {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    if (manage)
      components.add(this);
  }
  
  ArrayList<Component> getChildren() {
    return null;
  }

  void setId(String id) {
    this.id = id;
  }

  String getId() {
    return id;
  }

  void setX(float x) {
    this.x = x;
  }

  void setY(float y) {
    this.y = y;
  }

  float getX() {
    return x;
  }

  float getY() {
    return y;
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

  void dispose() {
    components.remove(this);
  }
}

class Container extends Component {
  ArrayList<Component> children; 
  
  Container(float _x, float _y, float _w, float _h) {
    this(_x, _y, _w, _h, true);
  }

  Container(float _x, float _y, float _w, float _h, boolean manage) {
    super(_x, _y, _w, _h, manage);
  }
  
  ArrayList<Component> getChildren() {
    ArrayList allChildren = new ArrayList<Component>();
    if (children != null) {
      for (Component child : children) {
        ArrayList<Component> grandChildren = child.getChildren();
        if (grandChildren != null) {
          allChildren.addAll(grandChildren);
        }
        allChildren.add(child);
      }
    }
    return allChildren;
  }
  
  void addChild(Component c) {
    if (children == null)
      children = new ArrayList<Component>();
    children.add(c);
  }

  void dispose() {
    super.dispose();
    if (children != null) {
      for (Component c : children) {
        c.dispose();
      }
    }
  }
  
  void setEnabled(boolean val) {
    super.setEnabled(val);
    if (children != null) {
      for (Component c : children) {
        c.setEnabled(val);
      }
    }
  }
}

