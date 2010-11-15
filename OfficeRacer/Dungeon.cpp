#include <math.h>

#include "Dungeon.h"

#define CAR_RADIUS			10
#define BASE_DIRECTION_X	CAR_RADIUS
#define BASE_DIRECTION_Y	0

CDungeon::CDungeon(void)
{
	//브러쉬 생성
	car_brush = jdd->CreateBrush(JColor(255, 0, 0), 1);
	line_brush = jdd->CreateBrush(JColor(0, 255, 0), 1);
	course_brush = jdd->CreateBrush(JColor(0, 0, 255), 1);

	//차 초기 위치
	car[0].x = car[1].x = car[2].x = car[3].x = 640;
	car[0].y = 40;
	car[1].y = 80;
	car[2].y = 120;
	car[3].y = 160;
	car[0].direction = car[1].direction = car[2].direction = car[3].direction = 180;

	//트랙 설정
	track[0].x1 = 200;
	track[0].x2 = 800;
	track[0].y1 = 10;
	track[0].y2 = 10;

	track[1].x1 = 200;
	track[1].x2 = 10;
	track[1].y1 = 10;
	track[1].y2 = 150;

	track[2].x1 = 10;
	track[2].x2 = 10;
	track[2].y1 = 100;
	track[2].y2 = 650;

	track[3].x1 = 10;
	track[3].x2 = 200;
	track[3].y1 = 600;
	track[3].y2 = 750;

	track[4].x1 = 200;
	track[4].x2 = 800;
	track[4].y1 = 750;
	track[4].y2 = 750;

	track[5].x1 = 800;
	track[5].x2 = 1000;
	track[5].y1 = 750;
	track[5].y2 = 600;

	track[6].x1 = 1000;
	track[6].x2 = 1000;
	track[6].y1 = 600;
	track[6].y2 = 150;

	track[7].x1 = 1000;
	track[7].x2 = 800;
	track[7].y1 = 150;
	track[7].y2 = 10;

	track[8].x1 = 200;
	track[8].x2 = 800;
	track[8].y1 = 200;
	track[8].y2 = 200;

	track[9].x1 = 200;
	track[9].x2 = 200;
	track[9].y1 = 200;
	track[9].y2 = 600;

	track[10].x1 = 200;
	track[10].x2 = 800;
	track[10].y1 = 600;
	track[10].y2 = 600;

	track[11].x1 = 800;
	track[11].x2 = 800;
	track[11].y1 = 600;
	track[11].y2 = 200;
}

CDungeon::~CDungeon(void)
{
	jdd->DeleteBrush(car_brush);
}

//조작
void CDungeon::Control()
{
}

//처리
void CDungeon::Process()
{
}

//보여주기
void CDungeon::Render()
{
	ClearScreen();

	//트랙
	for(int i = 0; i < 12; ++i)
	{
		jdd->DrawLine(backbuffer, line_brush, (int)track[i].x1, (int)track[i].y1, (int)track[i].x2, (int)track[i].y2, 1);
	}

	//라인
	jdd->DrawLine(backbuffer, course_brush, 640, 11, 640, 199, 1);

	//자동차
	for(int i = 0; i < PLAYER_MAX; ++i)
	{
		//차체
		RECT rect;
		SetRect(&rect, (int)car[i].x - CAR_RADIUS, (int)car[i].y - CAR_RADIUS, (int)car[i].x + CAR_RADIUS, (int)car[i].y + CAR_RADIUS);
		jdd->DrawEllipse(backbuffer, car_brush, &rect, 1);

		//방향
		double pi = 3.14159 * (double)car[i].direction / 180;
		double direction_x = (cos(pi) * BASE_DIRECTION_X) - (sin(pi) * BASE_DIRECTION_Y);
		double direction_y = (sin(pi) * BASE_DIRECTION_X) + (cos(pi) * BASE_DIRECTION_Y);
		double next_x = car[i].x + direction_x * 2;
		double next_y = car[i].y + direction_y * 2;
		jdd->DrawLine(backbuffer, car_brush, (int)car[i].x, (int)car[i].y, (int)next_x, (int)next_y, 1); 

		//차의 방향
		bool crash = false;
		double car_gradient;
		double b;

		if((next_x - car[i].x) == 0)
		{
			car_gradient = 999;
		}
		else
		{
			car_gradient = (double)(next_y - car[i].y) / (double)(next_x - car[i].x);
		}

		b = next_y - (car_gradient * (double)next_x);

		//트랙의 방향
		for(int j = 0; j < 12 && !crash; ++j)
		{
			double track_gradient;
			double track_b = track[j].y2;

			if((track[j].x2 - track[j].x1) == 0)
			{
				track_gradient = 999;
			}
			else
			{
				track_gradient = (double)(track[j].y2 - track[j].y1) / (double)(track[j].x2 - track[j].x1);
			}

			track_b = track[j].y2 - (track_gradient * (double)track[j].x2);

			//기울기 비교
			if(car_gradient == track_gradient)
			{
				/*if(b == track_b
					&& next_x >= (track[j].x1 < track[j].x2)?track[j].x1:track[j].x2
					&& next_x <= (track[j].x1 > track[j].x2)?track[j].x1:track[j].x2)
				{
					crash = true;
				}*/
			}
			else
			{
				if(track_gradient == 999)
				{
					double top = (track[i].y1 < track[i].y2)?track[i].y1:track[i].y2;
					double bottom = (track[i].y1 > track[i].y2)?track[i].y1:track[i].y2;

					double left = Min(track[i].x1 , track[i].x2);
					double right = Max(track[i].x1 , track[i].x2);

					if(car[i].y >= top && car[i].y <= bottom && car[i].x >= left && car[i].x <= right)
					{
						crash = true;
					}
				}
				else
				{
					double cross_x = (track_b - b) / (car_gradient - track_gradient);
					double start = (car[i].x < next_x)?car[i].x:next_x;
					double end = (car[i].x > next_x)?car[i].x:next_x;
					if(cross_x >= start && cross_x <= end)
					{
						crash = true;
					}
				}

				if(crash)
				{
					car[i].direction -= 5;
					if(car[i].direction < 0)
					{
						car[i].direction += 360;
					}
				}
			}
		}

		if(!crash || (next_x - car[i].x) == 0)
		{
			//이동
			car[i].x += direction_x / 4;
			car[i].y += direction_y / 4;
		}
	}
}