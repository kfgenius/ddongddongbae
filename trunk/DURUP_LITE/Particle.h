//
//  Particle.h
//  Tutorial1
//
//  Created by Michael Daley on 19/04/2009.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Types.h"
#import "Image.h"

@class ParticleEmitter;

@interface Particle : NSObject {
	
	ParticleEmitter *particleEmitter;
	
	Vector2f position;
	Vector2f lastPosition;
	Vector2f direction;
	
	NSTimeInterval timeToDie;
	NSTimeInterval timeWasBorn;
	
	Color4f color;
	Color4f endColor;
	Color4f startColor;
	
	GLfloat particleSize;
	
	Image *texture;
	
	BOOL alive;
	
}

@property(nonatomic) BOOL alive;
@property(nonatomic, readonly) Image *texture;
@property(nonatomic) Vector2f position;
@property(nonatomic) Vector2f direction;
@property(nonatomic) NSTimeInterval timeToDie;

- (id)initParticleWithPosition:(Vector2f)inPosition 
					 direction:(Vector2f)inDirection 
					 timeToDie:(NSTimeInterval)inTimeTodie 
			   particleEmitter:(ParticleEmitter*)inParticleEmitter;
- (void)renderParticle;
- (void)update:(GLfloat)delta;
@end
