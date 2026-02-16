#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "../renderlib/vec3.h"

class Triangle : public hittable {
    public:
        Triangle(const point3& a, const point3& b, const point3& c) : a(a), b(b), c(c) {}

        bool intersect(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
            double A = a.x() - b.x();
            double B = a.y() - b.y();
            double C = a.z() - b.z();
            double D = a.x() - c.x();
            double E = a.y() - c.y();
            double F = a.z() - c.z();
            double G = r.direction().x();
            double H = r.direction().y();
            double I = r.direction().z();
            double J = a.x() - r.origin().x();
            double K = a.y() - r.origin().y();
            double L = a.z() - r.origin().z();


            // Compute the determinant
            double m = A*(E*I - H*F) + B*(G*F - D*I) + C*(D*H - E*G);

            if (fabs(m) < 1e-8) {
                return false; // Ray is parallel to the triangle
            }

            double beta = (J*(E*I - H*F) + K*(G*F - D*I) + L*(D*H - E*G))/m;
            double gamma = (I*(A*K - J*B) + H*(J*C - A*L) + G*(B*L - K*C))/m;
            double t = -(F*(A*K - J*B) + E*(J*C - A*L) + D*(B*L - K*C))/m;

            if (t<ray_tmin || t>ray_tmax) {
                return false;
            }
            if (gamma < 0.0 || gamma > 1.0) {
                return false;
            }
            if (beta < 0.0 || beta > 1.0) {
                return false;
            }
            if (beta + gamma > 1.0) {
                return false;
            }

            // if we get here, we have a hit!
            rec.t = t;
            rec.p = r.at(rec.t);

            // compute normal using cross product of two edges
            rec.normal = unit_vector(cross(b-a, c-a));

            // ensure the normal faces against the ray direction
            if (dot(rec.normal, r.direction()) > 0.0) {
                rec.normal = -rec.normal;
            }

            rec.beta = beta;
            rec.gamma = gamma;
            rec.alpha = 1.0 - beta - gamma;
        
            return true;
        }
    private:
        point3 a, b, c;
};

#endif // TRIANGLE_H