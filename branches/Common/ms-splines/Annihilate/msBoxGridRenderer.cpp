/*
 *  MsOpenGL.c
 *  AnnihilateIt
 *
 *  Created by user on 11-03-05.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "msBoxGridRenderer.h"


msBoxGridRenderer::msBoxGridRenderer(msShaderPrograms *shaders)
{
	m_shaders = shaders;
    
    m_animate = 0;
}

msBoxGridRenderer::~msBoxGridRenderer()
{
    for(msExplosionIterator ei = m_explosions.begin(); ei != m_explosions.end(); ei ++)
    {
        delete (*ei);
    }

	for(msWaveIterator wi = m_waves.begin(); wi != m_waves.end(); wi ++)
	{
		delete (*wi);
	}
}

static const GLubyte g_indices[] = { 0, 1, 2, 3 };

#define MS_BOX_PART_NONE 0
#define MS_BOX_PART_LEFT 1
#define MS_BOX_PART_BOTTOM 2
#define MS_BOX_PART_RIGHT 4
#define MS_BOX_PART_TOP 8


void msBoxGridRenderer::_drawFigure(msBoxList &boxes, msBox *box, msPalette *palette, msShaderProgram *m_program)
{
	msColor *c = palette->getColor(box->m_colorIndex);

	msSpline spl;
	_getBoxSplinePoints(boxes, box, spl, MS_BOX_PART_NONE);

	msPoint points[10000];
	int count = 0;
	spl.getSplinePoints(7, points, &count);

	for(int i = 0; i < count; i ++)
	{
		mBoxColorsTemp[i][0] = c->r;
		mBoxColorsTemp[i][1] = c->g;
		mBoxColorsTemp[i][2] = c->b;
		mBoxColorsTemp[i][3] = c->a;
	}

	m_program->getAttribute("position")->setPointerAndEnable( 3, GL_FLOAT, 0, 0, points );
	m_program->getAttribute("color")->setPointerAndEnable( 4, GL_FLOAT, 0, 0, mBoxColorsTemp );

	glDrawArrays(GL_LINE_LOOP, 0, count);	
}


bool contains(msBoxList &list, msBox *item)
{
	for(msBoxIterator i = list.begin(); i != list.end(); i ++)
		if(*i == item)
			return true;
	return false;
}



void msBoxGridRenderer::_getBoxSplinePoints( msBoxList & boxes, msBox * box, msSpline &spl, char from)
{
	msPoint l = box->m_location;
	msSize s = box->m_size;

	float dx = s.width / 4.0f;
	float dy = s.height / 4.0f;

	boxes.remove(box);

	if((box->getLeft() == 0))
	{
		if(from == MS_BOX_PART_TOP)
		{
			spl.addControlPoint(l.x, l.y + dy);		
			spl.addControlPoint(l.x, l.y + s.height - dy);
			spl.addControlPoint(l.x + dx, l.y + s.height);
		}
		else if(from == MS_BOX_PART_RIGHT || from == MS_BOX_PART_BOTTOM || from == MS_BOX_PART_NONE)
		{
			spl.addControlPoint(l.x, l.y + s.height - dy);
			spl.addControlPoint(l.x + dx, l.y + s.height);
		}
		else
		{
			spl.addControlPoint(l.x + dx, l.y);				
			spl.addControlPoint(l.x, l.y + dy);		
			spl.addControlPoint(l.x, l.y + s.height - dy);
			spl.addControlPoint(l.x + dx, l.y + s.height);
		}
	}
	else
	{
		if(contains(boxes, box->getLeft()))
		{
			_getBoxSplinePoints(boxes, box->getLeft(), spl, MS_BOX_PART_RIGHT);
			from = MS_BOX_PART_LEFT;
		}
	}

	if((box->getBottom() == 0))
	{
		if(from == MS_BOX_PART_LEFT)
		{
			spl.addControlPoint(l.x + dx, l.y + s.height);
			spl.addControlPoint(l.x + s.width - dx, l.y + s.height);
			spl.addControlPoint(l.x + s.width, l.y + s.height - dy);
		}
		else if(from == MS_BOX_PART_TOP || from == MS_BOX_PART_RIGHT || from == MS_BOX_PART_NONE)
		{
			spl.addControlPoint(l.x + s.width - dx, l.y + s.height);
			spl.addControlPoint(l.x + s.width, l.y + s.height - dy);
		}
		else
		{
			spl.addControlPoint(l.x, l.y + s.height - dy);
			spl.addControlPoint(l.x + dx, l.y + s.height);
			spl.addControlPoint(l.x + s.width - dx, l.y + s.height);
			spl.addControlPoint(l.x + s.width, l.y + s.height - dy);
		}		
	}
	else
	{
		if(contains(boxes, box->getBottom()))
		{
			_getBoxSplinePoints(boxes, box->getBottom(), spl, MS_BOX_PART_TOP);
			from = MS_BOX_PART_BOTTOM;
		}
	}

	if((box->getRight() == 0))
	{
		if(from == MS_BOX_PART_BOTTOM)
		{
			spl.addControlPoint(l.x + s.width, l.y + s.height - dy);
			spl.addControlPoint(l.x + s.width, l.y + dy);
			spl.addControlPoint(l.x + s.width - dx, l.y);
		}
		else if(from == MS_BOX_PART_LEFT || from == MS_BOX_PART_TOP || from == MS_BOX_PART_NONE)
		{
			spl.addControlPoint(l.x + s.width, l.y + dy);
			spl.addControlPoint(l.x + s.width - dx, l.y);
		}
		else
		{
			spl.addControlPoint(l.x + s.width - dx, l.y + s.height);
			spl.addControlPoint(l.x + s.width, l.y + s.height - dy);
			spl.addControlPoint(l.x + s.width, l.y + dy);
			spl.addControlPoint(l.x + s.width - dx, l.y);
		}
	}
	else
	{
		if(contains(boxes, box->getRight()))
		{
			_getBoxSplinePoints(boxes, box->getRight(), spl, MS_BOX_PART_LEFT);
			from = MS_BOX_PART_RIGHT;
		}
	}

	if((box->getTop() == 0))
	{
		if(from == MS_BOX_PART_RIGHT)
		{
			spl.addControlPoint(l.x + s.width - dx, l.y);
			spl.addControlPoint(l.x + dx, l.y);
			spl.addControlPoint(l.x, l.y + dy);
		}
		else if(from == MS_BOX_PART_BOTTOM || from == MS_BOX_PART_LEFT || from == MS_BOX_PART_NONE)
		{
			spl.addControlPoint(l.x + dx, l.y);
			spl.addControlPoint(l.x, l.y + dy);
		}
		else
		{		
			spl.addControlPoint(l.x + s.width, l.y + dy);		
			spl.addControlPoint(l.x + s.width - dx, l.y);
			spl.addControlPoint(l.x + dx, l.y);
			spl.addControlPoint(l.x, l.y + dy);
		}
	}
	else
	{
		if(contains(boxes, box->getTop()))
		{
			_getBoxSplinePoints(boxes, box->getTop(), spl, MS_BOX_PART_BOTTOM);
			from = MS_BOX_PART_TOP;
		}
	}
}


void msBoxGridRenderer::_drawLine(msShaderProgram *m_program, msPoint &start, msPoint &end, msColor *color)
{
	GLfloat coords[] = {start.x, start.y, start.z /*+ 0.1f*/, end.x, end.y, end.z /*+ 0.1f*/};
	GLfloat colors[] = {color->r, color->g, color->b, color->a, color->r, color->g, color->b, color->a};

	m_program->getAttribute("position")->setPointerAndEnable( 3, GL_FLOAT, 0, 0, coords );
	m_program->getAttribute("color")->setPointerAndEnable( 4, GL_FLOAT, 0, 0, colors );
    
    glDrawArrays(GL_LINES, 0, 2);
}

