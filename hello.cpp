#include <easyx.h>			// 引用 EasyX 绘图库头文件
#include <conio.h>
#include<time.h>
#include<iostream>
#include<math.h>
#include<string>
#include<algorithm>
/*
#define WHITE  0
#define RED 1
#define BLUE  2
#define GREEN  3
#define ORANGE  4
#define YELLOW  5
layer 2 1 0 = down middle up
*/
const double sqrt2=1.414;
const double pi = 3.14;
const int len = 40;
const int lensqrt2 = round((len - 10) / sqrt2);
using namespace std;

class point2d
{
public:
	int x=0;
	int y=0;
	point2d() :x(0), y(0) {}
	point2d(int xx, int yy) :x(xx), y(yy) {}
	point2d& operator=(const point2d& cls)
	{
		// 避免自赋值
		if (this != &cls)
		{
			x = cls.x;
			y = cls.y;
		}
		return *this;
	}
};

class point3d
{
public:
	int x;
	int y;
	int z;
	point3d():x(0),y(0),z(0){}
	point3d(int xx,int yy,int zz):x(xx),y(yy),z(zz){}
	point3d& operator=(const point3d& cls)
	{
		// 避免自赋值
		if (this != &cls)
		{
			x = cls.x;
			y = cls.y;
			z = cls.z;
		}
		return *this;
	}
};

class surface
{
public:
	int face[3][3];
	surface* goabove, * gobottom, * goleft, * goright;
	surface()
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				face[i][j] = 0;
	}
	surface(int in)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				face[i][j] = in;
	}
};

class rubik
{
public:
	surface* front = new surface(0);
	surface* back = new surface(5);
	surface* left = new surface(2);
	surface* right = new surface(3);
	surface* above = new surface(1);
	surface* bottom = new surface(4);
	int x,y,z;
	rubik() {
		x = 0;
		y = 0;
		z = 0;
		front->goleft = left;
		front->goright = right;
		front->goabove = above;
		front->gobottom = bottom;
		
		back->goleft = right;
		back->goright = left;
		back->goabove = above;
		back->gobottom = bottom;

		left->goleft = back;
		left->goright = front;
		left->goabove = above;
		left->gobottom = bottom;

		right->goleft = front;
		right->goright = back;
		right->goabove = above;
		right->gobottom = bottom;

		above->goleft = left;
		above->goright = right;
		above->goabove = back;
		above->gobottom = front;

		bottom->goleft = left;
		bottom->goright = right;
		bottom->goabove = front;
		bottom->gobottom = back;
	}
};
class rubik* magicbox=new rubik();

class MAGICSTATE
{
public:
	int state[3][3][3];
	int now_operate;
	int now_layer;
	int istotal;
	MAGICSTATE()
	{
		istotal = 0;
		now_operate = 0;
		now_layer = -1;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++)
					state[i][j][k] = 0;
	}
};
class MAGICSTATE* magicboxstate = new MAGICSTATE();

int info[27][3] = {
	 -len,  +len,  +len,
	0,  +len, +len,
	 +len,  +len,  +len,
	 -len, 0,  +len,
	0, 0,  +len,
	 +len, 0,  +len,
	 -len,  -len,  +len,
	0,  -len,  +len,
	 +len, -len,  +len,
	 -len,  +len, 0,
	0,  +len, 0,
	 +len, +len, 0,
	 -len, 0, 0,
	0, 0, 0,
	 +len, 0, 0,
	 -len,  -len, 0,
	0,  -len, 0,
	 +len,  -len, 0,
	 -len,  +len,  -len,
	0,  +len,  -len,
	 +len,  +len,  -len,
	 -len, 0,  -len,
	0, 0,  -len,
	 +len, 0,  -len,
	 -len,  -len, -len,
	0,  -len,  -len,
	 +len,  -len,  -len
};

void show_initial()
{
	int width = 600, height = 600;
	initgraph(width, height, EW_SHOWCONSOLE);
	setbkcolor(WHITE);
	cleardevice();
	setorigin((int)width / 2, (int)height / 2);
	setaspectratio(1, -1);
	setlinecolor(BLACK);
}

// rotate from x to y
point3d rotate_xy(point3d in, double angle)
{
	point3d out;
	out.x = round(in.x * cos(angle) - in.y * sin(angle));
	out.y = round(in.y * cos(angle) + in.x * sin(angle));
	out.z = in.z;
	return out;
}
// rotate from y to z
point3d rotate_yz(point3d in, double angle)
{
	point3d out;
	out.x = in.x;
	out.y = round(in.y * cos(angle) - in.z * sin(angle));
	out.z = round(in.z * cos(angle) + in.y * sin(angle));
	return out;
}
// rotate from x to z
point3d rotate_xz(point3d in, double angle)
{
	point3d out;
	out.x = round(in.x * cos(angle) - in.z * sin(angle));
	out.y = in.y;
	out.z = round(in.z * cos(angle) + in.x * sin(angle));
	return out;
}

void second()
{
	Sleep(80);
}

void change_fillcolor(int color)
{
	switch (color)
	{
	case 0:setfillcolor(WHITE); break;
	case 1:setfillcolor(RED); break;
	case 2:setfillcolor(BLUE); break;
	case 3:setfillcolor(GREEN); break;
	case 4:setfillcolor(RGB(255, 140, 0)); break;
	case 5:setfillcolor(YELLOW); break;
	default:setfillcolor(BLACK);
	}
}

void show_surface(point2d a, point2d b, point2d c, point2d d,int color)
{
	change_fillcolor(color);
	POINT pts[] = { {a.x, a.y}, {b.x, b.y},{c.x, c.y},{d.x, d.y}, };
	fillpolygon(pts, 4);
}

int transform_point(point2d* in2d, point3d* in3d,int index)
{
	point3d temp[8];
	int i = index / 9;
	index -= i * 9;
	int j = index / 3;
	index -= j * 3;
	int k = index;
	int state = magicboxstate->state[i][j][k];
	int rotate_direction = (int)(state / 10);
	// 1 up   2 down   3left   4right  5clockwise 6anti-clockwise
	double rotate_angle = (double)((state % 10)-1)/8 * pi / 2;
	// rotate_angle=10=pi/2 
	for (int i = 0; i < 8; i++)
	{
		temp[i] = in3d[i];
		if (rotate_direction == 0);
		else if (rotate_direction == 1)temp[i] = rotate_yz(temp[i], -rotate_angle);
		else if (rotate_direction == 2)temp[i] = rotate_yz(temp[i], rotate_angle);
		else if (rotate_direction == 3)temp[i] = rotate_xy(temp[i], -rotate_angle);
		else if (rotate_direction == 4)temp[i] = rotate_xy(temp[i], rotate_angle);
		else if (rotate_direction == 5)temp[i] = rotate_xz(temp[i], -rotate_angle);
		else if (rotate_direction == 6)temp[i] = rotate_xz(temp[i], rotate_angle);
		else;
		temp[i]= rotate_yz(temp[i], pi / 4);//pi/6
		temp[i] = rotate_xy(temp[i], -pi /4);// -pi/8
		in2d[i].x = temp[i].x;
		in2d[i].y = temp[i].z;
	}
	return state;
}

