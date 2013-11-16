typedef struct {
	Particle items[512];
	float inc;
	float time;
	int length;
} Response;

char * substr(char * orig, int start, int length) {
	char * ret = (char *)malloc (sizeof (char) * length);
	strncpy (ret, orig+start, length);
	ret[length] = '\0';
	return ret;
}

int intlength(int l) {
	if (l != 0) {
		return floor(log10(abs(l))) + 1;
	}
	return -1;
}

char * to_si(float n) {
	
	int l = (int) n;
	int length = intlength(l);
	int primary = l/(pow(10, length));
	char * ret = (char *) malloc (sizeof(char)*500);
	//sprintf(ret, "%dx10^%d appr.\n", primary, length);
	sprintf(ret, "NaN");
	return ret;

}



Response get_config(char filename[]) {
	Response r;
	r.length = -1;
	FILE * file = fopen(filename, "r");
	if (file != NULL) {
		char line[128];
		while (fgets(line, sizeof(line), file) != NULL) {
			switch (line[0]) {
				case '[': r.length++; r.items[r.length].shown = 1; strcpy(r.items[r.length].name, substr(line, 1, strlen(line)-3)); break;
				case 'X': r.items[r.length].pos.x = atof(substr(line, 2, strlen(line)-2)); break;
				case 'Y': r.items[r.length].pos.y = atof(substr(line, 2, strlen(line)-2)); break;
				case 'A': r.items[r.length].force.angle = atof(substr(line, 2, strlen(line)-2)); break;
				case 'F': r.items[r.length].force.value = atof(substr(line, 2, strlen(line)-2)); break;
				case 'M': r.items[r.length].mass = atoll(substr(line, 2, strlen(line)-2)); break;
				case 'R': r.items[r.length].radius = atof(substr(line, 2, strlen(line)-2)); break;
				case 'S': 
					  switch (line[1]) {
					  	case 'A': r.items[r.length].speed.value = atof(substr(line, 3, strlen(line)-4)); break;
						case 'F': r.items[r.length].speed.angle = atof(substr(line, 3, strlen(line)-4)); break;
					  	case 'T': r.items[r.length].surface_tension = atof(substr(line,3,strlen(line)-4)); break;
					  }
				case ';':
				  	 switch (line[1]) {
						case 'i': r.inc = atof(substr(line, 3, strlen(line)-4)); break;	   
						case 't': r.time = atof(substr(line, 3, strlen(line)-4)); break;	   
					 }
			}
		  //fputs(line, stdout);
		}
		fclose(file);
	} else {
		perror(filename);
	}
	r.length++;
	return r;
}
