//
//  SettingsScene.m
//  Tutorial1
//
//  Created by Michael Daley on 07/06/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import "SettingsScene.h"
#import "CharacterID.h"
#import "Character.h"
#import "Unit.h"

#define HERO_HP_MAX			20
#define HERO_BASE_POSITION	CGPointMake(160, 165)

#define BOSS_HP_BAR_SIZE	281
#define SCROLL_SPEED		1
#define BOSS_LIFE_MAX		4
#define BOSS_BOMB_MAX		3
#define BOSS_LV_MAX			16

#define FIRST_CHAPTER		0
#define TITLE_SHOW_DELAY	340
#define CLEAR_DELAY			460

@implementation SettingsScene

@synthesize bossHp, bossLv, bossBomb, bossHpMax;
@synthesize characters;
@synthesize units, weapons, anis;
@synthesize sceneSoundManager;

- (void)dealloc {
	for (int i = 0; i < 6; i++) {
		[backGroundImage[i] release];
	}

	for (int i = 0; i < 5; i++) {
		[stageTitleImage[i] release];
	}
	
	[gameoverImage release];
	[pauseButtonImage release];
	[pausedImage release];
	[pauseResumeImage release];
	[pauseBackImage release];
	[theEndImage release];
	
	[moveControl release];
	[weaponControl release];
	
	[hpImage release];
	[hpBarImage release];
	[bossLifeImage release];
	[bossBombImage release];
	
	for (int i = 0; i < 5; i++) {
		[damageImage[i] release];
	}
	
	[shockImage release];
	
	[characters release];
	[units release];
	[weapons release];
	[anis release];
	
	[hiscoreFont release];
	
	if (hiscoreText != nil) {
		[hiscoreText release];
		hiscoreText = nil;
	}
	
	[super dealloc];
}

//캐릭터 추가
- (void)addNewCharacter:(CharacterID)characterId image:(NSString*)image frame:(int)frame pattern:(CharacterPattern)pattern
				  hpMax:(int)hpMax power:(int)power speed:(int)speed boomType:(BoomType)boomType
			  blendMode:(BlendMode)blendMode opacity:(float)opacity {
	Character *character = [[Character alloc] initCharacter:characterId image:image frame:frame pattern:pattern hpMax:hpMax power:power speed:speed boomType:boomType];
	
	//블렌드 모드
	if (blendMode == kBlendMode_lighten) {
		[character setBlendMode:kBlendMode_lighten];
	}
	
	//투명도
	if (opacity != 1.0f) {
		[character setAlpha:opacity];
	}
	
	[characters addObject:(Character*)character];
	[character release];
}

