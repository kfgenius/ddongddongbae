//
//  GameData.h
//  Durup
//
//  Created by Dong-hung Park on 11. 2. 14..
//  Copyright 2011 Hondoom. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface DataObject : NSObject <NSCoding> {
	int lastStage;
	int hiscore;
}

@property (nonatomic) int lastStage;
@property (nonatomic) int hiscore;

@end


@interface GameData : NSObject {
	int stage;
	int score;
	BOOL gameSetting;
	DataObject *gameData;
}

@property (nonatomic, readonly) int stage;
@property (nonatomic) int score;
@property (nonatomic) BOOL gameSetting;
@property (nonatomic, retain) DataObject *gameData;

- (void) setStage:(int)aStage;
- (void) loadData;
- (void) saveData;

//Singleton
+ (GameData*)sharedGameData;

@end
