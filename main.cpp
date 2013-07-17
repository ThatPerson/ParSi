#include <iostream>
#include <stdio.h>
#include <math.h>
using namespace std;

#define PI 3.14159265358979323

typedef struct {
	int x;
	int y;
} Position;

typedef struct {
	int angle;
	int force;
	int speed;
} Force;

float to_degrees(int radians) {
	return radians*PI/180;
}

class PS_Particle {
	Position pos;
	Force force;
	public:
		void set_position(Position);
		void set_force(Force);
		void balance_force(Force);
		void wait(int);

		Position get_pos();
		
		int get_time_to(Position);
};

void PS_Particle::set_position(Position l) {
	pos = l;
}
void PS_Particle::set_force(Force l) {
	force = l;
}
void PS_Particle::balance_force(Force l) {
}
Position PS_Particle::get_pos() {
	return pos;
}
void PS_Particle::wait(int time) {
	//Distance along the force
	// s = ut+1/2at^2
	int p = (force.speed * time) + (0.5*(force.force * time * time));
	int x = p * sin(to_degrees(force.angle));
	int y = p * cos(to_degrees(force.angle));
	pos.x = x+pos.x;
	pos.y = y+pos.y;
	force.speed = force.speed + (force.force*time);
	printf("Vals: %d, %d, %d\n      %d, %d, %d\n      %d\n", p, x, y, force.speed, time, force.force, 5*5);
}

float to_under_90(int angle) {
	while (angle > 90) {
		angle -= 90;
	}
	return angle;
}

int PS_Particle::get_time_to(Position q) {
	// Gradiant = force.angle / 50, for example 30 would be 0.6, or 45 would  be 1
	// we know what the value is at one point, so we use this to get c. This gives
	// us a formula of the force. From this we can then check if the new position 
	// is on said line, and if so we can then work out how far down, and by using 
	// sin and cos we can work out the distance needed. As we also know how fast i
	// t is going, we can use s = ut+1/2at^2 to work out the time, as we know the 
	// distance, starting speed and acceleration	
	
	float gradiant = this->force.angle / 50;
	
	// y = x + c
	// c = x - y
	float c = pos.x - pos.y;
	
	// y = gradiant * x + c
	float y = (gradiant * q.x) + c;
	printf("Form: y = %fx + %f\nAngle: %f PosX: %f PosY: %f\n", gradiant, c, force.angle, pos.x, pos.y);
	if (y == q.y) {
		////It is on the line!!!
		//int s = q.x - pos.x; //Work out the distance on the x axis
		//float forcet = to_under_90(force.angle);
		//if (force.angle >= 0 && force.angle < 180) {
			//float a = force.force * sin(forcet);
		//} else if (force.angle >= 180 && force.angle < 360) {
			//float a = -(force.force * sin(forcet));
		//}
		//if (s > 0 && a < 0) {
			//return -1;
		//} else if (s < 0 && a > 0) {
			//return -1;
		//}
		////s = ut+1/2(at^2)
		////t = (-u±√(u²+2as))/a
		//float t = ((-(force.speed))+sqrt((force.speed*force.speed)+2*force.force*s))/force.force;
		
		int x = pos.x - q.x;
		int y = pos.y - q.y;
		int c = sqrt((x*x)+(y*y));
		
		float t = ((-force.speed)+sqrt((force.speed*force.speed)+2*force.force*c))/force.force;
		
		return t;
	} else {
		return -1;
	}

	return 1;
}	


int main () {
	PS_Particle l;
	Position q;
	q.x = 0;
	q.y = 0;
	l.set_position(q);
	Force p;
	p.angle = 10;
	p.force = 1;
	p.speed = 10;
	l.set_force(p);
	//l.wait(3);
	Position o = l.get_pos();
	Position rq;
	rq.x = 0;
	rq.y = 34;
	printf("o: %d %d\n   %d", o.x, o.y, l.get_time_to(rq));
	return 0;
}
