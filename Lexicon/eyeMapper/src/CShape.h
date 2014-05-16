#ifndef CSHAPE_H
#define CSHAPE_H

#include <string>
#include "ofMain.h"
using namespace std;



class CShape
{
    public:
        CShape(bool temp);
        virtual ~CShape();
        unsigned int getId() { return id; }
        void setId(unsigned int val) { id = val; }
        bool isMask() { return mask; }
        void setMask(bool val) { mask = val; }
        bool getSelected() { return selected; }
        void setSelected(bool val) { selected = val; }
        virtual void draw() = 0;
        string serialize();
        virtual string getType() = 0;
        virtual string getData() = 0;

    protected:
    private:
        unsigned int id;
        bool mask;
        bool selected;
};

#endif // CSHAPE_H
