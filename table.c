#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct {
	float force;
	float angle;
} Force;

typedef struct {
	float x;
	float y;
} Position;

typedef struct {
	char name[500];
	Position pos;
	Force force;
	float speed;
} Particle;

Particle new_particle(float X, float Y, float Force, float Angle) {
	Particle p;
	p.pos.x = X;
	p.pos.y = Y;
	p.force.angle = Angle;
	p.force.force = Force;
	return p;
}

void tabulate_particles(Particle p[], int count) {
	printf("%d\n", sizeof(Particle));
	printf("%d\n", sizeof(p));
	printf("%d\n", sizeof(p)/sizeof(p[0]));
	printf("%10s %10s %10s %10s %10s %10s\n", "Name", "X", "Y", "Force", "Angle", "Speed");
	int i;
	for (i = 0; i < 65; i++) {
		printf("-");
	}
	printf("\n");
	for (i = 0; i < count; i++) {
		printf("%10s %10f %10f %10f %10f %10f\n", p[i].name, p[i].pos.x, p[i].pos.y, p[i].force.force, p[i].force.angle, p[i].speed);
	}
 	
}

int main(void) {
	Particle p[2];
	p[0].pos.x = 3;
	p[0].pos.y = 4;
	p[0].force.force = 4;
	p[0].force.angle = 20;
	strcpy(p[0].name, "A");
	p[0].speed = 34;
	p[1] = new_particle(3, 4, 5, 6);
	p[1].speed = 2;
	strcpy(p[1].name, "Bee");

	printf("%8s %8f %8f %8f %8f %8f\n", p[0].name, p[0].pos.x, p[0].pos.y, p[0].force.force, p[0].force.angle, p[0].speed);
	tabulate_particles(p, 2);
	return 1;
}
