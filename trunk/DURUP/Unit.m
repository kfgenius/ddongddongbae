//
//  Unit.m
//  Tutorial1
//
//  Created by Dong-hung Park on 11. 1. 5..
//  Copyright 2011 Hondoom. All rights reserved.
//

#import "Unit.h"
#import "Character.h"
#import "SettingsScene.h"

#define SHOCK_DELAY 20
#define HERO		0

@implementation Unit

@synthesize _character;
@synthesize life, first, hp, shock, unitPosition, direction;
@synthesize characterId;
@synthesize reflect;
@synthesize master, slave;
@synthesize tempo;

- (BOOL)SetUnit:(CharacterID)aCharacterId position:(CGPoint)aPosition direction:(double)aDirection 
	  gameScene:(SettingsScene*)scene {
	
	life = YES;
	first = YES;

	unitPosition = aPosition;
	addPosition = CGPointZero;
	shock = 0;
	direction = aDirection;
	tempo = 0;
	reflect = NO;
	master = nil;
	slave = nil;

	//게임 장면
	_scene = scene;
	
	//캐릭터 속성 받기
	characterId = aCharacterId;
	NSMutableArray *characters = (NSMutableArray*)_scene.characters;
	for (int i = 0; i < [characters count]; i++) {
		if (characterId == [[characters objectAtIndex:i] characterId]) {
			_character = [characters objectAtIndex:i];
			imageId = i;
			break;
		}
	}
	
	hp = _character.hpMax;

	//보스
	if (characterId >= kCharacter_boss1 && characterId <= kCharacter_boss5) {
		boss = YES;
		_scene.bossHp = hp;
		_scene.bossHpMax = _character.hpMax;
	} else {
		boss = NO;
	}
	
	return YES;
}

- (void)die {
	life = NO;
	
	//주인이 있을 때
	if (master != nil) {
		//관계 끊기
		master.slave = nil;
		master = nil;
	}
	//종이 있을 때
	else if (slave != nil) {
		slave.life = NO;	//종도 같이 소멸
		
		//관계 끊기
		slave.master = nil;
		slave = nil;
	}
}

- (void)SetPosition:(CGPoint)aPosition {
	unitPosition = aPosition;
}

- (CGPoint)centerPosition {
	return CGPointMake(unitPosition.x + (_character.width / 2), unitPosition.y + (_character.height / 2));
}

- (void)inScreen {
	if (unitPosition.y < 0) {
		unitPosition.y = 0;
	} else if (unitPosition.y >= 320 - _character.height) {
		unitPosition.y = 320 - _character.height;
	}
	
	if (unitPosition.x < 0) {
		unitPosition.x = 0;
	} else if (unitPosition.x >= 480 - _character.width) {
		unitPosition.x = 480 - _character.width;
	}
}

- (void)Move:(CGPoint)movePosition {
	unitPosition.x += movePosition.x;
	unitPosition.y += movePosition.y;

	//주인공, 보스는 화면을 벗어날 수 없음
	if (characterId == kCharacter_skull || boss) {
		[self inScreen];
	}
}

- (void)Render {
	if(life == NO) {
		return;
	}

	//전함의 경우 몸통 그리기
	if (characterId == kCharacter_skull) {
		CGPoint heroBodyPosition;
		heroBodyPosition = CGPointMake((int)unitPosition.x - 152, (int)unitPosition.y - 54);
		[[(NSMutableArray*)_scene.characters objectAtIndex:kCharacter_skull_body] renderAtPoint:heroBodyPosition];
	}

	//그림 출력
	[[(NSMutableArray*)_scene.characters objectAtIndex:imageId] renderAtPoint:[self getRenderPosition]];
}

- (void)goAhead {
	//방향 없음
	if (direction == NO_ANGLE) {
		return;
	}
	
	//자신의 방향으로 속도만큼 이동
	float dx = -(_character.speed * cosf(direction));
	float dy = -(_character.speed * sinf(direction));

	[self Move:CGPointMake(dx, dy)];
}

