//
//  AbstractEntity.h
//  OGLGame
//
//  Created by Michael Daley on 04/03/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Image.h"
#import "Animation.h"

@class GameScene;

// Entity states
enum entityState {
	kEntity_Idle = 0,
	kEntity_Dead = 1,
	kEntity_Dying = 2,
	kEntity_Alive = 3,
	kEntity_Following_Path = 4
};

@interface AbstractEntity : NSObject {

	// Entity image
	Image *image;
	// Entity position
	Vector2f position;
	// Velocity
	Vector2f velocity;
	// Entity state
	GLuint entityState;
    // Do we have a reference to the current scene
    BOOL _gotScene;

}

@property (nonatomic, assign) Vector2f position;
@property (nonatomic, assign) Vector2f velocity;
@property (nonatomic, assign) GLuint entityState;
@property (nonatomic, readonly) Image *image;

// Selector that updates the entities logic i.e. location, collision status etc
- (void)update:(GLfloat)delta;

// Selector that renders the entity
- (void)render;

@end
