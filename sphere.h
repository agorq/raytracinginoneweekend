#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"
#include "material.h"

class sphere : public hitable {
    public:
        sphere() {}
        sphere(vec3 cen, float r, material* m) : center(cen), radius(r), mat(m) {};
        ~sphere() { delete mat; }
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;

        float radius;
        vec3 center;
        material* mat;

};


bool sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(r.direction(), oc);
    float c = dot(oc, oc) - radius * radius;
    float delta = b*b - a*c;
    if(delta > 0) {
        float temp = (-b - sqrt(delta)) / a;
        if(temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(temp);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat;
            return true; 
        }
        temp = (-b + sqrt(delta)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(temp);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat;
            return true;
        }
    }
    return false;
    
}

#endif // SPHEREH
