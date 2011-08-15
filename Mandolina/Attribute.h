#pragma once

class CAttribute
{
public:
	CAttribute(char* pic_name, float radius, int cx, int cy);
	~CAttribute();

	char* pic_name;
	float radius;
	int cx;
	int cy;
};
