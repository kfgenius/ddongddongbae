//
//  GameState.m
//  OGLGame
//
//  Created by Michael Daley on 31/05/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//

#import "GameScene.h"
#import "Player.h"
#import "AlienTailWagger.h"
#import "Ghost.h"

#define FPS 0

#pragma mark -
#pragma mark Private interface

@interface GameScene (Private)
// Initialize the sound needed for this scene
- (void)initSound;
// Initialize the joypad i.e. the CGRects which will track movement on the pad
- (void)initJoyPad;
// Check to see if a touch was on one of the touchpad points
- (BOOL)checkJoyPadWithLocation:(CGPoint)aLocation;
@end

#pragma mark -
#pragma mark Public implementation

@implementation GameScene

@synthesize tileMap;
@synthesize joyPadMovex;
@synthesize joyPadMovey;

- (id)init {
	
	if(self == [super init]) {
		
        // Grab an instance of our singleton classes
		_sharedDirector = [Director sharedDirector];
		_sharedResourceManager = [ResourceManager sharedResourceManager];
		_sharedSoundManager = [SoundManager sharedSoundManager];
		
		// Grab the bounds of the screen
		_screenBounds = [[UIScreen mainScreen] bounds];
		
        _sceneFadeSpeed = 1.0f;
        
		// Init Sound
		[self initSound];
		
		// Init controls
		_pauseButton = [[Image alloc] initWithImage:@"pause.png" filter:GL_LINEAR];
		_pauseOverlay = [[Image alloc] initWithImage:@"paused.png" filter:GL_LINEAR];
		
		// Init font
        font = [[AngelCodeFont alloc] initWithFontImageNamed:@"GillSans16.png" controlFile:@"GillSans16" scale:CGSizeMake(1.0f, 1.0f) filter:GL_LINEAR];
        
        // Init the tilemap
		tileMap = [[TiledMap alloc] initWithTiledFile:@"house1" fileExtension:@"tmx"];
		_tileWidth = [tileMap tileWidth];
		_tileHeight = [tileMap tileHeight];
		_screenTilesWide = _screenBounds.size.width / _tileWidth;
		_screenTilesHeight = _screenBounds.size.height / _tileHeight;
        topOffsetInTiles = _screenTilesHeight / 2;
        leftOffsetInTiles = _screenTilesWide / 2;
		_tileMapScrollSpeed = 0.5;
		_tileMapPositionY = 430;
		_tileY = 0;
        _blendTileMap = YES;
        
        // Build a map of blocked locations within the tilemap.  This information is held on layer 1
        // of the tilemap which has been read in
        for(int layer=0; layer < 2; layer++) {
            for(int yy=0; yy < tileMap.mapHeight; yy++) {
                for(int xx=0; xx < tileMap.mapWidth; xx++) {
                    int _globalTileID = [[[tileMap layers] objectAtIndex:layer] getGlobalTileIDAtX:xx y:yy];
                    NSString *_value = [tileMap getTilePropertyForGlobalTileID:_globalTileID key:@"blocked" defaultValue:@"false"];
                    if([_value isEqualToString:@"true"]) {
                        _blocked[xx][yy] = YES;
                    }
                }
            }
        }
        
        // Player setup
        _player = [[Player alloc] initWithTileLocation:Vector2fMake(160, 240)];
        _player.tileX = 7;
        _player.tileY = 8;
		
		// Init particle emitter
		_explosionEmitter = [[ParticleEmitter alloc] initParticleEmitterWithImageNamed:@"texture.png"
															   position:Vector2fMake(240, 160)
												 sourcePositionVariance:Vector2fMake(5, 5)
																  speed:0.25f
														  speedVariance:0.125f
													   particleLifeSpan:50.0f	
											   particleLifespanVariance:1.0f
																  angle:0.0f
														  angleVariance:360
																gravity:Vector2fMake(0.0f, 0.0f)
															 startColor:Color4fMake(1.0f, 0.5f, 0.0f, 1.0f)
													 startColorVariance:Color4fMake(0.0f, 0.0f, 0.0f, 0.5f)
															finishColor:Color4fMake(0.2f, 0.5f, 0.0f, 0.0f)  
													finishColorVariance:Color4fMake(0.2f, 0.0f, 0.0f, 0.0f)
														   maxParticles:1000
														   particleSize:16
												   particleSizeVariance:8
															   duration:0
														  blendAdditive:YES];
		
		_entities = [[NSMutableArray alloc] init];
        
        for(int i=0; i < 15; i++) {
            ghost = [[Ghost alloc] initWithTileLocation:Vector2fMake(5, 8)];
            [_entities addObject:ghost];
            [ghost release];
        }

		_pauseButton = [[MenuControl alloc] initWithImageNamed:@"pause.png" location:Vector2fMake(300, 240) centerOfImage:YES type:kControlType_PauseGame];
        
        _joyPad = [[Image alloc] initWithImage:@"joypad.png"];
        [_joyPad setAlpha:0.5f];
        _joyPadX = 55;
        _joyPadY = 55;
        
        _firebutton = [[Image alloc] initWithImage:@"fire-button-1.png"];
        [_firebutton setAlpha:0.5f];
        [self initJoyPad];
    }
	
	return self;
}

