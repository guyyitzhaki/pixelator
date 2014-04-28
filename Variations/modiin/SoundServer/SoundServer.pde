import oscP5.*;
import netP5.*;
import ddf.minim.*;
import java.util.*;
import java.io.FilenameFilter;

int PORT = 12001;
int INITIAL_WAIT = 30000;
int SECOND_WAIT = 20000;
int CUT_BY_HALF = 4;

OscP5 osc;
String host;
ArrayList<String> log;
color console = color(24, 240, 65);

Map<String, DirData> soundData;
Minim minim;
ArrayList<AudioPlayer> songs = new ArrayList<AudioPlayer>();
TimerManager TM;
int start;
String sep = File.separator;
String soundPath;
int fileCount;
String curModule;

boolean fade = false;
int VOL_CHECK_EVERY = 3;
float  VOL_INC = 0.1;
float MAX_VOL = 10.0;
float MIN_VOL = -40.0;
float  volume = MAX_VOL;
int initial_wait;
int second_wait;

void setup() {
  size(400, 600);
  osc = new OscP5(this, PORT);
  textFont( loadFont("Monospaced-12.vlw"));
  log = new ArrayList<String>();  
  host = NetInfo.getHostAddress();

  soundData = new HashMap<String, DirData>();
  minim = new Minim(this);
  TM = new TimerManager();
  soundPath = sketchPath+sep+"data"+sep+"sounds";
  fillSoundDirs();

  start = millis();
}


void draw() {
  TM.check();
  background(0);
  fill(console);
  stroke(console);
  text("listening on " + host  + " @ port " + PORT, 10, 15);
  line(0, 25, width, 25);
  int y = 45;
  ArrayList<String> copy = new ArrayList<String>(log); 
  for (String msg : copy) {
    text(msg, 10, y);
    y+=20;
  }
  if (fade) {
    if (volume <= MIN_VOL) {
      fade = false;
      volume = MAX_VOL;
    }
    else {
      if (frameCount % VOL_CHECK_EVERY == 0) {
        volume -= VOL_INC;
        for (AudioPlayer ap : songs) {
          println("setting gain to "+volume);
          ap.setGain(volume);
        }
      }
    }
  }
}

void log(String msg) {
  println(msg);
  int numRows = (height - 40) / 20;
  String time =  nf(hour(), 2) + ":" + nf(minute(), 2) + ":" + nf(second(), 2);

  log.add("[" + time + "] " + msg);
  if (log.size() > numRows) {
    log.remove(0);
  }
}

void oscEvent(OscMessage oscMessage) {
  String msg = oscMessage.addrPattern().substring(1);
  String param = "";
  String typetag = oscMessage.typetag();  
  if (typetag.startsWith("s")) {
    param = oscMessage.get(0).stringValue();
  }
  log("received " + oscMessage.addrPattern() + ":" + param);
  handleMessage(msg, param);
}

void handleMessage(String msg, String param) {
  if (msg.equals("stop")) {
    exit();
    return;
  }
  if (msg.equals("fade")) {
    fade = true;
    return;
  }
  DirData data = soundData.get(msg);
  if (data == null) {
    log(msg + " NOT FOUND");
  }
  else {
    fileCount = 0;
    curModule = msg;
    initial_wait = data.initialWait;
    second_wait = data.secondWait;
    playAndRegister(soundPath +sep+data.dirName, 1, true, param, initial_wait);
  }
}

void fillSoundDirs() {

  String[] soundDirs = listFileNames(soundPath);
  for (int i = 0; i < soundDirs.length; i++) {
    String fname = soundDirs[i];
    int initial = INITIAL_WAIT;
    int second = SECOND_WAIT;
    int pos = fname.indexOf("-");
    if (pos != -1) {
      String times = fname.substring(pos+1);
      fname = fname.substring(0, pos);

      int pos2 = times.indexOf("-");
      if (pos2 != -1) {
        String firstStr = times.substring(0, pos2);
        String secondStr = times.substring(pos2+1);
        try {
          initial = int(firstStr);
          second = int(secondStr);
        } 
        catch (NumberFormatException nfe) {
          log("bad format" + fname);
        }
      }
      else {
        try {
          initial = int(times);
        } 
        catch (NumberFormatException nfe) {
          log("bad format" + fname);
        }
      }
    }
    println("dir="+fname + " initial="+initial+" second="+second);
    DirData data = new DirData(fname, soundDirs[i], initial, second);
    soundData.put(data.moduleName, data);
  }
  //println(soundDirs);
}

void playAndRegister(final String path, final int dirnum, boolean stopOthers, String method, int wait) {
  String spath = path + sep + dirnum;
  String[] fileNames = listFileNames(spath, "mp3");
  if (fileNames == null) {
    log("no directory " +dirnum);
    return;
  }
  if (fileNames.length == 0) {
    log("empty directory " +dirnum);
    return;
  }
  fileCount++;
  if (fileCount == CUT_BY_HALF) {
    second_wait = second_wait / 2;
    //addText("cutting time by half to "+ second_wait);
  }
  int idx = int(random(fileNames.length));
  String fileName = fileNames[idx];
  log("playing " +dirnum + sep + fileName + " waiting for "+wait);
  play(spath +sep+fileName, stopOthers, method);
  //addText("registering " + wait);

  TM.register(millis() + wait, new TimerListener(curModule) {

    public void expired(int time) {
      if (module.equals(curModule)) {
        playAndRegister(path, dirnum + 1, false, null, second_wait);
      }
    }
  }
  );
}

void play(String filename, boolean stopOthers, String method) {
  ArrayList doomed = new ArrayList();
  for (AudioPlayer ap : songs) {
    println("checking"+ap);
    if (!ap.isPlaying()) {
      ap.close();
      println("closed"+ap);
      doomed.add(ap);
    }
  }
  songs.removeAll(doomed);

  if (stopOthers) {
    println("remaining"+songs.size());
    for (AudioPlayer ap : songs) {
      println("method="+method);
      if (method.equals("stop")) {
        try {
          ap.pause();
          println("stopped "+ap);
        } 
        catch (Exception e) {
          println("caught exception" + e);
          log("EXCEPTION!!!");
        }
      }
      else {
        println("playing last "+ap);
        if (ap.isPlaying())  
        try {      
          ap.play();
        } 
        catch (Exception e) {
          println("caught exception");
          log("EXCEPTION!!!");
        }
      }
    }
  }

  AudioPlayer ap = minim.loadFile(filename);
  //ap.setGain(MAX_VOL);
  if (stopOthers) { //first scene
    ap.setGain(MAX_VOL);
  }
  else {
    ap.shiftGain(-40.0, 10.0, 8000);
  }  
  ap.loop();
  songs.add(ap);
}


void stop() {
  for (AudioPlayer ap : songs) {
    ap.close();
  }
  minim.stop();
}

class DirData {
  String moduleName, dirName;
  int initialWait, secondWait;

  public DirData(String mname, String dname, int init, int second) {
    moduleName = mname;
    dirName  = dname;
    initialWait = init * 1000;
    secondWait = second * 1000;
  }
}

