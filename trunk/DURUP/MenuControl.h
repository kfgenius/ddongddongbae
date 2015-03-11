//
//  MenuControl.h
//  OGLGame
//
//  Created by Michael Daley on 21/05/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//
// This class controls the appearance and state of menu options.  It takes an image
// which represents the menu option itself and is resposible for both the logic and
// rendering of the item

#import <Foundation/Foundation.h>
#import "AbstractControl.h"
#import "Image.h"
#import "Common.h"


@interface MenuControl : AbstractControl {
	BOOL visible;
	BOOL touchable;
}

- (id)initWithImageNamed:(NSString*)theImageName location:(Vector2f)theLocation centerOfImage:(BOOL)theCenter type:(uint)theType;
- (void)updateWithLocation:(NSString*)theTouchLocation;
- (void)updateWithDelta:(NSNumber*)theDelta;
- (void)render;
- (void)setVisible:(BOOL)aVisible;
- (void)setTouchable:(BOOL)aTouchable;
- (void)reset;

@end