void msBoxGridRenderer::drawLeftBorder(msShaderProgram *m_program, msBox *box, msColor *color)
{
	msPoint start = box->m_location;
	msPoint end = box->m_location;
	end.y += box->m_size.height;
    
    _drawLine(m_program, start, end, color);
}

void msBoxGridRenderer::drawTopBorder(msShaderProgram *m_program, msBox *box, msColor *color)
{
	msPoint start = box->m_location;
	msPoint end = box->m_location;
	end.x += box->m_size.width;

	_drawLine(m_program, start, end, color);
}

void msBoxGridRenderer::drawRightBorder(msShaderProgram *m_program, msBox *box, msColor *color)
{
	msPoint start = box->m_location;
	start.x += box->m_size.width;
	msPoint end = box->m_location;
	end.x += box->m_size.width;
	end.y += box->m_size.height;

	_drawLine(m_program, start, end, color);
}


void msBoxGridRenderer::drawBottomBorder(msShaderProgram *m_program, msBox *box, msColor *color)
{
	msPoint start = box->m_location;
	start.x += box->m_size.width;
	start.y += box->m_size.height;
	msPoint end = box->m_location;
	end.y += box->m_size.height;

	_drawLine(m_program, start, end, color);
}

GLuint gi = 0;

void msBoxGridRenderer::draw(msBoxGrid *boxGrid, msSize size)
{
    m_size = size;

    drawBoxesWithShockWave(boxGrid);

    drawExplosions();

    removeInactiveEmitters();

    // update all animations
    for(int y = 0; y < boxGrid->m_rowCount; y ++)
    {
        for(int x = 0; x < boxGrid->m_columnCount; x ++)
        {
            msBox *box = boxGrid->getItem(y, x);
            box->getAnimations()->performStep();
        }
    }

	boxGrid->getAnimations()->performStep();

    gi ++;
}

