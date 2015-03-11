//
//  MenuState.m
//  OGLGame
//
//  Created by Michael Daley on 31/05/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import "MenuScene.h"

@interface MenuScene (Private)
- (void)initMenuItems;
@end

@implementation MenuScene

- (void)resetMenu {
	for (int i = 0; i < 11; i++) {
		[[menuEntities objectAtIndex:i] setVisible:NO];
	}

	[[menuEntities objectAtIndex:0] setVisible:YES];
	if (_sharedGameData.stage >= 1 && _sharedGameData.stage <= 5) {
		[[menuEntities objectAtIndex:1] setVisible:YES];
	}	
	
	if (hiscoreText != nil) {
		[hiscoreText release];
		hiscoreText = nil;
	}
	
	hiscoreText = [[NSString alloc] initWithFormat:@"Hi-Score : %d", [[_sharedGameData gameData] hiscore]];
	hiscoreX = 240 - ([hiscoreFont getWidthForString:hiscoreText] / 2);
}

- (void)difficultMenu {
	for (int i = 0; i < 11; i++) {
		[[menuEntities objectAtIndex:i] setVisible:NO];
	}
	
	for (int i = 2; i < 5; i++) {
		[[menuEntities objectAtIndex:i] setVisible:YES];
	}
}

- (void)stageMenu {
	for (int i = 0; i < 11; i++) {
		[[menuEntities objectAtIndex:i] setVisible:NO];
	}

	//Stage1
	[[menuEntities objectAtIndex:5] setVisible:YES];

	//Stage2
	if ([[_sharedGameData gameData] lastStage] >= 2) {
		[[menuEntities objectAtIndex:6] setVisible:YES];
	} else {
		[[menuEntities objectAtIndex:6] setVisible:NO];
	}

	//Stage3
	if ([[_sharedGameData gameData] lastStage] >= 3) {
		[[menuEntities objectAtIndex:7] setVisible:YES];
	} else {
		[[menuEntities objectAtIndex:7] setVisible:NO];
	}

	//Stage4
	if ([[_sharedGameData gameData] lastStage] >= 4) {
		[[menuEntities objectAtIndex:8] setVisible:YES];
	} else {
		[[menuEntities objectAtIndex:8] setVisible:NO];
	}

	//Stage5
	if ([[_sharedGameData gameData] lastStage] >= 5) {
		[[menuEntities objectAtIndex:9] setVisible:YES];
	} else {
		[[menuEntities objectAtIndex:9] setVisible:NO];
	}

	//Back
	[[menuEntities objectAtIndex:10] setVisible:YES];
}

- (id)init {
	
	if(self = [super init]) {
		_sharedDirector = [Director sharedDirector];
		_sharedResourceManager = [ResourceManager sharedResourceManager];
		_sharedSoundManager = [SoundManager sharedSoundManager];
        
        _sceneFadeSpeed = 1.0f;
        sceneAlpha = 0.0f;
        [_sharedDirector setGlobalAlpha:sceneAlpha];

		menuEntities = [[NSMutableArray alloc] init];
		menuBackground = [(Image*)[Image alloc] initWithImage:@"title.png"];
		[self setSceneState:kSceneState_TransitionIn];
		nextSceneKey = nil;
		
		_sharedGameData = [GameData sharedGameData];
		[_sharedGameData loadData];
		
		[self initMenuItems];
		[self resetMenu];
		
		hiscoreFont = [[AngelCodeFont alloc] initWithFontImageNamed:@"arial.png" controlFile:@"arial" scale:CGSizeMake(1, 1) filter:GL_LINEAR];
		hiscoreText = nil;
	}
	return self;
}

- (void)initMenuItems {
	MenuControl *menuEntity = [[MenuControl alloc] initWithImageNamed:@"play_button.png" location:Vector2fMake(240, 150) centerOfImage:YES type:kControlType_NewGame];
	[menuEntities addObject:menuEntity];
	[menuEntity release];

	menuEntity = [[MenuControl alloc] initWithImageNamed:@"continue.png" location:Vector2fMake(240, 100) centerOfImage:YES type:kControlType_Continue];
	[menuEntities addObject:menuEntity];
	[menuEntity release];
	
	menuEntity = [[MenuControl alloc] initWithImageNamed:@"tutorial.png" location:Vector2fMake(240, 190) centerOfImage:YES type:kControlType_Tutorial];
	[menuEntities addObject:menuEntity];
	[menuEntity release];
	
	menuEntity = [[MenuControl alloc] initWithImageNamed:@"normal.png" location:Vector2fMake(240, 140) centerOfImage:YES type:kControlType_Normal];
	[menuEntities addObject:menuEntity];
	[menuEntity release];	

	menuEntity = [[MenuControl alloc] initWithImageNamed:@"back_button.png" location:Vector2fMake(240, 80) centerOfImage:YES type:kControlType_Back_to_Main];
	[menuEntities addObject:menuEntity];
	[menuEntity release];
	
	menuEntity = [[MenuControl alloc] initWithImageNamed:@"stage1_button.png" location:Vector2fMake(80, 180) centerOfImage:YES type:kControlType_Stage1];
	[menuEntities addObject:menuEntity];
	[menuEntity release];

	menuEntity = [[MenuControl alloc] initWithImageNamed:@"stage2_button.png" location:Vector2fMake(240, 180) centerOfImage:YES type:kControlType_Stage2];
	[menuEntities addObject:menuEntity];
	[menuEntity release];

	menuEntity = [[MenuControl alloc] initWithImageNamed:@"stage3_button.png" location:Vector2fMake(400, 180) centerOfImage:YES type:kControlType_Stage3];
	[menuEntities addObject:menuEntity];
	[menuEntity release];

	menuEntity = [[MenuControl alloc] initWithImageNamed:@"stage4_button.png" location:Vector2fMake(120, 130) centerOfImage:YES type:kControlType_Stage4];
	[menuEntities addObject:menuEntity];
	[menuEntity release];

	menuEntity = [[MenuControl alloc] initWithImageNamed:@"stage5_button.png" location:Vector2fMake(360, 130) centerOfImage:YES type:kControlType_Stage5];
	[menuEntities addObject:menuEntity];
	[menuEntity release];
	
	menuEntity = [[MenuControl alloc] initWithImageNamed:@"back_button.png" location:Vector2fMake(240, 80) centerOfImage:YES type:kControlType_Back];
	[menuEntities addObject:menuEntity];
	[menuEntity release];
}

