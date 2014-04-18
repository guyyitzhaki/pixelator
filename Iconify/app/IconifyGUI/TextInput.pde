class TextInput extends Container {

  ArrayList<TextButton> btns = new ArrayList<TextButton>(); 

  TextButton value;
  TextButton clear, add, del, heb, eng;

  TextInput(float _x, float _y, float _w, float _h) {
    super(_x, _y, _w, _h);

    setEnglish();

    value = new TextButton("", x + 10, y + 50, 300, 30);
    addChild(value);

    clear = new TextButton("CLR", x + 330, y + 50, 50, 30) {
      void mousePressed() {
        value.setText("");
      }
    };
    clear.setHGap(3);
    addChild(clear);
    del = new TextButton("DEL", x + 400, y + 50, 50, 30) {
      void mousePressed() {
        int len = value.getText().length();
        if (len > 0) {
          value.setText(value.getText().substring(0, len-1));
        }
      }
    };
    del.setHGap(3);
    addChild(del);
    add = new TextButton("ADD", x + 470, y + 50, 50, 30) {
      void mousePressed() {
        int len = value.getText().length();
        if (len > 0) {
          current = canvas.addLayer(new TextLayer(value.getText(), canvas.getWidth() / 2, canvas.getHeight() / 2, 16));
          value.setText("|");
        }
      }
    };
    add.setHGap(3);
    addChild(add);
    
    heb = new TextButton("HEB", x + 540, y + 50, 50, 30) {
      void mousePressed() {
        setHebrew();
      }
    };
    heb.setHGap(3);
    addChild(heb);
    
    eng = new TextButton("ENG", x + 610, y + 50, 50, 30) {
      void mousePressed() {
        setEnglish();
      }
    };
    eng.setHGap(3);
    addChild(eng);
  }
  
  void setEnglish() {
    setLanguage('A', 'Z', true);
  }
  
  void setHebrew() {
    setLanguage('א', 'ת', false);
  }
  
  void setLanguage(char start, char end, final boolean ltor) {
    if (children != null)
      children.removeAll(btns);
    for (TextButton b : btns) {
      b.dispose();
    }
    btns.clear();
    float buttonx = x + 10;
    for (int i = start; i <= end; i++) {
      TextButton b = new TextButton(""+char(i), buttonx, y, 18, 30) {
        public void mousePressed() {
          String newval = ltor ? value.getText() + getId() : getId() + value.getText();
          value.setText(newval);
        }
      };
      setBtnParams(b);

      buttonx += 22;
    }

    TextButton spc = new TextButton(" ", buttonx, y, 18, 30) {
      public void mousePressed() {
          String newval = ltor ? value.getText() + " " : " " + value.getText();
      }
    };
    setBtnParams(spc);
    buttonx += 22;

  }

  void setBtnParams(TextButton b) {
    b.setHGap(4);
    b.setVGap(5);
    btns.add(b);
    addChild(b);
  }
}