#pragma mark -
#pragma mark Update scene logic

- (void)updateWithDelta:(GLfloat)theDelta {

	switch (sceneState) {
		case kSceneState_Running:
            
            // Update the player
            [_player update:theDelta];

			// Update the particle emitter
			[_explosionEmitter update:theDelta];
			
			// Update the alien
			for(AbstractEntity *entity in _entities) {
				[entity update:theDelta];
			}
			
			// Update ingame controls
			[_pauseButton updateWithDelta:[NSNumber numberWithFloat:theDelta]];
			break;
			
		case kSceneState_TransitionIn:
			sceneAlpha += _sceneFadeSpeed * theDelta;
			[_sharedDirector setGlobalAlpha:sceneAlpha];
			if(sceneAlpha >= 1.0f) {
				sceneAlpha = 1.0f;
				sceneState = kSceneState_Running;
                _blendTileMap = NO;
			}
		default:
			break;
	}

}

- (BOOL)isBlocked:(float)x y:(float)y {
    return _blocked[(int)x][(int)y];
}

#pragma mark -
#pragma mark Touch events

- (void)updateWithTouchLocationBegan:(NSSet*)touches withEvent:(UIEvent*)event view:(UIView*)aView {
	
    UITouch *touch = [[event touchesForView:aView] anyObject];
    
	CGPoint _location;
	_location = [touch locationInView:aView];
    
	// Flip the y location ready to check it against OpenGL coordinates
	_location.y = 480 - _location.y;

	switch (sceneState) {
		case kSceneState_Running:
            [self checkJoyPadWithLocation:_location];
			break;
		default:
			break;
	}
}


- (void)updateWithTouchLocationMoved:(NSSet*)touches withEvent:(UIEvent*)event view:(UIView*)aView {
    UITouch *touch = [[event touchesForView:aView] anyObject];
	CGPoint _location;
	_location = [touch locationInView:aView];
    
	// Flip the y location ready to check it against OpenGL coordinates
	_location.y = 480 - _location.y;
    

    // Loop through the all the current touches.  If a touch    
	switch (sceneState) {
		case kSceneState_Running:
            //[self checkJoyPadWithLocation:_location];
			break;
		default:
			break;
	}
}

#pragma mark -
#pragma mark Accelerometer

- (void)updateWithAccelerometer:(UIAcceleration*)aAcceleration {
    // Populate the accelerometer array with the filtered accelerometer info
	_accelerometer[0] = aAcceleration.x * 0.1f + _accelerometer[0] * (1.0 - 0.1f);
	_accelerometer[1] = aAcceleration.y * 0.1f + _accelerometer[1] * (1.0 - 0.1f);
	_accelerometer[2] = aAcceleration.z * 0.1f + _accelerometer[2] * (1.0 - 0.1f);
}

