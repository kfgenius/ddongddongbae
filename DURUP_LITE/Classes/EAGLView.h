//
//  EAGLView.h
//  OGLGame
//
//  Created by Michael Daley on 28/02/2009.
//  Copyright Michael Daley 2009. All rights reserved.
//


#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import "OGLGameController.h"

/*
This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
The view content is basically an EAGL surface you render your OpenGL scene into.
Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
*/
@interface EAGLView : UIView {
    
@private
    /* The pixel dimensions of the backbuffer */
    GLint backingWidth;
    GLint backingHeight;
    
    EAGLContext *context;
    
    /* OpenGL names for the renderbuffer and framebuffers used to render to this view */
    GLuint viewRenderbuffer, viewFramebuffer;
    
    /* OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist) */
    GLuint depthRenderbuffer;
	
	/* Time since the last frame was rendered */
	CFTimeInterval lastTime;
    float _FPSCounter;
	
	/* Game loop timer */
	NSTimer *gameLoopTimer;
	
	/* Game Controller */
	OGLGameController *gameController;
    
    /* Shared Director */
    Director *_sharedDirector;
}

- (void) mainGameLoop;
- (void) startGameTimer;
- (void) resignActive;
- (void) becomeActive;

@end