void creat_cube(point3d center,int index3d)
{
	int layer = index3d / 9;
	int index2d = index3d % 9;
	point2d vertex2d[8];
	point3d vertex[8];
	for (int i = 0; i < 8; i++)
		if (i == 1 || i == 2 || i == 5 || i == 6)
			vertex[i].x = center.x + len / 2;
		else
			vertex[i].x = center.x - len / 2;
	for (int i = 0; i < 8; i++)
		if (i == 0 || i == 1 || i == 4 || i == 5)
			vertex[i].y = center.y + len / 2;
		else
			vertex[i].y = center.y - len / 2;
	for (int i = 0; i < 8; i++)
		if (i == 0 || i == 1 || i == 2 || i == 3)
			vertex[i].z = center.z + len/2 ;
		else
			vertex[i].z = center.z - len/2 ;
	//-----------------------------
	int cube_state=transform_point(vertex2d, vertex,index3d);
	//-----------------------------
	int color_above = 100, color_front = 100, color_right = 100;
	int color_bottom = 100, color_back = 100, color_left = 100;
	if(layer==0)
		color_above = magicbox->above->face[(int)(index2d / 3)][index2d % 3];
	if (layer == 2)
	{
		switch (index2d) {
		case 0:color_bottom = magicbox->bottom->face[2][0]; break;
		case 1:color_bottom = magicbox->bottom->face[2][1]; break;
		case 2:color_bottom = magicbox->bottom->face[2][2]; break;
		case 3:color_bottom = magicbox->bottom->face[1][0]; break;
		case 4:color_bottom = magicbox->bottom->face[1][1]; break;
		case 5:color_bottom = magicbox->bottom->face[1][2]; break;
		case 6:color_bottom = magicbox->bottom->face[0][0]; break;
		case 7:color_bottom = magicbox->bottom->face[0][1]; break;
		case 8:color_bottom = magicbox->bottom->face[0][2]; break;
		default:cout << "error in color_bottom" << endl;
		}
	}
	if(index2d==0)
	{
		color_back = magicbox->back->face[layer][2];
		color_left = magicbox->left->face[layer][0];
	}
	else if (index2d == 1)
	{
		color_back = magicbox->back->face[layer][1];
	}
	else if (index2d == 2)
	{
		color_back = magicbox->back->face[layer][0];
		color_right = magicbox->right->face[layer][2];
	}
	else if (index2d == 3)
	{
		color_left = magicbox->left->face[layer][1];
	}
	else if (index2d == 5)
	{
		color_right = magicbox->right->face[layer][1];
	}
	else if (index2d == 6)
	{
		color_left = magicbox->left->face[layer][2];
		color_front = magicbox->front->face[layer][0];
		//color_right = magicbox->right->face[layer][1];
	}
	else if (index2d == 7)
	{
		color_front = magicbox->front->face[layer][1];
		//color_right = magicbox->right->face[layer][1];
	}
	else if (index2d == 8)
	{
		color_right = magicbox->right->face[layer][0];
		color_front = magicbox->front->face[layer][2];
	}
	else;

	if (cube_state > 15 && cube_state <= 19)
	{
		show_surface(vertex2d[2], vertex2d[3], vertex2d[7], vertex2d[6], color_front);
		show_surface(vertex2d[4], vertex2d[5], vertex2d[6], vertex2d[7], color_bottom);
		show_surface(vertex2d[1], vertex2d[2], vertex2d[6], vertex2d[5], color_right);
	}
	else if (cube_state > 25 && cube_state <= 29)
	{
		show_surface(vertex2d[0], vertex2d[1], vertex2d[2], vertex2d[3], color_above);
		show_surface(vertex2d[0], vertex2d[1], vertex2d[5], vertex2d[4], color_back);
		show_surface(vertex2d[1], vertex2d[2], vertex2d[6], vertex2d[5], color_right);
	}
	else if (cube_state > 35 && cube_state <= 39)
	{
		show_surface(vertex2d[0], vertex2d[1], vertex2d[5], vertex2d[4], color_back);
		show_surface(vertex2d[1], vertex2d[2], vertex2d[6], vertex2d[5], color_right);
		show_surface(vertex2d[0], vertex2d[1], vertex2d[2], vertex2d[3], color_above);
	}
	else if (cube_state > 45 && cube_state <= 49)
	{
		show_surface(vertex2d[0], vertex2d[4], vertex2d[7], vertex2d[3], color_left);
		show_surface(vertex2d[2], vertex2d[3], vertex2d[7], vertex2d[6], color_front);
		show_surface(vertex2d[0], vertex2d[1], vertex2d[2], vertex2d[3], color_above);
	}
	else if (cube_state > 55 && cube_state <= 59)
	{
		show_surface(vertex2d[0], vertex2d[4], vertex2d[7], vertex2d[3], color_left);
		show_surface(vertex2d[2], vertex2d[3], vertex2d[7], vertex2d[6], color_front);
		show_surface(vertex2d[0], vertex2d[1], vertex2d[2], vertex2d[3], color_above);
	}
	else if (cube_state > 65 && cube_state <= 69)
	{
		show_surface(vertex2d[1], vertex2d[2], vertex2d[6], vertex2d[5], color_right);
		show_surface(vertex2d[2], vertex2d[3], vertex2d[7], vertex2d[6], color_front);
		show_surface(vertex2d[4], vertex2d[5], vertex2d[6], vertex2d[7], color_bottom);
	}
	else {
		
		show_surface(vertex2d[2], vertex2d[3], vertex2d[7], vertex2d[6], color_front);
		show_surface(vertex2d[1], vertex2d[2], vertex2d[6], vertex2d[5], color_right);
		show_surface(vertex2d[0], vertex2d[1], vertex2d[2], vertex2d[3], color_above);
		//show_surface(vertex2d[4], vertex2d[5], vertex2d[6], vertex2d[7], color_bottom);
		//show_surface(vertex2d[0], vertex2d[4], vertex2d[7], vertex2d[3], color_left);
		//show_surface(vertex2d[0], vertex2d[1], vertex2d[5], vertex2d[4], color_back);
	}
}

int lr_0[27] = { 18,19,20,21,22,23,24,25,26,
	9,10,11,12,13,14,15,16,17,
	0,1,2,3,4,5,6,7,8 };

int lr_1[27] = { 18,19,20,21,22,23,24,25,26,
9,10,11,12,13,14,15,16,17,
2,5,8,1,4,7,0,3,6 };

int lr_2[27] = { 18,19,20,21,22,23,24,25,26,
11,14,17,10,13,16,9,12,15,
0,1,2,3,4,5,6,7,8 };

int lr_3[27] = { 20,23,26,19,22,25,18,21,24,
9,10,11,12,13,14,15,16,17,
0,1,2,3,4,5,6,7,8 };

int lr_4[27] = { 18,19,20,21,22,23,24,25,26,
9,10,11,12,13,14,15,16,17,
6,3,0,7,4,1,8,5,2};

int lr_5[27] = { 18,19,20,21,22,23,24,25,26,
15,12,9,16,13,10,17,14,11,
0,1,2,3,4,5,6,7,8 };

int lr_6[27] = { 24,21,18,25,22,19,26,23,20,
9,10,11,12,13,14,15,16,17,
0,1,2,3,4,5,6,7,8 };

int ab_0[27] = { 18,21,24,9,12,15,0,3,6,
19,22,25,10,13,16,1,4,7,
20,23,26,11,14,17,2,5,8};

int ab_1[27] = { 0,3,6,9,12,15,18,21,24,
19,22,25,10,13,16,1,4,7,
20,23,26,11,14,17,2,5,8 };

