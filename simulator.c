#include <stdio.h>
#include <math.h>

typedef struct {
	double angle;
	int direction; // 1 = pos, 2 = neg
	double power;
} Force;

typedef struct {
	double x;
	double y;
} Position;

typedef struct {
	Force force;
	Position position;
} Particle;

int is_collision(Particle a, Particle b) {
	double force_x_1 = a.force.power * sin(a.force.angle);
	double force_y_1 = a.force.power * cos(a.force.angle);
	double force_x_2 = b.force.power * sin(b.force.angle);
	double force_y_2 = b.force.power * cos(b.force.angle);
	double o = b.position.y - a.position.y;
	double dist_a = force_y_1 * (o/(force_y_1+force_y_2));
	double dist_b = force_y_2 * (o/(force_y_1+force_y_2));
	
	double t_1 = sqrt((2 * dist_a)/force_y_1);  //Time taken y axis for a
	double t_2 = sqrt((2 * dist_b)/force_y_2); //And for b
	
	double s_1 = (force_x_1 * t_1 * t_1)/2;
	double s_2 = (force_x_2 * t_2 * t_2)/2;
	
	double location_x = s_1+s_2;
	
	printf("force_x_1 %f\nforce_y_1 %f\nforce_x_2 %f\nforce_y_2 %f\no %f\ndist_a %f\ndist_b %f\nt_1 %f\nt_2 %f\ns_1 %f\ns_2 %f\nlocation_x %f",
		    force_x_1,    force_y_1,    force_x_2,    force_y_2,    o,    dist_a,    dist_b,    t_1,    t_2,    s_1,    s_2,    location_x);
	
	if (location_x == (b.position.x - a.position.x)) {
		return 1;
	} else {
		return 0;
	}
	return -1;
}

Position loc_collision(Particle a, Particle b) {
}

Particle res_collision(Particle a, Particle b) {
}

int main(void) {
	Particle q, p;
	q.position.x = 0;
	q.position.y = 0;
	q.force.angle = 45;
	q.force.direction = 0;
	q.force.power = 5;
	p.position.x = 50;
	p.position.y = 50;
	p.force.power = 5;
	p.force.angle = 45;
	printf("\n%d\n", is_collision(q, p));
	return 1;
}