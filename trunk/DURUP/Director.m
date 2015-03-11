//
//  Director.m
//  OGLGame
//
//  Created by Michael Daley on 03/05/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import "Director.h"
#import "AbstractScene.h"

@implementation Director

@synthesize currentlyBoundTexture;
@synthesize currentGameState;
@synthesize currentScene;
@synthesize globalAlpha;
@synthesize framesPerSecond;

// Make this class a singleton class
SYNTHESIZE_SINGLETON_FOR_CLASS(Director);


- (id)init {
	// Initialize the arrays to be used within the state manager
	_scenes = [[NSMutableDictionary alloc] init];
	currentScene = nil;
	globalAlpha = 1.0f;
	return self;
}

- (AbstractScene*)getSceneWithKey:(NSString*)aSceneKey {
	return [_scenes objectForKey:aSceneKey];
}

- (void)addSceneWithKey:(NSString*)aSceneKey scene:(AbstractScene*)aScene {
	[_scenes setObject:aScene forKey:aSceneKey];
}


- (BOOL)setCurrentSceneToSceneWithKey:(NSString*)aSceneKey {
	if(![_scenes objectForKey:aSceneKey]) {
		if(DEBUG) NSLog(@"ERROR: Scene with key '%@' not found.", aSceneKey);
        return NO;
    }
	
    currentScene = [_scenes objectForKey:aSceneKey];
	[currentScene setSceneAlpha:0.0f];
	[currentScene setSceneState:kSceneState_TransitionIn];
    
    return YES;
}


- (BOOL)transitionToSceneWithKey:(NSString*)aSceneKey {

	// If the scene key exists then tell the current scene to transition to that
    // scene and return YES
    if([_scenes objectForKey:aSceneKey]) {
        [currentScene transitionToSceneWithKey:aSceneKey];
        return YES;
    }
    
    // If the scene does not exist then return NO;
    return NO;
}


- (void)dealloc {
	[_scenes release];
	[super dealloc];
}

@end
