import processing.video.*;
import ddf.minim.*;


String movieName;
int gridSize;
int trigger;
float speed;
int delay;

Minim minim;
ArrayList<AudioSample> notes;
ArrayList<Integer> lastTriggered;
Movie movie;
boolean paused;

void setup() {
  size(640, 360);
  loadSettings();
  PFont f = loadFont("Consolas-12.vlw");
  textFont(f);
  minim = new Minim(this);
  
  notes = new ArrayList<AudioSample>();
  lastTriggered = new ArrayList<Integer>();
  for (int i = 0; i < gridSize; i++) {
    String fileName = "notes/"+(i+1)+".mp3";
    AudioSample note = minim.loadSample(fileName);
    println("loaded " + fileName);
    notes.add(note);
    lastTriggered.add(0);
  }

  background(0);
  // Load and play the video in a loop
  movie = new Movie(this, movieName);
  movie.play();
  movie.speed(speed);
}

void draw() {
  
  image(movie, 0, 0);

  fill(255);
  int mill = millis();
  int secs = mill / 1000;
  int mins = secs / 60;
  String time = nf(mins, 2) + ":" + nf((secs-(mins*60)), 2) +":"+ (mill-secs*1000);
  text(movieName + " "+ nf(movie.time(),2,2) +":" + nf(movie.duration(),2,2) + "  ("+ time + ")", 10, 10);

  movie.loadPixels();
  noStroke();
  int rectWidth = 20;
  for (int i = 0; i < gridSize; i++) {
    int y = i * height / gridSize;
    color c = movie.pixels[y * width + (width/2)];
    float brightness = brightness(c);
    if (brightness > trigger) {
      if (millis() - lastTriggered.get(i) > delay) {
        notes.get(i).trigger();
        lastTriggered.set(i, millis()); 
        c = color(255);
      }
    }
    fill(c);
    rect(width/2-rectWidth/2, y, rectWidth, height/ gridSize);
  }

  stroke(255,0,0);
  line(width/2,0, width/2, height);
  for (int i = 1; i < gridSize; i++) {
    int y = i * height / gridSize;
    line(width/2 - 10, y, width/2 + 10, y);  
  }
  
  
  if (movie.time() == movie.duration())
    noLoop();
   
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
    else if (key.equals("gridSize")) {
      gridSize = int(val);
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
  }

  println("Movie:" + movieName);
  println("Grid Size:" + gridSize);
  println("Trigger:" + trigger);
  println("Speed:" + speed);
  println("Delay:" + delay);
}

void mousePressed() {
  paused = !paused;
  if (paused)
    movie.pause();
  else
    movie.play();  
}

