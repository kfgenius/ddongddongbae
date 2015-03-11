//
//  AbstractState.m
//  OGLGame
//
//  Created by Michael Daley on 01/06/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import "AbstractScene.h"


@implementation AbstractScene

@synthesize sceneState;
@synthesize sceneAlpha;

- (void)updateWithDelta:(GLfloat)aDelta {
}


- (void)updateWithTouchLocationBegan:(NSSet*)touches withEvent:(UIEvent*)event view:(UIView*)aView {
}

- (void)updateWithTouchLocationMoved:(NSSet*)touches withEvent:(UIEvent*)event view:(UIView*)aView {
}

- (void)updateWithTouchLocationEnded:(NSSet*)touches withEvent:(UIEvent*)event view:(UIView*)aView {
}

- (void)updateWithTouchLocationCanceled:(NSSet*)touches withEvent:(UIEvent*)event view:(UIView*)aView {
}

- (void)updateWithAccelerometer:(UIAcceleration*)aAcceleration {
}

- (void)transitionToSceneWithKey:(NSString*)aKey {
}

- (void)render {
}

- (void)resignActive {
}

- (void)becomeActive {
}

@end