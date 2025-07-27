/*
 * Snake.h
 *
 *  Created on: 26 juil. 2025
 *      Author: descourt
 */

#pragma once
#include <deque>
#include <algorithm>

#include "raylib.h"
#include "raymath.h"

#define MAX_SEGMENT_POINTS 100

#define RAD_TO_DEG 180.0/3.14

#define POINT_RADIUS 50

#include "Fruit.h"

struct position_orientation{
	Vector2 p;
	double angle;
};

class Snake {
public:
	Snake();
	virtual ~Snake();

	std::deque<position_orientation> segments;

	size_t segments_points_size = 0;

	float base = 20,height = 20;

	Vector2 collision_position;
	
	Vector2 last_position = {0,0};
	Vector2 current_position = {0.0,0.0};

	float segment_radius = POINT_RADIUS;
	float half_segment_radius = 0.5 * POINT_RADIUS;
	float points_distance_squared = (POINT_RADIUS)*(POINT_RADIUS);

	Vector2 speed = {1.0,0};

	Vector2 direction = {1.0,0};
	Vector2 perpendicular_direction = {0,1};

	float r_width = 10,r_height  = 10;

	float half_r_width = r_width/2, half_r_height = r_height/2;

	Vector2 t[3]; // head vertices
	Vector2 head_origin;
	float angle = 0;
	float d ,l;
	bool collided = false;
	size_t collision_index = -1;
	Vector2 p;
	Rectangle r;

	inline void initializePosition(Vector2 p){
		 current_position =p;
		 last_position = p;
	}

	inline void update(std::deque<Fruit> *fruits){

	if ( segments.size() > MAX_SEGMENT_POINTS - 1){
			segments.pop_back();
	}
	if ( Vector2LengthSqr(current_position - last_position) >= 2.5) //points_distance_squared)
	{
		segments.push_front({last_position,std::atan2(speed.y,speed.x)*RAD2DEG});
		last_position = current_position;
	}
	// detect collision
	for (size_t i = 0; i < fruits->size();i++){

		l = segment_radius + (*fruits)[i].radius;
		if ( Vector2LengthSqr(current_position-(*fruits)[i].p) <= l*l){
			// collision
			collision_position = current_position;
			collision_index = i;
		}
	}

	if ( collision_index != (size_t)-1 ){
		segments_points_size ++;
		fruits->erase (fruits->begin()+collision_index);
		collision_index = -1;
	}




	p = current_position;
	for ( size_t i =0; i < segments.size()/*std::min(segments_points_size,segments.size())*/;i++){
		r= {segments[i].p.x,segments[i].p.y,r_width,r_height};
			//DrawLine(p.x, p.y,segments[i].p.x,segments[i].p.y, LIME);
			DrawRectanglePro(r,{half_r_width,half_r_height},segments[i].angle,LIME);
			p = segments[i].p;
	}

	head_origin = {current_position.x-direction.x*0.5,current_position.y-direction.y*0.5};
	t[0]={head_origin.x+base*0.5*perpendicular_direction.x,head_origin.y+base*0.5*perpendicular_direction.y};
	t[1]={head_origin.x-base*0.5*perpendicular_direction.x,head_origin.y-base*0.5*perpendicular_direction.y};
	t[2]={head_origin.x+height*direction.x,head_origin.y+height*direction.y};

	//DrawCircle(current_position.x,current_position.y, half_segment_radius, LIME);

	DrawTriangle(t[0],t[1],t[2],LIME);

	for ( size_t i =0; i < segments.size()/*std::min(segments_points_size,segments.size())*/;i++){

				DrawCircle(segments[i].p.x,segments[i].p.y,2,ORANGE);
		}

	}
};

