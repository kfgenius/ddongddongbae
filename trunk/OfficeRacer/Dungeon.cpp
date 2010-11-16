#include <math.h>

#include "Dungeon.h"

#define CAR_RADIUS				10
#define BASE_DIRECTION_X		CAR_RADIUS
#define BASE_DIRECTION_Y		0
#define HANDLE_OUTLINE_RADIUS	50
#define HANDLE_INLINE_RADIUS	30

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
	boost_id = -1;
	boost_on = false;

	//핸들 위치
	car[0].handle_x = 52;
	car[0].handle_y = 55;
	control_handle_id = -1;

	car[1].handle_x = 960;
	car[1].handle_y = 55;

	car[2].handle_x = 52;
	car[2].handle_y = 710;

	car[3].handle_x = 960;
	car[3].handle_y = 710;

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
	track[2].y1 = 150;
	track[2].y2 = 600;

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

//차 이동
void CDungeon::MoveCar(int id)
{
	//차체 그리기
	RECT rect;
	SetRect(&rect, (int)car[id].x - CAR_RADIUS, (int)car[id].y - CAR_RADIUS, (int)car[id].x + CAR_RADIUS, (int)car[id].y + CAR_RADIUS);
	jdd->DrawEllipse(backbuffer, car_brush, &rect, 1);

	double pi = 3.14159 * (double)car[id].direction / 180;
	double direction_x = (cos(pi) * BASE_DIRECTION_X) - (sin(pi) * BASE_DIRECTION_Y);
	double direction_y = (sin(pi) * BASE_DIRECTION_X) + (cos(pi) * BASE_DIRECTION_Y);
	double next_x = car[id].x + direction_x * 2;
	double next_y = car[id].y + direction_y * 2;
	jdd->DrawLine(backbuffer, car_brush, (int)car[id].x, (int)car[id].y, (int)next_x, (int)next_y, 1); 

	//차의 이동
	if(boost_id == id && boost_on)
	{
		bool crash = false;
		double car_gradient;
		double b;

		if((next_x - car[id].x) == 0)
		{
			car_gradient = 999;
		}
		else
		{
			car_gradient = (double)(next_y - car[id].y) / (double)(next_x - car[id].x);
		}

		b = next_y - (car_gradient * (double)next_x);

		//트랙의 방향
		for(int j = 0; j < TRACK_MAX && !crash; ++j)
		{
			double track_gradient;
			double track_b;

			if((track[j].x2 - track[j].x1) == 0)
			{
				track_gradient = 999;
			}
			else
			{
				track_gradient = (track[j].y2 - track[j].y1) / (track[j].x2 - track[j].x1);
			}

			track_b = track[j].y2 - (track_gradient * track[j].x2);

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
				double left = Min(car[id].x, next_x);
				double right = Max(car[id].x, next_x);
				double top = Min(car[id].y, next_y);
				double bottom = Max(car[id].y, next_y);

				double track_left = Min(track[j].x1, track[j].x2);
				double track_right = Max(track[j].x1, track[j].x2);
				double track_top = Min(track[j].y1, track[j].y2);
				double track_bottom = Max(track[j].y1, track[j].y2);

				//사각 영역 검사
				if(track_right > left && track_left < right && track_bottom > top && track_top < bottom)
				{
					double cross_x = (track_b - b) / (car_gradient - track_gradient);
					double cross_y = track_gradient * cross_x + track_b;

					if(cross_x >= left && cross_x <= right && cross_y >= top && cross_y <= bottom)
					{
						crash = true;
					}

					//트랙과 충돌
					/*if(crash)
					{
						car[id].direction -= 3;
						if(car[id].direction < 0)
						{
							car[id].direction += 360;
						}
					}*/
				}
			}
		}

		if(!crash)
		{
			//차들끼리 충돌 여부
			car[id].x += (direction_x / 4);
			car[id].y += (direction_y / 4);

			for(int i = 0; i < PLAYER_MAX; ++i)
			{
				if(id == i)continue;	//자신과의 충돌은 검사하지 않음

				double distance = sqrt(pow(abs(car[i].x - car[id].x), 2) + pow(abs(car[i].y - car[id].y), 2));
				if(distance < CAR_RADIUS * 2)
				{
					car[id].x -= (direction_x / 2);
					car[id].y -= (direction_y / 2);
				}
			}
		}
	}
}

int GetAngle(double x, double y)
{
	//상, 하
	if(x==0)
	{
		if(y==0)return -1;
		else if(y>0)return 270;
		else return 90;
	}
	//좌, 우
	else if(y==0)
	{
		if(x>0)return 0;
		else return 180;
	}
	//기타 각도
	else
	{
		if(y>0)return (int)(atan(x/y)*57.3) + 270;
		else return (int)(atan(x/y)*57.3) + 90;
	}
}

