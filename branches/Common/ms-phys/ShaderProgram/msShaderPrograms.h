#pragma once

#include "msShaderProgram.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

typedef std::vector<msShaderProgram*>       msShaderProgramList;
typedef msShaderProgramList::iterator    msShaderProgramIterator;


class msShaderPrograms
{
    msFrameBuffer *mainFrameBuffer;

public:
    msShaderPrograms(void);
    ~msShaderPrograms(void);

    msShaderProgramList shaderPrograms;

    // load programs description
    bool loadFile(string fileName);

    msShaderProgram* getProgramByName( const char *name );
    msFrameBuffer* getMainFrameBuffer();
    void setMainFrameBuffer(GLint frameBufferId);

    void notifySizeChanged( GLint width, GLint height );
};