- (float)accelerometerValueForAxis:(uint)aAxis {
    return _accelerometer[aAxis];
}

#pragma mark -
#pragma mark Transition

- (void)transitionToSceneWithKey:(NSString*)theKey {
	sceneState = kSceneState_TransitionOut;
}

#pragma mark -
#pragma mark Render scene

- (void)render {

    // Calculate the players offset from the edge of the current tile.  Working this out
    // will allow us to move around the tilemap smoothly.
    int playerTileX = (int) _player.tileX;
    int playerTileY = (int) _player.tileY;
    int playerTileOffsetX = (int)((playerTileX - _player.tileX) * tileMap.tileWidth);
    int playerTileOffsetY = (int)((playerTileY - _player.tileY) * tileMap.tileHeight);
	
    // Render the tilemap.  We minus 1 from the mapX and mapY values so that the map is
    // drawn off the top and left of the scren.  This stops us seeing when new tiles are
    // added to the map.  The width and height is also increased to make sure we do not
    // see tiles which are removed from the rendered scene.  UseBlending is set to NO
    // to improve performance as the tilemap we are using is opaque.
    CGPoint point = CGPointMake(playerTileOffsetX - 50, 430 - playerTileOffsetY);
	[tileMap renderAtPoint:point 
                       mapX:playerTileX - leftOffsetInTiles - 1
                       mapY:playerTileY - topOffsetInTiles - 1
                      width:_screenTilesWide + 3
                     height:_screenTilesHeight + 3
                      layer:0
                useBlending:_blendTileMap];

	// Render the objects layer of the tilemap which will need to be blended as these objects
    // have alpha.
    [tileMap renderAtPoint:point 
                      mapX:playerTileX - leftOffsetInTiles - 1
                      mapY:playerTileY - topOffsetInTiles - 1
                     width:_screenTilesWide + 3
                    height:_screenTilesHeight + 3
                     layer:1
               useBlending:YES];
    
    // Render the rest of the scene.  We translate our location so that everything is drawn in relation
    // to where the player is in the tilemap.
    glPushMatrix();
    
    // Translate the matrix so that the player is drawn in the middle of the screen
    glTranslatef(160 - (int)((_player.tileX * tileMap.tileWidth) + 10.0f), 240 + (int)((_player.tileY * tileMap.tileHeight) - 10.0f), 0.0f);
   
	// Render the particle emitter and baddies.  This is done after the translate so that
    // they are rendered relative to the tilemap
	[_explosionEmitter renderParticles];
    for(AbstractEntity *entity in _entities) {
        [entity render];
    }
    
    // Render the player
    [_player render];   
    
    // Pop the matrix back off the stack which will undo the glTranslate we did above
    glPopMatrix();

    if(FPS)
        [font drawStringAt:CGPointMake(5, 450) text:[NSString stringWithFormat:@"FPS: %1.0f", [_sharedDirector framesPerSecond]]];
    
    
//    [_joyPad renderAtPoint:CGPointMake(_joyPadX, _joyPadY) centerOfImage:YES];
//    [_firebutton renderAtPoint:CGPointMake(285, 35) centerOfImage:YES];
  
}

@end


@implementation GameScene (Private)

#pragma mark -
#pragma mark Initialize joypad

