//
//  main.m
//  OGLGame
//
//  Created by Michael Daley on 28/02/2009.
//  Copyright Michael Daley 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

int main(int argc, char *argv[]) {
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, @"OGLGameAppDelegate");
    [pool release];
    return retVal;
}
