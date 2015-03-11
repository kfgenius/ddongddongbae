//
//  GameState.h
//  OGLGame
//
//  Created by Michael Daley on 31/05/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AbstractScene.h"

@class Ghost;
@class Player;

// This class is the core game scene.  It is responsible for game rendering, logic, user
// input etc.
//
@interface GameScene : AbstractScene {

	/* Game specific items */
  @private    
    AngelCodeFont *font;
	float _tileMapPositionX, _tileMapPositionY;
	TiledMap *tileMap;
	int _tileX;
	int _tileY;
	int _leftOffsetInTiles;
	int _topOffsetInTiles;
	int _screenTilesWide;
	int _screenTilesHeight;
	int _tileWidth, _tileHeight;
    int topOffsetInTiles, leftOffsetInTiles;
    BOOL _blocked[500][500];
	ParticleEmitter *_explosionEmitter;
	GLfloat _tileMapScrollSpeed;
    BOOL _blendTileMap;
	
	// Game Controls
	MenuControl *_pauseButton;
	Image *_pauseOverlay;
    Image *_joyPad;
    Image *_firebutton;
    
    // Joypad locations
    uint _joyPadX, _joyPadY;
    CGRect _joyPadTouchPoints[8];
    int joyPadMovex, joyPadMovey;

	// Alien timer
	GLfloat _alienTimer;
    
    // Player
    Player *_player;
	
	// Entity array
	NSMutableArray *_entities;
    Ghost *ghost;
    
    UIAccelerationValue _accelerometer[3];

}

// Provide readonly access to the tilemap which is being used
@property (nonatomic, readonly) TiledMap *tileMap;

// Provides readonly access to information on which joypad region has been pressed
@property (nonatomic, readonly) int joyPadMovex;
@property (nonatomic, readonly) int joyPadMovey;

// Returns a boolean which identifies if the coordinates provided are on a blocked
// tile or not on the tilemap
- (BOOL)isBlocked:(float)x y:(float)y;

// Returns the current accelerometer value for the given axis.  The axis is the location
// within an array in which the value is stored.  0 = x, 1 = y, 2 = z
- (float)accelerometerValueForAxis:(uint)aAxis;

@end
