String[] listFileNames(String dir, boolean filter) {
  File file = new File(dir);

  if (file.isDirectory()) {
    if (!filter)
      return file.list();
    String names[] = file.list(new FilenameFilter() {
      public boolean accept(File dir, String name) {
        return name.endsWith("mp3");
      }
    }
    );
    return names;
  } 
  else {
    // If it's not a directory
    return null;
  }
}
