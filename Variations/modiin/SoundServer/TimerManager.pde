public class TimerManager {
  PApplet parent;
  ArrayList<TimerData> registered = new ArrayList<TimerData>();
  int curr = 0;
  float next;
  boolean done = false;
  
  
  public void check() {
    if (registered.isEmpty()) {
      return;
    }
    int mill = millis();
    TimerData td = (TimerData) registered.get(0);
    if (mill >= td.time) {
      td.listener.expired(mill);
      registered.remove(td);
    }
  }

  void register(int time, TimerListener tl) {
    // int start = millis();
    // if (start < 0)
    // start = 0;
    TimerData td = new TimerData(start + time, tl);
    registered.add(td);
    done = false;

    Comparator comp = new Comparator() {
      public int compare(Object o1, Object o2) {
        TimerData td1 = (TimerData) o1;
        TimerData td2 = (TimerData) o2;
        if (td1.time > td2.time)
          return 1;
        if (td2.time > td1.time)
          return -1;
        else if (td1.time == td1.time)
          return td1.listener.toString().compareTo(
              td2.listener.toString());
        return 0;
      }
    };
    Collections.sort(registered, comp);
    next = ((TimerData) registered.get(0)).time;
  }

  class TimerData {
    int time;
    TimerListener listener;

    TimerData(int time, TimerListener listener) {
      this.time = time;
      this.listener = listener;
    }

    public String toString() {
      return "[" + time + "]";
    }
  }

}

abstract public class TimerListener {
          String module;
    public TimerListener(String m) {
            module = m;
    }

    protected void register(int time) {
      TM.register(time, this);
    }

    public abstract void expired(int time);

}

