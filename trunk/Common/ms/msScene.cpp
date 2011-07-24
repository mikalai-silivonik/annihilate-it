//=================================================================================================================================
//
// Author: Maurice Ribble
//         3D Application Research Group
//         ATI Research, Inc.
//
// Implementation for the Scene class.  This class wraps in drawing in GL.
//
//=================================================================================================================================
// $Id: //depot/3darg/Tools/Handheld/esTestApps/common/Scene.cpp#1 $ 
// 
// Last check-in:  $DateTime: 2008/01/23 11:45:40 $ 
// Last edited by: $Author: dginsbur $
//=================================================================================================================================
//   (C) ATI Research, Inc. 2006 All rights reserved. 
//=================================================================================================================================

#include "msScene.h"
#include "ShaderProgram/msShaderPrograms.h"
#include "msParticleEmitter.h"

#include "Annihilate/msSpline.h"
//=================================================================================================================================
///
/// Constructor
///
/// \param none
///
/// \return null
//=================================================================================================================================
msScene::msScene()
{
   GLfloat color[] = { 0.3f, 0.3f, 0.7f, 1.0f };
   m_clearColor[0] = color[0];
   m_clearColor[1] = color[1];
   m_clearColor[2] = color[2];
   m_clearColor[3] = color[3];

   m_afterShockRadius = -1.0f;
   m_afterShockPower = 1.0f;
   m_afterShockLocation[0] = 0.0f;
   m_afterShockLocation[1] = 0.0f;
   m_animate = 0;

   m_afterShockRadiusMin = 0.0f;
   m_afterShockRadiusMax = 1500.0f;
   m_afterShockRadiusStep = 37.0f;

   m_palette = 0;
   m_boxGrid = 0;
   m_renderer = 0;
}

void msScene::newSize(GLint width, GLint height)
{
    m_size.width = width;
    m_size.height = height;
    
    m_shaders.notifySizeChanged(width, height);    
}


//=================================================================================================================================
///
/// Destructor
///
/// \param void
///
/// \return void
//=================================================================================================================================
msScene::~msScene()
{
    if(m_palette != 0)
        delete m_palette;
    if(m_boxGrid != 0)
        delete m_boxGrid;
    if(m_renderer != 0)
        delete m_renderer;
}

//=================================================================================================================================
///
/// Loads all the data from the file (textures, attributes, uniforms, shaders...)
///
/// \param fileName is the name for the file where we get the data
///
/// \return bool saying whether we passed or failed
//=================================================================================================================================
bool msScene::loadData(string filename)
{
   m_shaders.loadFile(filename);
   return true;
}






//=================================================================================================================================
//
// Author: Maurice Ribble
//         3D Application Research Group
//         ATI Research, Inc.
//
// More definitions for the Scene class.
//
//=================================================================================================================================
// $Id: //depot/3darg/Tools/Handheld/esTestApps/esTriangle/src/Draw.cpp#1 $ 
// 
// Last check-in:  $DateTime: 2008/01/23 11:45:40 $ 
// Last edited by: $Author: dginsbur $
//=================================================================================================================================
//   (C) ATI Research, Inc. 2006 All rights reserved. 
//=================================================================================================================================







GLfloat colorMap[][4] = 
{
    //{1.0f, 1.0f, 1.0f, 1.0f},//temp white
    {0.000f, 0.000f, 0.000f, 1.0f}, // black
    {0.851f, 0.851f, 0.851f, 1.0f}, // gray    
    {1.000f, 0.412f, 0.337f, 1.0f}, // red
    {0.467f, 0.357f, 1.000f, 1.0f}, // blue
    {0.502f, 0.824f, 0.776f, 1.0f}, // green    
    {0.714f, 1.000f, 0.608f, 1.0f}, // lime
    {0.843f, 0.420f, 1.000f, 1.0f}, // violet
    {0.478f, 0.824f, 1.000f, 1.0f}, // turquoise
    {1.000f, 0.757f, 0.369f, 1.0f}, // yellow
};

void msScene::init()
{
    m_shaders.notifySizeChanged(m_size.width, m_size.height);

    // init palette
    m_palette = new msPalette(colorMap, 8);    
}


void msScene::drawFrame()
{
    if(m_renderer != 0)
        m_renderer->draw(m_size);
}

void msScene::mouseClick(int x, int y, int direction)
{
    if(m_boxGrid == 0 || m_renderer == 0)
        return;

    msPoint3f touchPoint;
    touchPoint.x = ((GLfloat)x / (GLfloat)m_size.width);
    touchPoint.y = ((GLfloat)y / (GLfloat)m_size.height);
    
    m_boxGrid->setDirection(direction);
    m_boxGrid->removeSimilarItemsAtPoint(touchPoint);
}

void msScene::setMainFrameBuffer(GLint id)
{
    m_shaders.setMainFrameBuffer(id);
}

void msScene::undoLastMove()
{
    if(m_boxGrid != 0)
        m_boxGrid->undo();
}

void msScene::start()
{
#define NUM_ROWS 10
#define NUM_COLS 8

    msBoxFaceData backFaces[NUM_ROWS * NUM_COLS];

    memset(backFaces, 0, sizeof(backFaces));

    if(m_boxGrid != 0)
    {
        m_boxGrid->extractPattern(backFaces);        

        delete m_boxGrid;
        delete m_renderer;
    }
    
    GLint pattern[NUM_ROWS * NUM_COLS] = 
    {
        3, 2, 1, 4,// 5, //6, 7, 2, 1, 4,
        2, 1, 3, 3,// 5, //6, 7, 2, 1, 4
        1, 3, 2, 4,// 5, //6, 7, 2, 1, 4,
        2, 3, 1, 5,// 1, //6, 7, 2, 1, 4,
        2, 3, 4, 5,// 1, //6, 7, 2, 1, 4,
        //2, 3, 4, //5, 1, //6, 7, 2, 1, 4,
        //2, 3, 4, //5, 1, //6, 7, 2, 1, 4,
        //1, 2, 3, 4, 5, 6, 7, 2, 1, 4,
        //1, 2, 3, 4, 5, 6, 7, 2, 1, 4,
        //1, 2, 3, 4, 5, 6, 7, 2, 1, 4,
        //1, 2, 3, 4, 5, 6, 7, 2, 1, 4,
        //1, 2, 3, 4, 5, 6, 7, 2, 1, 4,
        //1, 2, 3, 4, 5, 6, 7, 2, 1, 4,
        //1, 2, 3, 4, 5, 6, 7, 2, 1, 4,
        //1,1,
        //2,1,
    };
    
   //m_boxGrid = new msBoxGrid(m_palette, pattern, NUM_ROWS, NUM_COLS, 1.0f, 1.0f);

    m_boxGrid = new msBoxGrid(m_palette, 4, NUM_ROWS, NUM_COLS, 1.0f, 1.0f);
    m_renderer = new msBoxGridRenderer(&m_shaders, m_boxGrid);
   
    m_boxGrid->setBackPattern(backFaces);
    
    

    m_boxGrid->show();
    

}

void msScene::end()
{
    //m_boxGrid->hide();

    delete m_boxGrid;
    delete m_renderer;
    m_boxGrid = 0;
    m_renderer = 0;

    
}