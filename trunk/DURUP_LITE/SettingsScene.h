//
//  SettingsScene.h
//  Tutorial1
//
//  Created by Michael Daley on 07/06/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import "AbstractScene.h"
#import "CharacterID.h"
#import "SoundManager.h"
#import "GameData.h"

@class Character;
@class Unit;

typedef enum {
	kShooting_none,
	kShooting_lightning,
	kShooting_missile,
	kShooting_beam,
	kShooting_solar
}ShootingWeapon;

@interface SettingsScene : AbstractScene {
	
	Image *backGroundImage[6];
	Image *moveControl;
	Image *weaponControl;
	
	Image *hpBarImage;
	Image *hpImage;
	Image *bossLifeImage;
	Image *bossBombImage;
	
	Image *damageImage[5];	//데미지에 의한 균열
	Image *shockImage;
	
	Image *stageTitleImage[5];
	Image *gameoverImage;
	Image *pauseButtonImage;
	Image *pausedImage;
	Image *pauseResumeImage;
	Image *pauseBackImage;
	Image *theEndImage;
	
	//보스 정보
	int bossCharacterId;
	int bossHp;
	int bossLv;
	int bossLife;
	int bossBomb;
	float bossHpBar;
	float bossHpMax;
	
	//진행 정보
	int chapter;
	int gameTime;
	int clearDelay;
	float backGroundScroll;
	
	BOOL moveUp;
	BOOL moveDown;
	
	NSMutableArray *characters;

	NSMutableArray *units;
	NSMutableArray *weapons;
	NSMutableArray *anis;
	
	// Tracks if the joypad is moving or not
	BOOL joypadMoving;
	// Bounds of the rectangle in which a touch is classed as activating the joypad
	CGRect joypad;
	// Values used to control the joypad
	uint joypadCenterx, joypadCentery, joypadWidth, joypadHeight;
	// Holds the unique hash value given to a touch on the joypad.  This allows us to track the same 
	// touch during touchesMoved events
	int joypadTouchHash;
	
	float playerSpeed;
	float playerAngle;

	// Tracks if the joypad is moving or not
	BOOL weaponJoypadMoving;
	// Bounds of the rectangle in which a touch is classed as activating the joypad
	CGRect weaponJoypad;
	// Values used to control the joypad
	uint weaponJoypadCenterx, weaponJoypadCentery, weaponJoypadWidth, weaponJoypadHeight;
	// Holds the unique hash value given to a touch on the joypad.  This allows us to track the same 
	// touch during touchesMoved events
	int weaponJoypadTouchHash;
	
	ShootingWeapon oldShootingWeapon;
	ShootingWeapon shootingWeapon;
	int weaponDelay;
	
	int boomSoundDelay;
	int solarSoundDelay;
	
	int gameoverDelay;
	
	AngelCodeFont *hiscoreFont;
	AngelCodeFont *arialFont;
	NSString *hiscoreText;
	int hiscoreX;
	
	BOOL pauseAble;
	
	SoundManager *sceneSoundManager;
	GameData *_sharedGameData;
}

@property (nonatomic) int bossHp;
@property (nonatomic) int bossLv;
@property (nonatomic) int bossBomb;
@property (nonatomic) float bossHpMax;

@property (nonatomic, readonly) NSMutableArray *characters;
@property (nonatomic, readonly) NSMutableArray *units;
@property (nonatomic, readonly) NSMutableArray *weapons;
@property (nonatomic, readonly) NSMutableArray *anis;

@property (nonatomic, assign) SoundManager *sceneSoundManager;

- (void)initGame;
- (void)boom:(CGPoint)position direction:(double)direction boomType:(BoomType)aBoomType;
- (void)shoot:(CharacterID)shootObject position:(CGPoint)position direction:(double)direction;
- (void)boomAll;
- (void)explosion;
- (void)calScore;

@end
