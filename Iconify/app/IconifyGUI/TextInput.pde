class TextInput extends Container {

  ArrayList<TextButton> btns = new ArrayList<TextButton>(); 

  TextButton value;
  TextButton clear, add, del, heb, eng, arb;
  boolean ltor;

  TextInput(float _x, float _y, float _w, float _h) {
    this(_x, _y, _w, _h, true);
  }

  TextInput(float _x, float _y, float _w, float _h, boolean showAdd) {
    super(_x, _y, _w, _h);

    setEnglish();

    value = new TextButton("", x + 10, y + 50, 300, 30);
    addChild(value);

    float btnX = 330;
    float btnGap = 50;
    del = new TextButton("DEL", x + btnX, y + 50, 35, 30) {
      void mousePressed() {
        int len = value.getText().length();
        if (len > 0) {
          if (ltor) {
            value.setText(value.getText().substring(0, len-1));
          }
          else {        
            value.setText(value.getText().substring(1));
          }
        }
      }
    };
    del.setHGap(3);
    addChild(del);
    btnX += btnGap;

    clear = new TextButton("CLR", x + btnX, y + 50, 35, 30) {
      void mousePressed() {
        value.setText("");
      }
    };
    clear.setHGap(3);
    addChild(clear);
    btnX += btnGap;
    
    if (showAdd) {
      add = new TextButton("ADD", x + btnX, y + 50, 35, 30) {
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
      btnX += btnGap;
    }

    heb = new TextButton("HEB", x + btnX, y + 50, 35, 30) {
      void mousePressed() {
        setHebrew();
      }
    };
    heb.setHGap(3);
    addChild(heb);
    btnX += btnGap;

    eng = new TextButton("ENG", x + btnX, y + 50, 35, 30) {
      void mousePressed() {
        setEnglish();
      }
    };
    eng.setHGap(3);
    addChild(eng);
    btnX += btnGap;

    arb = new TextButton("ARB", x + btnX, y + 50, 35, 30) {
      void mousePressed() {
        setArabic();
      }
    };
    arb.setHGap(3);
    addChild(arb);
    btnX += btnGap;
  }

  void setEnglish() {
    setLanguage('A', 'Z', true);
  }

  void setHebrew() {
    setLanguage('א', 'ת', false);
  }

  void setArabic() {
    setLanguage('ا', 'غ', false);
  }

  void setLanguage(char start, char end, final boolean ltor) {
    if (children != null)
      children.removeAll(btns);
    for (TextButton b : btns) {
      b.dispose();
    }
    btns.clear();
    this.ltor = ltor;
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
        value.setText(newval);
      }
    };
    setBtnParams(spc);
    buttonx += 22;
  }
  
  String getValue() {
    return value.getText();
  }

  void setBtnParams(TextButton b) {
    b.setHGap(4);
    b.setVGap(5);
    btns.add(b);
    addChild(b);
  }
}