- (void)addAngle:(double)angle {
	direction += M_PI * angle / 180;
	
	if (direction > M_PI) {
		direction -= M_PI * 2;
	} else if (direction <= -M_PI) {
		direction += M_PI * 2;
	}
}

- (void)boundFrame {
	if(unitPosition.x < 0) {
		unitPosition.x = 0;
		[self addAngle:(75 + rand() % 30)];
	} else if(unitPosition.x > 480 - _character.width)	{
		unitPosition.x = 480 - _character.width;
		[self addAngle:(75 + rand() % 30)];
	}
	
	if(unitPosition.y < 0) {
		unitPosition.y = 0;
		[self addAngle:(75 + rand() % 30)];
	} else if(unitPosition.y > 320 - _character.height)
	{
		unitPosition.y = 320 - _character.height;
		[self addAngle:(75 + rand() % 30)];
	}
}

- (float)shootToHero {
	CGPoint heroPosition = [[_scene.units objectAtIndex:HERO] unitPosition];
	float heroX = heroPosition.x + ([[_scene.characters objectAtIndex:HERO] width] / 2);
	float heroY = heroPosition.y + ([[_scene.characters objectAtIndex:HERO] height] / 2);
	
	float dx = unitPosition.x - heroX;
	float dy = unitPosition.y - heroY;
	
	return atan2(dy, dx);
}

- (void)chaseY {
	CGPoint heroPosition = [[_scene.units objectAtIndex:HERO] unitPosition];
	float heroY = heroPosition.y + ([[_scene.characters objectAtIndex:HERO] height] / 2);

	if(heroY < unitPosition.y - _character.speed) {
		[self Move:CGPointMake(0, -_character.speed)];
	} else if(heroY > unitPosition.y + _character.speed) {
		[self Move:CGPointMake(0, _character.speed)];
	}
}

- (void)chaseX {
	CGPoint heroPosition = [[_scene.units objectAtIndex:HERO] unitPosition];
	float heroX = heroPosition.x + [[_scene.characters objectAtIndex:HERO] width];
	
	if(heroX < unitPosition.x - 20) {
		[self Move:CGPointMake(-_character.speed, 0)];
	} else if(heroX > unitPosition.x - 10) {
		[self Move:CGPointMake(_character.speed, 0)];
	}
}

- (void)upDown {
	if (unitPosition.y <= 0 || unitPosition.y >= 320 - _character.height) {
		direction *= -1;
	}
}

- (void)wilyMove {
	int nearX = 0;
	int attackY;
	int weaponId = 0;
	
	//주인공의 공격이 있는지 검사
	for (int i = 0; i < [_scene.weapons count]; i++) {
		CGPoint weaponPosition = [[_scene.weapons objectAtIndex:i] unitPosition];
		if (nearX < weaponPosition.x && weaponPosition.x >= unitPosition.x - 150 && weaponPosition.x < unitPosition.x) {
			nearX = weaponPosition.x;
			attackY = weaponPosition.y;
			weaponId = [[_scene.weapons objectAtIndex:i] characterId];
		}
	}
	
	CGPoint heroPosition = [[_scene.units objectAtIndex:HERO] unitPosition];
	float heroY = heroPosition.y + ([[_scene.characters objectAtIndex:HERO] height] / 2);

	if (nearX == 0) {
		[self chaseY];
	} else {
		//무기와 주인공 위치
		int top, bottom;
		if (heroY < attackY) {
			top = heroY;
			bottom = attackY;
		} else {
			top = attackY;
			bottom = heroY;
		}
		
		int topSafe = top;
		int bottomSafe = 320 - bottom;
		
		if (topSafe >= bottomSafe && unitPosition.y > attackY - 40) {
			[self Move:CGPointMake(0, -_character.speed)];
		} else if (topSafe < bottomSafe && unitPosition.y < attackY + 40){
			[self Move:CGPointMake(0, _character.speed)];
		}
	}
	
	//뒤로 내빼기
	if (unitPosition.x < 400) {
		[self Move:CGPointMake(_character.speed, 0)];
	}
}

