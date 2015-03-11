//
//  BezierCurve.m
//  OGLGame
//
//  Created by Michael Daley on 19/05/2009.
//  Copyright 2009 Michael Daley. All rights reserved.
//
//
// This implementation of a bezier curve was originated by Kev Glass 
// at http://slick.cokeandcode.com/index.php


#import "BezierCurve.h"


@implementation BezierCurve

- (id)initCurveFrom:(Vector2f)theStartPoint controlPoint1:(Vector2f)theControlPoint1 controlPoint2:(Vector2f)theControlPoint2 endPoint:(Vector2f)theEndPoint segments:(GLuint)theSegments {
	self = [super init];
	if (self != nil) {
		startPoint = theStartPoint;
		controlPoint1 = theControlPoint1;
		controlPoint2 = theControlPoint2;
		endPoint = theEndPoint;
		segments = theSegments;
	}
	
	return self;
		
}


- (Vector2f)getPointAt:(GLfloat)t {
	GLfloat a = 1 - t;
	GLfloat b = t;
	
	GLfloat f1 = a * a * a;
	GLfloat f2 = 3 * a * a * b;
	GLfloat f3 = 3 * a * b * b;
	GLfloat f4 = b * b * b;
	
	GLfloat nx = (startPoint.x * f1) + (controlPoint1.x * f2) + (controlPoint2.x * f3) + (endPoint.x * f4);
	GLfloat ny = (startPoint.y * f1) + (controlPoint1.y * f2) + (controlPoint2.y * f3) + (endPoint.y * f4);

	return Vector2fMake(nx, ny);
}

@end