int ab_2[27] = { 18,21,24,9,12,15,0,3,6,
1,4,7,10,13,16,19,22,25,
20,23,26,11,14,17,2,5,8 };

int ab_3[27] = { 18,21,24,9,12,15,0,3,6,
19,22,25,10,13,16,1,4,7,
2,5,8,11,14,17,20,23,26 };

int ab_4[27] = { 24,15,6,21,12,3,18,9,0,
19,22,25,10,13,16,1,4,7,
20,23,26,11,14,17,2,5,8 };

int ab_5[27] = { 18,21,24,9,12,15,0,3,6,
25,16,7,22,13,4,19,10,1,
20,23,26,11,14,17,2,5,8 };

int ab_6[27] = { 18,21,24,9,12,15,0,3,6,
19,22,25,10,13,16,1,4,7,
26,17,8,23,14,5,20,11,2 };

int cl_0[27] = { 18,19,20,9,10,11,0,1,2,
21,22,23,12,13,14,3,4,5,
24,25,26,15,16,17,6,7,8 };

int cl_1[27] = { 18,19,20,9,10,11,0,1,2,
21,22,23,12,13,14,3,4,5,
26,17,8,25,16,7,24,15,6 };

int cl_2[27] = { 18,19,20,9,10,11,0,1,2,
23,14,5,22,13,4,21,12,3,
24,25,26,15,16,17,6,7,8 };

int cl_3[27] = { 20,11,2,19,10,1,18,9,0,
21,22,23,12,13,14,3,4,5,
24,25,26,15,16,17,6,7,8 };

int cl_4[27] = { 18,19,20,9,10,11,0,1,2,
21,22,23,12,13,14,3,4,5,
6,15,24,7,16,25,8,17,26 };

int cl_5[27] = { 18,19,20,9,10,11,0,1,2,
3,12,21,4,13,22,5,14,23,
24,25,26,15,16,17,6,7,8 };

int cl_6[27] = { 0,9,18,1,10,19,2,11,20,
21,22,23,12,13,14,3,4,5,
24,25,26,15,16,17,6,7,8 };

int total_1[27];

int total_2[27] = {24,25,26,21,22,23,18,19,20,
	15,16,17,12,13,14,9,10,11,
	6,7,8,3,4,5,0,1,2};

int total_3[27] = {24,21,18,25,22,19,26,23,20,
15,12,9,16,13,10,17,14,11,
6,3,0,7,4,1,8,5,2};

int total_4[27] = { 20,23,26,19,22,25,18,21,24,
11,14,17,10,13,16,9,12,15,
2,5,8,1,4,7,0,3,6 };

int total_5[27] = {20,11,2,19,10,1,18,9,0,
23,14,5,22,13,4,21,12,3,
26,17,8,25,16,7,24,15,6};

int total_6[27] = {
	0,9,18,3,12,21,6,15,24,
	1,10,19,4,13,22,7,16,25,
	2,11,20,5,14,23,8,17,26 };

void creat_magicbox()
{
	cleardevice();
	point3d center = point3d(magicbox->x, magicbox->y, magicbox->z);
	//bottom layer
	int* p=lr_0;

	if (magicboxstate->istotal == 1)
	{
		if (magicboxstate->now_operate > 15 && magicboxstate->now_operate <= 19)
			cout<<"not defined";
		else if (magicboxstate->now_operate > 25 && magicboxstate->now_operate <= 29)
			p = total_2;
		else if (magicboxstate->now_operate > 35 && magicboxstate->now_operate <= 39)
			p = total_3;
		else if (magicboxstate->now_operate > 45 && magicboxstate->now_operate <= 49)
			p = total_4;
		else if (magicboxstate->now_operate > 55 && magicboxstate->now_operate <= 59)
			p = total_5;
		else if (magicboxstate->now_operate > 65 && magicboxstate->now_operate <= 69)
			p = total_6;
	}
	else if (magicboxstate->istotal == 0)
	{
		if (magicboxstate->now_operate >= 50 && magicboxstate->now_operate <= 69)
		{
			if (magicboxstate->now_layer == 0 && magicboxstate->now_operate > 55 && magicboxstate->now_operate <= 59)
				p = cl_1;
			else if (magicboxstate->now_layer == 1 && magicboxstate->now_operate > 55 && magicboxstate->now_operate <= 59)
				p = cl_2;
			else if (magicboxstate->now_layer == 2 && magicboxstate->now_operate > 55 && magicboxstate->now_operate <= 59)
				p = cl_3;
			else if (magicboxstate->now_layer == 0 && magicboxstate->now_operate > 65 && magicboxstate->now_operate <= 69)
				p = cl_4;
			else if (magicboxstate->now_layer == 1 && magicboxstate->now_operate > 65 && magicboxstate->now_operate <= 69)
				p = cl_5;
			else if (magicboxstate->now_layer == 2 && magicboxstate->now_operate > 65 && magicboxstate->now_operate <= 69)
				p = cl_6;
			else
				p = cl_0;
		}

		else if (magicboxstate->now_operate >= 30 && magicboxstate->now_operate <= 49)
		{
			if (magicboxstate->now_layer == 0 && magicboxstate->now_operate > 45 && magicboxstate->now_operate <= 49)
				p = lr_1;
			else if (magicboxstate->now_layer == 1 && magicboxstate->now_operate > 45 && magicboxstate->now_operate <= 49)
				p = lr_2;
			else if (magicboxstate->now_layer == 2 && magicboxstate->now_operate > 45 && magicboxstate->now_operate <= 49)
				p = lr_3;
			else if (magicboxstate->now_layer == 0 && magicboxstate->now_operate > 35 && magicboxstate->now_operate <= 39)
				p = lr_4;
			else if (magicboxstate->now_layer == 1 && magicboxstate->now_operate > 35 && magicboxstate->now_operate <= 39)
				p = lr_5;
			else if (magicboxstate->now_layer == 2 && magicboxstate->now_operate > 35 && magicboxstate->now_operate <= 39)
				p = lr_6;
			else
				p = lr_0;
		}

		else if (magicboxstate->now_operate >= 10 && magicboxstate->now_operate <= 29)
		{
			if (magicboxstate->now_layer == 0 && magicboxstate->now_operate > 15 && magicboxstate->now_operate <= 19)
				p = ab_1;
			else if (magicboxstate->now_layer == 1 && magicboxstate->now_operate > 15 && magicboxstate->now_operate <= 19)
				p = ab_2;
			else if (magicboxstate->now_layer == 2 && magicboxstate->now_operate > 15 && magicboxstate->now_operate <= 19)
				p = ab_3;
			else if (magicboxstate->now_layer == 0 && magicboxstate->now_operate > 25 && magicboxstate->now_operate <= 29)
				p = ab_4;
			else if (magicboxstate->now_layer == 1 && magicboxstate->now_operate > 25 && magicboxstate->now_operate <= 29)
				p = ab_5;
			else if (magicboxstate->now_layer == 2 && magicboxstate->now_operate > 25 && magicboxstate->now_operate <= 29)
				p = ab_6;
			else
				p = ab_0;
		}

		else
			;
	}
	else
		cout << "error magicboxstate->istotal " << endl;

	for (int i = 0; i < 27; i++)
		creat_cube(point3d(center.x+info[p[i]][0], center.y + info[p[i]][1], center.z + info[p[i]][2]), p[i]);
	
}

