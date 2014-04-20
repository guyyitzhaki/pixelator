class TextInput extends Container {

  ArrayList<TextButton> btns = new ArrayList<TextButton>(); 

  TextArea value;
  TextButton clear, add, del, heb, eng, arb, nums;
  boolean ltor;
  String extraChars;
  boolean showSpace = true;

  TextInput(float _x, float _y, float _w, float _h) {
    this(_x, _y, _w, _h, true, true, true, "");
  }

  TextInput(float _x, float _y, float _w, float _h, boolean showAdd, boolean showSpace, boolean showLangs, String extraChars) {
    super(_x, _y, _w, _h);
    this.extraChars = extraChars;
    this.showSpace = showSpace;
    setEnglish();

    value = new TextArea("", x + 10, y + 50, 300, 30);
    value.setHGap(3);
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
    addChild(del);
    btnX += btnGap;

    /*   clear = new TextButton("CLR", x + btnX, y + 50, 35, 30) {
     void mousePressed() {
     value.setText("");
     }
     };
     clear.setHGap(3);
     addChild(clear);
     btnX += btnGap;
     */

    if (showAdd) {
      add = new TextButton("⏎", x + btnX, y + 50, 35, 30) {
        void mousePressed() {
          int len = value.getText().length();
          if (len > 0) {
            current = canvas.addLayer(new TextLayer(value.getText(), canvas.getWidth() / 2, canvas.getHeight() / 2, 16));
            value.setText("");
          }
        }
      };
      addChild(add);
      btnX += btnGap;
    }



    if (showLangs) {
      nums = new TextButton("NUM", x + btnX, y + 50, 35, 30) {
        void mousePressed() {
          setNumbers();
        }
      };
      addChild(nums);
      btnX += btnGap;
      
      heb = new TextButton("תירבע", x + btnX, y + 50, 35, 30) {
        void mousePressed() {
          setHebrew();
        }
      };
      addChild(heb);
      btnX += btnGap;

      eng = new TextButton("ENG", x + btnX, y + 50, 35, 30) {
        void mousePressed() {
          setEnglish();
        }
      };
      addChild(eng);
      btnX += btnGap;

      arb = new TextButton("ARB", x + btnX, y + 50, 35, 30) {
        void mousePressed() {
          setArabic();
        }
      };
      addChild(arb);
      btnX += btnGap;
    }
  }

  void setEnglish() {
    setLanguage("ABCDEFGHIJKLMNOPQRSTUVWXYZ", true);
  }

  void setHebrew() {
    setLanguage("אבגדהוזחטיכךלמםנןסעפףצץקרשת", false);
  }

  void setNumbers() {
    setLanguage("0123456789!@#$%*()_+=-?/:,.", true);
  }

  void setArabic() {
    setLanguage("يوهنملكقفغعظطضصشسزرذدخحجثتبا", false);
  }

  void setLanguage(String chars, final boolean ltor) {
    if (children != null)
      children.removeAll(btns);
    for (TextButton b : btns) {
      b.dispose();
    }
    btns.clear();
    this.ltor = ltor;
    float buttonx = x + 10;

    for (int i = 0; i < chars.length(); i++) {
      TextButton b = new TextButton(""+chars.charAt(i), buttonx, y, 18, 30) {
        public void mousePressed() {
          String newval = ltor ? value.getText() + getId() : getId() + value.getText();
          value.setText(newval);
        }
      };
      setBtnParams(b);

      buttonx += 22;
    }

    if (showSpace) {
      TextButton spc = new TextButton("  ", buttonx, y, 30, 30) {
        public void mousePressed() {
          String newval = ltor ? value.getText() + " " : " " + value.getText();
          value.setText(newval);
        }
      };
      setBtnParams(spc);
      buttonx += 34;
    }

    for (int i = 0; i < extraChars.length(); i++) {
      char c = extraChars.charAt(i);  
      TextButton extra = new TextButton("" + c, buttonx, y, 18, 30) {
        public void mousePressed() {
          String newval = ltor ? value.getText() + getId() : getId() + value.getText();
          value.setText(newval);
        }
      };
      setBtnParams(extra);
      buttonx += 22;
    }
  }

  String getValue() {
    return value.getText();
  }

  void setBtnParams(TextButton b) {
    b.setVGap(5);
    btns.add(b);
    addChild(b);
  }
}

