#include <iostream>
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "hitablelist.h"
#include "float.h"

using namespace std;

hitable_list* random_scene() {
	int n = 500;
	hitable **list = new hitable *[n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
	int i = 1;
	for(int a = -11; a < 11; a++) {
		for(int b = -11; b < 11; b++) {
			float choose_mat = drand48();
			vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());

			if((center - vec3(4.0, 0.2, 0.0)).length() > 0.9) {
				if(choose_mat < 0.8) {
					list[i++] = new sphere(center, 0.2, 
						new lambertian(vec3(drand48() * drand48(), drand48() * drand48(), drand48() * drand48())));
				}
				else if(choose_mat < 0.95) {
					list[i++] = new sphere(center, 0.2, 
						new metal(vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()), 0.5 * drand48()), 0.5 * drand48()));
				}
				else{
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}

	list[i++] = new sphere(vec3( 0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3( 4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new hitable_list(list,i);
}

vec3 color(const ray& r, hitable* world, int depth) {
	hit_record rec;
	if(world->hit(r, 0.001, MAXFLOAT, rec)) {
		ray scattered;
		vec3 attenuation;
		if(depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		else {
			return vec3(0,0,0);
		}
	}
	else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return  t*vec3(0.5, 0.7, 1.0) + (1 - t)*vec3(1.0, 1.0, 1.0);
	}
}

int main() {
	float rx = 1920, ry = 1080;
	int ns = 1000;
	vec3 res;

	cout << "P3" << endl;
	cout << rx << " " << ry << " 255" << endl;
			
	hitable_list* world = random_scene();

	vec3 lookfrom(9,1.6,2.2);
	vec3 lookat(0,0.5,0);
	float dist_to_focus = (lookfrom - lookat).length();
	float aperture = 0.0;

	camera cam(lookfrom, lookat, vec3(0, 1, 0), 30, rx / ry, aperture, dist_to_focus);

	for (int j = ry-1; j >= 0; j--) {
		for(int i = 0 ; i < rx ; i++) {
			vec3 c(0.0, 0.0, 0.0);
			#pragma omp parallel for schedule(dynamic)
			for(int s = 0 ; s < ns ; s++) {
				float u = float(i + drand48())/rx;
				float v = float(j + drand48())/ry;
				ray r = cam.get_ray(u,v);
				res = color(r,world,0);
				#pragma omp critical
				c += res;
			}
			c /= float(ns);
			cout << int(sqrt(c[0])*255.9) << " " << int(sqrt(c[1])*255.9) << " " << int(sqrt(c[2])*255.9) << " ";
		}
		cout << endl;
	}
	for(int i = 0 ; i < world->list_size ; i++)
		if(world->list[i] != NULL) delete world->list[i];
	delete world->list;
	delete world;
}
