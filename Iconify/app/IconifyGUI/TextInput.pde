class TextInput extends Container {

  ArrayList<TextButton> btns = new ArrayList<TextButton>(); 

  TextButton value;
  TextButton clear, add, del;

  TextInput(float _x, float _y, float _w, float _h) {
    super(_x, _y, _w, _h);

    float buttonx = x + 10;
    for (int i = 'A'; i <= 'Z'; i++) {
      TextButton b = new TextButton(""+char(i), buttonx, y, 18, 30) {
        void mousePressed() {
          value.setText(value.getText() + getId());
        }
      };
      setBtnParams(b);

      buttonx += 22;
    }

    TextButton spc = new TextButton("_", buttonx, y, 18, 30) {
      void mousePressed() {
        value.setText(value.getText() + " ");
      }
    };
    setBtnParams(spc);
    buttonx += 22;

    value = new TextButton("", x + 10, y + 50, 300, 30);
    addChild(value);

    clear = new TextButton("CLEAR", x + 330, y + 50, 50, 30) {
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
          canvas.addLayer(new TextLayer(value.getText(), canvas.getWidth() / 2, canvas.getHeight() / 2));
          value.setText("");
        }
      }
    };
    add.setHGap(3);
    addChild(add);
  }

  void setBtnParams(TextButton b) {
    b.setHGap(3);
    b.setVGap(5);
    btns.add(b);
    addChild(b);
  }
}

