//
//  Character.h
//  Tutorial1
//
//  Created by Dong-hung Park on 11. 1. 5..
//  Copyright 2011 Hondoom. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "Image.h"
#import "Animation.h"
#import "CharacterID.h"

@interface Character : NSObject {

	CharacterID characterId;
	CharacterPattern pattern;
	
	uint frame;
	
	int hpMax;
	float width;
	float height;
	CGPoint shootingSpot;		//총알이 나가는 위치
	int power;
	float speed;
	
	BoomType boomType;
	
	Image* image;
    SpriteSheet *_spriteSheet;
    Animation *_animation;
}

@property (nonatomic, readonly) CharacterID characterId;
@property (nonatomic, readonly) CharacterPattern pattern;
@property (nonatomic, readonly) int hpMax;
@property (nonatomic, readonly) float width;
@property (nonatomic, readonly) float height;
@property (nonatomic, readonly) CGPoint shootingSpot;
@property (nonatomic, readonly) int power;
@property (nonatomic, readonly) float speed;
@property (nonatomic, readonly) BoomType boomType;

- (id)initCharacter:(CharacterID)aCharacterId image:(NSString*)image frame:(uint)aFrame pattern:(CharacterPattern)aPattern 
			   hpMax:(int)aHpMax power:(int)aPower speed:(int)aSpeed boomType:(BoomType)aBoomType;
- (void)update:(float)aDelta;
- (void)renderAtPoint:(CGPoint)point;
- (void)setBlendMode:(BlendMode)aBlendMode;
- (void)setAlpha:(float)alpha;

@end
