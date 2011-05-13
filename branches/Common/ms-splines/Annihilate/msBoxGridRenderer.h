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
#include "msBoxAnimation.h"
#include "msBoxGrid.h"
#include "msPalette.h"
#include "../ShaderProgram/msShaderPrograms.h"
#include "../ShaderProgram/msShaderProgram.h"
#include "../msParticleEmitter.h"
#include "../msWaveEmitter.h"
#include <list>

using namespace std;

typedef list<msParticleEmitter*> msExplosionList;
typedef msExplosionList::iterator msExplosionIterator;

typedef list<msWaveEmitter*> msWaveList;
typedef msWaveList::iterator msWaveIterator;

class msBoxGridRenderer
{
	msShaderPrograms *m_shaders;

    msExplosionList m_explosions;

	msWaveList m_waves;

    msSize m_size;

    GLfloat mBoxVertexesTemp[4][3]; // 6 vertexes for defining quad by means of two triangles (2 vertex for each)
    GLfloat mBoxColorsTemp[1000][4];
    GLfloat mBoxBordersTemp[6]; // array of vertexes for box borders

	void drawBox(msShaderProgram *m_program, msPalette *palette, msBox *box, msColor *color);
	void _drawLine(msShaderProgram *m_program, msPoint &start, msPoint &end, msColor *color);
	void drawLeftBorder(msShaderProgram *m_program, msBox *box, msColor *color);
	void drawTopBorder(msShaderProgram *m_program, msBox *box, msColor *color);
	void drawRightBorder(msShaderProgram *m_program, msBox *box, msColor *color);
	void drawBottomBorder(msShaderProgram *m_program, msBox *box, msColor *color);

    void drawBoxGrid(msShaderProgram *program, msBoxGrid *boxGrid, msSize size);

    void drawExplosions();


    void drawBoxesWithShockWave(msBoxGrid *boxGrid);

    msParticleEmitter* _createExplosionPe(msPoint location, msSize screenSize);

	msWaveEmitter* _createWave(msBox *box);

    void removeInactiveEmitters();

    void showExplosions();


    
	GLboolean m_animate;

	

    GLint c;

public:
	msBoxGridRenderer(msShaderPrograms *shaders);
    ~msBoxGridRenderer();

	void draw(msBoxGrid *boxGrid, msSize size);
};

