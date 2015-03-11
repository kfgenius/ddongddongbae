//
//  Unit.h
//  Tutorial1
//
//  Created by Dong-hung Park on 11. 1. 5..
//  Copyright 2011 Hondoom. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CharacterID.h"

@class Character;
@class SettingsScene;

@interface Unit : NSObject {
	SettingsScene *_scene;	//게임 장면
	Character *_character;	//캐릭터
	
	BOOL life;
	BOOL first;	//첫 행동을 확인하기 위한 스위치
	int imageId;	//캐릭터 ID와 별개인 이미지 ID
	
	CharacterID characterId;
	CGPoint unitPosition;
	CGPoint addPosition;	//기존의 위치에서 옮김(떨림 효과용)
	int shock;
	int hp;
	double direction;
	int tempo;
	BOOL reflect;
	
	BOOL boss;	//보스인지 여부
	
	Unit* master;	//주인
	Unit* slave;	//속한 유닛
};

@property (nonatomic, readonly) Character *_character;
@property (nonatomic) BOOL life;
@property (nonatomic, readonly) BOOL first;
@property (nonatomic, readonly) int hp;
@property (nonatomic, readonly) int shock;
@property (nonatomic) CGPoint unitPosition;
@property (nonatomic, readonly) double direction;
@property (nonatomic, readonly) CharacterID characterId;
@property (nonatomic) BOOL reflect;
@property (nonatomic, assign) Unit* master;
@property (nonatomic, assign) Unit* slave;
@property (nonatomic, readonly) int tempo;

- (BOOL)SetUnit:(CharacterID)aCharacterId position:(CGPoint)aPosition direction:(double)aDirection gameScene:(SettingsScene*)scene;
- (void)SetPosition:(CGPoint)aPosition;
- (CGPoint)centerPosition;
- (void)Move:(CGPoint)movePosition;
- (void)Render;
- (void)Action;
- (void)recover:(int)quantity;
- (void)Damage:(int)damage shock:(BOOL)isShock;
- (void)boom;
- (CGRect)getRect;
- (void)setDirection:(double)aDirection;
- (void)moveCenter;
- (CGPoint)getRenderPosition;

@end