- (id)init {
	
	if(self = [super init]) {
		_sharedDirector = [Director sharedDirector];
		_sharedResourceManager = [ResourceManager sharedResourceManager];
		sceneSoundManager = [SoundManager sharedSoundManager];
        _sceneFadeSpeed = 1.0f;
		_sharedGameData = [GameData sharedGameData];
		
		//BG
		backGroundImage[0] = [(Image*)[Image alloc] initWithImage:@"back1.png"];
		backGroundImage[1] = [(Image*)[Image alloc] initWithImage:@"back2.png"];
		backGroundImage[2] = [(Image*)[Image alloc] initWithImage:@"back3.gif"];
		backGroundImage[3] = [(Image*)[Image alloc] initWithImage:@"back4.gif"];
		backGroundImage[4] = [(Image*)[Image alloc] initWithImage:@"back5.gif"];
		backGroundImage[5] = [(Image*)[Image alloc] initWithImage:@"backe.gif"];
		
		//타이틀
		stageTitleImage[0] = [(Image*)[Image alloc] initWithImage:@"stage1title.png"];
		stageTitleImage[1] = [(Image*)[Image alloc] initWithImage:@"stage2title.png"];
		stageTitleImage[2] = [(Image*)[Image alloc] initWithImage:@"stage3title.png"];
		stageTitleImage[3] = [(Image*)[Image alloc] initWithImage:@"stage4title.png"];
		stageTitleImage[4] = [(Image*)[Image alloc] initWithImage:@"stage5title.png"];
		
		//메뉴
		moveControl = [(Image*)[Image alloc] initWithImage:@"control.png"];
		weaponControl = [(Image*)[Image alloc] initWithImage:@"weapon.png"];
		[moveControl setAlpha:0.5f];
		[weaponControl setAlpha:0.5f];
		
		//보스 상태
		hpImage = [(Image*)[Image alloc] initWithImage:@"hp.png"];
		hpBarImage = [(Image*)[Image alloc] initWithImage:@"boss_hp_bar.png"];
		bossLifeImage =  [(Image*)[Image alloc] initWithImage:@"flight_life.png"];
		bossBombImage =  [(Image*)[Image alloc] initWithImage:@"boss_bomb.png"];
		
		//주인공 상태
		damageImage[0] = [(Image*)[Image alloc] initWithImage:@"damage1.png"];
		damageImage[1] = [(Image*)[Image alloc] initWithImage:@"damage2.png"];
		damageImage[2] = [(Image*)[Image alloc] initWithImage:@"damage3.png"];
		damageImage[3] = [(Image*)[Image alloc] initWithImage:@"damage4.png"];
		damageImage[4] = [(Image*)[Image alloc] initWithImage:@"damage5.png"];
		shockImage = [(Image*)[Image alloc] initWithImage:@"skull2.png"];
		
		//게임 오버
		gameoverImage = [(Image*)[Image alloc] initWithImage:@"gameover.png"];
		
		//정지
		pausedImage = [(Image*)[Image alloc] initWithImage:@"paused.png"];
		pauseButtonImage = [(Image*)[Image alloc] initWithImage:@"pause.png"];
		[pauseButtonImage setAlpha:0.5f];
		pauseResumeImage = [(Image*)[Image alloc] initWithImage:@"resume_button.png"];
		pauseBackImage = [(Image*)[Image alloc] initWithImage:@"back_to_menu_button.png"];
		
		//끝
		theEndImage = [(Image*)[Image alloc] initWithImage:@"theend.png"];
		
		//캐릭터, 유닛 배열
		characters = [[NSMutableArray alloc] init];
		units = [[NSMutableArray alloc] init];
		weapons = [[NSMutableArray alloc] init];
		anis = [[NSMutableArray alloc] init];
		
		//캐릭터 추가
		[self addNewCharacter:kCharacter_skull image:@"skull.png" frame:1 pattern:kPattern_recover hpMax:HERO_HP_MAX power:1 speed:4 boomType:kBoom_hero blendMode:kBlendMode_normal opacity:1.0f];	//해골
		[self addNewCharacter:kCharacter_skull_body image:@"skullbody.png" frame:1 pattern:kPattern_none hpMax:1 power:0 speed:0 boomType:kBoom_none blendMode:kBlendMode_normal opacity:1.0f];	//몸
		[self addNewCharacter:kCharacter_boom1 image:@"boom.gif" frame:3 pattern:kPattern_boom hpMax:60 power:0 speed:4 boomType:kBoom_none blendMode:kBlendMode_lighten opacity:1.0f];	//폭발1
		[self addNewCharacter:kCharacter_boom2 image:@"boom2.gif" frame:3 pattern:kPattern_boom hpMax:60 power:0 speed:4 boomType:kBoom_none blendMode:kBlendMode_lighten opacity:1.0f];	//폭발2
		[self addNewCharacter:kCharacter_boom3 image:@"boom3.gif" frame:3 pattern:kPattern_boom hpMax:60 power:0 speed:4 boomType:kBoom_none blendMode:kBlendMode_lighten opacity:1.0f];	//폭발3
		[self addNewCharacter:kCharacter_boom4 image:@"boom4.gif" frame:3 pattern:kPattern_boom hpMax:60 power:0 speed:4 boomType:kBoom_none blendMode:kBlendMode_lighten opacity:1.0f];	//폭발4
		[self addNewCharacter:kCharacter_boom_pillar image:@"boom.gif" frame:3 pattern:kPattern_boom_pillar hpMax:60 power:0 speed:0 boomType:kBoom_none blendMode:kBlendMode_lighten opacity:1.0f];	//폭발4
		[self addNewCharacter:kCharacter_missile image:@"w_missile.png" frame:1 pattern:kPattern_go_ahead hpMax:1 power:1 speed:8 boomType:kBoom_weapon1 blendMode:kBlendMode_normal opacity:1.0f]; //기본 무기
		[self addNewCharacter:kCharacter_lightning image:@"w_thunder.png" frame:3 pattern:kPattern_go_ahead hpMax:999 power:1 speed:12 boomType:kBoom_none blendMode:kBlendMode_normal opacity:1.0f];	//천둥 미사일
		[self addNewCharacter:kCharacter_beam image:@"w_beam.gif" frame:1 pattern:kPattern_go_ahead hpMax:1 power:1 speed:5 boomType:kBoom_weapon3 blendMode:kBlendMode_lighten opacity:1.0f];	//와이드 빔
		[self addNewCharacter:kCharacter_solar image:@"w_solar.png" frame:1 pattern:kPattern_go_ahead hpMax:1 power:1 speed:3 boomType:kBoom_weapon4 blendMode:kBlendMode_lighten opacity:1.0f];	//광선 빔
		[self addNewCharacter:kCharacter_weapon_boom1 image:@"weapon_boom1.png" frame:3 pattern:kPattern_boom hpMax:10 power:0 speed:0 boomType:kBoom_none blendMode:kBlendMode_normal opacity:1.0f];	//무기 폭발1
		[self addNewCharacter:kCharacter_weapon_boom2 image:@"weapon_boom2.gif" frame:3 pattern:kPattern_boom hpMax:10 power:0 speed:0 boomType:kBoom_none blendMode:kBlendMode_lighten opacity:0.5f];	//무기 폭발2
		[self addNewCharacter:kCharacter_weapon_boom3 image:@"weapon_boom3.gif" frame:3 pattern:kPattern_boom hpMax:10 power:0 speed:0 boomType:kBoom_none blendMode:kBlendMode_lighten opacity:1.0f];	//무기 폭발3
		[self addNewCharacter:kCharacter_weapon_boom4 image:@"weapon_boom4.gif" frame:3 pattern:kPattern_boom hpMax:10 power:0 speed:0 boomType:kBoom_none blendMode:kBlendMode_lighten opacity:1.0f];	//무기 폭발4
		[self addNewCharacter:kCharacter_worm image:@"worm.png" frame:3 pattern:kPattern_go_ahead hpMax:1 power:1 speed:4 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//벌레
		[self addNewCharacter:kCharacter_angry_eye image:@"angry_eye.png" frame:1 pattern:kPattern_angry_eye hpMax:5 power:2 speed:1 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//화난 눈
		[self addNewCharacter:kCharacter_beauty_eye image:@"beauty_eye.png" frame:1 pattern:kPattern_beauty_eye hpMax:10 power:3 speed:4 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//아름다운 눈
		[self addNewCharacter:kCharacter_thunder_missile image:@"m_thunder.png" frame:1 pattern:kPattern_go_ahead hpMax:1 power:1 speed:10 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//번개 미사일
		[self addNewCharacter:kCharacter_light_missile image:@"m_light.gif" frame:1 pattern:kPattern_go_ahead hpMax:1 power:1 speed:4 boomType:kBoom_small blendMode:kBlendMode_lighten opacity:1.0f];	//빛 미사일
		[self addNewCharacter:kCharacter_boss_fly image:@"boss_fly.png" frame:3 pattern:kPattern_boss_fly hpMax:100 power:1 speed:4 boomType:kBoom_big blendMode:kBlendMode_normal opacity:1.0f];	//파리
		[self addNewCharacter:kCharacter_black_missile image:@"m_dot.png" frame:1 pattern:kPattern_go_ahead hpMax:1 power:1 speed:7 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//검은 미사일
		[self addNewCharacter:kCharacter_face image:@"face.png" frame:1 pattern:kPattern_face hpMax:200 power:1 speed:1 boomType:kBoom_big blendMode:kBlendMode_normal opacity:1.0f];	//얼굴
		[self addNewCharacter:kCharacter_parasite image:@"parasite.png" frame:3 pattern:kPattern_go_ahead hpMax:1 power:1 speed:7 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//기생충
		[self addNewCharacter:kCharacter_punch image:@"punch.png" frame:1 pattern:kPattern_ready_go hpMax:5 power:1 speed:6 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//주먹
		[self addNewCharacter:kCharacter_hand_x image:@"hand_x.png" frame:1 pattern:kPattern_hand_x hpMax:10 power:1 speed:4 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//가위
		[self addNewCharacter:kCharacter_hand_b image:@"hand_b.png" frame:1 pattern:kPattern_hand_b hpMax:5 power:1 speed:3 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//보
		[self addNewCharacter:kCharacter_foot image:@"foot.png" frame:1 pattern:kPattern_ready_go hpMax:5 power:1 speed:6 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//발
		[self addNewCharacter:kCharacter_egg_missile image:@"m_egg.png" frame:1 pattern:kPattern_go_ahead hpMax:1 power:1 speed:10 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//알 미사일
		[self addNewCharacter:kCharacter_cell image:@"cell1.png" frame:1 pattern:kPattern_cell hpMax:2 power:1 speed:5 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//세포
		[self addNewCharacter:kCharacter_cell2 image:@"cell2.png" frame:1 pattern:kPattern_cell hpMax:4 power:2 speed:6 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//이중 세포
		[self addNewCharacter:kCharacter_cell4 image:@"cell4.png" frame:1 pattern:kPattern_cell hpMax:16 power:4 speed:7 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//사중 세포
		[self addNewCharacter:kCharacter_transparent_cell image:@"cell1.png" frame:1 pattern:kPattern_cell hpMax:2 power:1 speed:5 boomType:kBoom_small blendMode:kBlendMode_normal opacity:0.1f];	//투명 세포
		[self addNewCharacter:kCharacter_cell_pillar image:@"cellpillar.gif" frame:1 pattern:kPattern_go_ahead_heavy hpMax:30 power:3 speed:2 boomType:kBoom_pillar blendMode:kBlendMode_normal opacity:1.0f];	//세포 기둥
		[self addNewCharacter:kCharacter_mini_cell image:@"minicell.png" frame:1 pattern:kPattern_go_ahead hpMax:1 power:1 speed:3 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//작은 세포
		[self addNewCharacter:kCharacter_skull_cell image:@"skullcell.png" frame:3 pattern:kPattern_skull_cell hpMax:35 power:4 speed:0 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//해골 세포
		[self addNewCharacter:kCharacter_king_cell image:@"boss_cell.png" frame:1 pattern:kPattern_king_cell hpMax:300 power:1 speed:2 boomType:kBoom_big blendMode:kBlendMode_normal opacity:1.0f];	//왕세포
		[self addNewCharacter:kCharacter_cell_boom image:@"bubble.png" frame:3 pattern:kPattern_boom hpMax:60 power:0 speed:0 boomType:kBoom_none blendMode:kBlendMode_normal opacity:1.0f];	//세포 생성 거품
		[self addNewCharacter:kCharacter_mouth image:@"mouth.png" frame:1 pattern:kPattern_mouth hpMax:20 power:1 speed:2 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//입
		[self addNewCharacter:kCharacter_gas image:@"gas.png" frame:3 pattern:kPattern_go_ahead hpMax:10 power:1 speed:1 boomType:kBoom_small blendMode:kBlendMode_normal opacity:0.5f];	//가스
		[self addNewCharacter:kCharacter_boss1 image:@"flight.png" frame:1 pattern:kPattern_boss1 hpMax:100 power:4 speed:4 boomType:kBoom_big blendMode:kBlendMode_normal opacity:1.0f];	//1탄 보스
		[self addNewCharacter:kCharacter_boss2 image:@"flight2.png" frame:1 pattern:kPattern_boss2 hpMax:100 power:4 speed:4 boomType:kBoom_big blendMode:kBlendMode_normal opacity:1.0f];	//2탄 보스
		[self addNewCharacter:kCharacter_boss3 image:@"flight3.png" frame:1 pattern:kPattern_boss1 hpMax:20 power:4 speed:4 boomType:kBoom_big blendMode:kBlendMode_normal opacity:1.0f];	//3탄 보스
		[self addNewCharacter:kCharacter_boss4 image:@"flight4.png" frame:1 pattern:kPattern_boss4 hpMax:100 power:4 speed:4 boomType:kBoom_big blendMode:kBlendMode_normal opacity:1.0f];	//4탄 보스
		[self addNewCharacter:kCharacter_boss5 image:@"flight5.png" frame:1 pattern:kPattern_boss5 hpMax:500 power:4 speed:4 boomType:kBoom_big blendMode:kBlendMode_normal opacity:1.0f];	//5탄 보스
		[self addNewCharacter:kCharacter_power_item1 image:@"item1.png" frame:1 pattern:kPattern_go_ahead hpMax:1 power:0 speed:1 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//회복 아이템
		[self addNewCharacter:kCharacter_power_item2 image:@"item2.png" frame:1 pattern:kPattern_go_ahead hpMax:2 power:0 speed:1 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//실드 아이템
		[self addNewCharacter:kCharacter_power_item3 image:@"item3.png" frame:1 pattern:kPattern_go_ahead hpMax:3 power:0 speed:1 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//봄버 아이템
		[self addNewCharacter:kCharacter_power_item4 image:@"1up.png" frame:2 pattern:kPattern_go_ahead hpMax:5 power:0 speed:1 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//1UP
		[self addNewCharacter:kCharacter_power_item5 image:@"item5.png" frame:1 pattern:kPattern_go_ahead hpMax:10 power:0 speed:1 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//파워 아이템
		[self addNewCharacter:kCharacter_boss_weapon1 image:@"boss_missile1.png" frame:1 pattern:kPattern_go_ahead hpMax:1 power:1 speed:6 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//보스 무기 Lv1
		[self addNewCharacter:kCharacter_boss_weapon2 image:@"boss_missile2.png" frame:1 pattern:kPattern_go_ahead hpMax:2 power:1 speed:6 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//보스 무기 Lv2
		[self addNewCharacter:kCharacter_boss_weapon3 image:@"boss_missile3.png" frame:1 pattern:kPattern_go_ahead hpMax:3 power:1 speed:6 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//보스 무기 Lv3
		[self addNewCharacter:kCharacter_boss_weapon4 image:@"boss_missile4.png" frame:1 pattern:kPattern_go_ahead hpMax:4 power:1 speed:6 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//보스 무기 Lv4
		[self addNewCharacter:kCharacter_shield image:@"shield.gif" frame:3 pattern:kPattern_shield hpMax:180 power:5 speed:0 boomType:kBoom_none blendMode:kBlendMode_lighten opacity:1.0f];	//방어막
		[self addNewCharacter:kCharacter_jelly image:@"jelly.png" frame:3 pattern:kPattern_jelly hpMax:9999 power:0 speed:1 boomType:kBoom_none blendMode:kBlendMode_normal opacity:0.2f];	//왜곡의 젤리
		[self addNewCharacter:kCharacter_jelly_core image:@"jelly_core.png" frame:1 pattern:kPattern_go_ahead_heavy hpMax:5 power:1 speed:1 boomType:kBoom_8way blendMode:kBlendMode_normal opacity:1.0f];	//젤리 핵
		[self addNewCharacter:kCharacter_jelly_core2 image:@"jelly_core2.png" frame:1 pattern:kPattern_jelly_core2 hpMax:5 power:1 speed:1 boomType:kBoom_8way blendMode:kBlendMode_normal opacity:1.0f];	//젤리 핵2
		[self addNewCharacter:kCharacter_jelly_missile image:@"m_jelly.png" frame:1 pattern:kPattern_jelly_missile hpMax:1 power:1 speed:4 boomType:kBoom_none blendMode:kBlendMode_normal opacity:1.0f];	//젤리 미사일
		[self addNewCharacter:kCharacter_evil_eye image:@"devileye.png" frame:1 pattern:kPattern_evil_eye hpMax:50 power:1 speed:1 boomType:kBoom_8way blendMode:kBlendMode_normal opacity:1.0f];	//악마의 눈
		[self addNewCharacter:kCharacter_eye_laser image:@"m_laser.gif" frame:1 pattern:kPattern_go_ahead_heavy hpMax:999 power:1 speed:12 boomType:kBoom_none blendMode:kBlendMode_lighten opacity:1.0f];	//눈 레이저
		[self addNewCharacter:kCharacter_robot image:@"robot.png" frame:3 pattern:kPattern_robot hpMax:400 power:1 speed:4 boomType:kBoom_big blendMode:kBlendMode_normal opacity:1.0f];	//로봇
		[self addNewCharacter:kCharacter_bomb image:@"bomb.png" frame:3 pattern:kPattern_go_ahead_heavy hpMax:10 power:3 speed:1 boomType:kBoom_explosion blendMode:kBlendMode_normal opacity:1.0f];	//폭탄
		[self addNewCharacter:kCharacter_explosion image:@"explosion.png" frame:3 pattern:kPattern_boom hpMax:60 power:1 speed:0 boomType:kBoom_none blendMode:kBlendMode_normal opacity:1.0f];	//폭발
		[self addNewCharacter:kCharacter_chase_missile image:@"m_missile.png" frame:3 pattern:kPattern_chase_missile hpMax:5 power:1 speed:1 boomType:kBoom_small blendMode:kBlendMode_normal opacity:1.0f];	//추격 미사일
		[self addNewCharacter:kCharacter_handsome image:@"handsome.png" frame:1 pattern:kPattern_handsome hpMax:1 power:0 speed:1 boomType:kBoom_none blendMode:kBlendMode_normal opacity:1.0f];	//얼굴
		[self addNewCharacter:kCharacter_nice image:@"nice.png" frame:1 pattern:kPattern_none hpMax:1 power:0 speed:0 boomType:kBoom_none blendMode:kBlendMode_normal opacity:1.0f];	//음표
		
		// Setup the joypad location and touch box
		joypadCentery = 40;
		joypadCenterx = 40;
		joypadWidth = 40;
		joypadHeight = 40;
		joypad = CGRectMake(joypadCenterx - joypadWidth, joypadCentery - joypadHeight, joypadCenterx + joypadWidth, joypadCentery + joypadHeight);

		// Setup the weaponjoypad location and touch box
		weaponJoypadCentery = 40;
		weaponJoypadCenterx = 440;
		weaponJoypadWidth = 40;
		weaponJoypadHeight = 40;
		weaponJoypad = CGRectMake(weaponJoypadCenterx - weaponJoypadWidth, weaponJoypadCentery - weaponJoypadHeight,
								  weaponJoypadCenterx + weaponJoypadWidth, weaponJoypadCentery + weaponJoypadHeight);
		
		//Font
		hiscoreFont = [[AngelCodeFont alloc] initWithFontImageNamed:@"arial.png" controlFile:@"arial" scale:CGSizeMake(1, 1) filter:GL_LINEAR];
		arialFont = [[AngelCodeFont alloc] initWithFontImageNamed:@"arial.png" controlFile:@"arial" scale:CGSizeMake(1, 1) filter:GL_LINEAR];
		hiscoreText = nil;
		
		//게임은 미설정 상태
		_sharedGameData.gameSetting = NO;
	}
	
	return self;
}

//음악 연주
- (void)playMusic {
	int stage = [_sharedGameData stage];
	if (stage == 1) {
		if (chapter == 10) {
			[sceneSoundManager playMusicWithKey:@"boss" timesToRepeat:-1];
		} else {
			[sceneSoundManager playMusicWithKey:@"stage1" timesToRepeat:-1];
		}
	} else if (stage == 2) {
		if (chapter == 9) {
			[sceneSoundManager playMusicWithKey:@"boss" timesToRepeat:-1];
		} else {
			[sceneSoundManager playMusicWithKey:@"stage2" timesToRepeat:-1];
		}
	} else if (stage == 3) {
		if (chapter == 10) {
			[sceneSoundManager playMusicWithKey:@"boss" timesToRepeat:-1];
		} else {
			[sceneSoundManager playMusicWithKey:@"stage3" timesToRepeat:-1];
		}
	} else if (stage == 4) {
		if (chapter == 9) {
			[sceneSoundManager playMusicWithKey:@"boss" timesToRepeat:-1];
		} else {
			[sceneSoundManager playMusicWithKey:@"stage4" timesToRepeat:-1];
		}
	} else if (stage == 5) {
		if (chapter == 5) {
			[sceneSoundManager playMusicWithKey:@"finalboss" timesToRepeat:-1];
		} else {
			[sceneSoundManager playMusicWithKey:@"stage5" timesToRepeat:-1];
		}
	} else if (stage == 6) {
		[sceneSoundManager playMusicWithKey:@"ending" timesToRepeat:-1];
	}
}

//게임 초기화
- (void)initGame {
	//무기, 애니 초기화
	[units removeAllObjects];
	[weapons removeAllObjects];
	[anis removeAllObjects];
	
	//주인공 배치
	Unit *unit = [[Unit alloc] init];
	[unit SetUnit:kCharacter_skull position:HERO_BASE_POSITION direction:0 gameScene:self];
	[units addObject:(Unit*)unit];
	[unit release];

	//조작 초기화
	moveUp = moveDown = NO;
	oldShootingWeapon = shootingWeapon = kShooting_none;
	joypadMoving = NO;
	weaponJoypadMoving = NO;
	
	//진행 초기화
	chapter = FIRST_CHAPTER;
	gameTime = 0;
	backGroundScroll = 0;
	
	bossLife = -1;
	bossHpBar = 0;
	boomSoundDelay = 0;
	solarSoundDelay = 0;
	
	gameoverDelay = 0;

	[sceneSoundManager unloadAllMusic];
	
	int stage = [_sharedGameData stage];
	if (stage == 0 || stage == 1) {
		[sceneSoundManager loadBackgroundMusicWithKey:@"stage1" fileName:@"stage1" fileExt:@"mp3"];
		[sceneSoundManager loadBackgroundMusicWithKey:@"boss" fileName:@"boss" fileExt:@"mp3"];
	} else if (stage == 2) {
		[sceneSoundManager loadBackgroundMusicWithKey:@"stage2" fileName:@"stage2" fileExt:@"mp3"];
		[sceneSoundManager loadBackgroundMusicWithKey:@"boss" fileName:@"boss" fileExt:@"mp3"];
	} else if (stage == 3) {
		[sceneSoundManager loadBackgroundMusicWithKey:@"stage3" fileName:@"stage3" fileExt:@"mp3"];
		[sceneSoundManager loadBackgroundMusicWithKey:@"boss" fileName:@"boss" fileExt:@"mp3"];
	} else if (stage == 4) {
		[sceneSoundManager loadBackgroundMusicWithKey:@"stage4" fileName:@"stage4" fileExt:@"mp3"];
		[sceneSoundManager loadBackgroundMusicWithKey:@"boss" fileName:@"boss" fileExt:@"mp3"];
	} else if (stage == 5) {
		[sceneSoundManager loadBackgroundMusicWithKey:@"stage5" fileName:@"stage5" fileExt:@"mp3"];
		[sceneSoundManager loadBackgroundMusicWithKey:@"finalboss" fileName:@"finalboss" fileExt:@"mp3"];
	} else if (stage == 6) {
		[sceneSoundManager loadBackgroundMusicWithKey:@"ending" fileName:@"ending" fileExt:@"mp3"];
	}
	
	//음악 시작 시간
	[self playMusic];
	
	//스테이지 타이틀 초기화
	for (int i = 0; i < 5; ++i) {
		[stageTitleImage[i] setAlpha:1.0f];
	}
	
	//The end 초기화
	[theEndImage setAlpha:0];
	
	//게임 설정 완료
	_sharedGameData.gameSetting = YES;
	pauseAble = YES;
}

//충돌 검사
- (BOOL)crush:(Unit*)unit target:(Unit*)target {
	CGRect unitRect = [unit getRect];
	CGRect targetRect = [target getRect];
	
	if (CGRectIntersectsRect(unitRect, targetRect)) {
		return YES;
	} else {
		return NO;
	}
}

//새로운 유닛 추가
- (void)addNewUnit:(CharacterID)characterId position:(CGPoint)position direction:(double)direction {
	Unit *unit = [[Unit alloc] init];
	[unit SetUnit:characterId position:position direction:direction gameScene:self];
	[units addObject:(Unit*)unit];
	[unit release];
	
	//세포 생성 거품
	if (characterId == kCharacter_skull_cell) {
		unit = [[Unit alloc] init];
		[unit SetUnit:kCharacter_cell_boom position:position direction:NO_ANGLE gameScene:self];
		[unit Move:CGPointMake(-10, -10)];
		[units addObject:(Unit*)unit];
		[unit release];

		[sceneSoundManager playSoundWithKey:@"rebirth" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
	}
}

- (void)explosion {
	[self addNewUnit:kCharacter_explosion position:CGPointMake(rand() % 130, rand() % 80) direction:NO_ANGLE];
	[self addNewUnit:kCharacter_explosion position:CGPointMake((rand() % 130) + 130, rand() % 80) direction:NO_ANGLE];
	[self addNewUnit:kCharacter_explosion position:CGPointMake((rand() % 130) + 260, rand() % 80) direction:NO_ANGLE];
	[self addNewUnit:kCharacter_explosion position:CGPointMake(rand() % 130, (rand() % 80) + 80) direction:NO_ANGLE];
	[self addNewUnit:kCharacter_explosion position:CGPointMake((rand() % 130) + 130, (rand() % 80) + 80) direction:NO_ANGLE];
	[self addNewUnit:kCharacter_explosion position:CGPointMake((rand() % 130) + 260, (rand() % 80) + 80) direction:NO_ANGLE];
	[self addNewUnit:kCharacter_explosion position:CGPointMake(rand() % 130, (rand() % 80) + 160) direction:NO_ANGLE];
	[self addNewUnit:kCharacter_explosion position:CGPointMake((rand() % 130) + 130, (rand() % 80) + 160) direction:NO_ANGLE];
	[self addNewUnit:kCharacter_explosion position:CGPointMake((rand() % 130) + 260, (rand() % 80) + 160) direction:NO_ANGLE];
	
	[sceneSoundManager playSoundWithKey:@"explosion" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
}

- (void)smallBoomSound {
	if(boomSoundDelay == 0) {
		int boomSound = rand() % 3;
		if(boomSound == 0) {
			[sceneSoundManager playSoundWithKey:@"boom1" gain:0.5f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
		} else if (boomSound == 1) {
			[sceneSoundManager playSoundWithKey:@"boom2" gain:0.5f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
		} else if (boomSound == 2) {
			[sceneSoundManager playSoundWithKey:@"boom3" gain:0.5f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
		}
		
		boomSoundDelay = 10;
	}
}

//폭발 발생
- (void)boom:(CGPoint)position characterId:(CharacterID)characterId direction:(double)direction{
	Unit* ani = [[Unit alloc] init];
	[ani SetUnit:characterId position:position direction:direction gameScene:self];
	[ani moveCenter];
	[anis addObject:(Unit*)ani];
	[ani release];
}

- (void)boom:(CGPoint)position direction:(double)direction boomType:(BoomType)aBoomType {
	//주인공 사망
	if (aBoomType == kBoom_hero || aBoomType == kBoom_big || aBoomType == kBoom_8way || aBoomType == kBoom_8way_nosound) {
		double angle[] = {0, M_PI / 4, M_PI / 2, M_PI * 3 / 4, M_PI, -M_PI / 4, -M_PI / 2, -M_PI * 3 / 4};
		
		for (int i = 0; i < 8; ++i) {
			CharacterID characterId = kCharacter_boom1 + (rand() % 4);
			[self boom:position characterId:characterId direction:angle[i]];
		}
		
		//사망 관련 처리
		playerSpeed = 0;
		shootingWeapon = kShooting_none;
		
		if (aBoomType == kBoom_hero) {
			[sceneSoundManager playSoundWithKey:@"hero_die" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
		} else if (aBoomType == kBoom_big) {
			[sceneSoundManager playSoundWithKey:@"boss_boom" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
		} else if (aBoomType == kBoom_8way) {
			[self smallBoomSound];
		}
	}
	//작은 폭발
	else if (aBoomType == kBoom_small) {
		CharacterID characterId = kCharacter_boom1 + (rand() % 4);
		[self boom:position characterId:characterId direction:NO_ANGLE];
		[self smallBoomSound];		
	}
	//무기1 폭발
	else if (aBoomType == kBoom_weapon1) {
		[self boom:position characterId:kCharacter_weapon_boom1 direction:0];
	}
	//무기3 폭발
	else if (aBoomType == kBoom_weapon3) {
		[self boom:position characterId:kCharacter_weapon_boom3 direction:0];
	}
	//무기4 폭발
	else if (aBoomType == kBoom_weapon4) {
		[self boom:position characterId:kCharacter_weapon_boom4 direction:0];
	}
	//기둥 폭발
	else if (aBoomType == kBoom_pillar) {
		[self boom:position characterId:kCharacter_boom_pillar direction:0];
	}
	//폭탄 폭발
	else if (aBoomType == kBoom_explosion) {
		[self explosion];
	}
	
}

//적의 미사일 발사
- (void)shoot:(CharacterID)shootObject position:(CGPoint)position direction:(double)direction {
	Unit *unit = [[Unit alloc] init];
	[unit SetUnit:shootObject position:position direction:direction gameScene:self];
	[unit moveCenter];
	[units addObject:(Unit*)unit];
	[unit release];
}

//액션 처리
- (void)processAction:(GLfloat)aDelta {
	//캐릭터 처리
	for(int i = 0; i < [characters count]; i++) {
		[[characters objectAtIndex:i] update:aDelta];
	}
	
	//유닛 행동
	for(int i = 0; i < [units count]; i++) {
		[[units objectAtIndex:i] Action];
	}
	
	//무기 행동
	for(int i = 0; i < [weapons count]; i++) {
		[[weapons objectAtIndex:i] Action];
	}
	
	//효과 애니메이션 행동
	for(int i = 0; i < [anis count]; i++) {
		[[anis objectAtIndex:i] Action];
	}
}

//충돌 처리
- (void)processCrush:(Unit*)hero {
	for(int i = HERO + 1; i < [units count]; i++) {
		
		Unit *unit = [units objectAtIndex:i];
		
		if(unit.life == YES)
		{
			//적과 주인공
			if(hero.life == YES && [self crush:hero target:unit] == YES) {
				if (unit.characterId < kCharacter_power_item1 || unit.characterId > kCharacter_power_item5) {	//아이템과 주인공의 충돌은 인정 안 됨
					
					//보스가 주인공을 때려서 얻는 아이템들
					if (hero.shock == 0 && unit.characterId >= kCharacter_boss_weapon1 && unit.characterId <= kCharacter_boss_weapon4) {
						int stage = [_sharedGameData stage];
						if (stage == 0 || stage == 1) {
							[self addNewUnit:kCharacter_power_item1 position:[hero centerPosition] direction:M_PI];
						} else if (stage == 2) {
							[self addNewUnit:kCharacter_power_item2 position:[hero centerPosition] direction:M_PI];
						} else if (stage == 3) {
							[self addNewUnit:kCharacter_power_item4 position:[hero centerPosition] direction:M_PI];
						} else if (stage == 4) {
							[self addNewUnit:kCharacter_power_item3 position:[hero centerPosition] direction:M_PI];
						} else if (stage == 5) {
							if (bossLv < BOSS_LV_MAX) {
								[self addNewUnit:kCharacter_power_item5 position:[hero centerPosition] direction:M_PI];
							} else {
								int rate = rand() % 5;
								if (rate == 0) {
									[self addNewUnit:kCharacter_power_item3 position:[hero centerPosition] direction:M_PI];
								} else if (rate == 1) {
									[self addNewUnit:kCharacter_power_item2 position:[hero centerPosition] direction:M_PI];
								} else {
									[self addNewUnit:kCharacter_power_item1 position:[hero centerPosition] direction:M_PI];
								}
							}
						}
					}
					
					//주인공의 피해
					[hero Damage:unit._character.power shock:YES];
					//적의 피해
					[unit Damage:hero._character.power shock:YES];
				}
			}
			
			//반사된 미사일
			if(unit.reflect == YES) {
				for(int j = HERO + 1; j < [units count]; j++) {
					Unit *target = [units objectAtIndex:j];
					if(i != j && target.life == YES && [self crush:unit target:target] == YES) {
						[unit Damage:target._character.power shock:YES];
						[target Damage:unit._character.power shock:YES];
					}
				}
			}
			   
			//세포 융합
			if(unit.characterId == kCharacter_cell) {
				for(int j = HERO + 1; j < [units count]; j++) {
					Unit *target = [units objectAtIndex:j];
					if(i != j && target.life == YES && target.characterId == kCharacter_cell 
					   && [self crush:unit target:target] == YES) {
						unit.life = NO;
						target.life = NO;
						[self addNewUnit:kCharacter_cell2 position:unit.unitPosition direction:unit.direction];
						continue;
					}
				}
			} else if(unit.characterId == kCharacter_cell2) {
				for(int j = HERO + 1; j < [units count]; j++) {
					Unit *target = [units objectAtIndex:j];
					if(i != j && target.life == YES && target.characterId == kCharacter_cell2
					   && [self crush:unit target:target] == YES) {
						unit.life = NO;
						target.life = NO;
						[self addNewUnit:kCharacter_cell4 position:unit.unitPosition direction:unit.direction];
						continue;
					}
				}
			}
			
			//보스의 아이템 먹기
			if (unit.characterId >= kCharacter_boss1 && unit.characterId <= kCharacter_boss5 && unit.life == YES) {
				for(int j = HERO + 1; j < [units count]; j++) {
					Unit *target = [units objectAtIndex:j];
					
					//회복 아이템
					if(i != j && target.life == YES) {
						if (target.characterId == kCharacter_power_item1 && [self crush:unit target:target] == YES) {
							target.life = NO;
							[unit recover:10];
							[sceneSoundManager playSoundWithKey:@"eat_item" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
						} else if (target.characterId == kCharacter_power_item2 && [self crush:unit target:target] == YES) {
							target.life = NO;
							
							if (unit.slave == nil) {
								[self addNewUnit:kCharacter_shield position:CGPointMake(480, 0) direction:NO_ANGLE];
								Unit* shield = [units objectAtIndex:([units count] - 1)];
								shield.master = unit;
								unit.slave = shield;
							}

							[sceneSoundManager playSoundWithKey:@"shield" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
						} else if (target.characterId == kCharacter_power_item3 && [self crush:unit target:target] == YES) {
							target.life = NO;
							if (bossBomb < BOSS_BOMB_MAX) {
								bossBomb++;
							}

							[sceneSoundManager playSoundWithKey:@"eat_item" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
						} else if (target.characterId == kCharacter_power_item4 && [self crush:unit target:target] == YES) {
							target.life = NO;
							if (bossLife < BOSS_LIFE_MAX) {
								bossLife++;
							}

							[sceneSoundManager playSoundWithKey:@"eat_item" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
						} else if (target.characterId == kCharacter_power_item5 && [self crush:unit target:target] == YES) {
							target.life = NO;
							if (bossLife < BOSS_LV_MAX) {
								bossLv++;
							}

							[sceneSoundManager playSoundWithKey:@"eat_item" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
						}
					}
				}
			}
		}
	}

	//무기와 유닛 충돌
	for(int i = 0; i < [weapons count]; i++) {
		
		Unit *weapon = [weapons objectAtIndex:i];
		
		//무기와 적
		if(weapon.life == YES)
		{
			int start = HERO + 1;
			if (weapon.reflect == YES && weapon.tempo >= 60) {	//반사된 미사일 경우 주인공에게 맞을 수 있음
				start = 0;
			}
			
			for(int i = start; i < [units count]; i++) {
				
				Unit *unit = [units objectAtIndex:i];
				
				//적과 무기
				if(unit.life == YES && [self crush:weapon target:unit] == YES) {
					//반사
					if (weapon._character.characterId == kCharacter_beam && unit._character.pattern == kPattern_go_ahead) {
						[weapon Damage:unit._character.power shock:NO];
						unit.direction = M_PI;					
						unit.reflect = YES;
					}
					//굴절
					else if (unit._character.characterId == kCharacter_jelly) {
						weapon.reflect = YES;
						if(weapon.unitPosition.y > unit.unitPosition.y + unit._character.height / 2)
						{
							weapon.direction += M_PI / 45;
							if (weapon.direction > M_PI) {
								weapon.direction -= M_PI * 2;
							}
						} else {
							weapon.direction -= M_PI / 45;
							if (weapon.direction <= -M_PI) {
								weapon.direction += M_PI * 2;
							}
						}

					}
					//보통
					else {
						[weapon Damage:unit._character.power shock:NO];
						[unit Damage:weapon._character.power shock:YES];
					}
				}
			}
		}
	}
}

//죽은 유닛 배열에서 제거
- (void)removeDeadUnit {
	//죽은 유닛 배열에서 제거(주인공 제외)
	for (int i = HERO + 1; i < [units count]; i++) {
		Unit *unit = [units objectAtIndex:i];
		if (unit.life == NO) {
			BOOL isRemove = YES;
			
			if (unit._character.pattern == kPattern_skull_cell) { // 해골 세포의 경우 동료가 살아있으면 죽지 않음
				for (int i = HERO + 1; i < [units count]; i++) {
					Unit *other = [units objectAtIndex:i];
					if (other._character.pattern == kPattern_skull_cell && other.life == YES) {
						isRemove = NO;
						break;
					}
				}
			}
			
			if (isRemove == YES) {
				[units removeObjectAtIndex:i];
			}			
		};
	}
	
	//사라진 무기 배열에서 제거
	for (int i = 0; i < [weapons count]; i++) {
		Unit *weapon = [weapons objectAtIndex:i];
		if (weapon.life == NO) {
			[weapons removeObjectAtIndex:i];
		};
	}
	
	//종료된 애니메이션 배열에서 제거
	for (int i = 0; i < [anis count]; i++) {
		Unit *ani = [anis objectAtIndex:i];
		if (ani.life == NO) {
			[anis removeObjectAtIndex:i];
		};
	}
}

//무기 발사
- (void)baseMissile:(Unit*)hero {
	static float shootDirection = -M_PI / 2;
	shootDirection -= M_PI / 36;
	if (shootDirection <= -M_PI) {
		shootDirection *= -1;
		shootDirection -= M_PI / 18;
	} else if (shootDirection > 0 && shootDirection <= M_PI / 2) {
		shootDirection = -M_PI / 2;
	}

	Unit *weapon = [[Unit alloc] init];
	[weapon SetUnit:kCharacter_missile position:CGPointMake(hero.unitPosition.x + 40, hero.unitPosition.y + 22) direction:shootDirection gameScene:self];
	[weapons addObject:(Unit*)weapon];
	[weapon release];
	
	weapon = [[Unit alloc] init];
	[weapon SetUnit:kCharacter_missile position:CGPointMake(hero.unitPosition.x + 40, hero.unitPosition.y + 22) direction:-shootDirection gameScene:self];
	[weapons addObject:(Unit*)weapon];
	[weapon release];
	
	static int soundDelay;
	soundDelay++;
	if (soundDelay >= 4) {
		[sceneSoundManager playSoundWithKey:@"base_missile" gain:0.2f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
		soundDelay = 0;
	}
}

- (void)thunderBeam:(Unit*)hero {
	Unit *weapon = [[Unit alloc] init];
	[weapon SetUnit:kCharacter_lightning position:CGPointMake(hero.unitPosition.x + 16, hero.unitPosition.y + 82) direction:M_PI gameScene:self];
	[weapons addObject:(Unit*)weapon];
	[weapon release];

	//발사 효과
	Unit *ani = [[Unit alloc] init];
	[ani SetUnit:kCharacter_weapon_boom2 position:CGPointMake(hero.unitPosition.x + 16, hero.unitPosition.y + 82) direction:0 gameScene:self];
	[ani moveCenter];
	[anis addObject:(Unit*)ani];
	[ani release];

	[sceneSoundManager playSoundWithKey:@"thunder" gain:0.5f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
}

- (void)wideBeam:(Unit*)hero {
	Unit *weapon = [[Unit alloc] init];
	[weapon SetUnit:kCharacter_beam position:CGPointMake(hero.unitPosition.x + 30, hero.unitPosition.y - 60) direction:M_PI gameScene:self];
	[weapons addObject:(Unit*)weapon];
	[weapon release];
	
	[sceneSoundManager playSoundWithKey:@"beam" gain:0.3f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
}

- (void)solarBeam:(Unit*)hero {
	Unit *weapon = [[Unit alloc] init];
	[weapon SetUnit:kCharacter_solar position:CGPointMake(hero.unitPosition.x + 4, hero.unitPosition.y - 32) direction:M_PI gameScene:self];
	[weapons addObject:(Unit*)weapon];
	[weapon release];

	if (solarSoundDelay == 0) {
		[sceneSoundManager playSoundWithKey:@"solar" gain:0.3f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
		solarSoundDelay = 35;
	}
}

//무기 발사 처리
- (void)shootingProcess:(Unit*)hero {
	if (weaponJoypadMoving == NO) {
		return;
	}
	
	//무기 발사
	if (shootingWeapon == kShooting_missile) {
		if (weaponDelay <= 0) {
			[self baseMissile:hero];
			weaponDelay = 2;
		}
		else {
			weaponDelay--;
		}
	} else if (shootingWeapon == kShooting_lightning) {
		if (weaponDelay <= 0) {
			[self thunderBeam:hero];
			weaponDelay = 30;
		}
		else {
			weaponDelay--;
		}
	} else if (shootingWeapon == kShooting_beam) {
		if (weaponDelay <= 0) {
			[self wideBeam:hero];
			weaponDelay = 10;
		}
		else {
			weaponDelay--;
		}
	} else if (shootingWeapon == kShooting_solar) {
		if (weaponDelay <= 0) {
			[self solarBeam:hero];
			weaponDelay = 2;
		}
		else {
			weaponDelay--;
		}
	}
}

- (void)boomAll {
	if([units count] > 1) {
		[sceneSoundManager playSoundWithKey:@"boss_boom" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
		boomSoundDelay = 10;
	}
	
	for (int i = HERO + 1; i < [units count]; i++) {
		[[units objectAtIndex:i] boom];
	}
}

- (void)nextChapter:(int)maxTime {
	if(gameTime >= maxTime)
	{
		gameTime=0;
		chapter++;
	}
}

//젤리 씌우기
- (void)createJelly {
	[self addNewUnit:kCharacter_jelly position:CGPointMake(560, 0) direction:0];

	Unit* jelly = [units objectAtIndex:([units count] - 1)];
	Unit* core = [units objectAtIndex:([units count] - 2)];
	jelly.master = core;
	core.slave = jelly;
}

- (void)calBossHpMax {
	bossHpMax = 0;
	for (int i = 0; i < [units count]; i++) {
		Unit *unit = [units objectAtIndex:i];
		if (unit.life == YES && unit._character.characterId == bossCharacterId) {
			bossHpMax += unit._character.hpMax;
		}
	}
}

- (void) appearBoss:(CharacterID)characterId Lv:(int)lv Life:(int)life LvMax:(int)lvMax{
	int stage = [_sharedGameData stage];
	backGroundScroll += SCROLL_SPEED;	//뒷 배경 스크롤 멈춤
	
	if (gameTime == 1) {
		[self boomAll];
		if (stage == 5) {
			[sceneSoundManager playMusicWithKey:@"finalboss" timesToRepeat:-1];
		} else {
			[sceneSoundManager playMusicWithKey:@"boss" timesToRepeat:-1];
		}
	} else if (gameTime == 79) {
		[self addNewUnit:characterId position:CGPointMake(480, 160) direction:0];

		bossLv = lv;
		bossLife = life;
		bossBomb = 0;
		bossCharacterId = characterId;
		[self calBossHpMax];
		clearDelay = 0;

		if (stage == 3) {
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(350, 280) direction:NO_ANGLE];
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(350, 230) direction:NO_ANGLE];
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(350, 180) direction:NO_ANGLE];
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(350, 130) direction:NO_ANGLE];
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(350, 80) direction:NO_ANGLE];
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(350, 30) direction:NO_ANGLE];
			[self addNewUnit:characterId position:CGPointMake(480, 160) direction:0];

			bossLv = 5;
			bossLife = 3;
		}
		else if(stage == 4) {
			[self createJelly];
		}
	} else if (gameTime >= 80) {
		if(bossHp <= 0)	//클리어
		{
			if (bossLife > 0) {
				clearDelay++;
				
				//다음 보스 등장
				if (clearDelay >= 100) {
					[self addNewUnit:characterId position:CGPointMake(480, 160) direction:0];
					bossBomb = 0;

					if(stage == 4) {
						[self createJelly];
					}
					
					if (bossLv < lvMax) {
						bossLv++;
					}
					
					bossLife--;
					
					clearDelay = 0;
				}
			} else {
				//뒷처리
				if(clearDelay == 0)
				{
					pauseAble = NO;
					[self boomAll];
					[sceneSoundManager stopMusic];
					bossLife = -1;
				}
				
				//클리어 팡파레
				if (clearDelay == 100) {
					[sceneSoundManager playSoundWithKey:@"fanfare" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
				}
				
				//다음 스테이지로
				if(clearDelay > CLEAR_DELAY)
				{
					//점수 계산
					[self calScore];
					if ([_sharedGameData stage] != 0 && [_sharedGameData score] > [[_sharedGameData gameData] hiscore]) {
						_sharedGameData.gameData.hiscore = [_sharedGameData score];
						[_sharedGameData saveData];
					}

					sceneState = kSceneState_TransitionOut;
					nextSceneKey = @"menu";
					_sharedGameData.gameSetting = NO;
				} else {
					clearDelay++;
				}
			}
		}
	}
}

- (void)showStageTitle {
	int stage = [_sharedGameData stage];

	if (gameTime > TITLE_SHOW_DELAY - 100 && gameTime <= TITLE_SHOW_DELAY) {
		[stageTitleImage[stage - 1] setAlpha:(0.01f * (float)(TITLE_SHOW_DELAY - gameTime))];
	} else if (gameTime > TITLE_SHOW_DELAY) {
		[self nextChapter:0];
	}
}

- (void)stage1Process {
	if (chapter == 0) {
		[self showStageTitle];
	} 
	else if(chapter == 1)	//눈구더기(조금)
	{
		if((gameTime % 24) == 0) {
			[self addNewUnit:kCharacter_worm position:CGPointMake(480, rand() % 310) direction:0];
		}
		
		[self nextChapter:1200];
	}
	else if(chapter == 2)	//노려보는 눈
	{
		if((gameTime % 50) == 0) {
			[self addNewUnit:kCharacter_angry_eye position:CGPointMake(480, rand() % 280 + 40) direction:0];
		}
		
		[self nextChapter:600];
	}
	else if(chapter == 3) //눈구더기(많이)
	{
		if((gameTime % 10) == 0) {
			[self addNewUnit:kCharacter_worm position:CGPointMake(480, rand() % 310) direction:0];
		}
		
		[self nextChapter:600];
	}
	else if(chapter == 4)	//빛나는 눈
	{
		if((gameTime % 80) == 0) {
			[self addNewUnit:kCharacter_beauty_eye position:CGPointMake(480, rand() % 260 + 40) direction:0];
		}
		
		[self nextChapter:600];
	}
	else if(chapter == 5)	//중간 보스 파리
	{
		backGroundScroll += SCROLL_SPEED;	//뒷 배경 스크롤 멈춤
		
		if (gameTime == 1) {
			[self boomAll];
			[self addNewUnit:kCharacter_boss_fly position:CGPointMake(480, 160) direction:0];

			bossLv = 0;
			bossLife = 0;
			bossBomb = 0;
			bossCharacterId = kCharacter_boss_fly;
			[self calBossHpMax];
		}
		
		//적 모두 죽인 뒤
		if ([units count] <= 1) {
			[self nextChapter:0];
			bossLife = -1;
		}
	}
	else if(chapter == 6)	//눈 합동 공격
	{
		if((gameTime % 50) == 0) {
			[self addNewUnit:kCharacter_angry_eye position:CGPointMake(480, rand() % 280 + 40) direction:0];
		}
		
		if((gameTime % 80) == 0) {
			[self addNewUnit:kCharacter_beauty_eye position:CGPointMake(480, rand() % 260 + 40) direction:0];
		}
		
		[self nextChapter:600];
	}
	else if(chapter == 7)	//눈구더기 떼
	{
		if((gameTime % 3) == 0) {
			[self addNewUnit:kCharacter_worm position:CGPointMake(480, rand() % 310) direction:0];
		}
		
		[self nextChapter:1200];
	}
	else if(chapter == 8)	//중간 보스 파리
	{
		backGroundScroll += SCROLL_SPEED;	//뒷 배경 스크롤 멈춤
		
		if (gameTime == 1) {
			[self boomAll];
			[self addNewUnit:kCharacter_boss_fly position:CGPointMake(480, 200) direction:0];
			[self addNewUnit:kCharacter_boss_fly position:CGPointMake(480, 100) direction:0];

			bossLv = 0;
			bossLife = 0;
			bossBomb = 0;
			bossCharacterId = kCharacter_boss_fly;
			[self calBossHpMax];
		}
		
		//적 모두 죽인 뒤
		if ([units count] <= 1) {
			[self nextChapter:0];
			
			bossLife = -1;
		}
	}
	else if(chapter == 9)	//총공격
	{
		if((gameTime % 100) == 50) {
			[self addNewUnit:kCharacter_angry_eye position:CGPointMake(480, rand() % 280 + 40) direction:0];
		}
		
		if((gameTime % 200) == 0) {
			[self addNewUnit:kCharacter_beauty_eye position:CGPointMake(480, rand() % 260 + 40) direction:0];
		}
		
		if((gameTime % 14) == 0) {
			[self addNewUnit:kCharacter_worm position:CGPointMake(480, rand() % 310) direction:0];
		}
		
		[self nextChapter:1800];
	}
	else if(chapter == 10)	//보스
	{
		[self appearBoss:kCharacter_boss1 Lv:1 Life:1 LvMax:4];
	}	
}

- (void)stage2Process {
	if (chapter == 0) {
		[self showStageTitle];
	} 
	else if(chapter == 1)	//보
	{
		if ((gameTime % 40) == 0) {
			[self addNewUnit:kCharacter_hand_b position:CGPointMake(480, (rand() % 280) + 40) direction:0]; 
		}
		
		[self nextChapter:600];
	}
	else if(chapter == 2)	//가리키는 손
	{
		if ((gameTime % 200) == 0) {
			[self addNewUnit:kCharacter_hand_x position:CGPointMake(480, (rand() % 280) + 40) direction:0];
		}
		
		[self nextChapter:1200];
	}
	else if(chapter == 3)	//보와 발
	{
		if ((gameTime % 40) == 0) {
			[self addNewUnit:kCharacter_hand_b position:CGPointMake(480, (rand() % 280) + 40) direction:0]; 
			[self addNewUnit:kCharacter_foot position:CGPointMake((rand() % 380) + 40, 310) direction:M_PI / 2]; 
		}
		
		[self nextChapter:600];
	}
	else if(chapter == 4)	//주먹과 손
	{
		if ((gameTime % 200) == 0) {
			[self addNewUnit:kCharacter_hand_x position:CGPointMake(480, (rand() % 280) + 40) direction:0];
		}

		if ((gameTime % 40) == 0) {
			[self addNewUnit:kCharacter_punch position:CGPointMake(470, (rand() % 280) + 40) direction:0]; 
		}
		
		[self nextChapter:900];
	}
	else if(chapter == 5)	//중간보스 얼굴
	{
		backGroundScroll += SCROLL_SPEED;	//뒷 배경 스크롤 멈춤
		
		if (gameTime == 1) {
			[self boomAll];
		} else if (gameTime == 49) {
			[self addNewUnit:kCharacter_face position:CGPointMake(480, 145) direction:0];

			bossLv = 0;
			bossLife = 0;
			bossBomb = 0;
			bossCharacterId = kCharacter_face;
			[self calBossHpMax];
		}
		
		//적 모두 죽인 뒤
		if ([units count] <= 1) {
			[self nextChapter:50];
			bossLife = -1;
		}
	}
	else if(chapter == 6)	//가리키는 손과 발
	{
		if ((gameTime % 150) == 0) {
			[self addNewUnit:kCharacter_hand_x position:CGPointMake(480, (rand() % 280) + 40) direction:0];
		}
		
		if ((gameTime % 40) == 0) {
			[self addNewUnit:kCharacter_foot position:CGPointMake((rand() % 380) + 40, 310) direction:M_PI / 2]; 
		}

		[self nextChapter:900];
	}
	else if(chapter == 7)	//발과 주먹
	{
		if ((gameTime % 40) == 0) {
			[self addNewUnit:kCharacter_foot position:CGPointMake((rand() % 380) + 40, 310) direction:M_PI / 2]; 
			[self addNewUnit:kCharacter_punch position:CGPointMake(470, (rand() % 280) + 40) direction:0]; 
		}
		
		[self nextChapter:1200];
	}
	else if(chapter == 8)	//총공격
	{
		if ((gameTime % 40) == 0) {
			[self addNewUnit:kCharacter_hand_b position:CGPointMake(480, (rand() % 280) + 40) direction:0]; 
			[self addNewUnit:kCharacter_foot position:CGPointMake((rand() % 380) + 40, 310) direction:M_PI / 2]; 
			[self addNewUnit:kCharacter_punch position:CGPointMake(470, (rand() % 280) + 40) direction:0]; 
		}

		if ((gameTime % 200) == 0) {
			[self addNewUnit:kCharacter_hand_x position:CGPointMake(480, (rand() % 280) + 40) direction:0];
		}
		
		[self nextChapter:1800];
	}
	else if(chapter == 9)	//보스
	{
		[self appearBoss:kCharacter_boss2 Lv:2 Life:2 LvMax:4];
	}
}

- (void)stage3Process {
	if (chapter == 0) {
		[self showStageTitle];
	} 
	else if(chapter == 1)	//세포
	{
		if ((gameTime % 40) == 0) {
			[self addNewUnit:kCharacter_cell position:CGPointMake(480, (rand() % 280) + 40) direction:((M_PI / 180) * (rand() % 45))];
		}
		
		[self nextChapter:900];
	}
	else if(chapter == 2)	//세포+기둥
	{
		if ((gameTime % 40) == 0) {
			[self addNewUnit:kCharacter_cell position:CGPointMake(480, (rand() % 280) + 40) direction:((M_PI / 180) * (rand() % 45))];
		}

		if ((gameTime % 140) == 0) {
			[self addNewUnit:kCharacter_cell_pillar position:CGPointMake(480, 0) direction:0];
		}
		
		[self nextChapter:900];
	}
	else if(chapter == 3)	//중간보스 왕세포
	{
		backGroundScroll += SCROLL_SPEED;	//뒷 배경 스크롤 멈춤
		
		if (gameTime == 1) {
			[self boomAll];
			[self addNewUnit:kCharacter_king_cell position:CGPointMake(480, 100) direction:0];

			bossLv = 0;
			bossLife = 0;
			bossBomb = 0;
			bossCharacterId = kCharacter_king_cell;
			[self calBossHpMax];
		}
		
		//적 모두 죽인 뒤
		if ([units count] <= 1) {
			[self nextChapter:0];
			bossLife = -1;
		}
	}
	else if(chapter == 4)	//세포2
	{
		if ((gameTime % 40) == 0) {
			[self addNewUnit:kCharacter_cell2 position:CGPointMake(480, (rand() % 280) + 40) direction:((M_PI / 180) * (rand() % 45))];
		}
		
		[self nextChapter:900];
	}
	else if(chapter == 5)	//투명 세포
	{
		if ((gameTime % 40) == 0) {
			[self addNewUnit:kCharacter_transparent_cell position:CGPointMake(480, (rand() % 280) + 40) direction:((M_PI / 180) * (rand() % 45))];
		}
		
		[self nextChapter:600];
	}
	else if(chapter == 6)	//작은 세포
	{
		if ((gameTime % 3) == 0) {
			[self addNewUnit:kCharacter_mini_cell position:CGPointMake(480, rand() % 320) direction:0];
		}
		
		[self nextChapter:600];
	}
	else if(chapter == 7)	//투명세포, 기둥
	{
		if ((gameTime % 40) == 0) {
			[self addNewUnit:kCharacter_transparent_cell position:CGPointMake(480, (rand() % 280) + 40) direction:((M_PI / 180) * (rand() % 45))];
		}
		
		if ((gameTime % 140) == 0) {
			[self addNewUnit:kCharacter_cell_pillar position:CGPointMake(480, 0) direction:0];
		}
		
		[self nextChapter:900];
	}
	else if(chapter == 8)	//중간보스 왕세포
	{
		backGroundScroll += SCROLL_SPEED;	//뒷 배경 스크롤 멈춤
		
		if (gameTime == 1) {
			[self boomAll];
			[self addNewUnit:kCharacter_king_cell position:CGPointMake(480, 50) direction:0];
			[self addNewUnit:kCharacter_king_cell position:CGPointMake(480, 150) direction:0];

			bossLv = 0;
			bossLife = 0;
			bossBomb = 0;
			bossCharacterId = kCharacter_king_cell;
			[self calBossHpMax];
		}
		
		//적 모두 죽인 뒤
		if ([units count] <= 1) {
			[self nextChapter:0];
			bossLife = -1;
		}
	}
	else if(chapter == 9)	//총공격
	{
		if ((gameTime % 40) == 0) {
			[self addNewUnit:kCharacter_transparent_cell position:CGPointMake(480, (rand() % 280) + 40) direction:((M_PI / 180) * (rand() % 45))];
			[self addNewUnit:kCharacter_cell position:CGPointMake(480, (rand() % 280) + 40) direction:((M_PI / 180) * (rand() % 45))];
		}

		if ((gameTime % 140) == 0) {
			[self addNewUnit:kCharacter_cell_pillar position:CGPointMake(480, 0) direction:0];
		}

		if ((gameTime % 20) == 0) {
			[self addNewUnit:kCharacter_mini_cell position:CGPointMake(480, rand() % 320) direction:0];
		}
		
		[self nextChapter:1800];
	}
	else if(chapter == 10)	//보스
	{
		[self appearBoss:kCharacter_boss3 Lv:5 Life:3 LvMax:8];
	}
}

- (void)stage4Process {
	if (chapter == 0) {
		[self showStageTitle];
	} 
	else if(chapter == 1)	//전진 젤리
	{
		if ((gameTime % 200) == 0) {
			[self addNewUnit:kCharacter_jelly_core position:CGPointMake(560, rand() % 280 + 20) direction:0];
			[self createJelly];
		}
		
		[self nextChapter:1200];
	}
	else if(chapter == 2)	//입
	{
		if ((gameTime % 50) == 0) {
			[self addNewUnit:kCharacter_mouth position:CGPointMake(480, rand() % 280 + 20) direction:0];
		}
		
		[self nextChapter:900];
	}
	else if(chapter == 3)	//공격 젤리
	{
		if ((gameTime % 200) == 0) {
			[self addNewUnit:kCharacter_jelly_core2 position:CGPointMake(560, rand() % 280 + 20) direction:0];
			[self createJelly];
		}
		
		[self nextChapter:1200];
	}
	else if(chapter == 4)	//악마의 눈
	{
		if ((gameTime % 200) == 100) {
			[self addNewUnit:kCharacter_evil_eye position:CGPointMake(480, rand() % 220) direction:0];
		}
		
		[self nextChapter:1200];
	}
	else if(chapter == 5)	//중간 보스 로봇
	{
		backGroundScroll += SCROLL_SPEED;	//뒷 배경 스크롤 멈춤
		
		if (gameTime == 1) {
			[self boomAll];
			[self addNewUnit:kCharacter_robot position:CGPointMake(480, 160) direction:0];

			bossLv = 0;
			bossLife = 0;
			bossBomb = 0;
			bossCharacterId = kCharacter_robot;
			[self calBossHpMax];
		}
		
		//적 모두 죽인 뒤
		if ([units count] <= 1) {
			[self nextChapter:0];
			bossLife = -1;
		}
	}
	else if(chapter == 6)	//입 + 전진 젤리
	{
		if ((gameTime % 50) == 0) {
			[self addNewUnit:kCharacter_mouth position:CGPointMake(480, rand() % 280 + 20) direction:0];
		}

		if ((gameTime % 200) == 0) {
			[self addNewUnit:kCharacter_jelly_core position:CGPointMake(560, rand() % 280 + 20) direction:0];
			[self createJelly];
		}
		
		[self nextChapter:900];
	}
	else if(chapter == 7)	//악마의 눈 + 공격 젤리
	{
		if ((gameTime % 200) == 100) {
			[self addNewUnit:kCharacter_evil_eye position:CGPointMake(480, rand() % 220) direction:0];
		}

		if ((gameTime % 200) == 0) {
			[self addNewUnit:kCharacter_jelly_core2 position:CGPointMake(560, rand() % 280 + 20) direction:0];
			[self createJelly];
		}
		
		[self nextChapter:900];
	}
	else if(chapter == 8)	//총 공격
	{
		if ((gameTime % 50) == 0) {
			[self addNewUnit:kCharacter_mouth position:CGPointMake(480, rand() % 280 + 20) direction:0];
		}

		if ((gameTime % 200) == 100) {
			[self addNewUnit:kCharacter_evil_eye position:CGPointMake(480, rand() % 220) direction:0];
		}
		
		if ((gameTime % 200) == 0) {
			[self addNewUnit:kCharacter_jelly_core2 position:CGPointMake(560, rand() % 280 + 20) direction:0];
			[self createJelly];
		}

		[self nextChapter:1800];
	}
	else if(chapter == 9)	//보스
	{
		[self appearBoss:kCharacter_boss4 Lv:10 Life:2 LvMax:12];
	}
	
}

- (void)stage5Process {
	if (chapter == 0) {
		[self showStageTitle];
	} 
	else if(chapter == 1)	//중간 보스 파리
	{
		backGroundScroll += SCROLL_SPEED;	//뒷 배경 스크롤 멈춤
		
		if (gameTime == 1) {
			[self boomAll];
			[self addNewUnit:kCharacter_boss_fly position:CGPointMake(480, 200) direction:0];
			[self addNewUnit:kCharacter_boss_fly position:CGPointMake(480, 160) direction:0];
			[self addNewUnit:kCharacter_boss_fly position:CGPointMake(480, 120) direction:0];
			
			bossLv = 0;
			bossLife = 0;
			bossBomb = 0;
			bossCharacterId = kCharacter_boss_fly;
			[self calBossHpMax];
		}
		
		//적 모두 죽인 뒤
		if ([units count] <= 1) {
			[self nextChapter:0];
			bossLife = -1;
		}
	}
	else if(chapter == 2)	//중간 보스 얼굴
	{
		backGroundScroll += SCROLL_SPEED;	//뒷 배경 스크롤 멈춤
		
		if (gameTime == 1) {
			[self boomAll];
		} else if (gameTime == 49) {
			[self addNewUnit:kCharacter_face position:CGPointMake(480, 145) direction:0];

			bossLv = 0;
			bossLife = 0;
			bossBomb = 0;
			bossCharacterId = kCharacter_face;
			[self calBossHpMax];
		}

		if ((gameTime % 40) == 0) {
			[self addNewUnit:kCharacter_foot position:CGPointMake((rand() % 380) + 40, 310) direction:M_PI / 2]; 
		}
		
		int face_count = 0;
		for (int i = 0; i < [units count]; i++) {
			if ([[[units objectAtIndex:i] _character] characterId] == kCharacter_face) {
				face_count = 1;
				break;
			}
		}
		
		//적 모두 죽인 뒤
		if (face_count == 0) {
			[self nextChapter:50];
			bossLife = - 1;
		}
	}
	else if(chapter == 3)	//중간 보스 좀비 세포
	{
		backGroundScroll += SCROLL_SPEED;	//뒷 배경 스크롤 멈춤
		
		if (gameTime == 1) {
			[self boomAll];
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(350, 230) direction:NO_ANGLE];
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(350, 180) direction:NO_ANGLE];
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(350, 130) direction:NO_ANGLE];
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(350, 80) direction:NO_ANGLE];
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(300, 230) direction:NO_ANGLE];
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(300, 180) direction:NO_ANGLE];
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(300, 130) direction:NO_ANGLE];
			[self addNewUnit:kCharacter_skull_cell position:CGPointMake(300, 80) direction:NO_ANGLE];

			bossLv = 0;
			bossLife = 0;
			bossBomb = 0;
			bossCharacterId = kCharacter_skull_cell;
			[self calBossHpMax];
		}
		
		//적 모두 죽인 뒤
		if ([units count] <= 1) {
			[self nextChapter:0];
			bossLife = -1;
		}
	}
	else if(chapter == 4)	//중간 보스 로봇
	{
		backGroundScroll += SCROLL_SPEED;	//뒷 배경 스크롤 멈춤
		
		if (gameTime == 1) {
			[self boomAll];
			[self addNewUnit:kCharacter_robot position:CGPointMake(480, 160) direction:0];
			[self createJelly];
			
			bossLv = 0;
			bossLife = 0;
			bossBomb = 0;
			bossCharacterId = kCharacter_robot;
			[self calBossHpMax];
		}
		
		//적 모두 죽인 뒤
		if ([units count] <= 1) {
			[self nextChapter:0];
			bossLife = -1;
		}
	}
	if(chapter == 5)	//최종 보스
	{
		[self appearBoss:kCharacter_boss5 Lv:5 Life:0 LvMax:16];
	}
}

- (void)tutorialProcess {
	if (chapter == 0) {
		[arialFont setColourFilterRed:1 green:1 blue:1 alpha:0];
		[self nextChapter:60];
	} else if (chapter >= 1 && chapter <= 8) {
		[self nextChapter:600];
		if (gameTime <= 61) {
			[arialFont setColourFilterRed:1 green:1 blue:1 alpha:(float)(gameTime - 1) / 60];
		} else if (gameTime >= 540) {
			[arialFont setColourFilterRed:1 green:1 blue:1 alpha:(float)(600 - gameTime) / 60];
		}
		
		if (chapter >= 3 && chapter <= 6) {
			if((gameTime % 24) == 0) {
				[self addNewUnit:kCharacter_worm position:CGPointMake(480, rand() % 310) direction:0];
			}
		} else if (chapter == 7) {
			if ((gameTime % 40) == 0) {
				[self addNewUnit:kCharacter_cell_pillar position:CGPointMake(480, 0) direction:0];
			}
		} else if (chapter == 8) {
			if ((gameTime % 200) == 1) {
				[self addNewUnit:kCharacter_hand_x position:CGPointMake(480, (rand() % 280) + 40) direction:0];
			}
		}
	} else if (chapter == 9) {
		if (gameTime <= 61) {
			[arialFont setColourFilterRed:1 green:1 blue:1 alpha:(float)(gameTime - 1) / 60];
		} else if (gameTime >= 540) {
			[arialFont setColourFilterRed:1 green:1 blue:1 alpha:(float)(600 - gameTime) / 60];
		}

		backGroundScroll += SCROLL_SPEED;	//뒷 배경 스크롤 멈춤
		
		if (gameTime == 1) {
			[self boomAll];
			[self addNewUnit:kCharacter_boss_fly position:CGPointMake(480, 160) direction:0];
			
			bossLv = 0;
			bossLife = 0;
			bossBomb = 0;
			bossCharacterId = kCharacter_boss_fly;
			[self calBossHpMax];
		}
		
		//적 모두 죽인 뒤
		if ([units count] <= 1) {
			[self nextChapter:0];
			bossLife = -1;
		}
	} else if (chapter == 10) {
		if (gameTime <= 61) {
			[arialFont setColourFilterRed:1 green:1 blue:1 alpha:(float)(gameTime - 1) / 60];
		}
		
		[self appearBoss:kCharacter_boss1 Lv:1 Life:1 LvMax:4];
	}
}

- (void)endingProcess {
	if (chapter == 0) {
		if (gameTime == 1) {
			[self calScore];
			if ([_sharedGameData score] > [[_sharedGameData gameData] hiscore]) {
				[hiscoreFont setColourFilterRed:1 green:0 blue:0 alpha:0];
			} else {
				[hiscoreFont setColourFilterRed:1 green:1 blue:1 alpha:0];
			}
			
			[arialFont setColourFilterRed:1 green:1 blue:1 alpha:0];
			
			if ([_sharedGameData score] > [[_sharedGameData gameData] hiscore]) {
				_sharedGameData.gameData.hiscore = [_sharedGameData score];
				[_sharedGameData saveData];
			}
		}
		[self nextChapter:60];
	} else if (chapter >= 1 && chapter <= 7) {
		[self nextChapter:300];
		if (gameTime <= 61) {
			[arialFont setColourFilterRed:1 green:1 blue:1 alpha:(float)(gameTime - 1) / 60];
		} else if (gameTime >= 240) {
			[arialFont setColourFilterRed:1 green:1 blue:1 alpha:(float)(300 - gameTime) / 60];
		}
	} else if (chapter == 8) {
		if (gameTime == 1) {
			[self addNewUnit:kCharacter_nice position:CGPointMake(210, 220) direction:0];
		} else if (gameTime == 30) {
			Unit* ani = [[Unit alloc] init];
			[ani SetUnit:kCharacter_handsome position:CGPointMake(480, 165) direction:0 gameScene:self];
			[anis addObject:(Unit*)ani];
			[ani release];
		} else if (gameTime == 340 || gameTime == 360 || gameTime == 380 || gameTime == 400) {
			[self boom:[[units objectAtIndex:HERO] centerPosition] direction:0 boomType:kBoom_8way_nosound];
		}
		
		[self nextChapter:420];
	} else if (chapter >= 9) {
		if (gameTime == 1) {
			[theEndImage setAlpha:0];
		} else if (gameTime <= 61) {
			if ([_sharedGameData score] > [[_sharedGameData gameData] hiscore]) {
				[hiscoreFont setColourFilterRed:1 green:0 blue:0 alpha:(float)(gameTime - 1) / 60];
			} else {
				[hiscoreFont setColourFilterRed:1 green:1 blue:1 alpha:(float)(gameTime - 1) / 60];
			}
			[theEndImage setAlpha: (float)(gameTime - 1) / 60];
		}
	}


}

//시간에 맞춰 적 발생 시키기
- (void)createEnemy {
	int stage = [_sharedGameData stage];
	
	gameTime++;
	
	if (stage == 0) {
		[self tutorialProcess];
	} else if (stage == 1) {
		[self stage1Process];
	} else if (stage == 2) {
		[self stage2Process];
	} else if (stage == 3) {
		[self stage3Process];
	} else if (stage == 4) {
		[self stage4Process];
	} else if (stage == 5) {
		[self stage5Process];
	} else if (stage == 6) {
		[self endingProcess];
	}
	
	//배경 스크롤
	backGroundScroll -= SCROLL_SPEED;
	
	if (backGroundScroll <= -160) {
		backGroundScroll = 0;
	}
}

- (void)calScore {
	//점수 계산
	if (hiscoreText != nil) {
		[hiscoreText release];
		hiscoreText = nil;
	}
	
	hiscoreText = [[NSString alloc] initWithFormat:@"Score : %d", [_sharedGameData score]];
	hiscoreX = 240 - ([hiscoreFont getWidthForString:hiscoreText] / 2);

	if ([_sharedGameData score] > [[_sharedGameData gameData] hiscore]) {
		[hiscoreFont setColourFilterRed:1 green:0 blue:0 alpha:1];
	} else {
		[hiscoreFont setColourFilterRed:1 green:1 blue:1 alpha:1];
	}
}

- (void)updateWithDelta:(GLfloat)aDelta {
	static int transitionCount = 0;

    switch (sceneState) {
		case kSceneState_Paused:
			break;
			
		case kSceneState_Running:
			if ([units count] == 0) {
				break;
			}

			//행동들 처리
			[self processAction:aDelta];
			
			//주인공 이동
			Unit *hero = [units objectAtIndex:HERO];
			if (joypadMoving == YES) {
				[hero Move:CGPointMake(-((aDelta * playerSpeed) * cosf(playerAngle)), -((aDelta * playerSpeed) * sinf(playerAngle)))];
			}

			//무기 발사
			[self shootingProcess:hero];
			
			//충돌 체크
			[self processCrush:hero];

			//죽은 유닛 제거
			[self removeDeadUnit];

			//적 발생
			[self createEnemy];
			
			//폭발 소리 조절
			if(boomSoundDelay > 0) {
				boomSoundDelay--;
			}
			
			if (solarSoundDelay > 0) {
				solarSoundDelay--;
			}
			
			//게임 오버
			if (hero.life == NO) {
				gameoverDelay++;
				if (gameoverDelay == 1) {
					//점수 계산
					[self calScore];
					if ([_sharedGameData stage] != 0 && [_sharedGameData score] > [[_sharedGameData gameData] hiscore]) {
						_sharedGameData.gameData.hiscore = [_sharedGameData score];
						[_sharedGameData saveData];
					}
				}else if (gameoverDelay >= 300) {
					sceneState = kSceneState_TransitionOut;
					nextSceneKey = @"menu";
					_sharedGameData.gameSetting = NO;
				}
			}
			
			break;
			
		case kSceneState_TransitionOut:
			sceneAlpha-= _sceneFadeSpeed * aDelta;
            [_sharedDirector setGlobalAlpha:sceneAlpha];
			if(sceneAlpha <= 0.0f) {
                // If the scene being transitioned to does not exist then transition
                // this scene back in
				if(![_sharedDirector setCurrentSceneToSceneWithKey:nextSceneKey])
                    sceneState = kSceneState_TransitionIn;
				
				[sceneSoundManager stopMusic];
			}
			break;
			
		case kSceneState_TransitionIn:
			if (transitionCount == 0) {
				if (_sharedGameData.gameSetting == NO) {
					[self initGame];
				} else {
					[self playMusic];
				}


				transitionCount = 1;
			}

			sceneAlpha += _sceneFadeSpeed * aDelta;
            [_sharedDirector setGlobalAlpha:sceneAlpha];
			if(sceneAlpha >= 1.0f) {
				sceneState = kSceneState_Running;
				transitionCount = 0;
			}
			break;
		default:
			break;
	}
    
}

//패드 위치에 따른 주인공 이동
- (void)setHeroMove:(CGPoint)touchLocation {
	if (CGRectContainsPoint(joypad, touchLocation)) {
		// Calculate the angle of the touch from the center of the joypad
		float dx = (float)joypadCenterx - (float)touchLocation.x;
		float dy = (float)joypadCentery - (float)touchLocation.y;
		
		float distance = sqrtf((joypadCenterx - touchLocation.x) * (joypadCenterx - touchLocation.x) + 
							   (joypadCentery - touchLocation.y) * (joypadCentery - touchLocation.y));
		
		// Calculate the angle of the players touch from the center of the joypad
		float touchAngle = atan2(dy, dx);
		playerAngle = touchAngle;
		playerSpeed = CLAMP(distance, 0, 50) * 4;
	}
}

//패드 위치에 따른 무기
- (void)setWeapon:(CGPoint)touchLocation {
	if (CGRectContainsPoint(weaponJoypad, touchLocation)) {
		// Calculate the angle of the touch from the center of the joypad
		float dx = (float)weaponJoypadCenterx - (float)touchLocation.x;
		float dy = (float)weaponJoypadCentery - (float)touchLocation.y;
		
		float distance = sqrtf((weaponJoypadCenterx - touchLocation.x) * (weaponJoypadCenterx - touchLocation.x) + 
							   (weaponJoypadCentery - touchLocation.y) * (weaponJoypadCentery - touchLocation.y));
		
		// Calculate the angle of the players touch from the center of the joypad
		float touchAngle = atan2(dy, dx);
		if(CLAMP(distance, 0, 50) >= 10) {
			if (touchAngle >= M_PI * 3 / 4 || touchAngle < -M_PI * 3 / 4 ) {
				shootingWeapon = kShooting_lightning;
			} else if (touchAngle >= -M_PI * 3 / 4 && touchAngle < -M_PI / 4) {
				shootingWeapon = kShooting_missile;
			} else if (touchAngle >= -M_PI / 4 && touchAngle < M_PI / 4) {
				shootingWeapon = kShooting_beam;
			} else if (touchAngle >= M_PI / 4 && touchAngle < M_PI * 3 / 4) {
				shootingWeapon = kShooting_solar;
			}
		}
	}

	//무기 딜레이 초기화
	if (oldShootingWeapon != shootingWeapon) {
		weaponDelay = 0;
	}
	
	oldShootingWeapon = shootingWeapon;
}

- (void)updateWithTouchLocationBegan:(NSSet*)touches withEvent:(UIEvent*)event view:(UIView*)aView {
	//엔딩 또는 주인공이 죽은 상태에서는 조작 불가능
	if ([[units objectAtIndex:HERO] life] == NO) {
		return;
	}
	
	NSSet *allTouches = [event allTouches];
    for (UITouch *touch in allTouches) {
        // Get the point where the player has touched the screen
        CGPoint touchLocation = [touch locationInView:aView];
		
		float tmp = touchLocation.y;
		touchLocation.y = touchLocation.x;
		touchLocation.x = tmp;

		NSLog(@"%f, %f", touchLocation.x, touchLocation.y);
		
		if ([_sharedGameData stage] >= 6) {
			if (chapter >= 9) {
				if ([_sharedGameData score] > [[_sharedGameData gameData] hiscore]) {
					_sharedGameData.gameData.hiscore = [_sharedGameData score];
					[_sharedGameData saveData];
				}

				sceneState = kSceneState_TransitionOut;
				nextSceneKey = @"menu";
				_sharedGameData.gameSetting = NO;
			}
			
			return;
		}
		
		
		if (sceneState == kSceneState_Running) {
			//이동
			if (CGRectContainsPoint(joypad, touchLocation) && !joypadMoving) {
				NSLog(@"joypad touched");
				joypadMoving = YES;
				joypadTouchHash = [touch hash];
				
				[self setHeroMove:touchLocation];
			}
			
			//무기
			if (CGRectContainsPoint(weaponJoypad, touchLocation) && !weaponJoypadMoving) {
				NSLog(@"weapon joypad touched");
				weaponJoypadMoving = YES;
				weaponJoypadTouchHash = [touch hash];
				
				[self setWeapon:touchLocation];
			}
			
			//정지
			CGRect pauseRect = CGRectMake(440, 280, 480, 320);
			if (pauseAble == YES && CGRectContainsPoint(pauseRect, touchLocation)) {
				sceneState = kSceneState_Paused;
				joypadMoving = NO;
				weaponJoypadMoving = NO;
				
				//점수 계산
				[self calScore];
			}
		} else if (sceneState == kSceneState_Paused) {
			CGRect resumeRect = CGRectMake(100, 70, 120, 40);
			CGRect backRect = CGRectMake(260, 70, 120, 40);
			if (CGRectContainsPoint(resumeRect, touchLocation)) {
				sceneState = kSceneState_Running;
			} else if (CGRectContainsPoint(backRect, touchLocation)) {
				sceneState = kSceneState_TransitionOut;
				nextSceneKey = @"menu";
			}
		}
    }
}


- (void)updateWithTouchLocationMoved:(NSSet*)touches withEvent:(UIEvent*)event view:(UIView*)aView {
    NSSet *allTouches = [event allTouches];
    
    for (UITouch *touch in allTouches) {
        
        if (sceneState == kSceneState_Running) {
			
            if ([touch hash] == joypadTouchHash && joypadMoving) {
                
				// Get the point where the player has touched the screen
                CGPoint touchLocation = [touch locationInView:aView];
				float tmp = touchLocation.y;
				touchLocation.y = touchLocation.x;
				touchLocation.x = tmp;
				 
				[self setHeroMove:touchLocation];
            }

			//무기 조작
            if ([touch hash] == weaponJoypadTouchHash && weaponJoypadMoving) {
                
				// Get the point where the player has touched the screen
                CGPoint touchLocation = [touch locationInView:aView];
				float tmp = touchLocation.y;
				touchLocation.y = touchLocation.x;
				touchLocation.x = tmp;
				
				[self setWeapon:touchLocation];
            }
			
		}
    }
}

- (void)updateWithTouchLocationEnded:(NSSet*)touches withEvent:(UIEvent*)event view:(UIView*)aView {
	for (UITouch *touch in touches) {
		// If the hash for the joypad has reported that its ended, then set the
		// state as necessary
		if ([touch hash] == joypadTouchHash) {
			joypadMoving = NO;
			joypadTouchHash = 0;
			playerAngle = 0;
			playerSpeed = 0;
		}

		if ([touch hash] == weaponJoypadTouchHash) {
			weaponJoypadMoving = NO;
			weaponJoypadTouchHash = 0;
			shootingWeapon = 0;
		}
	}
}

- (void)updateWithTouchLocationCanceled:(NSSet*)touches withEvent:(UIEvent*)event view:(UIView*)aView {
	for (UITouch *touch in touches) {
		// If the hash for the joypad has reported that its ended, then set the
		// state as necessary
		if ([touch hash] == joypadTouchHash) {
			joypadMoving = NO;
			joypadTouchHash = 0;
			playerAngle = 0;
			playerSpeed = 0;
		}
		
		if ([touch hash] == weaponJoypadTouchHash) {
			weaponJoypadMoving = NO;
			weaponJoypadTouchHash = 0;
			shootingWeapon = 0;
		}
	}
}

- (void)transitionToSceneWithKey:(NSString*)aKey {
}

- (void)render {
	int stage = [_sharedGameData stage];

	if ([units count] == 0) {
		return;
	}
	
	//배경
	for (float x = 0; x < 4; x++) {
		for (float y = 0; y < 2; y++) {
			CGPoint bg_position = CGPointMake(x * 160 + backGroundScroll, y * 160);
			
			if (stage == 0) {
				[backGroundImage[0] renderAtPoint:bg_position centerOfImage:NO];
			} else {
				[backGroundImage[stage - 1] renderAtPoint:bg_position centerOfImage:NO];
			}
		}
	}
	
	//유닛 그리기
	for(int i = 0; i < [units count]; i++) {
		Unit *unit = [units objectAtIndex:i];
		[unit Render];

		if (i == HERO && [[units objectAtIndex:HERO] life] == YES) {
			//상태
			Unit *hero = [units objectAtIndex:HERO];
			
			//붉은 해골
			if (hero.shock > 0) {
				[shockImage renderAtPoint:[hero getRenderPosition] centerOfImage:NO];
			}

			//금
			CGPoint damagePosition;
			damagePosition = [hero getRenderPosition];
			damagePosition.x += 12;
			damagePosition.y += 5;
			
			if (hero.hp < HERO_HP_MAX * 20 / 100) {
				[damageImage[4] renderAtPoint:damagePosition centerOfImage:NO];
			} else if (hero.hp < HERO_HP_MAX * 40 / 100) {
				[damageImage[3] renderAtPoint:damagePosition centerOfImage:NO];
			} else if (hero.hp < HERO_HP_MAX * 60 / 100) {
				[damageImage[2] renderAtPoint:damagePosition centerOfImage:NO];
			} else if (hero.hp < HERO_HP_MAX * 80 / 100) {
				[damageImage[1] renderAtPoint:damagePosition centerOfImage:NO];
			} else if (hero.hp < HERO_HP_MAX) {
				[damageImage[0] renderAtPoint:damagePosition centerOfImage:NO];
			}
		}
	}

	//무기 애니메이션 그리기
	for(int i = 0; i < [weapons count]; i++) {
		Unit *weapon = [weapons objectAtIndex:i];
		[weapon Render];
	}
	
	//효과 애니메이션 그리기
	for(int i = 0; i < [anis count]; i++) {
		Unit *ani = [anis objectAtIndex:i];
		[ani Render];
	}
	
	//스테이지 제목
	if (stage >= 1 && stage <= 5 && chapter == 0) {
		[stageTitleImage[stage - 1] renderAtPoint:CGPointMake(0, 60) centerOfImage:NO];
	}
	
	//튜토리얼
	if (stage == 0) {
		if (chapter == 1) {
			[arialFont drawStringAt:CGPointMake(20, 300) text:@"Slide the left panel/to move your ship."];
		} else if (chapter == 2) {
			[arialFont drawStringAt:CGPointMake(20, 300) text:@"Slide the right panel to fire."];
		} else if (chapter == 3) {
			[arialFont drawStringAt:CGPointMake(20, 300) text:@"If a bullet or a something/hit your head,/then you will be damaged."];
		} else if (chapter == 4) {
			[arialFont drawStringAt:CGPointMake(20, 300) text:@"You can guess/how much you damaged/by the cracks/on your head."];
		} else if (chapter == 5) {
			[arialFont drawStringAt:CGPointMake(20, 300) text:@"And your head will recover/as time goes by."];
		} else if (chapter == 6) {
			[arialFont drawStringAt:CGPointMake(20, 300) text:@"Thorn Missile(Top) is/a base weapon/for normal enemies."];
		} else if (chapter == 7) {
			[arialFont drawStringAt:CGPointMake(20, 300) text:@"Thunder Spark(Right) is/more powerful, less accurate,/and penetrates everything."];
		} else if (chapter == 8) {
			[arialFont drawStringAt:CGPointMake(20, 300) text:@"Wide Beam(Left) is/defensive weapon/which can reflect/small enemies and missiles."];
		} else if (chapter == 9) {
			[arialFont drawStringAt:CGPointMake(20, 300) text:@"Solar Beam(Bottom) can be used both/offensively or defensively./It has a short interval."];
		} else if (chapter == 10) {
			[arialFont drawStringAt:CGPointMake(20, 300) text:@"The boss plane can do power-up/by the items from you ship./And They can have/extra lives."];
		}
	}
	//스텝롤
	else if (stage == 6) {
		if (chapter == 1) {
			[arialFont drawStringAt:CGPointMake(240, 280) text:@"Director//My brain"];
		} else if (chapter == 2) {
			[arialFont drawStringAt:CGPointMake(240, 280) text:@"Programming//My hands"];
		} else if (chapter == 3) {
			[arialFont drawStringAt:CGPointMake(240, 280) text:@"Design//(Human)"];
		} else if (chapter == 4) {
			[arialFont drawStringAt:CGPointMake(240, 280) text:@"Music//ByunJa(Human)"];
		} else if (chapter == 5) {
			[arialFont drawStringAt:CGPointMake(240, 280) text:@"Sound Effect//My mouth/USB cap/Envelope"];
		} else if (chapter == 6) {
			[arialFont drawStringAt:CGPointMake(240, 280) text:@"Title Font//gartentika"];
		} else if (chapter == 7) {
			[arialFont drawStringAt:CGPointMake(240, 280) text:@"Special thanks//zzun(Human)/gguo(Human)/21B(Human)/YesMan(Human)"];
		} else if (chapter == 9) {
			[hiscoreFont drawStringAt:CGPointMake(240, 200) text:hiscoreText];
			[theEndImage renderAtPoint:CGPointMake(240, 60) centerOfImage:YES];
		}
	}
	
	//보스 상태
	bossHp = 0;
	if (bossLife >= 0) {
		for (int i = 0; i < [units count]; i++) {
			Unit *unit = [units objectAtIndex:i];
			if (unit.life == YES && unit._character.characterId == bossCharacterId) {
				bossHp += unit.hp;
			}
		}
	}
	
	if (bossLife >= 0 || bossHpBar > 0) {
		[hpBarImage renderAtPoint:CGPointMake(100, 20) centerOfImage:NO];

		int resultHp = bossHp * BOSS_HP_BAR_SIZE / bossHpMax;
		
		if (bossHpBar != resultHp) {
			int val = (resultHp - bossHpBar) / 10;
			if (val == 0) {
				if (resultHp > bossHpBar) {
					val = 1;
					
				} else if (resultHp < bossHpBar) {
					val = -1;
				}
			}
			
			bossHpBar += val;
		}
		
		if (bossHpBar > 0) {
			[hpImage renderAtPoint:CGPointMake(101, 21) centerOfImage:NO];
			[hpImage setScale:CGSizeMake((bossHpBar / BOSS_HP_BAR_SIZE), 1.0f)];
		}
		
		for (int i = 0; i < bossLife; i++) {
			[bossLifeImage renderAtPoint:CGPointMake(145 + (i * 22), 33) centerOfImage:NO];
		}
		
		for (int i = 0; i < bossBomb; i++) {
			[bossBombImage renderAtPoint:CGPointMake(101 + (i * 22), 5) centerOfImage:NO];
		}
	}
	
	//정지
	if (sceneState == kSceneState_Paused) {
		[pausedImage renderAtPoint:CGPointMake(240, 200) centerOfImage:YES];
		
		if ([_sharedGameData stage] > 0) {
			[hiscoreFont drawStringAt:CGPointMake(hiscoreX, 160) text:hiscoreText];
		}
		
		[pauseResumeImage renderAtPoint:CGPointMake(160, 90) centerOfImage:YES];
		[pauseBackImage renderAtPoint:CGPointMake(320, 90) centerOfImage:YES];
	}
	
	//게임 오버
	if ([[units objectAtIndex:HERO] life] == NO) {
		[gameoverImage renderAtPoint:CGPointMake(240, 200) centerOfImage:YES];
		[hiscoreFont drawStringAt:CGPointMake(hiscoreX, 160) text:hiscoreText];
	} else if (sceneState == kSceneState_Running && [_sharedGameData stage] < 6) {
		//정지 버튼
		
		if (pauseAble == YES) {
			[pauseButtonImage renderAtPoint:CGPointMake(440, 280) centerOfImage:NO];
		}

		if ([[units objectAtIndex:HERO] life] == YES) {
			//패드 그리기
			[moveControl renderAtPoint:CGPointZero centerOfImage:NO];
			[weaponControl renderAtPoint:CGPointMake(400, 0) centerOfImage:NO];
		}
	}
}

- (void)resignActive {
	joypadMoving = NO;
	weaponJoypadMoving = NO;
	if ([[units objectAtIndex:HERO] life] == YES) {
		sceneState = kSceneState_Paused;
	}
	[self calScore];
	
	[sceneSoundManager stopMusic];
}

- (void)becomeActive {
	[self playMusic];
}

@end