void clockwise_surface_date(surface* in)
{
	int temp[9];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			temp[i * 3 + j] =in->face[i][j];
	in->face[0][0] = temp[6];
	in->face[0][1] = temp[3];
	in->face[0][2] = temp[0];
	in->face[1][0] = temp[7];
	in->face[1][1] = temp[4];
	in->face[1][2] = temp[1];
	in->face[2][0] = temp[8];
	in->face[2][1] = temp[5];
	in->face[2][2] = temp[2];
}

void anticlockwise_surface_date(surface* in)
{
	int temp[9];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			temp[i * 3 + j] = in->face[i][j];
	in->face[0][0] = temp[2];
	in->face[0][1] = temp[5];
	in->face[0][2] = temp[8];
	in->face[1][0] = temp[1];
	in->face[1][1] = temp[4];
	in->face[1][2] = temp[7];
	in->face[2][0] = temp[0];
	in->face[2][1] = temp[3];
	in->face[2][2] = temp[6];
}

void rotate_data_right(int layer)
{
	if (layer == 0)
		anticlockwise_surface_date(magicbox->above);
	else if (layer == 2)
		clockwise_surface_date(magicbox->bottom);
	int temp0[3], temp1[3], temp2[3], temp3[3];
	for (int i = 0; i < 3; i++)
	{
		temp0[i] = magicbox->front->face[layer][i];
		temp1[i] = magicbox->right->face[layer][i];
		temp2[i] = magicbox->back->face[layer][i];
		temp3[i] = magicbox->left->face[layer][i];
	}
	for (int i = 0; i < 3; i++)
	{
		magicbox->front->face[layer][i] = temp3[i];
		magicbox->right->face[layer][i]= temp0[i];
		magicbox->back->face[layer][i]= temp1[i];
		magicbox->left->face[layer][i]= temp2[i];
	}
}

void rotate_data_left(int layer)
{
	rotate_data_right(layer);
	rotate_data_right(layer);
	rotate_data_right(layer);
}

void rotate_data_bottom(int layer)
{
	int temp0[3], temp1[3], temp2[3], temp3[3];
	for (int i = 0; i < 3; i++)
	{
		temp0[i] = magicbox->above->face[i][layer];
		temp1[i] = magicbox->front->face[i][layer];
		temp2[i] = magicbox->bottom->face[i][layer];
		temp3[i] = magicbox->back->face[2 - i][2 - layer];
	}
	for (int i = 0; i < 3; i++)
	{
		magicbox->above->face[i][layer] = temp3[i];
		magicbox->front->face[i][layer] = temp0[i];
		magicbox->bottom->face[i][layer] = temp1[i];
		magicbox->back->face[2 - i][2 - layer] = temp2[i];
	}
	if (layer == 0)
		clockwise_surface_date(magicbox->left);
	else if (layer == 2)
		anticlockwise_surface_date(magicbox->right);
}

void rotate_data_above(int layer)
{
	rotate_data_bottom(layer);
	rotate_data_bottom(layer);
	rotate_data_bottom(layer);
}

void rotate_data_clockwise(int layer)
{
	if(layer==0)
		clockwise_surface_date(magicbox->front);
	else if(layer==2)
		anticlockwise_surface_date(magicbox->back);
	int temp0[3], temp1[3], temp2[3], temp3[3];
	for (int i = 0; i < 3; i++)
	{
		temp0[i] = magicbox->above->face[2 - layer][i];
		temp1[i] = magicbox->right->face[i][layer];
		temp2[i] = magicbox->bottom->face[layer][2-i];
		temp3[i] = magicbox->left->face[2 - i][2-layer];
	}
	for (int i = 0; i < 3; i++)
	{
		magicbox->above->face[2 - layer][i] = temp3[i];
		magicbox->right->face[i][layer]= temp0[i];
		magicbox->bottom->face[layer][2 - i]= temp1[i];
		magicbox->left->face[2 - i][2 - layer]= temp2[i];
	}
}

void rotate_data_anticlockwise(int layer)
{
	rotate_data_clockwise(layer);
	rotate_data_clockwise(layer);
	rotate_data_clockwise(layer);
}
/////
void rotate_state_left_right(int layer,int temp_state)
{
	magicboxstate->now_operate = temp_state;
	magicboxstate->now_layer = layer;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			magicboxstate->state[layer][i][j] = temp_state;
}

void rotate_state_above_bottom(int layer, int temp_state)
{
	magicboxstate->now_operate = temp_state;
	magicboxstate->now_layer = layer;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			magicboxstate->state[i][j][layer] = temp_state;
}

void rotate_state_clockwise_anticlockwise(int layer, int temp_state)
{
	magicboxstate->now_operate = temp_state;
	magicboxstate->now_layer = layer;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			magicboxstate->state[i][2-layer][j] = temp_state;
}
/// ///
void rotate_magicbox_right(int layer)
{
	rotate_state_left_right(layer, 43);
	creat_magicbox();
	second();

	rotate_state_left_right(layer, 45);
	creat_magicbox();
	second();

	rotate_state_left_right(layer, 47);
	creat_magicbox();
	second();

	rotate_data_right(layer);
	rotate_state_left_right(layer, 0);
	creat_magicbox();
	second();
}

void rotate_magicbox_left(int layer)
{
	rotate_state_left_right(layer, 32);
	creat_magicbox();
	second();

	rotate_state_left_right(layer, 34);
	creat_magicbox();
	second();

	rotate_state_left_right(layer, 36);
	creat_magicbox();
	second();


	rotate_data_left(layer);
	rotate_state_left_right(layer, 0);
	creat_magicbox();
	second();
}

void rotate_magicbox_above(int layer)
{
	rotate_state_above_bottom(layer,13);
	creat_magicbox();
	second();

	rotate_state_above_bottom(layer, 15);
	creat_magicbox();
	second();

	rotate_state_above_bottom(layer, 17);
	creat_magicbox();
	second();

	rotate_data_above(layer);
	rotate_state_above_bottom(layer,0);
	creat_magicbox();
	second();
}

void rotate_magicbox_bottom(int layer)
{
	rotate_state_above_bottom(layer, 23);
	creat_magicbox();
	second();

	rotate_state_above_bottom(layer, 25);
	creat_magicbox();
	second();

	rotate_state_above_bottom(layer, 27);
	creat_magicbox();
	second();

	rotate_data_bottom(layer);
	rotate_state_above_bottom(layer, 0);
	creat_magicbox();
	second();
}

void rotate_magicbox_clockwise(int layer)
{
	rotate_state_clockwise_anticlockwise(layer, 53);
	creat_magicbox();
	second();

	rotate_state_clockwise_anticlockwise(layer, 55);
	creat_magicbox();
	second();

	rotate_state_clockwise_anticlockwise(layer, 57);
	creat_magicbox();
	second();

	rotate_data_clockwise(layer);
	rotate_state_clockwise_anticlockwise(layer, 0);
	creat_magicbox();
	second();
}

void rotate_magicbox_anticlockwise(int layer)
{
	rotate_state_clockwise_anticlockwise(layer, 63);
	creat_magicbox();
	second();

	rotate_state_clockwise_anticlockwise(layer, 64);
	creat_magicbox();
	second();

	rotate_state_clockwise_anticlockwise(layer, 67);
	creat_magicbox();
	second();

	rotate_data_anticlockwise(layer);
	rotate_state_clockwise_anticlockwise(layer, 0);
	creat_magicbox();
	second();
}
//
void rotate_total_clockwise()
{
	magicboxstate->istotal = 1;
	for (int i = 0; i < 3; i++)
		rotate_state_clockwise_anticlockwise(i, 53);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
		rotate_state_clockwise_anticlockwise(i, 55);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
		rotate_state_clockwise_anticlockwise(i, 57);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
	{
		rotate_data_clockwise(i);
		rotate_state_clockwise_anticlockwise(i, 0);
	}
	creat_magicbox();
	second();
	magicboxstate->istotal = 0;
}

