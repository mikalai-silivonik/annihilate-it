#include "msBox.h"
#include "stdlib.h"
#include "stdio.h"
#include "msPalette.h"
#include "msBoxAnimation.h"
#include "msBoxGrid.h"

msBox::msBox()
{
	_init(0, 0, 0, 0, MS_BOX_INVISIBLE);	
}

msBox::msBox(float x, float y, float width, float height, int colorIndex)
{	
	_init(x, y, width, height, colorIndex);	
}



void msBox::_init( float x, float y, float width, float height, int colorIndex )
{
	m_location.x = x;
	m_location.y = y;
	m_location.z = 0.0f;
	m_size.width = width;
	m_size.height = height;
	m_colorIndex = colorIndex;
	m_originalColorIndex = colorIndex;
	m_border = (msBorder*)malloc(sizeof(msBorder));
	m_border->left = 0;
	m_border->top = 0;
	m_border->right = 0;
	m_border->bottom = 0;

	m_colorDisturbance.a = 1.0f;
	m_colorDisturbance.r = 1.0f;
	m_colorDisturbance.g = 1.0f;
	m_colorDisturbance.b = 1.0f;

	m_animations = new msAnimationBundle();

	m_requiresExplosion = false;
	m_requiresWave = false;

	m_top = 0;
	m_right = 0;
	m_bottom = 0;
	m_left = 0;

	m_angle = 0.0f;	
}

msBox::~msBox()
{
	free(m_border);

	delete m_animations;
}

void msBox::makeInvisible()
{
	m_colorIndex = MS_BOX_INVISIBLE;
}


GLboolean msBox::isVisible()
{
	if(this == 0)
		return GL_FALSE;

	return (m_colorIndex != MS_BOX_INVISIBLE);
}

msAnimationBundle* msBox::getAnimations()
{
    return m_animations;
}

void msBox::unitTest()
{
	msBox *box = new msBox(0, 0, 100, 100, 2);
	msAnimationBundle *anims = box->getAnimations();

	anims->add(new msAnimation(0, 5, new msFromToAnimationContext<GLfloat>(0, 2.5), 0));
	anims->add(new msAnimation(0, 10, new msFromToAnimationContext<GLfloat>(1, 7), 0));

	for(GLuint i = 0; i < 100; i ++)
	{
		for(msAnimationIterator ai = anims->getFirst(); ai != anims->getLast(); ai ++)
		{
			msFromToAnimationContext<GLfloat> *c = (msFromToAnimationContext<GLfloat> *)(*ai)->getContext();
			
			printf("%f\t%f\r\n", c->getFrom(), c->getTo());			
		}

		anims->performStep();
	}

	delete box;
}




void msBox::_linearFalling(msAnimationContext *c)
{
	msKeyValueAnimationContext<msPointf*, msPointf> *context = (msKeyValueAnimationContext<msPointf*, msPointf> *)c;

	msPointf *from = context->getKey();
	msPointf to = context->getValue();
	
	from->x += (to.x - from->x) / context->getAnimation()->getCount();
	from->y += (to.y - from->y) / context->getAnimation()->getCount();
}


void msBox::_linearFalling2(msAnimationContext *c)
{
	msPointMoveAnimationContext *context = (msPointMoveAnimationContext*)c;
	msPointf *p = context->getPoint();
    switch(context->getDirection())
    {
        case MS_BOX_SHIFT_TOP:
            p->y += MS_BOUNCE_OFFSET;
            break;
            
        case MS_BOX_SHIFT_LEFT:
            p->x += MS_BOUNCE_OFFSET;
            break;
            
        case MS_BOX_SHIFT_RIGHT:
            p->x -= MS_BOUNCE_OFFSET;
            break;
            
        default:
            p->y -= MS_BOUNCE_OFFSET;
    }    
}


void msBox::fall(GLint delay, GLint direction, msPointf newLocation)
{
    int times = 10;

    // moving from top to bottom
    msKeyValueAnimationContext<msPointf*, msPointf> *c2 = new msKeyValueAnimationContext<msPointf*, msPointf>(&m_location, newLocation);
	getAnimations()->add(new msAnimation(delay, times, c2, _linearFalling));
    
    // move a litle bit up for effect of bounce
    msPointMoveAnimationContext *c3 = new msPointMoveAnimationContext(&m_location, direction);
    getAnimations()->add(new msAnimation(delay + times, 4, c3, _linearFalling2));
    
    // final falling (very quick)
    msKeyValueAnimationContext<msPointf*, msPointf> *c4 = new msKeyValueAnimationContext<msPointf*, msPointf>(&m_location, newLocation);
    getAnimations()->add(new msAnimation(delay + times + 4, 1, c4, _linearFalling));
}

void msBox::unfall( int delay, int direction, msPointf newLocation )
{
	int times = 10;

	// moving from top to bottom
	msKeyValueAnimationContext<msPointf*, msPointf> *c2 = new msKeyValueAnimationContext<msPointf*, msPointf>(&m_location, newLocation);
	getAnimations()->add(new msAnimation(delay, times, c2, _linearFalling));	
}


void msBox::_hiding1(msAnimationContext *c)
{
    msFromToAnimationContext<msBoxAnimation*> *context = (msFromToAnimationContext<msBoxAnimation*>*)c;

    GLfloat k = 1.0f / context->getAnimation()->getCount();

    //context->getFrom()->m_colorDisturbance.a = 0.5;// -= 0.01f;
    context->getFrom()->m_colorDisturbance.r -= k;
    context->getFrom()->m_colorDisturbance.g -= k;
    context->getFrom()->m_colorDisturbance.b -= k;
}


void msBox::hide(GLint delay)
{
    // set requires explosion to make renderer know about that
    m_requiresExplosion = false;

	// store coordinates of explosion
	m_explosionPoint.x = m_location.x + m_size.width / 2.0f;
	m_explosionPoint.y = m_location.y + m_size.height / 2.0f;

    _setFlag<GLboolean>(delay + 1, &m_requiresExplosion, true);

    // the following animation will be called only once and it will just reset the requires explosion flag to stop its animation
    _setFlag<GLboolean>(delay + 2, &m_requiresExplosion, false);

	_setFlag<int>(delay + 2, &m_colorIndex, MS_BOX_INVISIBLE);
}

void rotate(msAnimationContext *c)
{
	msValueAnimationContext<float*> *con = (msValueAnimationContext<float*> *)c;
	float *angle = con->getValue();
	*angle -= 9;
}

void msBox::show( int delay )
{
	// restore original color after some delay
	m_colorIndex = m_originalColorIndex;

	// make 
	m_colorDisturbance.a = 0.0;
	
	// moving from top to bottom
	msKeyValueAnimationContext<float*, float> *c = new msKeyValueAnimationContext<float*, float>(&m_colorDisturbance.a, 1.0);
	getAnimations()->add(new msAnimation(delay, 15, c, _appearing));	

	m_angle = 90;

	msValueAnimationContext<float*> *c2 = new msValueAnimationContext<float*>(&m_angle);
	msAnimation *a = new msAnimation(0, 10, c2, rotate);
	getAnimations()->add(a);
}

void msBox::_appearing(msAnimationContext *c)
{
	msKeyValueAnimationContext<float*, float> *context = (msKeyValueAnimationContext<float*, float> *)c;

	float *from = context->getKey();
	float to = context->getValue();

	*from += (to - *from) / context->getAnimation()->getCount();
}

void msBox::wave(GLint delay)
{
    m_requiresWave = false;

    _setFlag<GLboolean>(delay, &m_requiresWave, true);
    _setFlag<GLboolean>(delay + 1, &m_requiresWave, false);
}




