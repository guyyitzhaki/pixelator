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