void rotate_total_anticlockwise()
{
	magicboxstate->istotal = 1;
	for (int i = 0; i < 3; i++)
		rotate_state_clockwise_anticlockwise(i, 63);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
		rotate_state_clockwise_anticlockwise(i, 64);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
		rotate_state_clockwise_anticlockwise(i, 67);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
	{
		rotate_data_anticlockwise(i);
		rotate_state_clockwise_anticlockwise(i, 0);
	}
	creat_magicbox();
	second();
	magicboxstate->istotal = 0;
}

void rotate_total_bottom()
{
	magicboxstate->istotal = 1;
	for (int i = 0; i < 3; i++)
		rotate_state_above_bottom(i, 23);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
		rotate_state_above_bottom(i, 25);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
		rotate_state_above_bottom(i, 27);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
	{
		rotate_data_bottom(i);
		rotate_state_above_bottom(i, 0);
	}
	creat_magicbox();
	second();
	magicboxstate->istotal = 0;
}

void rotate_total_right()
{
	magicboxstate->istotal = 1;
	for (int i = 0; i < 3; i++)
		rotate_state_left_right(i, 43);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
		rotate_state_left_right(i, 45);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
		rotate_state_left_right(i, 47);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
	{
		rotate_data_right(i);
		rotate_state_left_right(i, 0);
	}
	creat_magicbox();
	second();
	magicboxstate->istotal = 0;
}

void rotate_total_left()
{
	magicboxstate->istotal = 1;
	for (int i = 0; i < 3; i++)
		rotate_state_left_right(i, 32);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
		rotate_state_left_right(i, 34);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
		rotate_state_left_right(i, 36);
	creat_magicbox();
	second();

	for (int i = 0; i < 3; i++)
	{
		rotate_data_left(i);
		rotate_state_left_right(i, 0);
	}
	creat_magicbox();
	second();
	magicboxstate->istotal = 0;
}

void disorganize()
{
	time_t t;
	srand((unsigned)time(&t));
	//srand(1613911203);
	cout << time(&t) << endl;
	for (int i = 0; i < 40; i++)
	{
		int operate = rand() % 6;
		int layer = rand() % 3;
		if (layer == 1)
			layer++;
		switch (operate)
		{
		case 0:rotate_data_left(layer); break;
		case 1:rotate_data_right(layer); break;
		case 2:rotate_data_above(layer); break;
		case 3:rotate_data_bottom(layer); break;
		case 4:rotate_data_clockwise(layer); break;
		case 5:rotate_data_anticlockwise(layer); break;
		default: cout << "error rand" << endl;
		}
	}
}

//check white cross
bool check_step1()
{
	if (magicbox->front->face[0][1] == 0 &&
		magicbox->front->face[1][0] == 0 &&
		magicbox->front->face[1][2] == 0 &&
		magicbox->front->face[2][1] == 0)
		return true;
	else
		return false;
}

void step1()
{
	while (!check_step1())
	{
		int white_position = -1;
		//section1 find white

		if (magicbox->above->face[1][2] == 0)
		{
			white_position = 1;
			rotate_magicbox_anticlockwise(1);
		}
		else if(magicbox->right->face[2][1] == 0)
		{
			white_position = 1;
			rotate_magicbox_anticlockwise(1);
			rotate_magicbox_anticlockwise(1);
		}
		else if (magicbox->bottom->face[1][0] == 0)
		{
			white_position = 1;
			rotate_magicbox_clockwise(1);
		}
		else if (magicbox->left->face[0][1] == 0)
		{
			white_position = 1;
		}
		//-----------------------------
		else if (magicbox->above->face[1][0] == 0)
		{
			white_position = 2;
			rotate_magicbox_clockwise(1);
		}
		else if (magicbox->right->face[0][1] == 0)
		{
			white_position = 2;
		}
		else if (magicbox->bottom->face[1][2] == 0)
		{
			white_position = 2;
			rotate_magicbox_anticlockwise(1);
		}
		else if (magicbox->left->face[2][1] == 0)
		{
			white_position = 2;
			rotate_magicbox_clockwise(1);
			rotate_magicbox_clockwise(1);
		}
		//------------------------
		else if(magicbox->above->face[0][1] == 0)
		{
			white_position = 3;
			rotate_magicbox_clockwise(2);
		}
		else if (magicbox->right->face[1][2] == 0)
		{
			white_position = 3;
		}
		else if (magicbox->bottom->face[2][1] == 0)
		{
			white_position = 3;
			rotate_magicbox_anticlockwise(2);
		}
		else if (magicbox->left->face[1][0] == 0)
		{
			white_position = 3;
			rotate_magicbox_anticlockwise(2);
			rotate_magicbox_anticlockwise(2);
		}
		else if(magicbox->back->face[0][1] == 0)
		{
			white_position = 4;
		}
		else if (magicbox->back->face[1][2] == 0)
		{
			white_position = 4;
			rotate_magicbox_clockwise(2);
		}
		else if (magicbox->back->face[2][1] == 0)
		{
			white_position = 4;
			rotate_magicbox_clockwise(2);
			rotate_magicbox_clockwise(2);
		}
		else if (magicbox->back->face[1][0] == 0)
		{
			white_position = 4;
			rotate_magicbox_anticlockwise(2);
		}
		else if(magicbox->above->face[2][1] == 0)
		{
			white_position = 5;
		}
		else if (magicbox->right->face[1][0] == 0)
		{
			white_position = 5;
			rotate_magicbox_anticlockwise(0);
		}
		else if (magicbox->bottom->face[0][1] == 0)
		{
			white_position = 5;
			rotate_magicbox_anticlockwise(0);
			rotate_magicbox_anticlockwise(0);
		}
		else if (magicbox->left->face[1][2] == 0)
		{
			white_position = 5;
			rotate_magicbox_clockwise(0);
		}
		else ;
		//

		//section2  operate
		if (white_position == 1)
		{
			//adjust white gap position
			if (magicbox->front->face[1][2] != 0)//no white
				;
			else if (magicbox->front->face[0][1] != 0)//no white
				rotate_magicbox_clockwise(0);
			else if (magicbox->front->face[2][1] != 0)//no white
				rotate_magicbox_anticlockwise(0);
			else if (magicbox->front->face[1][0] != 0)//no white
			{
				rotate_magicbox_clockwise(0);
				rotate_magicbox_clockwise(0);
			}
			else;
			// recover operate
			rotate_magicbox_above(2);
			rotate_magicbox_clockwise(1);
			rotate_magicbox_bottom(2);
		}
		else if (white_position == 2)
		{
			//adjust white gap position
			if (magicbox->front->face[1][0] != 0)//no white
			;
			else if (magicbox->front->face[0][1] != 0)//no white
				rotate_magicbox_anticlockwise(0);
			else if (magicbox->front->face[1][2] != 0)//no white
			{
				rotate_magicbox_anticlockwise(0);
				rotate_magicbox_anticlockwise(0);
			}
			else if (magicbox->front->face[2][1] != 0)//no white
				rotate_magicbox_clockwise(0);
			
			else;
			// recover operate
			rotate_magicbox_above(0);
			rotate_magicbox_anticlockwise(1);
			rotate_magicbox_bottom(0);
		}
		else if (white_position == 3)
		{
			//adjust white gap position
			if (magicbox->front->face[0][1] != 0)//no white
				;
			else if (magicbox->front->face[1][2] != 0)//no white
			{
				rotate_magicbox_anticlockwise(0);
			}
			else if (magicbox->front->face[2][1] != 0)//no white
			{
				rotate_magicbox_clockwise(0);
				rotate_magicbox_clockwise(0);
			}
			else if (magicbox->front->face[1][0] != 0)//no white
				rotate_magicbox_clockwise(0);
			else;
			// recover operate
			rotate_magicbox_above(1);
			rotate_magicbox_anticlockwise(2);
			rotate_magicbox_bottom(1);
		}
		else if (white_position == 4)
		{
			//adjust white gap position
			if (magicbox->front->face[0][1] != 0)//no white
				;
			else if (magicbox->front->face[1][2] != 0)//no white
			{
				rotate_magicbox_anticlockwise(0);
			}
			else if (magicbox->front->face[2][1] != 0)//no white
			{
				rotate_magicbox_clockwise(0);
				rotate_magicbox_clockwise(0);
			}
			else if (magicbox->front->face[1][0] != 0)//no white
				rotate_magicbox_clockwise(0);
			else;
			// recover operate
			rotate_magicbox_above(1);
			rotate_magicbox_clockwise(2);
			rotate_magicbox_clockwise(2);
			rotate_magicbox_bottom(1);
		}
		else if (white_position == 5)
		{
			rotate_magicbox_above(1);
			rotate_magicbox_clockwise(2);
			rotate_magicbox_bottom(1);
		}
		else;
	}
}

