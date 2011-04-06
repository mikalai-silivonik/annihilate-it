#include "msParticleEmitter.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "time.h"

#include "ShaderProgram/msTexture.h"
#include "ShaderProgram/msShaderProgram.h"


msParticleEmitter::msParticleEmitter(
									 Vector2f inPosition ,
									 Vector2f inSourcePositionVariance,
									 GLfloat inSpeed,
									 GLfloat inSpeedVariance,
									 GLfloat inParticleLifeSpan,
									 GLfloat inParticleLifeSpanVariance,
									 GLfloat inAngle,
									 GLfloat inAngleVariance,
									 Vector2f inGravity,
									 Color4f inStartColor,
									 Color4f inStartColorVariance,
									 Color4f inFinishColor ,
									 Color4f inFinishColorVariance,
									 GLuint inMaxParticles ,
									 GLfloat inParticleSize,
									 GLfloat inParticleSizeVariance,
									 GLfloat inDuration,
									 GLboolean inBlendAdditive)
{
	//sharedGameState = [SingletonGameState sharedGameStateInstance];

	// If the texture name is not nil then allocate the texture for the points.  If the texture name is nil
	// and we do not assign a texture, then a quad will be drawn instead
	/*	if(inTextureName) {		
	// Create a new texture which is going to be used as the texture for the point sprites
	texture = [[Image alloc] initWithImage:[UIImage imageNamed:inTextureName] scale:1.0f filter:GL_NEAREST];
	}
	*/
	// Take the parameters which have been passed into this particle emitter and set the emitters
	// properties
	sourcePosition = inPosition;
	sourcePositionVariance = inSourcePositionVariance;
	speed = inSpeed;
	speedVariance = inSpeedVariance;
	particleLifespan = inParticleLifeSpan;
	particleLifespanVariance = inParticleLifeSpanVariance;
	angle = inAngle;
	angleVariance = inAngleVariance;
	gravity = inGravity;
	startColor = inStartColor;
	startColorVariance = inStartColorVariance;
	finishColor = inFinishColor;
	finishColorVariance = inFinishColorVariance;
	maxParticles = inMaxParticles;
	particleSize = inParticleSize;
	particleSizeVariance = inParticleSizeVariance;
	emissionRate = maxParticles/particleLifespan;
	duration = inDuration;
	blendAdditive = inBlendAdditive;

	// Allocate the memory necessary for the particle emitter arrays
	particles = (Particle*)calloc(maxParticles, sizeof(Particle));
	vertices = (PointSprite*)calloc(maxParticles, sizeof(PointSprite));
	colors = (Color4f*)calloc(maxParticles, sizeof(Color4f));

	// If one of the arrays cannot be allocated, then report a warning and return nil
	if(!(particles && vertices && colors))
	{
		printf("WARNING: ParticleEmitter - Not enough memory");
		if(particles)
			free(particles);
		if(vertices)
			free(vertices);
		if(colors)
			free(colors);
		return;
	}

	// Generate the VBO's
	glGenBuffers(1, &verticesID);
	glGenBuffers(1, &colorsID);

	// By default the particle emitter is active when created
	active = GL_TRUE;

	// Set the particle count to zero
	particleCount = 0;

	// Reset the elapsed time
	elapsedTime = 0;

	emitCounter = 0;
}

msParticleEmitter::~msParticleEmitter(void)
{
	free(vertices);
	free(colors);
	free(particles);
	
	/*if(texture != 0)
	[texture release];*/
}

static const GLfloat g_vertexPositions[] = {
	-1.0f, 1.0f,  0.0f,  1.0f,
	1.0f, -1.0f,  0.0f,  1.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f,  1.0f,  0.0f, 1.0f,
};

static const GLfloat g_vertexColors[] = {
	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f,
};

static const GLfloat g_vertexTexcoord[] = {
	0.0f, 0.f,   
	0.0f, 1.f,    
	1.f,  0.f,    
	1.f,  1.f,   
};

static const GLuint g_indices[] = {
	0, 1, 2, 3,
};



void msParticleEmitter::renderParticles(msShaderProgram *particleProgram)
{
	msTexture *particleTexture = particleProgram->getTexture("u_texture");
	glEnable(GL_BLEND);
	glEnable ( GL_TEXTURE_2D );

	glActiveTexture( GL_TEXTURE0 + particleTexture->getUnit() );
	glBindTexture( GL_TEXTURE_2D, particleTexture->getId() );

	if(blendAdditive)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}
	else
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	// Load the vertex attributes
	particleProgram->getAttribute("a_position")->setPointerAndEnable(3, GL_FLOAT, GL_FALSE, 0, vertices);
	particleProgram->getAttribute("a_color")->setPointerAndEnable(4, GL_FLOAT, GL_FALSE, 0, colors);
	particleProgram->getUniform("u_texture")->set1i(particleTexture->getUnit());

	glDrawArrays( GL_POINTS, 0, particleCount );

	glDisable ( GL_TEXTURE_2D );
	glDisable(GL_BLEND);
}

