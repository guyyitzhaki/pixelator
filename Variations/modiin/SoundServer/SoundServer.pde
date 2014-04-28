import oscP5.*;
import netP5.*;
import ddf.minim.*;
import java.util.*;
import java.io.FilenameFilter;

int PORT = 12001;
int INITIAL_WAIT = 30000;
int SECOND_WAIT = 20000;

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


void setup() {
  size(400,600);
  osc = new OscP5(this,PORT);
  textFont( loadFont("Monospaced-12.vlw"));
  log = new ArrayList<String>();  
  host = NetInfo.getHostAddress();
  
  soundData = new HashMap<String, DirData>();
  minim = new Minim(this);
  TM = new TimerManager();
  soundPath = sketchPath+sep+"data";
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
  for (String msg : log) {
    text(msg, 10, y);
    y+=20;
  }

}

void log(String msg) {
  println(msg);
  int numRows = (height - 40) / 20;
  String time =  nf(hour(),2) + ":" + nf(minute(),2) + ":" + nf(second(),2);

  log.add("[" + time + "] " + msg);
  if (log.size() > numRows) {
    log.remove(0);
  }
}

void oscEvent(OscMessage oscMessage) {
  log("received " + oscMessage.addrPattern());
  handleMessage(oscMessage.addrPattern().substring(1));
}

void handleMessage(String msg) {
  if (msg.equals("stop")) {
    exit();
    return;
  }
}

void fillSoundDirs() {

  String[] soundDirs = listFileNames(soundPath, false, "mp3");
  for (int i = 0; i < soundDirs.length; i++) {
    String fname = soundDirs[i];
    int initial = INITIAL_WAIT;
    int second = SECOND_WAIT;
    int pos = fname.indexOf("-");
    if (pos != -1) {
      String times = fname.substring(pos+1);
      fname = fname.substring(0,pos);
      
      int pos2 = times.indexOf("-");
      if (pos2 != -1) {
        String firstStr = times.substring(0,pos2);
        String secondStr = times.substring(pos2+1);
        try {
           initial = int(firstStr);
           second = int(secondStr);
        } catch (NumberFormatException nfe) { }
      }
      else {
        try {
        initial = int(times); 
        } catch (NumberFormatException nfe) {}
      }
    }
    println("dir="+fname + " initial="+initial+" second="+second);
    DirData data = new DirData(fname, soundDirs[i], initial, second);
    soundData.put(data.moduleName, data);
    
    
    
  }
  //println(soundDirs);
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

