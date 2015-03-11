//
//  Animation.h
//  OGLGame
//
//  Created by Michael Daley on 06/03/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SpriteSheet.h"
#import "Frame.h"

enum {
	kDirection_Forward = 1,
	kDirection_Backwards = -1
};

@interface Animation : NSObject {
	
	// Frames to be used within this animation
	NSMutableArray *frames;
	// Accumulates the time while a frame is displayed
	float frameTimer;
	// Holds the animation running state
	BOOL running;
	// Repeat the animation
	BOOL repeat;
	// Should the animation ping pong backwards and forwards
	BOOL pingPong;
	// Direction in which the animation is running
	int direction;
	// The current frame of animation
	int currentFrame;
}

@property(nonatomic)BOOL repeat;
@property(nonatomic)BOOL pingPong;
@property(nonatomic)BOOL running;
@property(nonatomic)int currentFrame;
@property(nonatomic)int direction;

- (void)addFrameWithImage:(Image*)image delay:(float)delay;
- (void)update:(float)delta;
- (void)renderAtPoint:(CGPoint)point;
- (Image*)getCurrentFrameImage;
- (GLuint)getAnimationFrameCount;
- (GLuint)getCurrentFrameNumber;
- (Frame*)getFrame:(GLuint)frameNumber;
- (void)flipAnimationVertically:(BOOL)flipVertically horizontally:(BOOL)flipHorizontally;
- (void)setBlendMode:(BlendMode)aBlendMode;
- (void)setAlpha:(float)aAlpha;

@end