void msParticleEmitter::update(GLfloat delta)
{
	// If the emitter is active and the emission rate is greater than zero then emit
	// particles
	if(active && emissionRate)
	{
		GLfloat rate = 1.0f / emissionRate;
		emitCounter += delta;
		while(particleCount < maxParticles && emitCounter > rate)
		{
			addParticle();
			emitCounter -= rate;
		}

		elapsedTime += delta;
		if(duration != -1 && duration < elapsedTime)
			stopParticleEmitter();
	}

	// Reset the particle index before updating the particles in this emitter
	particleIndex = 0;

	// Loop through all the particles updating their location and color
	while(particleIndex < particleCount) {

		// Get the particle for the current particle index
		Particle *currentParticle = &particles[particleIndex];

		// If the current particle is alive then update it
		if(currentParticle->timeToLive > 0) {

			// Calculate the new direction based on gravity
			currentParticle->direction = Vector2fAdd(currentParticle->direction, gravity);
			//currentParticle->direction = Vector2fMultiply(currentParticle->direction, delta);
			currentParticle->position = Vector2fAdd(currentParticle->position, currentParticle->direction);

			// Reduce the life span of the particle
			currentParticle->timeToLive -= delta;

			// Place the position of the current particle into the vertices array
			vertices[particleIndex].x = currentParticle->position.x;
			vertices[particleIndex].y = currentParticle->position.y;

			// Place the size of the current particle in the size array
			vertices[particleIndex].size = currentParticle->particleSize;

			// Update the particles color
			currentParticle->color.red += (currentParticle->deltaColor.red * delta);
			currentParticle->color.green += (currentParticle->deltaColor.green * delta);
			currentParticle->color.blue += (currentParticle->deltaColor.blue * delta);
			currentParticle->color.alpha += (currentParticle->deltaColor.alpha * delta);

			// Place the color of the current particle into the color array
			colors[particleIndex] = currentParticle->color;

			// Update the particle counter
			particleIndex++;
		} else {

			// As the particle is not alive anymore replace it with the last active particle 
			// in the array and reduce the count of particles by one.  This causes all active particles
			// to be packed together at the start of the array so that a particle which has run out of
			// life will only drop into this clause once
			if(particleIndex != particleCount-1)
				particles[particleIndex] = particles[particleCount-1];
			particleCount--;
		}
	}
	/*
	// Now we have updated all the particles, update the VBOs with the arrays we have just updated
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PointSprite)*maxParticles, vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, colorsID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Color4f)*maxParticles, colors, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0); */
}

GLboolean msParticleEmitter::addParticle()
{
	// If we have already reached the maximum number of particles then do nothing
	if(particleCount == maxParticles)
		return GL_FALSE;

	// Take the next particle out of the particle pool we have created and initialize it
	Particle *particle = &particles[particleCount];
	initParticle(particle);

	// Increment the particle count
	particleCount++;

	// Return YES to show that a particle has been created
	return GL_TRUE; 
}

void msParticleEmitter::initParticle(Particle* particle)
{
	//srand((unsigned)time( NULL ));
	// Init the position of the particle.  This is based on the source position of the particle emitter
	// plus a configured variance.  The RANDOM_MINUS_1_TO_1 macro allows the number to be both positive
	// and negative
	particle->position.x = sourcePosition.x + sourcePositionVariance.x * RANDOM_MINUS_1_TO_1();
	particle->position.y = sourcePosition.y + sourcePositionVariance.y * RANDOM_MINUS_1_TO_1();

	// Init the direction of the particle.  The newAngle is calculated using the angle passed in and the
	// angle variance.
	float newAngle = (GLfloat)DEGREES_TO_RADIANS(angle + angleVariance * RANDOM_MINUS_1_TO_1());

	// Create a new Vector2f using the newAngle
	Vector2f vector = Vector2fMake(cosf(newAngle), sinf(newAngle));

	// Calculate the vectorSpeed using the speed and speedVariance which has been passed in
	float vectorSpeed = speed + speedVariance * RANDOM_MINUS_1_TO_1();

	// The particles direction vector is calculated by taking the vector calculated above and
	// multiplying that by the speed
	particle->direction = Vector2fMultiply(vector, vectorSpeed);

	// Calculate the particle size using the particleSize and variance passed in
	particle->particleSize = particleSize + particleSizeVariance * RANDOM_MINUS_1_TO_1();

	// Calculate the particles life span using the life span and variance passed in
	particle->timeToLive = particleLifespan + particleLifespanVariance * RANDOM_MINUS_1_TO_1();

	// Calculate the color the particle should have when it starts its life.  All the elements
	// of the start color passed in along with the variance as used to calculate the star color
	Color4f start = {0, 0, 0, 0};
	start.red = startColor.red + startColorVariance.red * RANDOM_MINUS_1_TO_1();
	start.green = startColor.green + startColorVariance.green * RANDOM_MINUS_1_TO_1();
	start.blue = startColor.blue + startColorVariance.blue * RANDOM_MINUS_1_TO_1();
	start.alpha = startColor.alpha + startColorVariance.alpha * RANDOM_MINUS_1_TO_1();

	// Calculate the color the particle should be when its life is over.  This is done the same
	// way as the start color above
	Color4f end = {0, 0, 0, 0};
	end.red = finishColor.red + finishColorVariance.red * RANDOM_MINUS_1_TO_1();
	end.green = finishColor.green + finishColorVariance.green * RANDOM_MINUS_1_TO_1();
	end.blue = finishColor.blue + finishColorVariance.blue * RANDOM_MINUS_1_TO_1();
	end.alpha = finishColor.alpha + finishColorVariance.alpha * RANDOM_MINUS_1_TO_1();

	// Calculate the delta which is to be applied to the particles color during each cycle of its
	// life.  The delta calculation uses the life space of the particle to make sure that the 
	// particles color will transition from the start to end color during its life time.
	particle->color = start;
	particle->deltaColor.red = (end.red - start.red) / particle->timeToLive;
	particle->deltaColor.green = (end.green - start.green) / particle->timeToLive;
	particle->deltaColor.blue = (end.blue - start.blue) / particle->timeToLive;
	particle->deltaColor.alpha= (end.alpha - start.alpha) / particle->timeToLive;
}

void msParticleEmitter::stopParticleEmitter()
{
	active = GL_FALSE;
	elapsedTime = 0;
	emitCounter = 0; 
}
