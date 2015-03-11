//
//  MenuState.m
//  OGLGame
//
//  Created by Michael Daley on 31/05/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import "HowtoplayScene.h"

@implementation HowtoplayScene

- (id)init {
	
	if(self = [super init]) {
		_sharedDirector = [Director sharedDirector];
		_sharedResourceManager = [ResourceManager sharedResourceManager];
		_sharedSoundManager = [SoundManager sharedSoundManager];
        
        _sceneFadeSpeed = 1.0f;
        sceneAlpha = 0.0f;
        [_sharedDirector setGlobalAlpha:sceneAlpha];

		howtoplayImage = [(Image*)[Image alloc] initWithImage:@"howtoplay.jpg"];
		[self setSceneState:kSceneState_TransitionIn];
		nextSceneKey = nil;
	}
	return self;
}

- (void)updateWithDelta:(GLfloat)aDelta {
	
	switch (sceneState) {
		case kSceneState_TransitionOut:
			sceneAlpha -= _sceneFadeSpeed * aDelta;
            [_sharedDirector setGlobalAlpha:sceneAlpha];
			if(sceneAlpha < 0)
                // If the scene being transitioned to does not exist then transition
                // this scene back in
				if(![_sharedDirector setCurrentSceneToSceneWithKey:nextSceneKey])
                    sceneState = kSceneState_TransitionIn;
			break;
			
		case kSceneState_TransitionIn:
			
			// I'm not using the delta value here as the large map being loaded causes
            // the first delta to be passed in to be very big which takes the alpha
            // to over 1.0 immediately, so I've got a fixed delta for the fade in.
            sceneAlpha += _sceneFadeSpeed * 0.02f;
            [_sharedDirector setGlobalAlpha:sceneAlpha];
			if(sceneAlpha >= 1.0f) {
				sceneState = kSceneState_Running;
			}
			break;
		default:
			break;
	}
	
}

- (void)setSceneState:(uint)theState {
	sceneState = theState;
	if(sceneState == kSceneState_TransitionOut)
		sceneAlpha = 1.0f;
	if(sceneState == kSceneState_TransitionIn)
		sceneAlpha = 0.0f;
}

- (void)updateWithTouchLocationBegan:(NSSet*)touches withEvent:(UIEvent*)event view:(UIView*)aView {
	if (sceneState == kSceneState_Running) {
		sceneState = kSceneState_TransitionOut;
		[_sharedSoundManager playSoundWithKey:@"touch" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
		
		nextSceneKey = @"menu";
	}
}

- (void)transitionToSceneWithKey:(NSString*)theKey {
	sceneState = kSceneState_TransitionOut;
	sceneAlpha = 1.0f;
}

- (void)render {
	[howtoplayImage renderAtPoint:CGPointMake(240, 160) centerOfImage:YES];
}


@end
