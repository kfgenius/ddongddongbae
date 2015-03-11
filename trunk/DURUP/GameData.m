//
//  GameData.m
//  Durup
//
//  Created by Dong-hung Park on 11. 2. 14..
//  Copyright 2011 Hondoom. All rights reserved.
//

#import "GameData.h"

//데이터 오브젝트

@implementation DataObject

@synthesize lastStage, hiscore;

- (id)initWithCoder:(NSCoder *)aDecoder {
	if (self = [super init]) {
		self.lastStage = [aDecoder decodeIntForKey:@"lastStage"];
		self.hiscore = [aDecoder decodeIntForKey:@"hiscore"];
	}
	
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder {
	[aCoder encodeInt:lastStage forKey:@"lastStage"];
	[aCoder encodeInt:hiscore forKey:@"hiscore"];
}

@end

//게임 데이터

@implementation GameData

@synthesize stage, score, gameSetting, gameData;

static GameData *_sharedGameData = nil;

+ (GameData*)sharedGameData {
	@synchronized([GameData class]) {
		if (!_sharedGameData) {
			[[self alloc] init];
		}
		
		return _sharedGameData;
	}
	
	return nil;
}

+ (id) alloc {
	@synchronized([GameData class]) {
		_sharedGameData = [super alloc];
		return _sharedGameData;
	}
	
	return nil;
}

- (void)setStage:(int)aStage {
	stage = aStage;
	
	if (self.gameData != nil && self.gameData.lastStage < aStage) {
		self.gameData.lastStage = aStage;
		[self saveData];
	}
}

- (void)loadData {
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *documentPath = [documentsDirectory stringByAppendingPathComponent:@"gameState.dat"];

	// Check to see if there is a gameState.dat file.  If there is then load the contents
    NSMutableData *tmpGameData;
    NSKeyedUnarchiver *decoder;
    
    // Check to see if the ghosts.dat file exists and if so load the contents into the
    // entities array
    tmpGameData = [NSData dataWithContentsOfFile:documentPath];
	
	if (tmpGameData) {
		decoder = [[NSKeyedUnarchiver alloc] initForReadingWithData:tmpGameData];
		
		// Set the local instance of myObject to the object held in the gameState file with the key "myObject"
		self.gameData = [decoder decodeObjectForKey:@"data"];
		
		// We have finishd decoding the objects and retained them so we can now release the
		// decoder object
		[decoder release];
	} else {
		self.gameData = [DataObject alloc];
		self.gameData.lastStage = 1;
		self.gameData.hiscore = 0;
		
		[self saveData];
	}
}

- (void)saveData {
	if (self.gameData == nil) {
		return;
	}
	
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *filepath = [documentsDirectory stringByAppendingPathComponent:@"gameState.dat"];
	
	// Set up the encoder and storage for the game state data
	NSMutableData *tmpGameData;
	NSKeyedArchiver *encoder;
	tmpGameData = [NSMutableData data];
	encoder = [[NSKeyedArchiver alloc] initForWritingWithMutableData:tmpGameData];
	
	// Archive our object
	[encoder encodeObject:self.gameData forKey:@"data"];
	
	// Finish encoding and write to the gameState.dat file
	[encoder finishEncoding];
	[tmpGameData writeToFile:filepath atomically:YES];
	[encoder release];
}


@end
