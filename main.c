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

float to_radians(float degrees) {
	// degrees * pi/180
	return degrees * PI / 180;
}

float to_degrees(float radians) {
	return radians / (PI/180);

	//=B2/(D1/180)
}

Force balance_force(Force a, Force b) {
	//For Force a
	Position ax;
	
	if (a.angle >= 0 && a.angle < 90) {
		ax.x = a.force * sin(to_radians(a.angle));
		ax.y = a.force * cos(to_radians(a.angle));
		printf("1 %f %f\n", ax.x, ax.y);
	} else if (a.angle >= 90 && a.angle < 180) {
		a.angle = a.angle - 90;
		ax.x = a.force * cos(to_radians(a.angle));
		ax.y = -a.force * sin(to_radians(a.angle));
		printf("2 %f %f\n", ax.x, ax.y);
	} else if (a.angle >= 180 && a.angle < 270) {
		a.angle = a.angle - 180;
		ax.x = -a.force * sin(to_radians(a.angle));
		ax.y = -a.force * cos(to_radians(a.angle));
		printf("3 %f %f\n", ax.x, ax.y);
	} else {
		a.angle = a.angle - 270;
		ax.x = -a.force * cos(to_radians(a.angle));
		ax.y = a.force * sin(to_radians(a.angle));
		printf("4 %f %f\n", ax.x, ax.y);
	}

	//For Force b
	Position bx;
	
	if (b.angle >= 0 && b.angle < 90) {
		bx.x = b.force * sin(to_radians(b.angle));
		bx.y = b.force * cos(to_radians(b.angle));
		printf("1 %f %f\n", bx.x, bx.y);
	} else if (b.angle >= 90 && b.angle < 180) {
		b.angle = b.angle - 90;
		bx.x = b.force * cos(to_radians(b.angle));
		bx.y = -b.force * sin(to_radians(b.angle));
		printf("2 %f %f\n", bx.x, bx.y);
	} else if (b.angle >= 180 && b.angle < 270) {
		b.angle = b.angle - 180;
		bx.x = -b.force * sin(to_radians(b.angle));
		bx.y = -b.force * cos(to_radians(b.angle));
		printf("3 %f %f\n", bx.x, bx.y);
	} else {
		b.angle = b.angle - 270;
		bx.x = -b.force * cos(to_radians(b.angle));
		bx.y = b.force * sin(to_radians(b.angle));
		printf("4 %f %f\n", bx.x, bx.y);
	}

	Position new;
	new.x = ax.x + bx.x;
	new.y = ax.y + bx.y;

	printf("New %f %f\n", new.x, new.y);

	Force result;

	result.force = sqrt((new.x*new.x)+(new.y*new.y));
	float tmpangle = to_degrees(atan(new.y/new.x));
	if (new.x >= 0 && new.y >= 0) {
		tmpangle = tmpangle;
		// We are in the top right
	} else if (new.x >= 0 && new.y <= 0) {
		tmpangle = 90 + tmpangle;
	} else if (new.x <= 0 && new.y <= 0) {
		tmpangle = 180 + tmpangle;
	} else if (new.x <= 0 && new.y >= 0) {
		tmpangle = 270 + tmpangle;
	}
	result.angle = tmpangle;
	return result;
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
	q.force = 12;
	q.angle = 30;
	l.force = 6;
	l.angle = 200;

	Force res = balance_force(q, l);
	print_force(q);
	print_force(l);
	print_force(res);

}
