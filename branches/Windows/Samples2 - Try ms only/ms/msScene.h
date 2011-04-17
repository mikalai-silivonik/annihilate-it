//=================================================================================================================================
//
// Author: Maurice Ribble
//         3D Application Research Group
//         ATI Research, Inc.
//
// Definitions for the Scene class.  This class wraps in drawing in GL.
//
//=================================================================================================================================
// $Id: //depot/3darg/Tools/Handheld/esTestApps/common/Scene.h#1 $ 
// 
// Last check-in:  $DateTime: 2008/01/23 11:45:40 $ 
// Last edited by: $Author: dginsbur $
//=================================================================================================================================
//   (C) ATI Research, Inc. 2006 All rights reserved. 
//=================================================================================================================================

#ifndef _SCENE_H_
#define _SCENE_H_

#include "msGL.h"

#include <iostream>
#include <fstream>

#include "ShaderProgram/msShaderPrograms.h"
#include "msParticleEmitter.h"


//=================================================================================================================================
/// Scene class - This class loads and draws the scene.
//=================================================================================================================================
class msScene
{
private:
	msParticleEmitter *pe1;
	msParticleEmitter *pe2;
	msParticleEmitter *pe3;

public:
   /// \brief Constructor.
   msScene();

   /// \brief Destructor.
   ~msScene();

   void init();

   /// \brief Draws the frame.
   void drawFrame();

   /// \brief Loads all the data from the file (textures, attributes, uniforms, shaders...)
   bool loadData(const char* filename);

   /// \brief Puts the named program in use
   void useProgram( const char* name );

   /// \brief Updates the value of a uniform
   bool updateUniform( const char* name, GLfloat* vals );

   /// \brief Binds a texture into GL
   bool bindTexture( const char* name, GLint width, GLint height );

   /// \brief Binds an FBO into GL
   bool bindFbo( const char* name, GLint width, GLint height );

   void mouseClick(int x, int y);

   void newSize(int width, int height);

   void drawBackground();
   void drawExplosion();

private:

   /// Contains all the uniform/texture/attribute/shader data
   msShaderPrograms m_shaders;

   /// Pointer to the clear color
   GLfloat m_clearColor[4];

   int _width;
   int _height;
};

#endif // _SCENE_H_