int check_step2()
{
	bool a = magicbox->above->face[1][1] == magicbox->above->face[2][1];
	bool b = magicbox->right->face[1][0] == magicbox->right->face[1][1];
	bool c = magicbox->bottom->face[0][1] == magicbox->bottom->face[1][1];
	bool d = magicbox->left->face[1][1] == magicbox->left->face[1][2];

	if (a == true && b == true && c == true && d == true)
		return 1; //success
	else if(a==true && c==true && b==false && d==false)
		return 2; // need more step above==bottom
	else if(b==true && d==true && a==false && c==false)
		return 3;//need more step left==right
	else if(a==true && b==true && c==false && d==false)
		return 4;// need one step above==right
	else if(b==true && c==true && d==false && a==false)
		return 5;// need one step right==bottom
	else if(c==true && d==true && a==false && b==false)
		return 6;// need one step bottom==left
	else if(d==true && a==true && b==false && c==false)
		return 7;// need one step left==above
	else if (a == false && b == false && c == false && d == false)
	{
		return 8;
		cout << "error in check step2" << endl;
	}
	else
		return 0;
}

void step2_right_hand()
{
	rotate_magicbox_bottom(0);
	rotate_magicbox_clockwise(0);
	rotate_magicbox_above(0);
	rotate_magicbox_anticlockwise(0);
	rotate_magicbox_bottom(0);
}

//alignment middle layer center point
void step2()
{
	while (1)
	{
		int flag = check_step2();
		if (flag == 1)
		{
			break;
		}
		else if (flag == 2)
		{
			step2_right_hand();
		}
		else if (flag == 3)
		{
			rotate_total_clockwise();
			step2_right_hand();
		}
		else if (flag == 4)
		{
			step2_right_hand();
		}
		else if (flag == 5)
		{
			rotate_total_anticlockwise();
			step2_right_hand();
		}
		else if (flag == 6)
		{
			rotate_total_clockwise();
			rotate_total_clockwise();
			step2_right_hand();
		}
		else if (flag == 7)
		{
			rotate_total_clockwise();
			step2_right_hand();
		}
		else if (flag == 8 || flag==0)
		{
			rotate_magicbox_clockwise(0);
			if (check_step2() != 0)
				continue;
			rotate_magicbox_clockwise(0);
			if (check_step2() != 0)
				continue;
			rotate_magicbox_clockwise(0);
			if (check_step2() != 0)
				continue;
		}
		else;
	}
}

bool check_step3()
{
	bool a = magicbox->front->face[0][2] == 0 &&
		magicbox->above->face[2][2] == magicbox->above->face[1][1] &&
		magicbox->right->face[0][0] == magicbox->right->face[1][1];
	bool b = magicbox->front->face[2][2] == 0 &&
		magicbox->right->face[2][0] == magicbox->right->face[1][1] &&
		magicbox->bottom->face[0][2] == magicbox->bottom->face[1][1];
	bool c = magicbox->front->face[2][0] == 0 &&
		magicbox->left->face[2][2] == magicbox->left->face[1][1] &&
		magicbox->bottom->face[0][0] == magicbox->bottom->face[1][1];
	bool d = magicbox->front->face[0][0] == 0 &&
		magicbox->above->face[2][0] == magicbox->above->face[1][1] &&
		magicbox->left->face[0][2] == magicbox->left->face[1][1];
	if (a && b && c && d)
		return true;
	else
		return false;
}

bool check_step3_right_white_corner()
{
	if (magicbox->front->face[0][2] == 0 &&
		magicbox->above->face[2][2] == magicbox->above->face[1][1] &&
		magicbox->right->face[0][0] == magicbox->right->face[1][1])
		return true;
	else
		return false;
}

void step3_right_hand()
{
	rotate_magicbox_above(2);
	rotate_magicbox_anticlockwise(2);
	rotate_magicbox_bottom(2);
	rotate_magicbox_clockwise(2);
}

//recover white layer
void step3()
{
	while (!check_step3())
	{
		int white_state = -1l;
		if (magicbox->above->face[0][2] == 0 ||
			magicbox->right->face[0][2] == 0 ||
			magicbox->back->face[0][0] == 0)
			white_state = 1;
		else if (magicbox->right->face[2][2] == 0 ||
			magicbox->back->face[2][0] == 0 ||
			magicbox->bottom->face[2][2] == 0)
			white_state = 2;
		else if (magicbox->left->face[2][0] == 0 ||
			magicbox->back->face[2][2] == 0 ||
			magicbox->bottom->face[2][0] == 0)
			white_state = 3;
		else if (magicbox->above->face[0][0] == 0 ||
			magicbox->back->face[0][2] == 0 ||
			magicbox->left->face[0][0] == 0)
			white_state = 4;
		else
		{
			white_state = 5;
			while (check_step3_right_white_corner())
			{
				rotate_total_clockwise();
			}
		}
		//section3
		if (white_state == 1 || white_state == 2 || white_state == 3 || white_state == 4)
		{
			if (white_state == 1)
				;
			else if (white_state == 2)
				rotate_magicbox_anticlockwise(2);
			else if (white_state == 3)
			{
				rotate_magicbox_anticlockwise(2);
				rotate_magicbox_anticlockwise(2);
			}
			else if (white_state == 4)
				rotate_magicbox_clockwise(2);
			else;

			int  b[3] = { magicbox->above->face[0][2],
					magicbox->right->face[0][2],
					magicbox->back->face[0][0] };
			sort(b, b + 3);
			while (1)
			{
				int  a[3] = { magicbox->front->face[1][1],
					magicbox->above->face[1][1],
					magicbox->right->face[1][1] };
				sort(a, a + 3);
				if (a[0] == b[0] && a[1] == b[1] && a[2] == b[2])
					break;
				else
				{
					rotate_magicbox_clockwise(0);
					rotate_magicbox_clockwise(1);
				}
			}
			while (1)
			{
				if (check_step3_right_white_corner() == true)
					break;
				else
					step3_right_hand();
			}
		}
		else if (white_state == 5)
		{ 
			step3_right_hand();
		}
		else;
	}
}

