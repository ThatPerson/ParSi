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
#include <stdint.h>
int CSV_ON = 0; 
int RAD_ON = 0;
int INTERACTIVE_ON = 0;
int READ_FILE = 0;
typedef struct {
	float x;
	float y;
} Position;

typedef struct {
	float accel;
	float angle;
} Vector;

typedef struct {
	char name[500];
	Vector speed;
	Vector accel;
	Position pos;
	float mass;
	int shown;
	float radius;
} Particle;

typedef struct {
	Vector x;
	Vector y;
} Resolved;

typedef struct {
	Particle a;
	Particle b;
	float time;
	int is_collision;
} TestCase;

#include "sim/parse.c"

int check_collision(Particle p, Particle q) {
	/* To calculate a collision between two items each of which has a radius r, we use Pythag.
	 * Ie, we get sqrt(abs(a.x-b.x)^2 + abs(a.y-b.y))
	 */
	float dist = sqrt(pow(abs(p.pos.x-q.pos.x), 2) + pow(abs(p.pos.y-q.pos.y), 2));
	if (dist <= p.radius+q.radius) {
	  	//Collision
	  	return 1;
	}
	return 0;
}

Particle after_graph(Particle q, Particle p) {
	Particle resp = q;
	if (check_collision(p, q) == 1) {
		// There is a collision
	  	printf("WE HAVE COLLISION\n");
	  	//resp.pos.y = sqrt(p.radius - pow(q.pos.x, 2));
		resp.accel.angle = 360-resp.accel.angle;
		resp.speed.angle = 360-resp.speed.angle;
	}
	return resp;
}

float power(float val, int power) {
	float res = 1;
	int i;
	for (i = 0; i < power; i ++) {
		res = res*val;
	}
	return res;
}

float deg_to_rad(float degrees) {
	// degrees * pi/180
	return degrees * M_PI / 180;
}

