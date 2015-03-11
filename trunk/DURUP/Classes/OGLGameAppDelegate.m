//
//  Tutorial1AppDelegate.m
//  Tutorial1
//
//  Created by Michael Daley on 28/02/2009.
//  Copyright Michael Daley 2009. All rights reserved.
//

#import "OGLGameAppDelegate.h"
#import "EAGLView.h"

@implementation OGLGameAppDelegate


- (void)applicationDidFinishLaunching:(UIApplication *)application {

	/*[application setStatusBarOrientation: UIInterfaceOrientationLandscapeRight animated:NO];
	
	// Not using any NIB files anymore, we are creating the window and the
    // EAGLView manually.
    window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
	[window setUserInteractionEnabled:YES];
	[window setMultipleTouchEnabled:YES];

	//가로 화면 만들기
	CGRect windowRect = [[UIScreen mainScreen] bounds];
	NSLog(@"window's size : width - %f, height - %f", windowRect.size.width, windowRect.size.height);
	
	// 화면의 가로세로 크기를 바꾸어 CGRect를 생성한다. 배경과 구분을 두기위해 일부러 가로세로에서 10Point를 뺐다.
	CGRect viewRect = CGRectMake(0.0f, 0.0f, windowRect.size.height, windowRect.size.width);
	
	// 화면의 회전값을 구한다.
	CGAffineTransform transform = CGAffineTransformRotate(CGAffineTransformIdentity, M_PI * 90.0f / 180.0f);
	
	glView = [[EAGLView alloc] initWithFrame:viewRect];

	// 화면을 회전시키고 중앙 지점을 맞춘다.
	[glView setTransform:transform];
	[glView setCenter:window.center];
	
	//glView.frame = window.frame;

	//화면 회전
	//glView.bounds = CGRectMake(0, 0, 480, 320);
	//glView.center = CGPointMake(240, 160); 

	//transform = glView.transform;
	//transform = CGAffineTransformRotate(transform, (M_PI / 2.0f)); 
	//glView.transform = transform;
	
    // Add the glView to the window which has been defined
	[window addSubview:glView];
	[window makeKeyAndVisible];
    
	// Since OS 3.0, just calling [glView mainGameLoop] did not work, you just got a black screen.
    // It appears that others have had the same problem and to fix it you need to use the 
    // performSelectorOnMainThread call below.
    [glView performSelectorOnMainThread:@selector(mainGameLoop) withObject:nil waitUntilDone:NO]; */
	
    
	// Not using any NIB files anymore, we are creating the window and the
    // EAGLView manually.
    window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
	[window setUserInteractionEnabled:YES];
	[window setMultipleTouchEnabled:YES];
	
	glView = [[EAGLView alloc] initWithFrame:[UIScreen mainScreen].bounds];
	
    // Add the glView to the window which has been defined
	[window addSubview:glView];
	[window makeKeyAndVisible];
    
	// Since OS 3.0, just calling [glView mainGameLoop] did not work, you just got a black screen.
    // It appears that others have had the same problem and to fix it you need to use the 
    // performSelectorOnMainThread call below.
    [glView performSelectorOnMainThread:@selector(mainGameLoop) withObject:nil waitUntilDone:NO];
}


- (void)applicationWillResignActive:(UIApplication *)application {
	[glView resignActive];
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
	[glView becomeActive];
}


- (void)dealloc {
	[window release];
	[glView release];
	[super dealloc];
}

@end
