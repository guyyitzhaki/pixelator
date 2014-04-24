import processing.video.*;
import httprocessing.*;


Capture cam;
Rekognition rekog;
int idx = 0;
String prefix;
String lastFile;
RFace[] faces;
PImage lastImage;

int curx, cury;
int maxw;
void setup() {
  size(800, 600);
  String[] keys = loadStrings("key.txt");
  String api_key = keys[0];
  String api_secret = keys[1];

  // Create the face recognizer object
  rekog = new Rekognition(this, api_key, api_secret);

  prefix = "data/img-" + nf(month(), 2) + nf(day(), 2) + nf(hour(), 2) + nf(minute(), 2) + "-";
  println(Capture.list());
  cam = new Capture(this, 640, 480);
  cam.start();
  background(0);
  curx = 0;
  cury = 0;
  maxw = 0;
}

void draw() {
  //  background(0);
  if (cam.available()) {
    cam.read();
  }
  image(cam, width-128, height-96, 128, 96);
  
}

void captureImage() {
  lastFile = prefix+idx+".png"; 
  cam.save(lastFile);
  idx++;
}

void detect() {
  println("detecting" + lastFile);
  faces = rekog.detect(lastFile);
  PImage img = loadImage(lastFile);
  
  for (int i = 0; i < faces.length; i++) {
    int facex = (int)(faces[i].center.x-(faces[i].w/2));
    int facey = (int)(faces[i].center.y-(faces[i].h/2));
    image(img.get(facex, facey, (int)(faces[i].w), (int)(faces[i].h)), curx, cury);
    stroke(255, 0, 0);
    strokeWeight(1);
    noFill();
    rectMode(CENTER);
    //rect(faces[i].center.x, faces[i].center.y, faces[i].w, faces[i].h);  // Face center, with, and height
    pushMatrix();
    translate(curx-facex, cury-facey);
    rect(faces[i].eye_right.x, faces[i].eye_right.y, 4, 4);              // Right eye
    rect(faces[i].eye_left.x, faces[i].eye_left.y, 4, 4);                // Left eye
    rect(faces[i].mouth_left.x, faces[i].mouth_left.y, 4, 4);            // Mouth Left
    rect(faces[i].mouth_right.x, faces[i].mouth_right.y, 4, 4);          // Mouth right
    rect(faces[i].nose.x, faces[i].nose.y, 4, 4);     // Nose
    popMatrix();
    fill(0, 255, 0);
    String display = "Age: " + int(faces[i].age) + "\n";                        // Age
    display += "Gender: " + faces[i].gender +"   ("+nf(faces[i].gender_rating, 1, 2) +")\n";                               // Gender
    display += "Smiling: " + faces[i].smiling + "   ("+nf(faces[i].smile_rating, 1, 2) +")\n";                             // Smiling
    display += "Glasses: " + faces[i].glasses + "   ("+nf(faces[i].glasses_rating, 1, 2) +")\n";// Glasses
    display += "Glasses rating: " + nf(faces[i].glasses_rating, 1, 2) + "\n";   // Glasses from 0 to 1
    display += "Eyes closed: " + faces[i].eyes_closed  + "   ("+nf(faces[i].eyes_closed_rating, 1, 2) +")\n";                             // Eyes closed
    for (int j = 0; j < faces[i].emotions.size(); j++) {
      display += faces[i].emotions.get(j).getName() + ": "+faces[i].emotions.get(j).getRating()+"\n";
    }
    text(display, curx + faces[i].w, cury+12);
    
    cury += faces[i].h;
    if (faces[i].w > maxw) 
      maxw = (int)faces[i].w;
    if (cury > height - 100) {
      cury = 0;
      curx += maxw;
      maxw = 0;
    }  
  }
  
  println("done");
} 

void keyPressed() {
  switch (key) {
  case ' ':
    captureImage();
    detect();
    break;
  }
}