void CDungeon::DrawHandle(int id)
{
	//핸들
	RECT rect;
	SetRect(&rect, car[id].handle_x - HANDLE_OUTLINE_RADIUS, car[id].handle_y - HANDLE_OUTLINE_RADIUS,
			car[id].handle_x + HANDLE_OUTLINE_RADIUS, car[id].handle_y + HANDLE_OUTLINE_RADIUS);
	jdd->DrawEllipse(backbuffer, car_brush, &rect, 1);

	SetRect(&rect, car[id].handle_x - HANDLE_INLINE_RADIUS, car[id].handle_y - HANDLE_INLINE_RADIUS,
			car[id].handle_x + HANDLE_INLINE_RADIUS, car[id].handle_y + HANDLE_INLINE_RADIUS);
	jdd->DrawEllipse(backbuffer, car_brush, &rect, 1);

	//부스터
	SetRect(&rect, car[id].handle_x - 10, car[id].handle_y - 10,
			car[id].handle_x + 10, car[id].handle_y + 10);
	jdd->DrawRect(backbuffer, car_brush, &rect, 1);

	//조작
	if(LButton && control_handle_id == id)
	{
		double distance = sqrt(pow((double)abs(car[id].handle_x - MouseX), 2) + pow((double)abs(car[id].handle_y - MouseY), 2));
		if(distance <= HANDLE_OUTLINE_RADIUS + 5/*마진*/ && distance >= HANDLE_INLINE_RADIUS)
		{
			SetRect(&rect, car[id].handle_x - HANDLE_OUTLINE_RADIUS, car[id].handle_y - HANDLE_OUTLINE_RADIUS,
					car[id].handle_x + HANDLE_OUTLINE_RADIUS, car[id].handle_y + HANDLE_OUTLINE_RADIUS);
			jdd->DrawEllipse(backbuffer, line_brush, &rect, 1);

			SetRect(&rect, car[id].handle_x - HANDLE_INLINE_RADIUS, car[id].handle_y - HANDLE_INLINE_RADIUS,
					car[id].handle_x + HANDLE_INLINE_RADIUS, car[id].handle_y + HANDLE_INLINE_RADIUS);
			jdd->DrawEllipse(backbuffer, line_brush, &rect, 1);

			//처리
			int old_angle = GetAngle(old_mouse_x - car[id].handle_x, old_mouse_y - car[id].handle_y);
			int angle = GetAngle(MouseX - car[id].handle_x, MouseY - car[id].handle_y);
			int angle_gap = old_angle - angle;

			if(abs(angle_gap) <= 90)
			{
				car[id].direction += (old_angle - angle);
				if(car[id].direction < 0)car[id].direction += 360;
				if(car[id].direction >= 360)car[id].direction -= 360;
			}

			/*double pi = 3.14159 * (double)car[id].direction / 180;
			double direction_x = (cos(pi) * BASE_DIRECTION_X) - (sin(pi) * BASE_DIRECTION_Y);
			double direction_y = (sin(pi) * BASE_DIRECTION_X) + (cos(pi) * BASE_DIRECTION_Y);
			double next_x = car[id].x + direction_x;
			double next_y = car[id].y + direction_y;
			jdd->DrawLine(backbuffer, car_brush, (int)car[id].x, (int)car[id].y, (int)next_x, (int)next_y, 1); */
			

			old_mouse_x = (double)MouseX;
			old_mouse_y = (double)MouseY;
		}
	}

	if(LButton)
	{
		if(boost_id == id)
		{
			if(MouseX >= car[id].handle_x - 10 && MouseX <= car[id].handle_x + 10
				&& MouseY >= car[id].handle_y - 10 && MouseY <= car[id].handle_y + 10)
			{
				boost_on = true;

				SetRect(&rect, car[id].handle_x - 10, car[id].handle_y - 10,
						car[id].handle_x + 10, car[id].handle_y + 10);
				jdd->DrawRect(backbuffer, line_brush, &rect, 1);
			}
			else
			{
				boost_on = false;
			}
		}
	}
	else
	{
		boost_on = false;
	}
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
	for(int i = 0; i < TRACK_MAX; ++i)
	{
		jdd->DrawLine(backbuffer, line_brush, (int)track[i].x1, (int)track[i].y1, (int)track[i].x2, (int)track[i].y2, 1);
	}

	//스타트 라인
	jdd->DrawLine(backbuffer, course_brush, 640, 11, 640, 199, 1);

	//자동차, 핸들
	for(int i = 0; i < PLAYER_MAX; ++i)
	{
		MoveCar(i);
		DrawHandle(i);
	}

	if(LeftDown())
	{
		control_handle_id = -1;

		for(int id = 0; id < PLAYER_MAX; ++id)
		{
			double distance = sqrt(pow((double)abs(car[id].handle_x - MouseX), 2) + pow((double)abs(car[id].handle_y - MouseY), 2));
			if(distance <= HANDLE_OUTLINE_RADIUS + 5/*마진*/ && distance >= HANDLE_INLINE_RADIUS)
			{
				old_mouse_x = (double)MouseX;
				old_mouse_y = (double)MouseY;
				control_handle_id = id;
			}

			if(MouseX >= (int)(car[id].handle_x - 10) && MouseX <= (int)(car[id].handle_x + 10)
				&& (int)(MouseY >= car[id].handle_y - 10) && MouseY <= (int)(car[id].handle_y + 10))
			{
				boost_id = id;
			}
		}
	}
}