- (void)greedMove {
	int nearX = 0;
	int itemY;
	
	//아이템 찾기
	for (int i = 0; i < [_scene.units count]; i++) {
		//아이템이 아니면 무시
		if ([[[_scene.units objectAtIndex:i] _character] characterId] < kCharacter_power_item1
			|| [[[_scene.units objectAtIndex:i] _character] characterId] > kCharacter_power_item4) {
			continue;
		}
		
		CGPoint itemPosition = [[_scene.units objectAtIndex:i] unitPosition];
		if (nearX < itemPosition.x && itemPosition.x >= unitPosition.x - 50 && itemPosition.x < unitPosition.x) {
			nearX = itemPosition.x;
			itemY = itemPosition.y;
		}
	}
	
	if (nearX == 0) {
		[self goAhead];
		[self upDown];
	} else if (unitPosition.y < itemY) {
		[self Move:CGPointMake(0, _character.speed)];
	} else if (unitPosition.y > itemY + 4) {
		[self Move:CGPointMake(0, -_character.speed)];
	}
}

- (void)dashMove {
	[self chaseY];
	[self chaseX];
}

- (void)bossAttack {
	int shootSpeed = 9 - _character.speed;
	int shootTempo = tempo % (shootSpeed * 10);
	if((tempo % shootSpeed) == 0 && shootTempo < shootSpeed * 8)	{
		
		if (_scene.bossLv == 1) {
			[_scene shoot:kCharacter_boss_weapon1 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:0];
		} else if (_scene.bossLv == 2) {
			[_scene shoot:kCharacter_boss_weapon1 position:CGPointMake(unitPosition.x, unitPosition.y + 12) direction:0];
			[_scene shoot:kCharacter_boss_weapon1 position:CGPointMake(unitPosition.x, unitPosition.y + 6) direction:0];
		} else if (_scene.bossLv == 3) {
			[_scene shoot:kCharacter_boss_weapon1 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:-M_PI / 45];
			[_scene shoot:kCharacter_boss_weapon1 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:0];
			[_scene shoot:kCharacter_boss_weapon1 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:M_PI / 45];
		} else if (_scene.bossLv == 4) {
			[_scene shoot:kCharacter_boss_weapon1 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:-M_PI / 45];
			[_scene shoot:kCharacter_boss_weapon1 position:CGPointMake(unitPosition.x, unitPosition.y + 12) direction:0];
			[_scene shoot:kCharacter_boss_weapon1 position:CGPointMake(unitPosition.x, unitPosition.y + 6) direction:0];
			[_scene shoot:kCharacter_boss_weapon1 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:M_PI / 45];
		} else if (_scene.bossLv == 5) {
			[_scene shoot:kCharacter_boss_weapon2 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:0];
		} else if (_scene.bossLv == 6) {
			[_scene shoot:kCharacter_boss_weapon2 position:CGPointMake(unitPosition.x, unitPosition.y + 12) direction:0];
			[_scene shoot:kCharacter_boss_weapon2 position:CGPointMake(unitPosition.x, unitPosition.y + 6) direction:0];
		} else if (_scene.bossLv == 7) {
			[_scene shoot:kCharacter_boss_weapon2 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:-M_PI / 45];
			[_scene shoot:kCharacter_boss_weapon2 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:0];
			[_scene shoot:kCharacter_boss_weapon2 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:M_PI / 45];
		} else if (_scene.bossLv == 8) {
			[_scene shoot:kCharacter_boss_weapon2 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:-M_PI / 45];
			[_scene shoot:kCharacter_boss_weapon2 position:CGPointMake(unitPosition.x, unitPosition.y + 12) direction:0];
			[_scene shoot:kCharacter_boss_weapon2 position:CGPointMake(unitPosition.x, unitPosition.y + 6) direction:0];
			[_scene shoot:kCharacter_boss_weapon2 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:M_PI / 45];
		} else if (_scene.bossLv == 9) {
			[_scene shoot:kCharacter_boss_weapon3 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:0];
		} else if (_scene.bossLv == 10) {
			[_scene shoot:kCharacter_boss_weapon3 position:CGPointMake(unitPosition.x, unitPosition.y + 12) direction:0];
			[_scene shoot:kCharacter_boss_weapon3 position:CGPointMake(unitPosition.x, unitPosition.y + 6) direction:0];
		} else if (_scene.bossLv == 11) {
			[_scene shoot:kCharacter_boss_weapon3 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:-M_PI / 45];
			[_scene shoot:kCharacter_boss_weapon3 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:0];
			[_scene shoot:kCharacter_boss_weapon3 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:M_PI / 45];
		} else if (_scene.bossLv == 12) {
			[_scene shoot:kCharacter_boss_weapon3 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:-M_PI / 45];
			[_scene shoot:kCharacter_boss_weapon3 position:CGPointMake(unitPosition.x, unitPosition.y + 12) direction:0];
			[_scene shoot:kCharacter_boss_weapon3 position:CGPointMake(unitPosition.x, unitPosition.y + 6) direction:0];
			[_scene shoot:kCharacter_boss_weapon3 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:M_PI / 45];
		} else if (_scene.bossLv == 13) {
			[_scene shoot:kCharacter_boss_weapon4 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:0];
		} else if (_scene.bossLv == 14) {
			[_scene shoot:kCharacter_boss_weapon4 position:CGPointMake(unitPosition.x, unitPosition.y + 12) direction:0];
			[_scene shoot:kCharacter_boss_weapon4 position:CGPointMake(unitPosition.x, unitPosition.y + 6) direction:0];
		} else if (_scene.bossLv == 15) {
			[_scene shoot:kCharacter_boss_weapon4 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:-M_PI / 45];
			[_scene shoot:kCharacter_boss_weapon4 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:0];
			[_scene shoot:kCharacter_boss_weapon4 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:M_PI / 45];
		} else {
			[_scene shoot:kCharacter_boss_weapon4 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:-M_PI / 45];
			[_scene shoot:kCharacter_boss_weapon4 position:CGPointMake(unitPosition.x, unitPosition.y + 12) direction:0];
			[_scene shoot:kCharacter_boss_weapon4 position:CGPointMake(unitPosition.x, unitPosition.y + 6) direction:0];
			[_scene shoot:kCharacter_boss_weapon4 position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:M_PI / 45];
		}
	}
	
	if (shootTempo == 0) {
		//폭탄 사용
		if(_scene.bossBomb > 0) {
			[_scene explosion];
			_scene.bossBomb--;
		}
	}
}

