#include "Attribute.h"

CAttribute::CAttribute(char* pic_name, float radius, int cx, int cy)
{
	this->pic_name = pic_name;
	this->radius = radius;
	this->cx = cx;
	this->cy = cy;
}

CAttribute::~CAttribute()
{
}
