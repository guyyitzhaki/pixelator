#ifndef CLINE_H
#define CLINE_H

#include <string>
#include "CPoint.h"
#include "CShape.h"
#include "ofMain.h"

using namespace std;

class CLine : public CShape
{
    public:
        CLine(CPoint _p1, CPoint _p2, bool temp);
        virtual ~CLine();
        virtual void draw();
        string getType();
        string getData();

    protected:
    private:
        CPoint p1,p2;
};

#endif // CLINE_H