bool check_step4()
{
	bool a = magicbox->above->face[1][2] == magicbox->above->face[1][1] &&
		magicbox->right->face[0][1] == magicbox->right->face[1][1];
	bool b = magicbox->right->face[2][1] == magicbox->right->face[1][1] &&
		magicbox->bottom->face[1][2] == magicbox->bottom->face[1][1];
	bool c = magicbox->bottom->face[1][0] == magicbox->bottom->face[1][1] &&
		magicbox->left->face[2][1] == magicbox->left->face[1][1];
	bool d = magicbox->left->face[0][1] == magicbox->left->face[1][1] &&
		magicbox->above->face[1][0] == magicbox->above->face[1][1];
	if (a && b && c && d)
		return true;
	else
		return false;
}

void step4_right_hand()
{
	rotate_magicbox_above(2);
	rotate_magicbox_anticlockwise(2);
	rotate_magicbox_bottom(2);
	rotate_magicbox_clockwise(2);
}

void step4_left_hand()
{
	rotate_magicbox_above(0);
	rotate_magicbox_clockwise(2);
	rotate_magicbox_bottom(0);
	rotate_magicbox_anticlockwise(2);
}

//recover middle layer
void step4()
{
	while (!check_step4()) 
	{
		int no_yellow_state = -1;
		if (magicbox->above->face[0][1] != 5 && magicbox->back->face[0][1] != 5)
			no_yellow_state = 1;
		else if (magicbox->right->face[1][2] != 5 && magicbox->back->face[1][0] != 5)
			no_yellow_state = 2;
		else if (magicbox->bottom->face[2][1] != 5 && magicbox->back->face[2][1] != 5)
			no_yellow_state = 3;
		else if (magicbox->left->face[1][0] != 5 && magicbox->back->face[1][2] != 5)
			no_yellow_state = 4;
		else if (magicbox->above->face[1][1] != magicbox->above->face[1][2])
			no_yellow_state = 5;
		else if (magicbox->right->face[1][1] != magicbox->right->face[2][1])
			no_yellow_state = 6;
		else if (magicbox->bottom->face[1][0] != magicbox->bottom->face[1][1])
			no_yellow_state = 7;
		else if (magicbox->left->face[0][1] != magicbox->left->face[1][1])
			no_yellow_state = 8;
		else;

		if (no_yellow_state == 1 || no_yellow_state == 2 || no_yellow_state == 3 || no_yellow_state == 4)
		{
			if (no_yellow_state == 1)
				;
			else if (no_yellow_state == 2)
				rotate_magicbox_anticlockwise(2);
			else if (no_yellow_state == 3)
			{
				rotate_magicbox_anticlockwise(2);
				rotate_magicbox_anticlockwise(2);
			}
			else if (no_yellow_state == 4)
				rotate_magicbox_clockwise(2);
			else;
			while (1)
			{
				if (magicbox->above->face[1][1] == magicbox->above->face[0][1])
					break;
				else
				{
					rotate_magicbox_clockwise(0);
					rotate_magicbox_clockwise(1);
				}
			}
			if (magicbox->back->face[0][1] == magicbox->right->face[1][1])
			{
				rotate_magicbox_anticlockwise(2);
				step4_right_hand();
				rotate_total_anticlockwise();
				step4_left_hand();
			}
			else if (magicbox->back->face[0][1] == magicbox->left->face[1][1])
			{
				rotate_magicbox_clockwise(2);
				step4_left_hand();
				rotate_total_clockwise();
				step4_right_hand();
			}
			else
				cout << "error in step 4" << endl;
		}
		else if (no_yellow_state == 5 || no_yellow_state == 6 || no_yellow_state == 7 || no_yellow_state == 8)
		{
			if (no_yellow_state == 5)
				;
			else if (no_yellow_state == 6)
			{
				rotate_total_anticlockwise();
			}
			else if (no_yellow_state == 7)
			{
				rotate_total_clockwise();
				rotate_total_clockwise();
			}
			else if (no_yellow_state == 8)
			{
				rotate_total_clockwise();
			}
			else;
			step4_right_hand();
			rotate_total_anticlockwise();
			step4_left_hand();
		}
		else;
	}
	rotate_total_bottom();
}

bool check_step5()
{
	if (magicbox->above->face[1][1] == 5 &&
		magicbox->above->face[0][1] == 5 &&
		magicbox->above->face[1][0] == 5 &&
		magicbox->above->face[1][2] == 5 &&
		magicbox->above->face[2][1] == 5)
		return true;
	else
		return false;
}
//recover yellow cross
void step5()
{
	while (!check_step5())
	{
		if (magicbox->above->face[1][0] == 5 && magicbox->above->face[1][2] == 5)
			;
		else if (magicbox->above->face[0][1] == 5 && magicbox->above->face[2][1] == 5)
			rotate_total_left();

		else if (magicbox->above->face[1][2] == 5 && magicbox->above->face[2][1] == 5)
			;
		else if (magicbox->above->face[0][1] == 5 && magicbox->above->face[1][2] == 5)
			rotate_total_left();
		else if (magicbox->above->face[2][1] == 5 && magicbox->above->face[1][0] == 5)
			rotate_total_right();
		else if (magicbox->above->face[1][0] == 5 && magicbox->above->face[0][1] == 5)
		{
			rotate_total_left();
			rotate_total_left();
		}

		else if (magicbox->above->face[2][2] == 5)
			;
		else if (magicbox->above->face[0][2] == 5)
			rotate_total_left();
		else if (magicbox->above->face[2][0] == 5)
			rotate_total_right();
		else if (magicbox->above->face[0][0] == 5)
		{
			rotate_total_left();
			rotate_total_left();
		}
		else;

		rotate_magicbox_clockwise(0);
		rotate_magicbox_above(2);
		rotate_magicbox_left(0);
		rotate_magicbox_bottom(2);
		rotate_magicbox_right(0);
		rotate_magicbox_anticlockwise(0);
	}
}

bool check_step6()
{
	if (magicbox->front->face[0][1] == magicbox->front->face[1][1] &&
		magicbox->right->face[0][1] == magicbox->right->face[1][1] &&
		magicbox->back->face[0][1] == magicbox->back->face[1][1] &&
		magicbox->left->face[0][1] == magicbox->left->face[1][1])
		return true;
	else
		return false;
}

void step6_right_hand()
{
	rotate_magicbox_clockwise(0);
	
	rotate_magicbox_above(2);
	rotate_magicbox_left(0);
	rotate_magicbox_bottom(2);
	rotate_magicbox_right(0);

	rotate_magicbox_above(2);
	rotate_magicbox_left(0);
	rotate_magicbox_bottom(2);
	rotate_magicbox_right(0);

	rotate_magicbox_clockwise(0);
	rotate_magicbox_clockwise(0);

	rotate_magicbox_above(0);
	rotate_magicbox_right(0);
	rotate_magicbox_bottom(0);
	rotate_magicbox_left(0);
	
	rotate_magicbox_clockwise(0);
}

