#pragma once

#include "GameProcess.h"

#define PLAYER_MAX	4
#define TRACK_MAX	12

struct Car
{
	double x;
	double y;

	int handle_x;
	int handle_y;

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
	int control_handle_id;
	int boost_id;
	bool boost_on;

	double old_mouse_x, old_mouse_y;

	JBrush car_brush;
	JBrush line_brush;
	JBrush course_brush;

	void MoveCar(int id);
	void DrawHandle(int id);

public:
	void Control();
	void Process();
	void Render();

	CDungeon();
	~CDungeon();
};
