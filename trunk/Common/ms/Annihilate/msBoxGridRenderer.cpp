/*
 *  MsOpenGL.c
 *  AnnihilateIt
 *
 *  Created by user on 11-03-05.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "msBoxGridRenderer.h"
#include "msSpline.h"
#include "../msMatrix.h"
#include "../msMatrixTransform.h"


msBoxGridRenderer::msBoxGridRenderer(msShaderPrograms *shaders, msBoxGrid *boxGrid)
{
    m_shaders = shaders;

    // calculate projection matrix
    float fovy = 45.0f / 180.0f * 3.1415926f;
    float z = -0.5f / tanf(fovy / 2.0f);

    msMatrixTransform transform;
    transform.translate(-0.5f, -0.5f, z)
        ->perspective(fovy, 1.0f, -1.0f, 1.0f)
        ->viewport(2.0f, 2.0f)          
        ->translate(-1.0f, -1.0f, 0.0f);
    m_projectionMatrix = *transform.getMatrix();
    
    m_boxGrid = boxGrid;

    // buffer for particles array
    glGenBuffers(1, &this->particleBuffer);
    this->particleBufferSize = 0;
    
    // buffer for vertices data
    unsigned long size = sizeof(msBoxData) * boxGrid->getRowCount() * boxGrid->getColCount();   
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
    
    
    // buffer for indicies
    GLushort boxIndicies[] = { 0, 1, 2, 3 };    
    glGenBuffers(1, &m_indexBuffer);
    // NOTE: there is only one element array buffer, so we bind it only once
    // be carefull if you created another one
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 4, boxIndicies, GL_STATIC_DRAW);
    
    
    // buffer for texture texcoords
    GLfloat textureOrientation[] = {
        // left
        0.f, 0.f,
        1.f, 0.f,
        0.f, 1.f,
        1.f, 1.f,
        // bottom
        1.f, 0.f,
        1.f, 1.f,
        0.f, 0.f,
        0.f, 1.f,
        // right
        1.f, 1.f,
        0.f, 1.f,
        1.f, 0.f,
        0.f, 0.f,
        // top
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f, 
        1.f, 0.f,
    };    
    glGenBuffers(1, &m_textureOrientationBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_textureOrientationBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 32, textureOrientation, GL_STATIC_DRAW);

    
    // buffer for position attribute
    static const GLfloat vertexPositions[] = {
        -1.f, -1.f, 1.f, 1.f,
        1.f, -1.f, 1.f, 1.f,
        -1.f,  1.f, 1.f, 1.f,
        1.f,  1.f, 1.f, 1.f,
    };
    glGenBuffers(1, &m_positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, vertexPositions, GL_STATIC_DRAW);
    
    
    glViewport(0, 0, m_size.width, m_size.height);
    
    
    // provide some static values
    msShaderProgram *program = m_shaders->getProgramByName("boxgrid");
    program->use();
    program->getAttribute("position")->enableVertexArray();
    program->getAttribute("borderTexelLeft")->enableVertexArray();
    program->getAttribute("borderTexelBottom")->enableVertexArray();
    program->getAttribute("borderTexelRight")->enableVertexArray();
    program->getAttribute("borderTexelTop")->enableVertexArray();
    program->getUniform("borderExternalLineTex")->set1i(program->getTexture("borderExternalLineTex")->getUnit());
    program->getUniform("borderInternalLineTex")->set1i(program->getTexture("borderInternalLineTex")->getUnit());
    program->getUniform("borderCornerTex")->set1i(program->getTexture("borderCornerTex")->getUnit());

    
    program = m_shaders->getProgramByName("shockwave");
    program->use();
    program->getAttribute("position")->enableVertexArray();
    program->getAttribute("texcoord")->enableVertexArray();
    program->getUniform("tex")->set1i(m_shaders->getProgramByName("boxgrid")->getFrameBuffer("renderTex")->getTexture()->getUnit());

    program = m_shaders->getProgramByName("particle_complete");
    program->use();
    program->getAttribute("a2_position")->enableVertexArray();
    program->getAttribute("a2_texcoord")->enableVertexArray();
    program->getUniform("u2_texture")->set1i(m_shaders->getProgramByName("particle_create")->getFrameBuffer("renderTex")->getTexture()->getUnit());

    program = m_shaders->getProgramByName("particle_create");
    program->use();
    program->getAttribute("a_position")->enableVertexArray();
    program->getAttribute("a_color")->enableVertexArray();
    program->getAttribute("a_size")->enableVertexArray();
    program->getUniform("u_texture")->set1i(program->getTexture("u_texture")->getUnit());
}

msBoxGridRenderer::~msBoxGridRenderer()
{
    for(msWaveIterator wi = m_waves.begin(); wi != m_waves.end(); wi ++)
    {
        delete (*wi);
    }
    
    glDeleteBuffers(1, &this->particleBuffer);
    glDeleteBuffers(1, &m_indexBuffer);
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteBuffers(1, &m_textureOrientationBuffer);
    glDeleteBuffers(1, &m_positionBuffer);
}

void msBoxGridRenderer::draw(msSizef size)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(msBoxData) * m_boxGrid->getRowCount() * m_boxGrid->getColCount(), m_boxGrid->m_boxVertexData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    
    m_size = size;
    
    if(m_boxGrid == 0)
        return;
    
    _drawBoxesWithShockWave();
    
    _drawExplosions();
    
    _removeInactiveEmitters();
    
    // update all animations
    for(int y = 0; y < m_boxGrid->m_rowCount; y ++)
    {
        for(int x = 0; x < m_boxGrid->m_columnCount; x ++)
        {
            msBox *box = m_boxGrid->getItem(y, x);
            box->getAnimations()->performStep();
        }
    }
    
    m_boxGrid->getAnimations()->performStep();
}

void msBoxGridRenderer::_drawBoxGrid(msShaderProgram *program, msSizef size)
{    
    for(int y = 0; y < m_boxGrid->m_rowCount; y ++)
    {
        for(int x = 0; x < m_boxGrid->m_columnCount; x ++)
        {
            msBox *box = m_boxGrid->getItem(y, x);
            
            // first check for explosion and if box is required one put it into list to be used after grid rendering
            if(box->getRequiresExplosion())
            {
                GLfloat ratio = m_boxGrid->getRowCount() / size.height / 2.0f + m_boxGrid->getColCount() / size.width / 2.0f;
                msParticleEmitterSettings settings = _createExplosionSettings(box->getExplosionPoint(), ratio);
                explosionsBundle.addParticleEmitter(settings);
            }
            
            if(box->getRequiresWave())
            {
                m_waves.push_back(_createWave(box));
            }
            
            if(box->isVisible())
            {
                // set common data for both next _drawBox calls
                _drawBox(program, box);
            }
        }
    }
    
}

void msBoxGridRenderer::_drawBox(msShaderProgram *program, msBox *box)
{
    msMatrixTransform transform;
    float boxAngle = box->getAngle();
    if(boxAngle != 0.0f)
    {
        msPoint3f angleVector = box->getAngleVector();
        msPoint3f center = box->getVerticesData()->getCenter();
        transform.translate(-center.x, -center.y, -center.z)
        ->rotate(boxAngle, angleVector)
        ->translate(center.x, center.y, center.z);
    }
    
    transform.multiplyMatrix(m_projectionMatrix);
    
    program->getUniform("mvp")->setMatrix4fv(1, false, transform.getMatrix()->getArray());
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);    
    unsigned long offset = (unsigned long)box->getVerticesData() - (unsigned long)m_boxGrid->m_boxVertexData;
    program->getAttribute("position")->setPointer(3, GL_FLOAT, GL_FALSE, sizeof(msPoint3f), (void*)offset);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_textureOrientationBuffer);    
    program->getAttribute("borderTexelLeft")->setPointer(2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void*)0);
    program->getAttribute("borderTexelBottom")->setPointer(2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void*)(sizeof(GLfloat) * 8));
    program->getAttribute("borderTexelRight")->setPointer(2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void*)(sizeof(GLfloat) * 16));
    program->getAttribute("borderTexelTop")->setPointer(2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void*)(sizeof(GLfloat) * 24));
    
    glCullFace(GL_FRONT);
    _drawFace(program, &box->getVerticesData()->frontFace);
    
    glCullFace(GL_BACK);
    _drawFace(program, &box->getVerticesData()->backFace);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void msBoxGridRenderer::_drawFace(msShaderProgram *program, msBoxFaceData *faceData)
{        
    program->getUniform("lineBorder")->set4iv(1, faceData->getHasBorder());

    msColor faceColor = *m_boxGrid->m_palette->getColor(faceData->getColorIndex());
    faceColor.r *= faceData->getColorDisturbance().r;
    faceColor.g *= faceData->getColorDisturbance().g;
    faceColor.b *= faceData->getColorDisturbance().b;
    faceColor.a *= faceData->getColorDisturbance().a;
    program->getUniform("color")->set4fv(1, (GLfloat*)&faceColor);
  
    program->getUniform("cornerBorder")->set4iv(1, faceData->getHasCornerBorder());

    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);       

    glDisable(GL_BLEND);    
    glDisable(GL_CULL_FACE);    
}


void msBoxGridRenderer::_removeInactiveEmitters()
{
    this->explosionsBundle.removeInactiveEmitters();

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



msParticleEmitterSettings msBoxGridRenderer::_createExplosionSettings(msPoint3f location, GLfloat ratio)
{
    float k = 0.03333333f / ratio;

    msParticleEmitterSettings settings;
    settings.position                   = msPoint2f(location.x, location.y);
    settings.sourcePositionVariance     = msPoint2f(0.031f, 0.031f);
    settings.speed                      = 0.0001f;
    settings.speedVariance              = 0.007f;
    settings.particleLifeSpan           = 0.5f;
    settings.particleLifeSpanVariance   = 0.25f;
    settings.angle                      = 0.0f;
    settings.angleVariance              = 360.0f;
    settings.gravity                    = msPoint2f(0.0f, -0.0000025f);
    settings.startColor                 = msColor(1.0f, 0.5f, 0.05f, 1.0f);
    settings.startColorVariance         = msColor(0.0f, 0.0f, 0.0f, 0.5f);
    settings.finishColor                = msColor(0.2f, 0.0f, 0.0f, 0.0f);
    settings.finishColorVariance        = msColor(0.2f, 0.0f, 0.0f, 0.0f);
    settings.maxParticles               = 200;
    settings.particleSize               = 50 * k;
    settings.particleSizeVariance       = 3 * k;
    settings.duration                   = 0.125f;
    settings.blendAdditive              = GL_TRUE;
    return settings;
}



void msBoxGridRenderer::_drawBoxesWithShockWave()
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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT );

        // render background
        _drawBoxGrid(program, m_size);
    }

    // Unbind the FBO so rendering will return to the backbuffer.
    m_shaders->getMainFrameBuffer()->bind();

    // usual renderer

    program = m_shaders->getProgramByName("shockwave");
    program->use();
    
    glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);    
    program->getAttribute("position")->setPointer(4, GL_FLOAT, 0, 0, (void*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_textureOrientationBuffer);    
    program->getAttribute("texcoord")->setPointer(2, GL_FLOAT, 0, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);    

    // wave
    for(msWaveIterator i = m_waves.begin(); i != m_waves.end(); i ++)
    {
        msWaveEmitter *we = *i;
        program->getUniform("radius")->set1f(we->m_radius);
        program->getUniform("power")->set1f(we->m_power);
        program->getUniform("location")->set2f(we->m_location.x / we->m_size.width, we->m_location.y / we->m_size.height);
        
        we->step();
    }

    // draw with client side arrays (in real apps you should use cached VBOs which is much better for performance)
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);
}

void msBoxGridRenderer::_drawExplosions()
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
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // render particles        
        _drawParticles(explosionsBundle, *program);
        explosionsBundle.update(0.015f);
    }

    // Unbind the FBO so rendering will return to the backbuffer.
    m_shaders->getMainFrameBuffer()->bind();

    // usual renderer

    // Set viewport to size of framebuffer and clear color and depth buffers

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind updated texture map
    //program->getFrameBuffer("renderTex")->getTexture()->active();
    //program->getFrameBuffer("renderTex")->getTexture()->bind();

    msShaderProgram *particleCompleteProgram = m_shaders->getProgramByName("particle_complete");
    particleCompleteProgram->use();
    
    glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);    
    particleCompleteProgram->getAttribute("a2_position")->setPointer(4, GL_FLOAT, 0, 0, (void*)0 );
    
    glBindBuffer(GL_ARRAY_BUFFER, m_textureOrientationBuffer);    
    particleCompleteProgram->getAttribute("a2_texcoord")->setPointer(2, GL_FLOAT, 0, 0, (void*)0 );
    glBindBuffer(GL_ARRAY_BUFFER, 0);    

    // draw with client side arrays (in real apps you should use cached VBOs which is much better for performance)
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);

    glDisable(GL_BLEND);
}

msWaveEmitter* msBoxGridRenderer::_createWave( msBox* box)
{
    msPoint3f location = box->getVerticesData()->getCenter();    
    location.x *= m_size.width;
    location.y = 1.0f - location.y;
    location.y *= m_size.height;

    return new msWaveEmitter(location, m_size);
}

void msBoxGridRenderer::_drawParticles(msParticleEmitterBundle &particleEmitters, msShaderProgram &particleProgram)
{
    int particlesCount = particleEmitters.getParticleCount();

    if(particlesCount == 0)
        return;

    if(this->particleBufferSize < particlesCount)
    {
        // if buffer size is not enough than recreate new buffer to fit the size and copy new data into it
        glBindBuffer(GL_ARRAY_BUFFER, this->particleBuffer);
        glBufferData(GL_ARRAY_BUFFER, particlesCount * sizeof(msParticleData), particleEmitters.getParticleData(), GL_DYNAMIC_DRAW);
        this->particleBufferSize = particlesCount;
    }
    else
    {
        // if buffer size is enough for particles than just copy new data into buffer
        glBindBuffer(GL_ARRAY_BUFFER, this->particleBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(msParticleData), particleEmitters.getParticleData());
    }

    glEnable(GL_BLEND);
    glEnable ( GL_TEXTURE_2D );

    //particleTexture->active();
    //particleTexture->bind();

    // todo: make blendAddictive as global setting for all emitters inside bundle
    //if(pe->blendAdditive)
    //{
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //}
    //else
    //{
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //}    

    // Load the vertex attributes
    particleProgram.getAttribute("a_position")->setPointer(2, GL_FLOAT, GL_FALSE, sizeof(msParticleData), (void*)0);
    particleProgram.getAttribute("a_color")->setPointer(4, GL_FLOAT, GL_FALSE, sizeof(msParticleData), (void*)offsetof(msParticleData, color));
    particleProgram.getAttribute("a_size")->setPointer(1, GL_FLOAT, GL_FALSE, sizeof(msParticleData), (void*)offsetof(msParticleData, size));

    glDrawArrays( GL_POINTS, 0, particlesCount);

    glDisable ( GL_TEXTURE_2D );
    glDisable(GL_BLEND);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
