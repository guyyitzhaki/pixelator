String[] listFileNames(String dir, boolean filter, final String ext) {
  File file = new File(dir);
  if (file.isDirectory()) {
    if (!filter)
      return file.list();
    String names[] = file.list(new FilenameFilter() {
      public boolean accept(File dir, String name) {
        return name.endsWith(ext);
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

void handlePrint(final PGraphics img) {
  PrinterJob job = PrinterJob.getPrinterJob();
  job.setPrintable(new Printable() {
    public int print(java.awt.Graphics graphics, java.awt.print.PageFormat pageFormat, int pageIndex) throws PrinterException {
      switch (pageIndex)
      {
      case 0 :
        graphics.drawImage(img.image, 200, 200, img.width, img.height, null);
        return Printable.PAGE_EXISTS;
      default: 
        return NO_SUCH_PAGE;        // No other pages left
      }
    }
  }
  );

  try {
    job.print();
  } 
  catch (PrinterException e) {
    System.out.println(e);
  }
}