size_t m_lastExplosions = 0;

void msBoxGridRenderer::showExplosions()
{
    for(msExplosionIterator ei = m_explosions.begin(); ei != m_explosions.end(); ei ++)
    {
        printf("explosions:\t%d\r\n", (*ei)->particleCount);
    }
}

void msBoxGridRenderer::drawBoxGrid(msShaderProgram *program, msBoxGrid *boxGrid, msSize size)
{
	msBoxList boxesToDraw;

    for(int y = 0; y < boxGrid->m_rowCount; y ++)
	{
        for(int x = 0; x < boxGrid->m_columnCount; x ++)
		{
			msBox *box = boxGrid->getItem(y, x);
			boxesToDraw.push_back(box);

			// first check for explosion and if box is required one put it into list to be used after grid rendering
			if(box->getRequiresExplosion())
			{
				m_explosions.push_back(_createExplosionPe(box->m_explosionPoint, size));
			}

			if(box->getRequiresWave())
			{
				m_waves.push_back(_createWave(box));
			}
		}
	}

	while(boxesToDraw.begin() != boxesToDraw.end())
	{
		msBox *box = *boxesToDraw.begin();

		if(box->isVisible())
		{
			_drawFigure(boxesToDraw, box, boxGrid->m_palette, program);
		}
	}
}

void msBoxGridRenderer::removeInactiveEmitters()
{
    msExplosionList explosionsToDelete;

    for(msExplosionIterator ei = m_explosions.begin(); ei != m_explosions.end(); ei ++)
        if(!(*ei)->isAlive())
            explosionsToDelete.push_back(*ei);

    for(msExplosionIterator ei = explosionsToDelete.begin(); ei != explosionsToDelete.end(); ei ++)
    {
        delete (*ei);
        m_explosions.remove(*ei);
    }

	msWaveList wavesToDelete;

	for(msWaveIterator wi = m_waves.begin(); wi != m_waves.end(); wi ++)
		if(!(*wi)->isAlive())
			wavesToDelete.push_back(*wi);

	for(msWaveIterator wi = wavesToDelete.begin(); wi != wavesToDelete.end(); wi ++)
	{
		delete (*wi);
		m_waves.remove(*wi);
	}
}

static const GLfloat g_fbVertexPositions2[] = {
	-1.f, -1.f, -1.f, 1.f,
	 1.f, -1.f, -1.f, 1.f,
	-1.f,  1.f, -1.f, 1.f,
	 1.f,  1.f, -1.f, 1.f,
};

static const GLfloat g_fbVertexTexcoord2[] = {
	0.f, 0.f,   
	1.f, 0.f,    
	0.f, 1.f,    	
	1.f, 1.f,   
};

static const GLubyte g_fbIndices2[] = {
	0, 1, 2, 3,
};

void msBoxGridRenderer::drawExplosions()
{
	// render fire into texture using particle shaders
    msShaderProgram *program = m_shaders->getProgramByName("particle_create");
	program->use();

	// Switch the render target to the current FBO to update the texture map
	program->getFrameBuffer("renderTex")->bind();

	// FBO attachment is complete?
	if (program->getFrameBuffer("renderTex")->isComplete())		
	{
		// Set viewport to size of texture map and erase previous image
		glViewport(0, 0, (GLsizei)m_size.width, (GLsizei)m_size.height);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// render particles
        for(msExplosionIterator ei = m_explosions.begin(); ei != m_explosions.end(); ei ++)
        {
            (*ei)->renderParticles(program);
            (*ei)->update(0.015f);
        }
	}

	// Unbind the FBO so rendering will return to the backbuffer.
	m_shaders->getMainFrameBuffer()->bind();

	// usual renderer

	// Set viewport to size of framebuffer and clear color and depth buffers

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind updated texture map
	program->getFrameBuffer("renderTex")->getTexture()->active();
	program->getFrameBuffer("renderTex")->getTexture()->bind();

	msShaderProgram *particleCompleteProgram = m_shaders->getProgramByName("particle_complete");
	particleCompleteProgram->use();

	particleCompleteProgram->getUniform("u2_texture")->set1i(program->getFrameBuffer("renderTex")->getTexture()->getUnit());
	particleCompleteProgram->getAttribute("a2_position")->setPointerAndEnable(4, GL_FLOAT, 0, 0, g_fbVertexPositions2 );
	particleCompleteProgram->getAttribute("a2_texcoord")->setPointerAndEnable(2, GL_FLOAT, 0, 0, g_fbVertexTexcoord2 );

	// draw with client side arrays (in real apps you should use cached VBOs which is much better for performance)
	glDrawElements( GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, g_fbIndices2 );	

	glDisable(GL_BLEND);
}

