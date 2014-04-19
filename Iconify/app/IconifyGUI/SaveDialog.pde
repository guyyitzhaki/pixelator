class SaveDialog extends Dialog {
  TextInput email;
  
  SaveDialog() {
    super("Enter email:", 900,300);
    email = new TextInput(x + 5, y+100, 700, 90, false, false, false, "_.@0123456789");
    setMsgX(10);
    setMsgY(50);
    addChild(email);
  }
  
  void okPressed() {
    String emailStr = email.getValue().trim();
    if (emailStr.isEmpty()) {
      return;
    }
    String filename = canvas.save();
    String[] lines = loadStrings("output/output.txt");
    String[] newLines;
    if (lines != null) {
      newLines = new String[lines.length + 1];
      System.arraycopy(lines, 0, newLines, 0, lines.length);
    }
    else {
      newLines = new String[1];
    }
    newLines[newLines.length-1] = filename + " : " + emailStr;
    saveStrings("output/output.txt", newLines);
    close();
  }
}
