#pragma once

#include "../msCommon.h"
#include "msBox.h"
#include "msGrid.h"
#include "msPalette.h"
#include <list>
#include <map>


using namespace::std;


#define MS_BOX_SHIFT_DOWN 0
#define MS_BOX_SHIFT_LEFT 1
#define MS_BOX_SHIFT_TOP 2
#define MS_BOX_SHIFT_RIGHT 3

#define MS_BOUNCE_OFFSET 0.008f 


#define MS_WAVE_DELAY 5


typedef list<msBox*> msBoxList;
typedef msBoxList::iterator msBoxIterator;

typedef map<msBox*, int> msBoxExplMap;
typedef msBoxExplMap::iterator msBoxExplIterator;


struct msMoveAction
{
    msPointi from;
    msPointi to;
    int direction;

    msMoveAction(int x1, int y1, int x2, int y2, int dir)
    {
        from = msPointi(x1, y1);
        to = msPointi(x2, y2);
        direction = dir;
    }

    msMoveAction invert()
    {
        int dir = direction;
        if(dir == MS_BOX_SHIFT_DOWN)
            dir = MS_BOX_SHIFT_TOP;
        else if(dir == MS_BOX_SHIFT_LEFT)
            dir = MS_BOX_SHIFT_RIGHT;
        else if(dir == MS_BOX_SHIFT_TOP)
            dir = MS_BOX_SHIFT_DOWN;
        else if(dir == MS_BOX_SHIFT_RIGHT)
            dir = MS_BOX_SHIFT_LEFT;

        return msMoveAction(to.x, to.y, from.x, from.y, dir);
    }
};

typedef list<msMoveAction> msMoveActionList;
typedef msMoveActionList::iterator msMoveActionIterator;


struct msHideAction
{
    msBox *box;

    msHideAction(msBox *b)
    {
        box = b;
    }
};

typedef list<msHideAction> msHideActionList;
typedef msHideActionList::iterator msHideActionIterator;




class msBoxGrid : public msGrid<msBox*>
{
    msSizef size;
    msGrid<msPoint3f*> *coordinateGrid;
    msMoveActionList _lastMovedBoxes;
    msHideActionList _lastHiddenBoxes;
    msAnimationBundle animations;    
    int lastKnownDirection;
    void _refreshBorders();

  

    void _refreshBoxFaceBorders(int y, int x, msBoxFaceData* (*faceResolver)(msBox *));

    static void _borderInvertion( msAnimationContext *context );

    void _animateBorderInvertion( msBox * box, GLint positive );

    GLint _checkNeighbours(GLint y1, GLint x1, GLint y2, GLint x2);

    GLint* _generate_random_pattern(GLint numRows, GLint numCols, GLint numColors);

    void display2();



    void _removeSimilarBoxes(GLint y, GLint x, GLint c, msBoxExplMap &removedBoxes, GLint level);

    GLint _checkBoxColor(GLint y, GLint x, GLint colorIndex, msBoxFaceData* (*faceResolver)(msBox *));

    void _exchangeBoxes(msGrid<msBox*> *grid, GLint y1, GLint x1, GLint y2, GLint x2);

    void _animateBoxHiding(msBoxExplMap &boxes);

    void _moveBox(msMoveAction move);

    void _shiftDown(msGrid<msBox*> *grid, msMoveActionList *moves);

    void _shiftTop(msGrid<msBox*> *grid, msMoveActionList *moves);

    void _shiftLeft(msGrid<msBox*> *grid, msMoveActionList *moves);

    void _shiftRight(msGrid<msBox*> *grid, msMoveActionList *moves);

    static void doBoxesFallingCallback(msAnimationContext *c);

public:

    msBoxData *boxVertexData;

    msPalette *palette;

    msAnimationBundle *getAnimations();

    void init(msPalette *palette, GLint *pattern, GLint numRows, GLint numCols, GLfloat gridHeight, GLfloat gridWidth);

    msBoxGrid(msPalette *palette, GLint *pattern, GLint numRows, GLint numCols, GLfloat gridHeight, GLfloat gridWidth);

    msBoxGrid(msPalette *palette, GLint numColors, GLint numRows, GLint numCols, GLfloat gridHeight, GLfloat gridWidth);

    ~msBoxGrid();

    void display();    
    
    void setDirection(int direction)
    {
        this->lastKnownDirection = direction;
    }

    void removeSimilarItems(GLint y, GLint x);

    void shiftPendentBoxes(GLint direction);

    GLint checkNeighbours(GLint y1, GLint x1, GLint y2, GLint x2);

    static void unitTest();

    void removeSimilarItemsAtPoint( msPoint3f touchPoint );

    void undo();
    void _moveBackBox( msMoveAction action );
    void show();
    void hide();


    void setBackPattern(msBoxFaceData *faces)
    {
        for(int y = 0; y < this->rowCount; y ++)
            for(int x = 0; x < this->columnCount; x ++)
                memcpy(&getItem(y, x)->verticesData->backFace, &faces[y * this->columnCount + x], sizeof(msBoxFaceData));                
    }

    void extractPattern(msBoxFaceData *faces)
    {
        for(int y = 0; y < this->rowCount; y ++)
        {
            for(int x = 0; x < this->columnCount; x ++)
            {
                msBox *box = getItem(y, x);
                memcpy(&faces[y * this->columnCount + x], &box->getVerticesData()->frontFace, sizeof(msBoxFaceData));
                
                if(!box->isVisible())
                    faces[y * this->columnCount + x].colorIndex = 0;
            }
        }
    }
};