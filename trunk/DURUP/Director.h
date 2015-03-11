//
//  Director.h
//  OGLGame
//
//  Created by Michael Daley on 03/05/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>
#import "SynthesizeSingleton.h"
#import "Common.h"

@class AbstractScene;

@interface Director : NSObject {
	
	// Currently bound texture name
	GLuint currentlyBoundTexture;
	// Current game state
	GLuint currentGameState;
	// Current scene
	AbstractScene *currentScene;
	// Dictionary of scenes
	NSMutableDictionary *_scenes;
	// Global alpha
	GLfloat globalAlpha;
    // Frames Per Second
    float framesPerSecond;
	
}

@property (nonatomic, assign) GLuint currentlyBoundTexture;
@property (nonatomic, assign) GLuint currentGameState;
@property (nonatomic, retain) AbstractScene *currentScene;
@property (nonatomic, assign) GLfloat globalAlpha;
@property (nonatomic, assign) float framesPerSecond;

+ (Director*)sharedDirector;
- (void)addSceneWithKey:(NSString*)aSceneKey scene:(AbstractScene*)aScene;
- (AbstractScene*)getSceneWithKey:(NSString*)aSceneKey;
- (BOOL)setCurrentSceneToSceneWithKey:(NSString*)aSceneKey;
- (BOOL)transitionToSceneWithKey:(NSString*)aSceneKey;

@end
