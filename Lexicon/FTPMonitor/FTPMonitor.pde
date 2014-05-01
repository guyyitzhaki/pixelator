String propertiesFile = "settings.txt";

String host;
String username;
String password;
int port;
String remotePath;
String localPath;
int checkEvery;
int lastCheck;

FTPClient ftp;
ArrayList<String> log;
boolean active = true;
color console = color(24, 240, 65);
int buttonX, buttonY = 5, buttonWidth = 50, buttonHeight = 15;

void setup() {
  size(600, 500);
  buttonX = width - 100;
  PFont f = loadFont("Courier-12.vlw");
  textFont(f);
  log = new ArrayList<String>();
  loadSettings();
  setupConnection();
  lastCheck = millis();
}

void loadSettings() {
  String[] settings = loadStrings(propertiesFile);
  for (int i = 0; i < settings.length; i++) {
    String line = settings[i];
    if (line.startsWith("#"))
      continue;
    String key = line.substring(0, line.indexOf(' ')); 
    String val = line.substring(line.indexOf(' ')+1);

    if (key.equals("host"))
      host = val;
    else if (key.equals("port"))
      port = int(val);
    else if (key.equals("user"))
      username = val;
    else if (key.equals("password"))
      password = val;
    else if (key.equals("remotePath"))
      remotePath = val;
    else if (key.equals("localPath"))
      localPath = val;  
    else if (key.equals("checkEvery"))
      checkEvery = int(val) * 1000;  
    else {
      println("unknown key "+key);
    }
  }
  log("read settings, host:"+host);
}

void draw() {
  background(0);
  fill(console);
  stroke(console);
  text("connected to " + host, 10, 15);
  rect(buttonX, buttonY, buttonWidth, buttonHeight);
  line(0, 25, width, 30);
  fill(0);
  String action = active ? "pause" : "resume";
  text(action, width-95, 15);
  fill(console); 
  int y = 45;
  for (String msg : log) {
    text(msg, 10, y);
    y+=20;
  }
  //  println(checkEvery);
  if (active && (millis() - lastCheck > checkEvery)) {
    println("checking");
    log("checking...");  
    lastCheck = millis();
    checkFiles();
  }
}

void log(String msg) {
  println(msg);
  int numRows = (height - 40) / 20;
  log.add("> " + msg);
  if (log.size() > numRows) {
    log.remove(0);
  }
}

void setupConnection() {
  log("connecting...");
  try {

    ftp = new FTPClient();
    ftp.setRemoteHost(host); 
    ftp.setRemotePort(port);
    FTPMessageCollector listener = new FTPMessageCollector();
    ftp.setMessageListener(listener);
    ftp.connect();
    ftp.login(username, password);
    ftp.setType(FTPTransferType.BINARY);
    ftp.setConnectMode(FTPConnectMode.ACTIVE);
    log("ok");
  }
  catch (Exception e) {
    log("error:"+e);
    e.printStackTrace();
  }
}

void checkFiles() {
  File baseDir = new File(localPath);
  File[] newFiles = findTransfers(baseDir);

  if (newFiles.length > 0) {
    log("found "+newFiles.length+" new files");
    try {
      setupConnection();
      for (int i = 0; i < newFiles.length; i++) {
        File f = newFiles[i];
        log("checking "+f+"...");
        String fileName = f.getName();
        log("   transferring " + fileName);
        ftp.put(f.getAbsolutePath(), fileName);
        f.renameTo(new File(f.getParent(), "transferred"+f.getName()));
      }
      ftp.quit();
    }
    catch (Exception e) {
      log(e.toString());
      e.printStackTrace();
    }
  }
}

boolean dirReady(File dir) {
  File[] indicator = dir.listFiles(new java.io.FilenameFilter() {
    public boolean accept(File dir, String name) {
      return name.equals("done.txt");
    }
  }
  );
  return indicator.length > 0;
}



File[] findTransfers(File dir) {
  return dir.listFiles(new java.io.FileFilter() {
    public boolean accept(File f) {
      String name = f.getName();
      return (name.endsWith("mov") && !name.startsWith("transferred"));
    }
  }
  );
} 

void mousePressed() {
  if (mouseX >= buttonX && mouseX <= buttonX + buttonWidth) {
    if (mouseY >= buttonY && mouseY <= buttonY + buttonHeight) {
      active = !active;
    }
  }
}

