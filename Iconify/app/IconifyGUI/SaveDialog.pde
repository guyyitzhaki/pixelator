class SaveDialog extends Dialog {
  TextInput email;
  
  SaveDialog() {
    super("Enter email:", 800,300);
    email = new TextInput(x + 60, y+10, 300, 90, false);
    addChild(email);
  }
  
  void okPressed() {
    String emailStr = email.getValue().trim();
    if (emailStr.isEmpty()) {
      return;
    }
    String filename = canvas.save();
    String[] lines = loadStrings("data/output.txt");
    String[] newLines = new String[lines.length + 1];
    System.arraycopy(lines, 0, newLines, 0, lines.length);
    newLines[newLines.length-1] = filename + " : " + emailStr;
    saveStrings("data/output.txt", newLines);
    close();
  }
}
