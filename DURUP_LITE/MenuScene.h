//
//  MenuState.h
//  OGLGame
//
//  Created by Michael Daley on 31/05/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AbstractScene.h"
#import "GameData.h"

@interface MenuScene : AbstractScene {
	Image *menuBackground;
	NSMutableArray *menuEntities;
	AngelCodeFont *hiscoreFont;
	NSString *hiscoreText;
	int hiscoreX;
	
	GameData* _sharedGameData;
}

@end
