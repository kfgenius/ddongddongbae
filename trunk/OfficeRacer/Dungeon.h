#pragma once

#include "GameProcess.h"

#define PLAYER_MAX	4

struct Car
{
	double x;
	double y;

	int direction;
	int speed;
};

struct Track
{
	double x1;
	double y1;
	double x2;
	double y2;
};

class CDungeon : public CGameProcess
{
private:
	Car car[PLAYER_MAX];
	Track track[16];

	JBrush car_brush;
	JBrush line_brush;
	JBrush course_brush;

public:
	void Control();
	void Process();
	void Render();

	CDungeon();
	~CDungeon();
};