- (BOOL)appear:(int)delayTime {
	if (first == YES) {
		if(tempo < delayTime)
		{
			[self goAhead];
			return YES;
		} else {
			first = NO;
			tempo = 0;
			return NO;
		}
	}
	
	return NO;
}

- (void)Action {
	if (_character.pattern == kPattern_skull_cell) {	//해골 세포 부활
		tempo++;
		
		if (tempo >= 1000) {
			hp = _character.hpMax;
			life = YES;
			[_scene shoot:kCharacter_cell_boom position:[self centerPosition] direction:NO_ANGLE];
			[_scene.sceneSoundManager playSoundWithKey:@"rebirth" gain:1.0f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
		}
	}
	
	if (life == NO) {
		return;
	}
	
	//패턴별 행동
	if (_character.pattern == kPattern_recover) {	//회복
		if (hp < _character.hpMax) {
			tempo++;
			
			if (tempo >= 180) {
				[self recover:1];
				tempo = 0;
			}
		}
	} else if (_character.pattern == kPattern_go_ahead || _character.pattern == kPattern_go_ahead_heavy) {	//직진
		[self goAhead];

		tempo++;
		
		//가스는 생명이 300
		if(_character.characterId == kCharacter_gas) {
			if (tempo >= 300) {
				[self die];
			}
		}
	} else if (_character.pattern == kPattern_boom) {	//효과 애니메이션
		[self goAhead];
		[self Damage:1 shock:NO];
	} else if (_character.pattern == kPattern_boom_pillar) {	//효과 애니메이션(기둥)
		unitPosition.y = rand() % 320;
		[self Damage:1 shock:NO];
	} else if (_character.pattern == kPattern_angry_eye) {	//성난 눈
		tempo++;

		if ([self appear:15] == NO) {
			//통통 튀면서 이동
			[self goAhead];
			unitPosition.y -= (tempo - 20) / 4;
			
			if(tempo >= 40)
			{
				CGPoint shootPosition = CGPointMake(unitPosition.x, unitPosition.y + (_character.height / 2));
				[_scene shoot:kCharacter_thunder_missile position:shootPosition direction:0];
				tempo=0;
			}
		}
	} else if (_character.pattern == kPattern_beauty_eye) {	//아름다운 눈
		tempo++;

		if ([self appear:50] == NO) {
			direction = NO_ANGLE;

			if (tempo >= 360) {
				tempo = 0;
			}
			
			float shootDirection = M_PI * tempo / 180;
			if (shootDirection > M_PI) {
				shootDirection -= M_PI * 2;
			}
			
			if((tempo % 9) == 0) {
				[_scene shoot:kCharacter_light_missile position:[self centerPosition] direction:shootDirection];
			}
		}
		
	} else if (_character.pattern == kPattern_boss_fly) {	//파리
		tempo++;

		if([self appear:25] == NO) {
			if(tempo >= 160) {
				tempo=0;
			}
			
			//움직임
			int act_no = tempo / 20;
			if(act_no == 0) {
				direction = -(M_PI / 4);
			} else if(act_no == 1) {
				direction = M_PI * 3 / 4;
			} else if(act_no==2) {
				direction = -(M_PI * 3 / 4);
			} else if(act_no==3) {
				direction = M_PI / 4;
			} else if(act_no==4) {
				direction = M_PI * 3 / 4;
			} else if(act_no==5) {
				direction = -(M_PI / 4);
			} else if(act_no==6) {
				direction = M_PI / 4;
			} else if(act_no==7) {
				direction = -(M_PI * 3 / 4);
			}
			
			[self goAhead];

			//미사일 발사
			if((tempo % 15) == 0) {
				[_scene shoot:kCharacter_black_missile position:[self centerPosition] direction:0];
			}
		}
	} else if (_character.pattern == kPattern_face) {	//얼굴
		tempo++;
		
		if ([self appear:_character.width] == NO) {
			if(tempo >= 3)
			{
				tempo = 0;
				CGPoint shootPosition = CGPointMake(unitPosition.x + 10, unitPosition.y + (_character.height / 2) - 20);
				double shootDirection = (M_PI / 180) * (rand() % 90) - (M_PI / 4);
				[_scene shoot:kCharacter_parasite position:shootPosition direction:shootDirection];
			}
		}
	} else if (_character.pattern == kPattern_ready_go) {	//멈춰 있다가 달려감
		tempo++;
		
		//등장
		if(first) {
			if(tempo >= 45) {
				first = NO;
			}
		}
		//이동
		else {
			[self goAhead];
		}		
	} else if (_character.pattern == kPattern_hand_x) {	//가위
		tempo++;
		
		//등장
		if([self appear:15] == NO) {
			direction = NO_ANGLE;

			if(tempo >= 20)	{
				tempo = 0;
				[_scene shoot:kCharacter_egg_missile position:CGPointMake(unitPosition.x, unitPosition.y + 9) direction:0];
			}
			
			if((rand() % 3) == 0) {
				[self chaseY];
			}
		}
		
	} else if (_character.pattern == kPattern_hand_b) {	//보
		tempo++;
		[self goAhead];
		
		//이동
		if(tempo < 30) {
			unitPosition.y -= 2;
		} else if(tempo < 40) {
			unitPosition.y -= 1;
		} else if(tempo < 50) {
			unitPosition.y += 1;
		} else if(tempo < 110) {
			unitPosition.y += 2;
		} else if(tempo < 120) {
			unitPosition.y += 1;
		} else if(tempo < 130) {
			unitPosition.y -= 1;
		} else if(tempo < 160) {
			unitPosition.y -= 2;
		} else {
			tempo=0;
		}
	} else if (_character.pattern == kPattern_cell) {	//세포
		[self goAhead];

		//벽에 부딪혀서 방향 전환
		[self boundFrame];
	} else if (_character.pattern == kPattern_king_cell) {	//왕 세포
		tempo++;
		
		//등장
		if(first == YES)
		{
			if(tempo < 30) {
				[self goAhead];
				[self goAhead];
			} else {
				direction = (M_PI / 180 * (rand() % 360)) - M_PI;
				first = NO;
			}
		}
		else
		{
			[self goAhead];
			[self boundFrame];
			
			if(tempo >= 360) {
				tempo=0;
			}
			
			if((tempo % 3) == 0) {
				[_scene shoot:kCharacter_egg_missile position:[self centerPosition] direction: (M_PI / 180 * tempo) - M_PI];
			}
		}
	} else if (_character.pattern == kPattern_skull_cell) {	//해골 세포
		tempo++;
		
		if(tempo >= 40)
		{
			[_scene shoot:kCharacter_mini_cell position:[self centerPosition] direction: (M_PI * ((rand() % 180) + 90) / 180) - M_PI];
			tempo=0;
		}
	} else if (_character.pattern == kPattern_mouth) {	//입
		tempo++;
		
		if([self appear:50] == NO) {
			direction = NO_ANGLE;

			if((tempo % 100) == 0) {
				[_scene shoot:kCharacter_gas position:[self centerPosition] direction:[self shootToHero]];
			}
		}
	} else if (_character.pattern == kPattern_boss1) {	//1탄 보스
		tempo++;
		
		//등장
		if(first == YES) {
			if(tempo < 8) {
				[self goAhead];
			} else {
				first = NO;
				direction = M_PI / 2;
			}
		}
		//공격
		else {
			[self bossAttack];
			[self goAhead];
			[self upDown];
		}
	} else if (_character.pattern == kPattern_boss2) {	//2탄 보스
		tempo++;
		
		//등장
		if(first == YES) {
			if(tempo < 8) {
				[self goAhead];
			} else {
				first = NO;
				direction = M_PI / 2;
			}
		}
		//공격
		else {
			if (tempo < 200) {
				[self chaseY];
			} else if (tempo < 400) {
				[self wilyMove];
			} else {
				tempo = 0;
			}
			
			[self bossAttack];
		}
	} else if (_character.pattern == kPattern_shield) {	//방패
		if (master != nil) {
			unitPosition = master.unitPosition;
			[self Damage:1 shock:NO];
			unitPosition.y -= 8;
			unitPosition.x -= 13;
		}
	} else if (_character.pattern == kPattern_jelly) {	//젤리
		if (master != nil) {
			//주인에게 붙어 있음
			unitPosition = [master centerPosition];
			[self moveCenter];
		}
	} else if (_character.pattern == kPattern_jelly_core2) {	//젤리 핵2
		tempo++;
		
		[self goAhead];

		if((tempo % 20) == 0) {
			[_scene shoot:kCharacter_jelly_missile position:[self centerPosition] direction:(M_PI / 180) * (rand() % 360) - M_PI];
		}
	} else if (_character.pattern == kPattern_jelly_missile) {	//젤리 미사일
		[self goAhead];
		
		direction += M_PI / 180;
		if (direction > M_PI) {
			direction -= M_PI * 2;
		}

		tempo++;
		if (tempo >= 1000) {
			[self die];
		}
	} else if (_character.pattern == kPattern_evil_eye) {	//악마의 눈
		tempo++;
		
		if ([self appear:_character.width - 20] == NO) {
			if (tempo >= 50) {
				tempo = 0;
				[_scene shoot:kCharacter_eye_laser position:CGPointMake(unitPosition.x - 20, unitPosition.y + (_character.height / 2)) direction:0];
			}
		}
	} else if (_character.pattern == kPattern_robot) {	//로봇
		tempo++;
		
		//등장
		if(first == YES) {
			if(tempo < 15) {
				[self goAhead];
			} else {
				first = NO;
				direction = M_PI / 2;
			}
		}
		//공격
		else {
			if ((tempo % 30) == 0) {
				[_scene shoot:kCharacter_chase_missile position:[self centerPosition] direction:0];
			}
			
			if (tempo >= 200) {
				[_scene shoot:kCharacter_bomb position:[self centerPosition] direction:0];
				tempo = 0;
			}
			
			[self wilyMove];
			[self inScreen];
		}
	} else if (_character.pattern == kPattern_chase_missile) {	//추적 미사일
		tempo++;
		
		[self Move:CGPointMake(-tempo / 15, 0)];
		CGPoint heroPosition = [[_scene.units objectAtIndex:HERO] unitPosition];
		if (heroPosition.y + 20 < unitPosition.y) {
			[self Move:CGPointMake(0, -tempo / 50)];
		} else if (heroPosition.y > unitPosition.y) {
			[self Move:CGPointMake(0, tempo / 50)];
		}

		//흔적 효과
		if ((tempo % 15) == 0) {
			[_scene shoot:(kCharacter_boom1 + (rand() % 4)) position:[self centerPosition] direction:NO_ANGLE];
		}
	} else if (_character.pattern == kPattern_boss4) {	//보스4
		tempo++;
		
		//등장
		if(first == YES) {
			if(tempo < 8) {
				[self goAhead];
			} else {
				first = NO;
				direction = M_PI / 2;
			}
		}
		//공격
		else {
			[self greedMove];
			[self bossAttack];
		}
	} else if (_character.pattern == kPattern_boss5) {	//보스5
		tempo++;
		
		//등장
		if(first == YES) {
			if(tempo < 8) {
				[self goAhead];
			} else {
				first = NO;
				direction = M_PI / 2;
			}
		}
		//공격
		else {
			if (tempo < 200) {
				[self upDown];
				[self goAhead];
			} else if (tempo < 400) {
				[self wilyMove];
			} else if (tempo < 450) {
				[self dashMove];
			} else if (tempo < 650) {
				[self greedMove];
			} else {
				tempo = 0;
			}

			[self bossAttack];
		}
	} else if (_character.pattern == kPattern_handsome) {
		CGPoint heroPosition = [[_scene.units objectAtIndex:HERO] unitPosition];
		if (heroPosition.x + 8 < unitPosition.x) {
			[self goAhead];
		}
	}
	
	//충격에 의한 흔들림
	if(shock > 0) {
		shock--;
		
		addPosition.x = (rand() % 3) - 1;
		addPosition.y = (rand() % 3) - 1;
	} else {
		addPosition = CGPointZero;
	}
	
	//사망 판정
	if (unitPosition.x < -120 || unitPosition.x >= 600 || unitPosition.y < -120 || unitPosition.y >= 440) {
		[self die];
	}
}

- (void)recover:(int)quantity {
	hp += quantity;
	if (hp > _character.hpMax) {
		hp = _character.hpMax;
	}
}

- (void)addScore {
	GameData* _shareGameData = [GameData sharedGameData];
	
	if (_character.characterId == kCharacter_worm) {
		_shareGameData.score += 10;
	} else if (_character.characterId == kCharacter_angry_eye) {
		_shareGameData.score += 20;
	} else if (_character.characterId == kCharacter_beauty_eye) {
		_shareGameData.score += 30;
	} else if (_character.characterId == kCharacter_boss_fly) {
		_shareGameData.score += 300;
	} else if (_character.characterId == kCharacter_face) {
		_shareGameData.score += 600;
	} else if (_character.characterId == kCharacter_punch) {
		_shareGameData.score += 20;
	} else if (_character.characterId == kCharacter_hand_x) {
		_shareGameData.score += 50;
	} else if (_character.characterId == kCharacter_hand_b) {
		_shareGameData.score += 20;
	} else if (_character.characterId == kCharacter_foot) {
		_shareGameData.score += 20;
	} else if (_character.characterId == kCharacter_cell) {
		_shareGameData.score += 20;
	} else if (_character.characterId == kCharacter_cell2) {
		_shareGameData.score += 40;
	} else if (_character.characterId == kCharacter_cell4) {
		_shareGameData.score += 80;
	} else if (_character.characterId == kCharacter_transparent_cell) {
		_shareGameData.score += 25;
	} else if (_character.characterId == kCharacter_cell_pillar) {
		_shareGameData.score += 100;
	} else if (_character.characterId == kCharacter_skull_cell) {
		_shareGameData.score += 300;
	} else if (_character.characterId == kCharacter_king_cell) {
		_shareGameData.score += 500;
	} else if (_character.characterId == kCharacter_mouth) {
		_shareGameData.score += 50;
	} else if (_character.characterId == kCharacter_boss1) {
		_shareGameData.score += 1000;
	} else if (_character.characterId == kCharacter_boss2) {
		_shareGameData.score += 2000;
	} else if (_character.characterId == kCharacter_boss3) {
		_shareGameData.score += 1000;
	} else if (_character.characterId == kCharacter_boss4) {
		_shareGameData.score += 4000;
	} else if (_character.characterId == kCharacter_boss5) {
		_shareGameData.score += 10000;
	} else if (_character.characterId == kCharacter_jelly_core) {
		_shareGameData.score += 30;
	} else if (_character.characterId == kCharacter_jelly_core2) {
		_shareGameData.score += 50;
	} else if (_character.characterId == kCharacter_evil_eye) {
		_shareGameData.score += 80;
	} else if (_character.characterId == kCharacter_robot) {
		_shareGameData.score += 1000;
	}
}

- (void)Damage:(int)damage shock:(BOOL)isShock{
	if (life == YES && damage > 0) {
		//주인공의 경우 대미지 상태에서 누적 대미지를 입지 않음
		if (_character.characterId == kCharacter_skull) {
			if (shock > 0) {
				return;
			} else {
				[_scene.sceneSoundManager playSoundWithKey:@"damage" gain:0.5f pitch:1.0f location:Vector2fZero shouldLoop:NO sourceID:-1];
			}
		}
		
		if (isShock) {
			shock = SHOCK_DELAY;
		}

		hp -= damage;
		
		//사망 판정
		if(hp <= 0) {
			GameData* _shareGameData = [GameData sharedGameData];
			if ([_shareGameData stage] == 0 && _character.characterId == kCharacter_skull) {
				hp = 1;
			} else {
				[self boom];
				
				//로봇이 죽으면 다 같이 사망
				if(_character.pattern == kPattern_robot) {
					[_scene boomAll];
				}
				
				[self addScore];
			}
		}
	}
}

- (void)boom {
	hp = 0;
	[self die];
	
	//특수 효과 애니메이션
	if (_character.boomType != kBoom_none) {
		[_scene boom:[self centerPosition] direction:direction boomType:_character.boomType];
	}
}

- (CGRect)getRect {
	return CGRectMake(unitPosition.x, unitPosition.y, _character.width, _character.height);
}

- (void)setDirection:(double)aDirection {
	direction = aDirection;
}

- (void)moveCenter {
	[self Move:CGPointMake(-(_character.width / 2), -(_character.height / 2))];
}

- (CGPoint)getRenderPosition {
	return CGPointMake((int)unitPosition.x + addPosition.x, (int)unitPosition.y + addPosition.y);
}

@end
