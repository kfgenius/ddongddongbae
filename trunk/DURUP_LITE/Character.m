//
//  Character.m
//  Tutorial1
//
//  Created by Dong-hung Park on 11. 1. 5..
//  Copyright 2011 Hondoom. All rights reserved.
//

#import "Character.h"


@implementation Character

@synthesize characterId, pattern;
@synthesize hpMax, width, height, shootingSpot, power, speed, boomType;

- (void)dealloc {
	[image release];
	[_spriteSheet release];
	[_animation release];
	
	[super dealloc];
}

- (id)initCharacter:(CharacterID)aCharacterId image:(NSString*)aImage  frame:(uint)aFrame pattern:(CharacterPattern)aPattern 
			   hpMax:(int)aHpMax power:(int)aPower speed:(int)aSpeed boomType:(BoomType)aBoomType{

	if ([super init] != nil) {
		characterId = aCharacterId;
		
		//이미지 읽기
		image = [(Image*)[Image alloc] initWithImage:aImage];
		width = image.imageWidth / aFrame;
		height = image.imageHeight;
		
		if (aFrame > 1) {
			[image release];	//이미지 정보 지우기
			
			_spriteSheet = [[SpriteSheet alloc] initWithImageNamed:aImage spriteWidth:width spriteHeight:height spacing:0 imageScale:CGSizeMake(1.0f, 1.0f)];
			_animation = [[Animation alloc] init];
			uint _frameIndex;
			float _animationDelay = 0.05f;
			
			for(_frameIndex=0; _frameIndex < aFrame; _frameIndex++) {
				[_animation addFrameWithImage:[_spriteSheet getSpriteAtX:_frameIndex y:0] delay:_animationDelay];
			}
			
			[_animation setRunning:YES];
			[_animation setPingPong:YES];
		}
		
		frame = aFrame;
		pattern = aPattern;
		hpMax = aHpMax;
		shootingSpot = CGPointZero;
		power = aPower;
		speed = aSpeed;
		boomType = aBoomType;
	}
	
	return self;	
}

- (void)update:(float)aDelta {
	if (frame > 1) {
		[_animation update:aDelta];
	}
}

- (void)renderAtPoint:(CGPoint)point {
	if (frame == 1) {
		[image renderAtPoint:point centerOfImage:NO];
	} else {
		[_animation renderAtPoint:point];
	}
}

- (void)setBlendMode:(BlendMode)aBlendMode {
	if (frame > 1) {
		[_animation setBlendMode:aBlendMode];
	} else {
		[image setBlendMode:aBlendMode];
	}
}

- (void)setAlpha:(float)alpha {
	if (frame > 1) {
		[_animation setAlpha:alpha];
	} else {
		[image setAlpha:alpha];
	}
}

@end