msParticleEmitter* msBoxGridRenderer::_createExplosionPe(msPoint location, msSize screenSize)
{
    float k = (screenSize.width / 320.0f / 2.0f) + (screenSize.height / 480.0f / 2.0f);

    return new msParticleEmitter(
		// explosion
		Vector2fMake(location.x, location.y),//position:
		Vector2fMake(0.031f, 0.031f),//sourcePositionVariance:
		0.0001f,//speed:
		0.007f,//speedVariance:
		0.5f,//particleLifeSpan:
		0.25f,//particleLifespanVariance:
		0.0f,//angle:
		360.0f,//angleVariance:
		Vector2fMake(0.0f, -0.0000025f),//gravity:
		colorMake(1.0f, 0.5f, 0.05f, 1.0f),//startColor:
		colorMake(0.0f, 0.0f, 0.0f, 0.5f),//startColorVariance:
		colorMake(0.2f, 0.0f, 0.0f, 0.0f),//finishColor:
		colorMake(0.2f, 0.0f, 0.0f, 0.0f),//finishColorVariance:
		200,//maxParticles:
		50 * k,//particleSize:
		3 * k,//particleSizeVariance:
		0.125f,//duration:
		GL_TRUE//blendAdditive:
		);
}

static const GLfloat g_vertexPositions[] = {
	-1.0f, -1.0f,  -1.0f,  1.0f,
	1.0f, -1.0f,  -1.0f,  1.0f,
	-1.0f,  1.0f,  -1.0f, 1.0f,
	1.0f,  1.0f,  -1.0f, 1.0f,
};

static const GLfloat g_vertexTexcoord[] = {
	0.0f, 0.f,   
	1.f,  0.f,    
	0.0f, 1.f,    	
	1.f,  1.f,   
};


void msBoxGridRenderer::drawBoxesWithShockWave(msBoxGrid *boxGrid)
{
    // render fire into texture using particle shaders
	msShaderProgram *program = m_shaders->getProgramByName("boxgrid");
	program->use();

	// Switch the render target to the current FBO to update the texture map
	program->getFrameBuffer("renderTex")->bind();

	// FBO attachment is complete?
	if (program->getFrameBuffer("renderTex")->isComplete())
	{
		// Set viewport to size of texture map and erase previous image
		glViewport(0, 0, m_size.width, m_size.height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT );

		// render background
        drawBoxGrid(program, boxGrid, m_size);
	}

	// Unbind the FBO so rendering will return to the backbuffer.
	m_shaders->getMainFrameBuffer()->bind();

	// usual renderer

	// Bind updated texture map
	program->getFrameBuffer("renderTex")->getTexture()->active();
	program->getFrameBuffer("renderTex")->getTexture()->bind();

    GLuint u = program->getFrameBuffer("renderTex")->getTexture()->getUnit();

    program = m_shaders->getProgramByName("shockwave");
    program->use();

	program->getUniform("tex")->set1i(u);
	program->getUniform("tex")->set1i(u);
	program->getAttribute("position")->setPointerAndEnable(4, GL_FLOAT, 0, 0, g_vertexPositions );
	program->getAttribute("texcoord")->setPointerAndEnable(2, GL_FLOAT, 0, 0, g_vertexTexcoord );

	// wave
	for(msWaveIterator i = m_waves.begin(); i != m_waves.end(); i ++)
	{
		(*i)->render(program);
		(*i)->step();
	}

	// draw with client side arrays (in real apps you should use cached VBOs which is much better for performance)
	glDrawElements( GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, g_indices );
}

msWaveEmitter* msBoxGridRenderer::_createWave( msBox* box)
{
	msPoint location;
	location.x = box->m_location.x + box->m_size.width / 2.0;
	location.x /= 2.0f;
	location.x *= m_size.width;
	location.y = box->m_location.y + box->m_size.height / 2.0;
	location.y /= 2.0f;
	location.y = 1.0f - location.y;
	location.y *= m_size.height;

	return new msWaveEmitter(location, m_size);
}
