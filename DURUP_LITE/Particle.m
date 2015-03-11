//
//  Particle.m
//  Tutorial1
//
//  Created by Michael Daley on 19/04/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Particle.h"
#import "ParticleEmitter.h"

@implementation Particle

@synthesize alive;
@synthesize texture;
@synthesize position;
@synthesize direction;
@synthesize timeToDie;

- (id)initParticleWithPosition:(Vector2f)inPosition 
					 direction:(Vector2f)inDirection 
					 timeToDie:(NSTimeInterval)inTimeTodie 
			   particleEmitter:(ParticleEmitter*)inParticleEmitter

{
	self = [super init];
	if (self != nil) {
		NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
		particleEmitter = inParticleEmitter;
		position = inPosition;
		direction = inDirection;
		timeToDie = now + inTimeTodie;
		alive = NO;
	}
	return self;
}


- (void)update:(GLfloat)delta {
	
	// Update the position of the particle based on its current direction
	position.x += direction.x * delta;
	position.y += direction.y * delta;
	
	// Update the direction of the particle
	//direction.x += (particleEmitter.force.x + (particleEmitter.forceVariance.x * ParticleRandom())) * delta;
	//direction.y += (particleEmitter.force.y + (particleEmitter.forceVariance.y * ParticleRandom())) * delta;

	NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
	if(timeToDie < now) {
		alive = NO;
	}
}


- (void)renderParticle {

}


- (void)dealloc {
	[super dealloc];
}

@end
