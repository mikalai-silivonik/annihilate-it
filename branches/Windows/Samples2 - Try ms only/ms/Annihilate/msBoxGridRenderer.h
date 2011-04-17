/*
 *  MsOpenGL.h
 *  AnnihilateIt
 *
 *  Created by user on 11-03-05.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "../msGL.h"
#include "../msCommon.h"
#include "msBox.h"
#include "msBoxGrid.h"
#include "msPalette.h"
#include "../ShaderProgram/msShaderProgram.h"

class msBoxGridRenderer
{
	msBoxGrid *boxGrid;

	void drawBox(msShaderProgram *program, msPalette *palette, msBox *box, msColor *color);
	void _drawLine(float v[6], msColor *color);
	void drawLeftBorder(msBox *box, msColor *color);
	void drawTopBorder(msBox *box, msColor *color);
	void drawRightBorder(msBox *box, msColor *color);
	void drawBottomBorder(msBox *box, msColor *color);

public:
	msBoxGridRenderer(msBoxGrid *boxGrid);

	void draw(msShaderProgram *program);
};

