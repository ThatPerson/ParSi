/*
 * main.c
 * 
 * Copyright 2013 Ben Tatman <ben@tatmans.co.uk>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <stdio.h>
#include <math.h>

#define PI 3.14159265358

typedef struct {
	float x;
	float y;
} Position;

typedef struct {
	float force;
	float angle;
} Force;

typedef struct {
	Force force;
	Position pos;
} Particle;

typedef struct {
	Force x;
	Force y;
} Resolved;

float to_radians(float degrees) {
	// degrees * pi/180
	return degrees * PI / 180;
}

float to_degrees(float radians) {
	return radians / (PI/180);

	//=B2/(D1/180)
}

Resolved resolve(Force f) {
	Resolved retur;
	retur.x.angle = 90; //It is 90 degrees from the y axis
	retur.y.angle = 0;
	
	if (f.angle >= 0 && f.angle < 90) {
		retur.x.force = f.force * sin(to_radians(f.angle));
		retur.y.force = f.force * cos(to_radians(f.angle));
	} else if (f.angle >= 90 && f.angle < 180) {
		f.angle = f.angle - 90;
		retur.x.force = f.force * cos(to_radians(f.angle));
		retur.y.force = -f.force * sin(to_radians(f.angle));
	} else if (f.angle >= 180 && f.angle < 270) {
		f.angle = f.angle - 180;
		retur.x.force = -f.force * sin(to_radians(f.angle));
		retur.y.force = -f.force * cos(to_radians(f.angle));
	} else {
		f.angle = f.angle - 270;
		retur.x.force = -f.force * cos(to_radians(f.angle));
		retur.y.force = f.force * sin(to_radians(f.angle));
	}
	
	return retur;
}

float absol(float in) {
	float new = in;
	if (in < 0) {
		new = new - (2*in); // two times the number would get enough to make up for each side, then - because - - is +. For example, if in = -3, then we get -3 - (-3*2), so -3 - - 6, so -3 + 6, so 3.
	}
	return new; //If value is +, then we do nothing.
}

Force balance_force(Force a, Force b) {


	Resolved ax = resolve(a);
	Resolved bx = resolve(b);


	Position new;
	new.x = ax.x.force + bx.x.force;
	new.y = ax.y.force + bx.y.force;

	printf("New %f %f\n", new.x, new.y);

	Force result;

	result.force = sqrt((new.x*new.x)+(new.y*new.y)); // if the values are negative then them times themselves is positive. For it to be under 0, such as sqrt(-1) it would need to be a complex number, hence no validation
	float tmpangle;
	printf("New %f %f\n", absol(new.x), absol(new.y));
	if (new.x >= 0 && new.y >= 0) {
		printf("0 - 90\n");
		tmpangle = to_degrees(atan(absol(new.x)/absol(new.y)));
		// We are in the top right
	} else if (new.x >= 0 && new.y <= 0) {
		printf("90 - 180\n");
		tmpangle = to_degrees(atan(absol(new.y)/absol(new.x))) + 90;
	} else if (new.x <= 0 && new.y <= 0) {
		printf("180 - 270\n");
		tmpangle = to_degrees(atan(absol(new.x)/absol(new.y))) + 180;
	} else if (new.x <= 0 && new.y >= 0) {
		printf("270 - 360\n");
		tmpangle = to_degrees(atan(absol(new.y)/absol(new.x))) + 270;
	}
	result.angle = tmpangle;
	return result;
}

Position wait(Particle a, float time) {
	// s = ut+1/2at^2
	float s = (a.force.force * (time*time))/2;
	float xinc, yinc;
	if (a.force.angle < 90) {
		xinc = s * sin(a.force.angle);
		yinc = s * cos(a.force.angle);
	} else if (a.force.angle < 180) {
		float q;
		q = a.force.angle - 90;
		xinc = s * cos(q);
		yinc = s * sin(q);
	} else if (a.force.angle < 270) {
		float q = a.force.angle - 180;
		xinc = -(s * sin(q));
		yinc = -(s * cos(q));
	} else {
		float q = a.force.angle - 270;
		xinc = -(s * cos(q));
		yinc = s * sin(q);
	}
	Position l;
	l.x = a.pos.x + xinc;
	l.y = a.pos.y + yinc;
	return l;
}

void print_position(Position l) {
	printf("Position:\n\tX: %f\n\tY: %f\n", l.x, l.y);
}

void print_force(Force i) {
	printf("Particle\n\tPower: %fN\n\tAngle: %f\n", i.force, i.angle);
}

int main(int argc, char * argv) {
	Particle p;
	p.pos.x = 10;
	p.pos.y = 40;
	p.force.angle = 0;
	p.force.force = 5;

	Force l, q;
	q.force = 14;
	q.angle = 110;
	l.force = 8;
	l.angle = 210;

	Force res = balance_force(q, l);
	print_force(q);
	print_force(l);
	print_force(res);

	print_position(wait(p, 2));

}

