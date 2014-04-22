class SaveDialog extends Dialog {
  TextInput email,comment;
  
  SaveDialog() {
    super("Enter email:", 900, 400, true);
    setMsgX(15);
    setMsgY(30);

    float componentY = y + 50;
    email = new TextInput(x + 5, componentY, 700, 90, false, false, false, "_.@0123456789");
    addChild(email);

    componentY += 140;
    Label l = new Label("Comment:", x + 15, componentY);
    addChild(l);
    
    componentY += 20;
    comment = new TextInput(x + 5, componentY, 700, 90, false, true, true, "");
    addChild(comment);
    
     
  }
  
  void okPressed() {
    String emailStr = email.getValue().trim();
    if (emailStr.isEmpty()) {
      return;
    }
    String commentStr = comment.getValue().trim();
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
    newLines[newLines.length-1] = filename + ": email=" + emailStr + " ,comment=" + commentStr;
    saveStrings("output/output.txt", newLines);
    close();
  }
}
