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
int CSV_ON = 0; 
int RAD_ON = 0;
int INTERACTIVE_ON = 0;
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
	Force speed;
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

float absol(float in) {
	float new = in;
	if (in < 0) {
		//new = new - (2*in); // two times the number would get enough to make up for each side, then - because - - is +. For example, if in = -3, then we get -3 - (-3*2), so -3 - - 6, so -3 + 6, so 3.
		new = 0 - in;
	}
	return new; //If value is +, then we do nothing.
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

Force anti_resolve(Resolved f) {
	Force p;
	
	p.force = sqrt(power(f.x.force, 2) + power(f.y.force, 2));

	if ((f.x.force < 0)	&& (f.y.force < 0)) {
		// it matches >= 180 < 270
		p.angle = atan((absol(f.x.force)/absol(f.y.force)));
		p.angle = 180 + to_degrees(p.angle);
	} else if ((f.x.force < 0) && (f.y.force >= 0)) {
		// then it is in the final one, or > 270
		p.angle = atan(f.y.force/absol(f.x.force));
		p.angle = 270 + to_degrees(p.angle);
	} else if ((f.x.force >= 0) && (f.y.force < 0)) {
		// then it is >= 90, < 180
		p.angle = atan(absol(f.y.force)/f.x.force);
		p.angle = 90 + to_degrees(p.angle);
	} else {
		// then it is the first one, or 0 < x < 90
		p.angle = atan(f.x.force/f.y.force);
		p.angle = to_degrees(p.angle);
	}
	return p;
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
		tmpangle = to_degrees(atan(absol(new.x)/absol(new.y)));
		// We are in the top right
	} else if (new.x >= 0 && new.y <= 0) {
		tmpangle = to_degrees(atan(absol(new.y)/absol(new.x))) + 90;
	} else if (new.x <= 0 && new.y <= 0) {
		tmpangle = to_degrees(atan(absol(new.x)/absol(new.y))) + 180;
	} else if (new.x <= 0 && new.y >= 0) {
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
	Resolved p = resolve(a.speed);

	//xinc = (p.x.force*time) + ((x.x.force*time*time)/2);
	//yinc = (p.y.force*time) + ((x.y.force*time*time)/2);
	// What on earth went on there?
	xinc = (p.x.force*time) + ((x.x.force/2)*(time*time));
	yinc = (p.y.force*time) + ((x.y.force/2)*(time*time));
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

TestCase is_collision(TestCase q, float within, float precision) {
	float i;
	TestCase p = q;
	int curr_num, curr_den;
	float xdiff, ydiff;
	for (i = 0; i < within;) {
		p.a.pos = wait(q.a, i);
		p.b.pos = wait(q.b, i);
		//printf("%f\n", i/precision);
		q.a.pos.x = ceilf(q.a.pos.x * precision) / precision;
		q.a.pos.y = ceilf(q.a.pos.y * precision) / precision;
		q.b.pos.x = ceilf(q.b.pos.x * precision) / precision;
		q.b.pos.y = ceilf(q.b.pos.y * precision) / precision;
		xdiff = p.a.pos.x - p.b.pos.x;
		xdiff = absol(xdiff);
		ydiff = p.a.pos.y - p.b.pos.y;
		ydiff = absol(ydiff);
		if ((ydiff < 0.1) && (xdiff < 0.1)) {
		
			q.is_collision = 1;
			q.time = i;
			return q;
		}
		i = i + within/precision;
	}	
	return q;
}
	

Particle new_particle(float x, float y, float force, float angle) {
	Particle t;
	t.pos.x = x;
	t.pos.y = y;
	t.force.force = force;
	t.force.angle = angle;
	t.speed.angle = angle;
	t.shown = 1;
	return t;
}


Position new_position(float x, float y) {
	Position p;
	p.x = x;
	p.y = y;
	return p;
}

void tabulate_particles(Particle p[], int count, float time, int csv, int headers, int radians, FILE * stream) {
	int i;

	if (headers == 1) {	
		fprintf(stream, (csv == 0)?"%10s %10s %10s %10s %10s %10s %10s %10s\n":"%s,%s,%s,%s,%s,%s,%s,%s\n", "Time", "Name", "X", "Y", "Force", "Angle", "Speed", "SAngle");
		if (csv == 0) {
			for (i = 0; i < 87; i++) {
				fprintf(stream, "-");
			}
			fprintf(stream, "\n");
		}
	}
	for (i = 0; i < count; i++) {
		if (p[i].shown == 1) {
			fprintf(stream,(csv == 0)?"%10f %10s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n":"%f,%s,%f,%f,%f,%f,%f,%f\n", time, p[i].name, p[i].pos.x, p[i].pos.y, p[i].force.force, (radians == 1)?to_radians(p[i].force.angle):p[i].force.angle, p[i].speed.force, (radians == 1)?to_radians(p[i].speed.angle):p[i].speed.angle);
		} else {
			fprintf(stream,(csv == 0)?"%10f %10s %10s %10s %10s %10s %10s %10s\n":"%f,%s,%s,%s,%s,%s,%s,%s\n", time, p[i].name, "-", "-", "-","-","-","-");
		}
	}
	fprintf(stream,"\n");
}	

Force get_speed(Particle a, float time) {
	// v = u + at. a = a.force.force. t = time. u = a.speed.
	
	Resolved q = resolve(a.speed);
	Resolved p = resolve(a.force);
	
	float xsp = q.x.force + (p.x.force*time);
	float ysp = q.y.force + (p.y.force*time);
	
	
	Resolved l;
	l.x.force = xsp;
	l.y.force = ysp;
	l.x.angle = 90;
	l.y.angle = 0;
	return anti_resolve(l);
}

void wait_all(Particle p[], int count, float time, float display_time, int show_headers, int radians, FILE * output) {
	int i,o;
	float waittime = time;
	TestCase watermelon;
	watermelon.is_collision = 0;
	for (i = 0; i < count; i ++) {
		watermelon.is_collision = 0;
		if (p[i].shown == 1) {
			waittime = time;
			watermelon.a = p[i];
			for (o = i+1; o < count; o++) {
				if (p[o].shown == 1) {
					watermelon.b = p[o];
					watermelon.time = 0;
					watermelon.is_collision = 0;
					watermelon = is_collision(watermelon, time, 10);
					if (watermelon.is_collision == 1) {

						if (watermelon.time < (2*time)) {
							waittime -= watermelon.time;
							p[i].pos = wait(p[i], watermelon.time);
							p[o].shown = 0;
							p[i].force = balance_force(p[i].force, p[o].force);
							p[i].speed = balance_force(p[i].speed, p[o].speed);
						}
					}
				}
			}
			p[i].pos = wait(p[i], waittime);
			p[i].speed = get_speed(p[i], waittime);
		}
	}
	tabulate_particles(p, count, display_time, CSV_ON, show_headers, radians, output);
}

Particle string_to_particle(char string[500]) {
	Particle p;
  	char buffer[500];
	int i, l;
	int buffer_pos = 0;
	int buffer_inc = 0;
	p.shown = 1;
	for (i = 0; i < strlen(string); i++) {
	  	if ((string[i] == 44) || (i == (((int)strlen(string))-1))) {
		  	switch (buffer_inc) {
			  	case 0: strcpy(p.name, buffer); break;
				case 1: p.force.force = atof(buffer); break;
				case 2: p.force.angle = atof(buffer); break;
				case 3: p.speed.force = atof(buffer); break;
				case 4: p.speed.angle = atof(buffer); break;
				case 5: p.pos.x = atof(buffer); break;
				case 6: p.pos.y = atof(buffer); break;
			}
		  	buffer_inc++;
			for (l = 0; l < 500; l++) {
			  	buffer[l] = 0;
			}
			buffer_pos = 0;
		} else {
		  	buffer[buffer_pos] = string[i];
			buffer_pos++;
		}
	}
	return p;
}

int count(char str[500][500]) {
  	int i;
	for (i = 0; i < 500; i++) {
	  	if (strcmp(str[i], "")) {
		  	return i;
		}
	}
	return i;
}

void read_lines(Particle * p, char strings[500][500]) {
	int i;
	for (i = 0; i < count(strings); i++) {
	  	p[i] = string_to_particle(strings[i]);
	}
	return;
}

char * substring(char * string, int start) {
  	int i;
	char * resp;

	for (i = start; i < strlen(string); i++) {
		resp[i-start] = string[i];
	}
	return resp;
}	

int main(int argc, char * argv[]) {
	int pq;
	for (pq = 1; pq < argc; pq++) {
	  	if (strcmp(argv[pq], "-c") == 0) {
			CSV_ON = 1;
		} else if (strcmp(argv[pq], "-r") == 0) {
			RAD_ON = 1;
		} else if (strcmp(argv[pq], "-i") == 0) {
		  	INTERACTIVE_ON = 1;
		} else if (strcmp(argv[pq], "-h") == 0) {
		  	printf("-c Enter CSV output mode");
			printf("\n-r Use Radians as opposed to degrees");
			printf("\n-i enter interactive mode.\n");
			return 1;
		}
	}
  	Particle p[500];
	Particle * q;
	q = p;
	int run = 0, curr = 0;
	char temp[500];
	if (INTERACTIVE_ON == 1) {
	  	while (run == 0) {
	  		scanf("%499s", temp);
			if ((temp[0] == 'r') && (temp[1] == 'u') && (temp[2] == 'n')) {
			  	
			  	run = 1;
			} else {
			 	p[curr] = string_to_particle(temp);
				curr++;
			}
		}
	} else {
		p[0] = string_to_particle("Cannon,9.8,180,4,45,25,10,");
		p[1] = string_to_particle("Simulation,9.8,180,17.34705,0,11.879393,0,");
		p[2] = string_to_particle("Cannonball,9.8,180,8,45,0,25,");
		curr = 3;
	}
	int i;
	for (i = 0; i < 60; i++) {
		wait_all(q, curr, 0.1, i*0.1, (i==0)?1:0, RAD_ON,stdout);
	}
}

