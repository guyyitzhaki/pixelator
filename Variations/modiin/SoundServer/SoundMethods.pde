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
