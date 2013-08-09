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

float power(float val, int power) {
	float res = 1;
	int i;
	for (i = 0; i < power; i ++) {
		res = res*val;
	}
	return res;
}

float to_radians(float degrees) {
	// degrees * pi/180
	return degrees * M_PI / 180;
}

float to_degrees(float radians) {
	return radians / (M_PI/180);

	//=B2/(D1/180)
}


Force new_force(float force, float angle) {
	Force f;
	f.force = force;
	f.angle = angle;
	return f;
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

	//We need to resolve the speed of the particle into x and y to use in s=ut+1/2at^2. Because resolve can do this, we just make a new Force with the speed as the force
	Resolved p = resolve(new_force(a.speed, a.force.angle));

	xinc = (p.x.force*time) + ((x.x.force*time*time)/2);
	yinc = (p.y.force*time) + ((x.y.force*time*time)/2);
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

Position posdiff(Position a, Position b) {
	Position l;
	l.x = absol(a.x)+absol(b.x);
	l.y = absol(a.y)+absol(b.y);
	return l;
}

TestCase is_collision(TestCase q) {
	/*
	 * This uses a graphing mechanism. Basically it works out the equation of the line of a particle, and 
	 * uses this to work out the intercept. Then, using Pythag it works out the distance for each to reac
	 * h said point, and then uses t = (-u + sqrt(2sa+u^2))/a to work out the time required. Then, it com
	 * pares the two times, and if they are the same (well, 1/1000th margin for change) it will regard it 
	 * as true
	 */
	// Firstly we do A
	Resolved a = resolve(q.a.force);
	float gradianta = a.y.force/a.x.force;
	float ac = q.a.pos.y - (gradianta*q.a.pos.x); // y-px = c, use the position values as y and x
	// hence, formula = y = gradiant * x + ac
	// And now we do B
	Resolved b = resolve(q.b.force);
	float gradiantb = b.y.force/b.x.force;
	float bc = q.b.pos.y - (gradiantb*q.b.pos.x);

	// Now,   gradianta*x + ac = gradiantb*x + bc
	// Hence, gradianta*x - gradiantb*x = bc - ac
	//        x*(gradianta-gradiantb) = bc - ac
	//        x = (bc - ac)/(gradianta-gradiantb)
	float x = (bc - ac)/(gradianta-gradiantb);
	float y = (gradianta * x) + ac; // Well, if y=gradianta*x+ac, why not just use it here?

	// Now we find out the distances for each
	float ax = absol(q.a.pos.x - x);
	float ay = absol(q.a.pos.y - y);
	float ac2 = (power(ax,2)) + (power(ay,2));
	float al = sqrt(ac2);

	// And for B
	float bx = absol(q.b.pos.x - x);
	float by = absol(q.b.pos.y - y);
	float bc2 = power(bx,2) + power(by,2);
	float bl = sqrt(bc2);

	// So, we have s values for each. u was given to us (q.a.speed), as was a (q.a.force.force) so we can go ahead
	float at = ((-q.a.speed + sqrt((2*al*q.a.force.force)+(power(q.a.speed,2))))/q.a.force.force);
	float bt = ((-q.b.speed + sqrt((2*bl*q.b.force.force)+(power(q.b.speed,2))))/q.b.force.force);
	at = ceilf(at * 100) / 100;
	bt = ceilf(bt * 100) / 100;
	if (at == bt) {
		q.time = at;
		q.is_collision = 1;
	} else {
		q.time = 0;
		q.is_collision = 0;
	}
	return q;
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

float get_speed(Particle a, float time) {
	// v = u + at. a = a.force.force. t = time. u = a.speed.
	return a.speed + (a.force.force*time);
}

void wait_all(Particle p[], int count, float time, float display_time) {
	int i,o;
	float waittime = time;
	TestCase watermelon;
	for (i = 0; i < count; i ++) {
		if (p[i].shown == 1) {
			waittime = time;
			watermelon.a = p[i];
			for (o = 0; o < count; o++) {
				if (p[o].shown == 1) {
					watermelon.b = p[o];
					watermelon = is_collision(watermelon);
					if (watermelon.is_collision == 1) {

						if (watermelon.time < (2*time)) {
							waittime -= watermelon.time;
							p[i].pos = wait(p[i], watermelon.time);
							p[o].shown = 0;
							p[i].force = balance_force(p[i].force, p[o].force);
						}
					}
				}
			}
			p[i].pos = wait(p[i], waittime);
			p[i].speed = get_speed(p[i], waittime);

		}
	}
	tabulate_particles(p, count, display_time);
}						

int main(int argc, char * argv[]) {
	
	Particle lo = new_particle(0,0,2,90);
	lo.speed = 2;
	lo.pos = wait(lo, 0.05);
	printf("%f\n", lo.pos.x);


	Particle p[4];

	p[0] = new_particle(0,0,4,45);
	strcpy(p[0].name,"Test");
	p[0].speed = 0;

	Position lq = wait(p[0], 3);
	print_position(lq);

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
		wait_all(p, 4, 0.1, i*0.1);
	}
}