- (void)initJoyPad {
    
    // Top left
    CGRect _touchArea0 = CGRectMake((_joyPadX - 50), (_joyPadY + 50), 30, 20);
    _joyPadTouchPoints[0] = _touchArea0;
    
    // Top
    CGRect _touchArea1 = CGRectMake((_joyPadX - 10), (_joyPadY + 30), 30, 30);
    _joyPadTouchPoints[1] = _touchArea1;

    // Top right
    CGRect _touchArea2 = CGRectMake((_joyPadX + 30), (_joyPadY + 30), 30, 30);
    _joyPadTouchPoints[2] = _touchArea2;

    // Right
    CGRect _touchArea3 = CGRectMake((_joyPadX + 30), (_joyPadY - 10), 30, 30);
    _joyPadTouchPoints[3] = _touchArea3;

    // Bottom right
    CGRect _touchArea4 = CGRectMake((_joyPadX + 30), (_joyPadY - 50), 30, 30);
    _joyPadTouchPoints[4] = _touchArea4;

    // Bottom
    CGRect _touchArea5 = CGRectMake((_joyPadX - 10), (_joyPadY - 50), 30, 30);
    _joyPadTouchPoints[5] = _touchArea5;

    // Bottom left
    CGRect _touchArea6 = CGRectMake((_joyPadX - 50), (_joyPadY - 50), 30, 30);
    _joyPadTouchPoints[6] = _touchArea6;

    // Left
    CGRect _touchArea7 = CGRectMake((_joyPadX - 50), (_joyPadY - 10), 30, 30);
    _joyPadTouchPoints[7] = _touchArea7;

}

#pragma mark -
#pragma mark Initialize sound

- (void)initSound {
	
    [_sharedSoundManager setListenerPosition:Vector2fMake(160, 0)];
	
    // Initialize the sound effects
	[_sharedSoundManager loadSoundWithKey:@"laser" fileName:@"mylaser" fileExt:@"wav"];
	[_sharedSoundManager loadSoundWithKey:@"explosion" fileName:@"explosion" fileExt:@"caf"];
	
	// Initialize the background music
	//[_sharedSoundManager loadBackgroundMusicWithKey:@"music" fileName:@"music" fileExt:@"caf"];
	[_sharedSoundManager setMusicVolume:0.75f];
	//[sharedSoundManager playMusicWithKey:@"music"  timesToRepeat:-1];
	
	// Set the master sound FX volume
	[_sharedSoundManager setFXVolume:1.0f];
}

- (BOOL)checkJoyPadWithLocation:(CGPoint)aLocation {
    
    //NSLog(@"%@", NSStringFromCGPoint(aLocation));
    
    if(CGRectContainsPoint(_joyPadTouchPoints[0], aLocation)) {
        NSLog(@"UP LEFT");
        joyPadMovex = -1;
        joyPadMovey = 1;
        return YES;
    }
    
    if(CGRectContainsPoint(_joyPadTouchPoints[1], aLocation)) {
        NSLog(@"UP");
        joyPadMovex = 0;
        joyPadMovey = 1;
        return YES;
    }
    
    if(CGRectContainsPoint(_joyPadTouchPoints[2], aLocation)) {
        NSLog(@"UP RIGHT");
        joyPadMovex = 1;
        joyPadMovey = 1;
        return YES;
    }
    
    if(CGRectContainsPoint(_joyPadTouchPoints[3], aLocation)) {
        NSLog(@"RIGHT");
        joyPadMovex = 1;
        joyPadMovey = 0;
        return YES;
    }
    
    if(CGRectContainsPoint(_joyPadTouchPoints[4], aLocation)) {
        NSLog(@"DOWN RIGHT");
        joyPadMovex = 1;
        joyPadMovey = -1;
        return YES;
    }
    
    if(CGRectContainsPoint(_joyPadTouchPoints[5], aLocation)) {
        NSLog(@"DOWN");
        joyPadMovex = 0;
        joyPadMovey = -1;
        return YES;
    }
    
    if(CGRectContainsPoint(_joyPadTouchPoints[6], aLocation)) {
        NSLog(@"DOWN LEFT");
        joyPadMovex = -1;
        joyPadMovey = -1;
        return YES;
    }
    
    if(CGRectContainsPoint(_joyPadTouchPoints[7], aLocation)) {
        NSLog(@"LEFT");
        joyPadMovex = -1;
        joyPadMovey = 0;
        return YES;
    }
    
    return NO;
}

@end