//alignment yellow cross and side
void step6()
{
	while (!check_step6())
	{
		int flag = 1;
		if (magicbox->left->face[0][1] == magicbox->left->face[1][1] &&
			magicbox->back->face[0][1] == magicbox->back->face[1][1])
			;
		else if (magicbox->front->face[0][1] == magicbox->front->face[1][1] &&
			magicbox->left->face[0][1] == magicbox->left->face[1][1])
			rotate_total_left();
		else if (magicbox->back->face[0][1] == magicbox->back->face[1][1] &&
			magicbox->right->face[0][1] == magicbox->right->face[1][1])
			rotate_total_right();
		else if (magicbox->right->face[0][1] == magicbox->right->face[1][1] &&
			magicbox->front->face[0][1] == magicbox->front->face[1][1])
		{
			rotate_total_right();
			rotate_total_right();
		}
		else if (magicbox->front->face[0][1] == magicbox->front->face[1][1] &&
			magicbox->back->face[0][1] == magicbox->back->face[1][1])
			;
		else if (magicbox->left->face[0][1] == magicbox->left->face[1][1] &&
			magicbox->right->face[0][1] == magicbox->right->face[1][1])
			rotate_total_right();
		else
		{
			flag = 0;
			rotate_magicbox_left(0);
		}
		if (flag == 1)
			step6_right_hand();
	}
}

bool check_step7_corner00() 
{
	int c[3] = { magicbox->above->face[0][0],magicbox->back->face[0][2] ,magicbox->left->face[0][0] };
	int cc[3] = { magicbox->above->face[1][1],magicbox->back->face[1][1] ,magicbox->left->face[1][1] };
	sort(c, c + 3);
	sort(cc, cc + 3);
	if (c[0] == cc[0] && c[1] == cc[1] && c[2] == cc[2])
		return true;
	else
		return false;
}

bool check_step7_corner02()
{
	int b[3] = { magicbox->above->face[0][2],magicbox->back->face[0][0] ,magicbox->right->face[0][2] };
	int bb[3] = { magicbox->above->face[1][1],magicbox->back->face[1][1] ,magicbox->right->face[1][1] };
	sort(b, b + 3);
	sort(bb, bb + 3);
	if (b[0] == bb[0] && b[1] == bb[1] && b[2] == bb[2])
		return true;
	else
		return false;
}

bool check_step7_corner20()
{
	int d[3] = { magicbox->above->face[2][0],magicbox->left->face[0][2] ,magicbox->front->face[0][0] };
	int dd[3] = { magicbox->above->face[1][1],magicbox->left->face[1][1] ,magicbox->front->face[1][1] };
	sort(d, d + 3);
	sort(dd, dd + 3);
	if(d[0] == dd[0] && d[1] == dd[1] && d[2] == dd[2])
		return true;
	else
		return false;
}

bool check_step7_corner22()
{
	int a[3] = { magicbox->front->face[0][2],magicbox->above->face[2][2] ,magicbox->right->face[0][0] };
	int aa[3] = { magicbox->front->face[1][1],magicbox->above->face[1][1] ,magicbox->right->face[1][1] };
	sort(a, a + 3);
	sort(aa, aa + 3);
	if (a[0] == aa[0] && a[1] == aa[1] && a[2] == aa[2])
		return true;
	else
		return false;
}

bool check_step7()
{
	if (check_step7_corner00() &&
		check_step7_corner02() &&
		check_step7_corner20() &&
		check_step7_corner22())
		return true;
	else
		return false;
}

void step7_right_hand()
{
	rotate_magicbox_above(2);
	rotate_magicbox_left(0);
	rotate_magicbox_bottom(2);
	rotate_magicbox_right(0);
	rotate_magicbox_above(2);
	rotate_magicbox_left(0);
	rotate_magicbox_bottom(2);
	rotate_magicbox_right(0);
	rotate_magicbox_above(2);
	rotate_magicbox_left(0);
	rotate_magicbox_bottom(2);
	rotate_magicbox_right(0);

	rotate_magicbox_above(0);

	rotate_magicbox_above(2);
	rotate_magicbox_left(0);
	rotate_magicbox_bottom(2);
	rotate_magicbox_right(0);
	rotate_magicbox_above(2);
	rotate_magicbox_left(0);
	rotate_magicbox_bottom(2);
	rotate_magicbox_right(0);
	rotate_magicbox_above(2);
	rotate_magicbox_left(0);
	rotate_magicbox_bottom(2);
	rotate_magicbox_right(0);

	rotate_magicbox_bottom(0);
}

void step7()
{
	while (1)
	{
		bool corner00 = check_step7_corner00();
		bool corner02 = check_step7_corner02();
		bool corner20 = check_step7_corner20();
		bool corner22 = check_step7_corner22();
		if (corner00 && corner02 && corner20 && corner22)
			break;
		else if (corner22)
			;
		else if (corner02)
			rotate_total_left();
		else if (corner20)
			rotate_total_right();
		else if (corner00)
		{
			rotate_total_right();
			rotate_total_right();
		}
		else;
		step7_right_hand();
	}
}

bool check_step8()
{
	bool result = true;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (magicbox->left->face[i][j] != 5)
				result = false;
	return result;
}

void step8_right_hand()
{
	rotate_magicbox_above(2);
	rotate_magicbox_left(0);
	rotate_magicbox_bottom(2);
	rotate_magicbox_right(0);
}

void step8()
{
	rotate_total_anticlockwise();
	while (!check_step8())
	{
		if (magicbox->above->face[0][0] == 5)
		{
			step8_right_hand();
			step8_right_hand();
			step8_right_hand();
			step8_right_hand();
		}
		else if (magicbox->back->face[0][2] == 5)
		{
			step8_right_hand();
			step8_right_hand();
		}
		else
			rotate_magicbox_above(0);
	}
	while (1)
	{
		if (magicbox->front->face[0][0] == magicbox->front->face[1][1])
			break;
		else
			rotate_magicbox_above(0);
	}
}

int main()
{
	////////////////////////////////////
	show_initial();
	creat_magicbox();
	second();



	

	
	disorganize();
	creat_magicbox();
	second();
	step1();
	creat_magicbox();
	second();

	step2();
	creat_magicbox();
	second();
	
	step3();
	creat_magicbox();
	second();

	step4();
	creat_magicbox();
	second();

	step5();
	creat_magicbox();
	second();

	step6();
	creat_magicbox();
	second();

	step7();
	creat_magicbox();
	second();

	step8();
	creat_magicbox();
	second();
	

		/*
		rotate_total_right();
		rotate_total_left();
		rotate_total_bottom();
		rotate_total_clockwise();
		rotate_total_anticlockwise();

		rotate_magicbox_left(0);
		rotate_magicbox_left(1);
		rotate_magicbox_left(2);
		rotate_magicbox_right(0);
		rotate_magicbox_right(1);
		rotate_magicbox_right(2);
		

		
		rotate_magicbox_above(0);
		rotate_magicbox_above(1);
		rotate_magicbox_above(2);
		rotate_magicbox_bottom(0);
		rotate_magicbox_bottom(1);
		rotate_magicbox_bottom(2);
		

		
		rotate_magicbox_clockwise(0);
		rotate_magicbox_clockwise(1);
		rotate_magicbox_clockwise(2);
		rotate_magicbox_anticlockwise(0);
		rotate_magicbox_anticlockwise(1);
		rotate_magicbox_anticlockwise(2);
		*/
	
	_getch();				// 按任意键继续
	closegraph();			// 关闭绘图窗口
	return 0;
	return 0;
}