- (void)startStage:(int)aStage {
	_sharedGameData.score = 0;
	_sharedGameData.gameSetting = NO;
	[_sharedGameData setStage:aStage];
	sceneState = kSceneState_TransitionOut;
	nextSceneKey = @"settings";
}

- (void)updateWithDelta:(GLfloat)aDelta {
	
	switch (sceneState) {
		case kSceneState_Running:
			[menuEntities makeObjectsPerformSelector:@selector(updateWithDelta:) withObject:[NSNumber numberWithFloat:aDelta]];
			
			for (MenuControl *control in menuEntities) {
				if([control state] == kControl_Selected) {
					[control setState:kControl_Idle];
					switch ([control type]) {
						case kControlType_NewGame:
							[self difficultMenu];
							break;

						case kControlType_Normal:
							if([[_sharedGameData gameData] lastStage] <= 1 && [_sharedGameData gameSetting] == NO) {
								_sharedGameData.score = 0;
								_sharedGameData.gameSetting = NO;
								[_sharedGameData setStage:1];
								sceneState = kSceneState_TransitionOut;
								nextSceneKey = @"settings";
							} else {
								[self stageMenu];
							}

							break;
							
						case kControlType_Continue:
							sceneState = kSceneState_TransitionOut;
							nextSceneKey = @"settings";
							if ([_sharedGameData gameSetting] == NO) {
								_sharedGameData.score = 0;
							}
							
							break;

						case kControlType_Back_to_Main:
							[self resetMenu];
							break;
							
						case kControlType_Tutorial:
							[self startStage:0];
							break;
							
						case kControlType_Stage1:
							[self startStage:1];
							break;
							
						case kControlType_Stage2:
							[self startStage:2];
							break;
							
						case kControlType_Stage3:
							[self startStage:3];
							break;

						case kControlType_Stage4:
							[self startStage:4];
							break;
							
						case kControlType_Stage5:
							[self startStage:5];
							break;
							
						case kControlType_Back:
							[self difficultMenu];
							break;
							
						default:
							break;
					}
				}
			}
			break;
			
		case kSceneState_TransitionOut:
			sceneAlpha -= _sceneFadeSpeed * aDelta;
            [_sharedDirector setGlobalAlpha:sceneAlpha];
			if(sceneAlpha < 0)
                // If the scene being transitioned to does not exist then transition
                // this scene back in
				if(![_sharedDirector setCurrentSceneToSceneWithKey:nextSceneKey]) {
                    sceneState = kSceneState_TransitionIn;
				}
			break;
			
		case kSceneState_TransitionIn:
			if (sceneAlpha == 0.0f) {
				[menuEntities makeObjectsPerformSelector:@selector(reset)];
				[self resetMenu];
			}

			// I'm not using the delta value here as the large map being loaded causes
            // the first delta to be passed in to be very big which takes the alpha
            // to over 1.0 immediately, so I've got a fixed delta for the fade in.
            sceneAlpha += _sceneFadeSpeed * 0.02f;
            [_sharedDirector setGlobalAlpha:sceneAlpha];
			if(sceneAlpha >= 1.0f) {
				sceneState = kSceneState_Running;
			}
			break;
		default:
			break;
	}

}


- (void)setSceneState:(uint)theState {
	sceneState = theState;
	if(sceneState == kSceneState_TransitionOut)
		sceneAlpha = 1.0f;
	if(sceneState == kSceneState_TransitionIn)
		sceneAlpha = 0.0f;
}

- (void)updateWithTouchLocationBegan:(NSSet*)touches withEvent:(UIEvent*)event view:(UIView*)aView {
	if (sceneState != kSceneState_Running) {
		return;
	}
	
	for (MenuControl *control in menuEntities) {
		if([control state] == kControl_Scaling) {
			return;
		}
	}
	
    UITouch *touch = [[event touchesForView:aView] anyObject];
	CGPoint location;
	location = [touch locationInView:aView];
    
	float tmp = location.y;
	location.y = location.x;
	location.x = tmp;

	[menuEntities makeObjectsPerformSelector:@selector(updateWithLocation:) withObject:NSStringFromCGPoint(location)];
}


- (void)transitionToSceneWithKey:(NSString*)theKey {
	sceneState = kSceneState_TransitionOut;
	sceneAlpha = 1.0f;
}


- (void)render {
	[menuBackground renderAtPoint:CGPointMake(240, 160) centerOfImage:YES];
	[menuEntities makeObjectsPerformSelector:@selector(render)];
	[hiscoreFont drawStringAt:CGPointMake(hiscoreX, 40) text:hiscoreText];
}


@end
