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

float absol(float in) {
	float new = in;
	if (in < 0) {
		new = new - (2*in); // two times the number would get enough to make up for each side, then - because - - is +. For example, if in = -3, then we get -3 - (-3*2), so -3 - - 6, so -3 + 6, so 3.
	}
	return new; //If value is +, then we do nothing.
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

