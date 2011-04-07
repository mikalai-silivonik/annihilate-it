#include "msFrameBuffer.h"

msFrameBuffer::msFrameBuffer(GLuint id)
{
	m_id = id;
}

msFrameBuffer::msFrameBuffer(const char *name, msTexture *texture)
{
	m_name = name;
	m_texture = texture;

	// Generate handles for two Frame Buffer Objects
	glGenFramebuffers(1, &m_id);
}

msFrameBuffer::~msFrameBuffer(void)
{
		// don't forget release them:
	
	// Delete the FBOs
	//glDeleteFramebuffers(2, m_id);
}

const char* msFrameBuffer::getName()
{
	return m_name;
}

void msFrameBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

bool msFrameBuffer::isComplete()
{
	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

msTexture* msFrameBuffer::getTexture()
{
	return m_texture;
}

void msFrameBuffer::setSize( GLint width, GLint height )
{
	m_width = width;
	m_height = height;

	// init framebuffer and texture to render to (for fire)	
	glActiveTexture(GL_TEXTURE0 + m_texture->getUnit());

	// Bind and configure the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);	

	// Attach the texture to the first color buffer of an FBO and clear it
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getId(), 0);
}