float rad_to_deg(float radians) {
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

Vector new_accel(float accel, float angle) {
	Vector f;
	f.accel = accel;
	f.angle = angle;
	return f;
}

Resolved vtof(Vector f) {
	Resolved retur;
	retur.x.angle = 90; //It is 90 degrees from the y axis
	retur.y.angle = 0;
	
	if (f.angle >= 0 && f.angle < 90) {
		retur.x.accel = f.accel * sin(deg_to_rad(f.angle));
		retur.y.accel = f.accel * cos(deg_to_rad(f.angle));
	} else if (f.angle >= 90 && f.angle < 180) {
		f.angle = f.angle - 90;
		retur.x.accel = f.accel * cos(deg_to_rad(f.angle));
		retur.y.accel = -f.accel * sin(deg_to_rad(f.angle));
	} else if (f.angle >= 180 && f.angle < 270) {
		f.angle = f.angle - 180;
		retur.x.accel = -f.accel * sin(deg_to_rad(f.angle));
		retur.y.accel = -f.accel * cos(deg_to_rad(f.angle));
	} else {
		f.angle = f.angle - 270;
		retur.x.accel = -f.accel * cos(deg_to_rad(f.angle));
		retur.y.accel = f.accel * sin(deg_to_rad(f.angle));
	}
	
	return retur;
}

Vector ftov(Resolved f) {
	Vector p;
	
	p.accel = sqrt(power(f.x.accel, 2) + power(f.y.accel, 2));

	if ((f.x.accel < 0)	&& (f.y.accel < 0)) {
		// it matches >= 180 < 270
		p.angle = atan((absol(f.x.accel)/absol(f.y.accel)));
		p.angle = 180 + rad_to_deg(p.angle);
	} else if ((f.x.accel < 0) && (f.y.accel >= 0)) {
		// then it is in the final one, or > 270
		p.angle = atan(f.y.accel/absol(f.x.accel));
		p.angle = 270 + rad_to_deg(p.angle);
	} else if ((f.x.accel >= 0) && (f.y.accel < 0)) {
		// then it is >= 90, < 180
		p.angle = atan(absol(f.y.accel)/f.x.accel);
		p.angle = 90 + rad_to_deg(p.angle);
	} else {
		// then it is the first one, or 0 < x < 90
		p.angle = atan(f.x.accel/f.y.accel);
		p.angle = rad_to_deg(p.angle);
	}
	return p;
}

Vector grav_accel(Particle a, Particle b) {
	float xdiff = b.pos.x - a.pos.x;
	float ydiff = b.pos.y - a.pos.y;

	float r = (xdiff * xdiff) + (ydiff * ydiff);
	//Simple bit of pythag to get the difference
	//For this next bit, we use the fact that F = ma
	// and a = MG/r (We did not take the sqrt of the pythag
	// In this, we are solving for the affect on a. So, M is equal to b.mass. G is the gravitational constant
	
	//Oh Ben you idiot, this entire section is wrong! That will just end up wrong
	// Right, no time now to fix it, it is on my TODO list.
	/*float q = g/b.mass;
	q = q/r;
	// F = m * a
	float accel = a.mass * q;
	*/
	float top = 6.67 * a.mass * b.mass;
	top = top * pow(10, -11);
	float ma = top/pow(r,2);
	float accel = ma/a.mass;
	// I believe we need to work on this bit, as it currentl pulls both in the same direction, which is not good.
	Resolved w;
	w.x.accel = xdiff;
	w.x.angle = 90;
	w.y.accel = ydiff;
	w.y.angle = 0;
	Vector ret = ftov(w);
	
	/*if (ret.angle <= 90) {
		ret.angle += 180;
	} else if (ret.angle <= 180) {
		ret.angle += 270;
	} else if (ret.angle <= 270) {
		ret.angle -= 180;
	} else {
		ret.angle -= 180;
	}*/

	ret.accel = accel;
	return ret;
}

Vector balance_accel(Vector a, Vector b) {


	Resolved ax = vtof(a);
	Resolved bx = vtof(b);


	Position new;
	new.x = ax.x.accel + bx.x.accel;
	new.y = ax.y.accel + bx.y.accel;

	//printf("New %f %f\n", new.x, new.y);

	Vector result;

	result.accel = sqrt((new.x*new.x)+(new.y*new.y)); // if the values are negative then them times themselves is positive. For it to be under 0, such as sqrt(-1) it would need to be a complex number, hence no validation
	float tmpangle;
	//printf("New %f %f\n", absol(new.x), absol(new.y));
	if (new.x >= 0 && new.y >= 0) {
		tmpangle = rad_to_deg(atan(absol(new.x)/absol(new.y)));
		// We are in the top right
	} else if (new.x >= 0 && new.y <= 0) {
		tmpangle = rad_to_deg(atan(absol(new.y)/absol(new.x))) + 90;
	} else if (new.x <= 0 && new.y <= 0) {
		tmpangle = rad_to_deg(atan(absol(new.x)/absol(new.y))) + 180;
	} else if (new.x <= 0 && new.y >= 0) {
		tmpangle = rad_to_deg(atan(absol(new.y)/absol(new.x))) + 270;
	}
	result.angle = tmpangle;
	return result;
}

Position wait(Particle a, float time) {
	// s = ut+1/2at^2
	float xinc, yinc;
	Resolved x = vtof(a.accel);
	//s = 1/2at^2

	//We need to resolve the speed of the particle into x and y to use in s=ut+1/2at^2. Because resolve can do this, we just make a new Vector with the speed as the accel
	Resolved p = vtof(a.speed);

	//xinc = (p.x.accel*time) + ((x.x.accel*time*time)/2);
	//yinc = (p.y.accel*time) + ((x.y.accel*time*time)/2);
	// What on earth went on there?
	xinc = (p.x.accel*time) + ((x.x.accel/2)*(time*time));
	yinc = (p.y.accel*time) + ((x.y.accel/2)*(time*time));
	Position l;
	l.x = a.pos.x + xinc;
	l.y = a.pos.y + yinc;
	return l;
}

void print_position(Position l) {
	printf("Position:\n\tX: %f\n\tY: %f\n", l.x, l.y);
}

void print_accel(Vector i) {
	printf("Particle\n\tPower: %fN\n\tAngle: %f\n", i.accel, i.angle);
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
	

Particle new_particle(float x, float y, float accel, float angle) {
	Particle t;
	t.pos.x = x;
	t.pos.y = y;
	t.accel.accel = accel;
	t.accel.angle = angle;
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
		fprintf(stream, (csv == 0)?"%10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s\n":"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", "Time", "Name", "X", "Y", "Accel", "Force",  "Angle", "Speed", "SAngle", "Mass", "Radius");
		if (csv == 0) {
			for (i = 0; i < 109; i++) {
				fprintf(stream, "#");
			}
			fprintf(stream, "\n");
		}
	}
	for (i = 0; i < count; i++) {
		if (p[i].shown == 1) {
			fprintf(stream,(csv == 0)?"%10.2G %10s %10.2G %10.2G %10.2G %10g %10.2G %10.2G %10.2G %10g %10.2g\n":"%f,%s,%f,%f,%f,%G,%f,%f,%f,%G, %g\n", time, p[i].name, p[i].pos.x, p[i].pos.y, p[i].accel.accel,(((float)p[i].mass) * p[i].accel.accel), (radians == 1)?deg_to_rad(p[i].accel.angle):p[i].accel.angle, p[i].speed.accel, (radians == 1)?deg_to_rad(p[i].speed.angle):p[i].speed.angle,(p[i].mass), p[i].radius);
		} else {
			fprintf(stream,(csv == 0)?"%10.2f %10s %10s %10s %10s %10s %10s %10s %10s %10s\n":"%f,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", time, p[i].name, "-", "-", "-", "-", "-","-","-","-");
		}
	}
}	

Vector get_speed(Particle a, float time) {
	// v = u + at. a = a.accel.accel. t = time. u = a.speed.
	
	Resolved q = vtof(a.speed);
	Resolved p = vtof(a.accel);
	
	float xsp = q.x.accel + (p.x.accel*time);
	float ysp = q.y.accel + (p.y.accel*time);
	
	
	Resolved l;
	l.x.accel = xsp;
	l.y.accel = ysp;
	l.x.angle = 90;
	l.y.angle = 0;
	return ftov(l);
}

void wait_all(Particle p[], int count, float time, float display_time, int show_headers, int radians, FILE * output) {
	Vector * temp;
	temp = (Vector *) malloc (count * sizeof(Vector *));
	int i, o;
	Vector tmp;
	float waittime = time;
	TestCase particle_collision;
	for (i = 0; i < count; i ++ ){
		if (p[i].shown == 1) {
			temp[i] = p[i].accel;
			for (o = 0; o < count; o++) {
			  	if (o != i && p[o].shown == 1) {
					p[i].accel = balance_accel(p[i].accel, grav_accel(p[i], p[o]));
				}
			}
		}

	}
	for (i = 0; i < count; i ++ ){ 
		if (p[i].shown == 1) {
			waittime = time;
	  		particle_collision.is_collision = 0;
//			for (o = 0; o < count; o ++ ){
		/*		if (o != i && p[o].shown == 1) {
					p[i].accel = balance_accel(p[i].accel, grav_accel(p[i], p[o]));
				// Eeek. This is using the updated values for the positions in the particles. Try doing this in the first loop with setting temp[i] to prevent this.
				}
			}*/
			/*
				* Right. We have the balanced accels in the particle accel. The original is in temp. 
				* Unfortunately, it is INCREDIBLY computationally taxing to do is_collide with gravity.
				* For this reason, and ONLY this reason, we do not use it in this. Instead we are very 
				* careful, and instead we ASSUME that the person is running this relatively often. Ie,
				* if you run this for 6 seconds with 0.01 second intervals, it will be pretty accurate.
				* of course, if very high speeds are involved, it may miss (ie just checks that the x 
				* and y value differences are less than the time update (time) times 10. This should 
				* work for most, however if you are doing very important stuff, you may want to find
				* another way.
			*/
			p[i].pos = wait(p[i], waittime);
			p[i].speed = get_speed(p[i], waittime);
			tmp = p[i].accel;
			p[i].accel = temp[i];
			temp[i] = tmp;
		}
	}
	for (i = 0; i < count; i ++) {
		if (p[i].shown == 1) {
  			  for (o = i+1; o < count; o++) {
				if (p[o].shown == 1) {		  
		  		/*	float xdiff = absol(p[i].pos.x - p[o].pos.x);
					float ydiff = absol(p[i].pos.y - p[o].pos.y);
					float maj = xdiff + ydiff;
					if (maj < (time)) {
						//Collision (more or less
						p[o].shown = 0;
						p[i].accel = balance_accel(p[i].accel, p[o].accel);
						p[i].mass += p[o].mass;
						p[i].speed = balance_accel(p[i].speed, p[o].speed);
					}*/
				  	p[i] = after_graph(p[i], p[o]);
				}
			}
		}	
	}
	for (i = 0; i < count; i++ ){
		if (p[i].shown == 1) {
	  		tmp = p[i].accel;
			p[i].accel = temp[i];
			temp[i] = tmp;
		}
	}
	tabulate_particles(p, count, display_time, CSV_ON, show_headers, radians, output);
	for (i = 0; i < count; i++) {
		p[i].accel = temp[i];
	}
	free(temp);
	//Make it do collision detection down here.
	return;
		
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
				case 1: p.accel.accel = atof(buffer); break;
				case 2: p.accel.angle = atof(buffer); break;
				case 3: p.speed.accel = atof(buffer); break;
				case 4: p.speed.angle = atof(buffer); break;
				case 5: p.pos.x = atof(buffer); break;
				case 6: p.pos.y = atof(buffer); break;
				case 7: p.mass = atof(buffer); printf("%f\n", p.mass); break;
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
	char * file;
	FILE * output = stdout;
	for (pq = 1; pq < argc; pq++) {
	  	if (strcmp(argv[pq], "-c") == 0) {
			CSV_ON = 1;
		} else if (strcmp(argv[pq], "-r") == 0) {
			RAD_ON = 1;
		} else if (strcmp(argv[pq], "-i") == 0) {
		  	INTERACTIVE_ON = 1;
		} else if (strcmp(argv[pq], "-if") == 0) {
			if (pq < argc-1) {
				file = (char *) malloc(strlen(argv[pq+1])*sizeof(char *));
				strcpy(file, argv[pq+1]);
				READ_FILE = 1;
			}
			//file = (char *) malloc (sizeof(argv[pq+1]));
			//strcpy(file, argv[pq+1]);
		} else if (strcmp(argv[pq], "-of") == 0) {
			if (pq < argc-1)
				output = fopen(argv[pq+1], "w");
		} else if (strcmp(argv[pq], "-h") == 0) {
		  	printf("-c Enter CSV output mode");
			printf("\n-r Use Radians as opposed to degrees");
			printf("\n-if Use .sim file as basis for simulation");
			printf("\n-of Put output to file.");
			printf("\n-i enter interactive mode.");
			printf("\nAn example of this would be:\n\tParSi -if file.sim -of output.csv -r -c\n");
			return 1;
		}
	}
  	if (READ_FILE == 0) {
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
			//p[0] = string_to_particle("Cannon,9.8,180,4,45,25,10,0,");
			//p[1] = string_to_particle("Simulation,9.8,180,17.34705,0,11.879393,0,0,");
			//p[2] = string_to_particle("Cannonball,9.8,180,8,45,0,25,0,");
			p[0] = string_to_particle("Large,0,0,0,0,0,0,1000000000000000000000");
			p[1] = string_to_particle("Small,0,0,0,0,50,50,10000000000000");
		  	curr = 2;
		}
		int i;
		for (i = 0; i < 60; i++) {
			wait_all(p, curr, 0.1, i*0.1, (i==0)?1:0, RAD_ON, output);
		}
	} else {
		Response p = get_config(file);
		float qr;
	//	printf("%f %f %f %f %f %s", p.items[0].pos.x, p.items[0].pos.y, p.items[0].accel.accel, p.items[0].accel.angle, p.items[0].mass, p.items[0].name);
		for (qr = 0; qr < p.time;) {
	 		wait_all(p.items, p.length, p.inc, qr, (qr == 0)?1:0, RAD_ON, output);
			qr += p.inc;
		}
	}
	//free(file);
}

