import processing.video.*;
import themidibus.*;

String movieName;
int triggerCount;
int trigger = 0;
float speed = 1.0;
int triggerHeight;
int delay = 0;
int midiDevice = -1;
int midiChannel = 0;
int lowNote = 0;
int highNote = 127;
int velocity = 64;

int gridSize;
int movieWidth = -1;
int noteGap;
MidiBus midi; 
ArrayList<Integer> lastTriggered;
ArrayList<Boolean> triggered;
Movie movie;
boolean paused = false;
boolean ended = false;

void setup() {
  size(1024, 768);
  MidiBus.list();
  triggerHeight = height / 2;

  loadSettings();

  midi = new MidiBus(this, -1, midiDevice);
  PFont f = loadFont("Consolas-12.vlw");
  textFont(f);

  lastTriggered = new ArrayList<Integer>();
  triggered = new ArrayList<Boolean>();
  for (int i = 0; i < triggerCount; i++) {
    lastTriggered.add(0);
    triggered.add(false);
  }

  background(100, 100, 100);
  movie = new Movie(this, movieName);
  movie.play();
  movie.speed(speed);
}

void draw() {
  image(movie, 0, 0);
  if (ended)
    return;

  fill(255);
  timer();

  movie.loadPixels();

  if (movie.pixels.length == 0) {
    return;
  }
  else {
    if (movieWidth == -1) {
      movieWidth = movie.width;
      gridSize = movieWidth / triggerCount;
    }
  }



  for (int i = 0; i < triggerCount; i++) {
    int y = triggerHeight;
    boolean foundTrigger = false;
    int startx = i * gridSize;
    for (int j = 0; j < gridSize; j++) {
      int x = startx + j;
      color c = movie.pixels[y * movie.width + x];
      float brightness = brightness(c);
      if (brightness > trigger) {
        foundTrigger = true;
      }
    }

    boolean alreadyTriggered = triggered.get(i);
    if (alreadyTriggered && !foundTrigger) {
      triggered.set(i, false);
    }
    else if (!alreadyTriggered && foundTrigger) {
      if (millis() - lastTriggered.get(i) > delay) {
        trigger(startx, i);
      }
    }
  }

  stroke(255, 0, 0);
  line(0, triggerHeight, movieWidth, triggerHeight);

  for (int i = 0; i < triggerCount; i++) {
    int x = i * movieWidth / triggerCount;
    line(x, triggerHeight - 10, x, triggerHeight + 10);
    text(lowNote + i * noteGap, x + movieWidth/triggerCount/2-5, triggerHeight);
  }


  if (movie.time() == movie.duration())
    ended = true;
}

void trigger(int startx, int n) {
  int rectWidth = 20;
  int note = lowNote + n * noteGap;
  println("triggering " + n + " note: " + note);
  midi.sendNoteOn(midiChannel, note, velocity);
  lastTriggered.set(n, millis()); 
  triggered.set(n, true);
  stroke(0, 255, 0);
  rect(startx, triggerHeight - rectWidth / 2, gridSize, rectWidth);
}

void timer() {
  int mill = millis();
  int secs = mill / 1000;
  int mins = secs / 60;
  String time = nf(mins, 2) + ":" + nf((secs-(mins*60)), 2) +":"+ (mill-secs*1000);
  text(movieName + " "+ nf(movie.time(), 2, 2) +":" + nf(movie.duration(), 2, 2) + "  ("+ time + ")", 10, 10);
}

void movieEvent(Movie m) {
  m.read();
}

void loadSettings() {
  String[] settings = loadStrings("settings.txt");
  for (int i = 0; i < settings.length; i++) {
    String line = settings[i];
    if (line.startsWith("#"))
      continue;
    String key = line.substring(0, line.indexOf(' ')); 
    String val = line.substring(line.indexOf(' ')+1);

    if (key.equals("movie")) {
      movieName = val;
    }
    else if (key.equals("triggerCount")) {
      triggerCount = int(val);
    }
    else if (key.equals("trigger")) {
      trigger = int(val);
    }
    else if (key.equals("speed")) {
      speed = float(val);
    }
    else if (key.equals("delay")) {
      delay = int(val);
    }  
    else if (key.equals("midiDevice")) {
      midiDevice = int(val);
    }
    else if (key.equals("triggerHeight")) {
      triggerHeight = int(val);
    }
    else if (key.equals("midiChannel")) {
      midiChannel = int(val);
    }
    else if (key.equals("lowNote")) {
      lowNote = int(val);
    }
    else if (key.equals("highNote")) {
      highNote = int(val);
    }
    else if (key.equals("velocity")) {
      velocity = int(val);
    }
  }

  if (midiDevice == -1) {
    println("Error: No MIDI device specified");
  }
  if (triggerCount == 0) {
    println("Error: No triggerCount specified");
  }
  if (movieName == null) {
    println("Error: No movie specified");
  }
  if (trigger == 0) {
    println("Warning: Trigger value is 0");
  }
  if (highNote < lowNote) {
    println("Warning: High note is less than low note");
  }

  noteGap = (highNote - lowNote) / triggerCount;
}

void mousePressed() {
  if (ended) {
    movie.jump(0);
    movie.play();
    ended = false;
  }
  else {
    paused = !paused;
    if (paused)
      movie.pause();
    else
      movie.play();
  }
}

