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
#include <string.h>
#include <stdlib.h>

typedef struct {
	float x;
	float y;
} Position;

typedef struct {
	float force;
	float angle;
} Force;

typedef struct {
	char name[500];
	float speed;
	Force force;
	Position pos;
	int shown;
} Particle;

typedef struct {
	Force x;
	Force y;
} Resolved;

typedef struct {
	Particle a;
	Particle b;
	float time;
	int is_collision;
} TestCase;


float to_radians(float degrees) {
	// degrees * pi/180
	return degrees * M_PI / 180;
}

float to_degrees(float radians) {
	return radians / (M_PI/180);

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
		//new = new - (2*in); // two times the number would get enough to make up for each side, then - because - - is +. For example, if in = -3, then we get -3 - (-3*2), so -3 - - 6, so -3 + 6, so 3.
		new = 0 - in;
	}
	return new; //If value is +, then we do nothing.
}

Force balance_force(Force a, Force b) {


	Resolved ax = resolve(a);
	Resolved bx = resolve(b);


	Position new;
	new.x = ax.x.force + bx.x.force;
	new.y = ax.y.force + bx.y.force;

	//printf("New %f %f\n", new.x, new.y);

	Force result;

	result.force = sqrt((new.x*new.x)+(new.y*new.y)); // if the values are negative then them times themselves is positive. For it to be under 0, such as sqrt(-1) it would need to be a complex number, hence no validation
	float tmpangle;
	//printf("New %f %f\n", absol(new.x), absol(new.y));
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
	float xinc, yinc;
	Resolved x = resolve(a.force);
	//s = 1/2at^2
	xinc = ((x.x.force*time*time)/2);
	yinc = ((x.y.force*time*time)/2);
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

int poscmp(Position a, Position b) {
	if ((a.x == b.x) && (a.y == b.y)) {
		return 1;
	}
	return -1;
}

TestCase is_collision(TestCase q) {
	float xdist = absol(q.a.pos.x - q.b.pos.x);
	float ydist = absol(q.a.pos.y - q.b.pos.y);
	//printf("XDIST %f YDIST %f\n", xdist, ydist);
	TestCase retur = q;
	retur.is_collision = 0;
	if (xdist > ydist) {
		//printf("H");
		Resolved ar, br;
		ar = resolve(q.a.force);
		br = resolve(q.b.force);
		//printf("ar x %f ar y %f, br x %f br y %f\n", ar.x.force, ar.y.force, br.x.force, br.y.force); 
		float dt = (xdist/(absol(ar.x.force - br.x.force))) * absol(ar.x.force); //If the y force is negative it is heading towards it, so it would become a positive, else it would be a negative
		float l = 2 * dt;
		float z = l / ar.x.force;
		//printf("%f\n", ar.x.force);
		float t = sqrt(z);
		
		Position ac = wait(q.a, t);
		Position bc = wait(q.b, t);
		//printf("AR: \n\tX %f\n\tY %f\nBR: \n\tX %f\n\tY %f\nDT %f\nL %f\nZ %f\nT %f\nAC\n\tX %f\n\tY %f\nBC\n\tX %f\n\tY %f\n", 
						//ar.x.force,	ar.y.force,		  br.x.force,	  br.y.force, dt,    l,    z,    t,          ac.x,   ac.y,       bc.x,   bc.y);
		ac.x = floorf(ac.x * 100+0.5)/100;
		ac.y = floorf(ac.y * 100+0.5)/100;
		bc.x = floorf(bc.x * 100+0.5)/100;
		bc.y = floorf(bc.y * 100+0.5)/100;
		if ((ac.x == bc.x) && (ac.y == bc.y)) {
			retur.time = t;
			retur.is_collision = 1;
		}
	} else {
		//printf("Q");
		Resolved ar, br;
		ar = resolve(q.a.force);
		br = resolve(q.b.force);
		float dt = (ydist/(absol(ar.y.force - br.y.force))) * absol(br.y.force); //If the y force is negative it is heading towards it, so it would become a positive, else it would be a negative
		
		// s = 1/2at^2
		// dt = s
		// l = at^2
		// z = t^2
		
		float l = 2 * dt;
		float z = l / br.y.force;
		float t = sqrt(z);
		
		Position ac = wait(q.a, t);
		//printf("q.a.force.force %f q.a.force.angle %f q.a.pos.x %f q.a.pos.y %f", q.a.force.force, q.a.force.angle, q.a.pos.x, q.a.pos.y);
		Position bc = wait(q.b, t);
		
		//printf("AR: \n\tX %f\n\tY %f\nBR: \n\tX %f\n\tY %f\nDT %f\nL %f\nZ %f\nT %f\nAC\n\tX %f\n\tY %f\nBC\n\tX %f\n\tY %f\n", 
		//				ar.x.force,	ar.y.force,		  br.x.force,	  br.y.force, dt,    l,    z,    t,          ac.x,   ac.y,       bc.x,   bc.y);
		
		/*float ydistc = ac.x+bc.y;
		float xdistc = ac.y+bc.x;
		
		ydistc = floorf(ydistc * 100 + 0.5) / 100;
		xdistc = floorf(xdistc * 100 + 0.5) / 100;

		printf("xdistc %f ydistc %f\n", xdistc, ydistc);
		
		if ((ydistc == ydist) && (xdistc == xdist)) {*/
		
		ac.x = floorf(ac.x * 100+0.5)/100;
		ac.y = floorf(ac.y * 100+0.5)/100;
		bc.x = floorf(bc.x * 100+0.5)/100;
		bc.y = floorf(bc.y * 100+0.5)/100;
		
		if ((ac.x == bc.x) && (ac.y == bc.y)) {
			retur.time = t;
			retur.is_collision = 1;
		}
	}
	return retur;
}

Particle new_particle(float x, float y, float force, float angle) {
	Particle t;
	t.pos.x = x;
	t.pos.y = y;
	t.force.force = force;
	t.force.angle = angle;
	t.shown = 1;
	return t;
}

Force new_force(float force, float angle) {
	Force f;
	f.force = force;
	f.angle = angle;
	return f;
}

Position new_position(float x, float y) {
	Position p;
	p.x = x;
	p.y = y;
	return p;
}

void tabulate_particles(Particle p[], int count, float time) {
	printf("%10s %10s %10s %10s %10s %10s %10s\n", "Time", "Name", "X", "Y", "Force", "Angle", "Speed");
	int i;
	for (i = 0; i < 76; i++) {
		printf("-");
	}
	printf("\n");
	for (i = 0; i < count; i++) {
		if (p[i].shown == 1) {
			printf("%10f %10s %10.2f %10.2f %10.2f %10.2f %10.2f\n", time, p[i].name, p[i].pos.x, p[i].pos.y, p[i].force.force, p[i].force.angle, p[i].speed);
		}
	}
	printf("\n");
}	

void wait_all(Particle p[], int count, float time) {
	int i, o;
	for (i = 0; i < count; i++) {
		for (o = i; o < count; o++) {
			if (p[0].shown == 1 && p[1].shown == 1) {
				TestCase qlo;
				qlo.a = p[i];
				qlo.b = p[o];
				qlo = is_collision(qlo);
				if (qlo.is_collision == 1) {
					if (qlo.time < time) {
						p[i].force = balance_force(p[i].force, p[o].force);
						p[o] = new_particle(0,0,0,0);
						p[o].shown = -1;
					}
				}
			}		
		}
	}
	printf("%10s %10s %10s %10s %10s %10s %10s\n", "Time", "Name", "X", "Y", "Force", "Angle", "Speed");
	for (i = 0; i < 76; i ++) {
		printf("-");
	}
	printf("\n");
	Position tmp;
	for (i = 0; i < count; i++) {
		if (p[i].shown == 1) {
			tmp = wait(p[i], time);
			printf("%10f %10s %10.2f %10.2f %10.2f %10.2f %10.2f\n", time, p[i].name, tmp.x, tmp.y, p[i].force.force, p[i].force.angle, p[i].speed);
		}
	}

}						

int main(int argc, char * argv) {
	Particle p[4];
	p[0] = new_particle(0,0,4,45);
	strcpy(p[0].name,"Test");
	p[0].speed = 0;
	p[1] = new_particle(25,0,4,315);
	strcpy(p[1].name,"Sim");
	p[1].speed = 0;
	p[2] = new_particle(0, 30, 20, 90);
	p[2].speed = 0;
	strcpy(p[2].name, "Followup");
	p[3] = new_particle(0, 40, 2, 90);
	strcpy(p[3].name, "Miss");
	p[3].speed = 0;
	int i;
	for (i = 0; i < 90; i++) {
		wait_all(p, 4, i*0.1);
	}
}

