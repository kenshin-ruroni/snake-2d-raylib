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

#define MAX_SEGMENT_POINTS 200

#define RAD_TO_DEG 180.0/3.14

#define POINT_RADIUS 50

#include "Fruit.h"

struct body_segment{
	Vector2 origin;
	double angle;
};

class Snake {
public:
	Snake();
	virtual ~Snake();

	std::deque<body_segment> body_segments;

	size_t segments_points_size = 0;

	float base = 20,height = 20;

	Vector2 collision_position;
	
	Vector2 last_position = {0,0};
	Vector2 current_position = {0.0,0.0};

	float reptation_frequency = 0.5;
	float reptation_amplitude = 20;
	float omega = 2.0 * M_PIf * reptation_frequency;
	float body_segment_reptation_motion_angle; // extra reptation motion for body segment
	float body_segment_reptation_motion_angle_max_amplitude = 15; // in degrees
	float segment_radius = POINT_RADIUS;
	float half_segment_radius = 0.5 * POINT_RADIUS;
	float points_distance_squared = (POINT_RADIUS)*(POINT_RADIUS);

	Vector2 speed = {1.0,0};

	Vector2 direction = {1.0,0};
	Vector2 perpendicular_direction = {0,1};

	float r_width = 5,r_height  = 20;

	float half_r_width = r_width/2, half_r_height = r_height/2;

	Vector2 t[3]; // head vertices
	Vector2 head_origin;
	float angle = 45;
	float d ,l;
	bool collided = false;
	size_t collision_index = -1;
	Vector2 p;
	Rectangle r;

	Vector2 head_position,last_head_position;

	float direction_angle_in_radians,reptation_angle_in_radians;

	float current_phase , current_amplitude;
	Vector2 reptation_direction, oscillation_direction;
	Vector2 perpendicular_reptation_direction = {0,1};

	inline void initializePosition(Vector2 p){
		 current_position =p;
		 last_position = p;
		 head_position = p;
		 last_head_position = head_position;
	}

	inline void update(std::deque<Fruit> *fruits){

	if ( body_segments.size() > MAX_SEGMENT_POINTS - 1){
			body_segments.pop_back();
	}
	if ( Vector2LengthSqr(current_position - last_position) >= 10) //points_distance_squared)
	{



		current_amplitude = (current_phase = std::sin(omega*GetTime())) * reptation_amplitude;

		oscillation_direction = {current_amplitude*perpendicular_direction.x,
				current_amplitude*perpendicular_direction.y};


		head_position = {last_position.x +  oscillation_direction.x,last_position.y+ oscillation_direction.y};


		reptation_direction=Vector2Normalize({head_position.x-last_head_position.x,head_position.y-last_head_position.y});

		perpendicular_reptation_direction = {reptation_direction.y,-reptation_direction.x};

		reptation_angle_in_radians = std::atan2(reptation_direction.y,reptation_direction.x);
		direction_angle_in_radians = std::atan2(direction.y,direction.x);

		last_head_position = head_position;
		last_position = current_position;
		body_segments.push_front({ head_position,
			(reptation_angle_in_radians )*RAD2DEG });


	}
	// detect collision
	for (size_t i = 0; i < fruits->size();i++){

		l = segment_radius + (*fruits)[i].radius;
		if ( Vector2LengthSqr(head_position-(*fruits)[i].p) <= l*l){
			// collision
			collision_position = head_position;
			collision_index = i;
		}
	}

	if ( collision_index != (size_t)-1 ){
		segments_points_size ++;
		fruits->erase (fruits->begin()+collision_index);
		collision_index = -1;
	}




	p = current_position;
	for ( size_t i =0; i < body_segments.size()/*std::min(segments_points_size,segments.size())*/;i++){
		r= {body_segments[i].origin.x,body_segments[i].origin.y,r_width,r_height};
			//DrawLine(p.x, p.y,segments[i].p.x,segments[i].p.y, LIME);
			body_segment_reptation_motion_angle = std::sin( -current_phase) * 20;
			DrawRectanglePro(r,{half_r_width,half_r_height},body_segments[i].angle+ body_segment_reptation_motion_angle,LIME);
			p = body_segments[i].origin;
			DrawCircle(body_segments[i].origin.x,body_segments[i].origin.y,2,ORANGE);
	}

	if ( false){
	head_origin = {head_position.x-direction.x*0.5,head_position.y-direction.y*0.5};
	t[0]={head_origin.x+base*0.5*perpendicular_direction.x,head_origin.y+base*0.5*perpendicular_direction.y};
	t[1]={head_origin.x-base*0.5*perpendicular_direction.x,head_origin.y-base*0.5*perpendicular_direction.y};
	t[2]={head_origin.x+height*direction.x,head_origin.y+height*direction.y};
	}
	else{
		head_origin = {head_position.x-reptation_direction.x*0.5,head_position.y-reptation_direction.y*0.5};
			t[0]={head_origin.x+base*0.5*perpendicular_reptation_direction.x,head_origin.y+base*0.5*perpendicular_reptation_direction.y};
			t[1]={head_origin.x-base*0.5*perpendicular_reptation_direction.x,head_origin.y-base*0.5*perpendicular_reptation_direction.y};
			t[2]={head_origin.x+height*reptation_direction.x,head_origin.y+height*reptation_direction.y};
	}
	//DrawCircle(current_position.x,current_position.y, half_segment_radius, LIME);

	DrawTriangle(t[0],t[1],t[2],LIME);


	}
};

