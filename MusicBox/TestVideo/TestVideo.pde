boolean saveFrames = true;

int triggerSize = 10;
ArrayList<Trigger> notes = new ArrayList<Trigger>();
int gridSize;
int nextx = 0;

void setup() {
  size(640, 480);

  gridSize = width / 10;
}

void draw() {
  background(0);
  ArrayList<Trigger> doomed = new ArrayList<Trigger>(); 
  for (Trigger t : notes) {
    t.draw();
    if (t.isDone())
      doomed.add(t);
  }  
  notes.removeAll(doomed);
  
  if (saveFrames) {
    saveFrame("####.png");
  }
}

void mousePressed() {
  Trigger t = new Trigger(20 + nextx * gridSize, 0, triggerSize, triggerSize);
  notes.add(t);
  nextx++;
  if (20 + nextx  * gridSize > width)
    nextx = 0;
}


class Trigger {
  int x, y, w, h;

  Trigger(int x, int y, int w, int h) {
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
  }


  void draw() {
    y++;
    fill(255);    
    rect(x, y, w, h);
  }

  boolean isDone() {
    return y > height;
  }
}

