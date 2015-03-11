//
//  MenuControl.m
//  OGLGame
//
//  Created by Michael Daley on 21/05/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import "MenuControl.h"
#import "SoundManager.h"

@interface MenuControl (Private)
- (void)scaleImage:(GLfloat)delta;
@end


@implementation MenuControl

#define MAX_SCALE 3.0f
#define SCALE_DELTA 2.0f
#define ALPHA_DELTA 2.0f

- (id)initWithImageNamed:(NSString*)theImageName location:(Vector2f)theLocation centerOfImage:(BOOL)theCenter type:(uint)theType {
	self = [super init];
	if (self != nil) {
		sharedDirector = [Director sharedDirector];
		image = [(Image*)[Image alloc] initWithImage:theImageName filter:GL_LINEAR];
		location = theLocation;
		centered = theCenter;
		type = theType;
		state = kControl_Idle;
		scale = CGSizeMake(1.0f, 1.0f);
		alpha = 1.0f;
		visible = YES;
		touchable = YES;
	}
	return self;
}

- (void)updateWithLocation:(NSString*)theTouchLocation {
	if (visible == NO || touchable == NO) {
		return;
	}
	
	CGRect controlBounds = CGRectMake(location.x - (([image imageWidth]*image.scale.width)/2), location.y - (([image imageHeight]*image.scale.height)/2), [image imageWidth]*image.scale.width, [image imageHeight]*image.scale.height);
	
	CGPoint touchPoint = CGPointFromString((NSString*)theTouchLocation);
	
	if(CGRectContainsPoint(controlBounds, touchPoint) && state != kControl_Scaling) {
		state = kControl_Scaling;

		SoundManager *_sharedSoundManager = [SoundManager sharedSoundManager];
		[_sharedSoundManager playSoundWithKey:@"touch" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
	}
}

- (void)updateWithDelta:(NSNumber*)theDelta {

	// Get the delta value which has been passed in
	GLfloat delta = [theDelta floatValue];

	if(state == kControl_Scaling) {		
		[self scaleImage:delta];
	}
	if(state == kControl_Idle) {
		scale = CGSizeMake(1.0f, 1.0f);
		alpha = 1.0f;
	}
}


- (void)scaleImage:(GLfloat)delta {
	scale.width += SCALE_DELTA * delta;
	scale.height += SCALE_DELTA * delta;
	alpha -= ALPHA_DELTA * delta;
	if(scale.width > MAX_SCALE) {
		scale.width = scale.height = MAX_SCALE;
		state = kControl_Selected;
	}
}


- (void)render {
	if (visible == NO) {
		return;
	}
	
	[image setAlpha:alpha];
	[image setScale:scale];
	[image renderAtPoint:CGPointMake(location.x, location.y) centerOfImage:centered];	
}

- (void)setVisible:(BOOL)aVisible {
	visible = aVisible;
}

- (void)setTouchable:(BOOL)aTouchable {
	touchable = aTouchable;
}

- (void)reset {
	state = kControl_Idle;
	scale = CGSizeMake(1.0f, 1.0f);
	alpha = 1.0f;
	visible = YES;
	touchable = YES;
}

@end
