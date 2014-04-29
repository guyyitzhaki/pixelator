String[] listFileNames(String dir) {
  return listFileNames(dir, null);
}

String[] listFileNames(String dir, final String suffix) {
  File file = new File(dir);

  if (file.isDirectory()) {
    if (suffix == null)
      return file.list();
    String names[] = file.list(new FilenameFilter() {
      public boolean accept(File dir, String name) {
        return name.endsWith(suffix);
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
