//
//  Frame.m
//  OGLGame
//
//  Created by Michael Daley on 06/03/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import "Frame.h"


@implementation Frame

@synthesize frameDelay;
@synthesize frameImage;

- (id)initWithImage:(Image*)image delay:(float)delay {
	self = [super init];
	if(self != nil) {
		frameImage = image;
		frameDelay = delay;
	}
	return self;
}

- (void)setBlendMode:(BlendMode)aBlendMode {
	[frameImage setBlendMode:aBlendMode];
}

- (void)dealloc {
    [frameImage release];
	[super dealloc];
}

@end
