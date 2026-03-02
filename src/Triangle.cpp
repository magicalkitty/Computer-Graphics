#include "Triangle.h"

Triangle::Triangle() : a(point3(0,0,0)), b(point3(0,0,0)), c(point3(0,0,0)), shader(nullptr) {}

Triangle::Triangle(const point3& a, const point3& b, const point3& c, std::shared_ptr<Shader> shaderPtr)
    : a(a), b(b), c(c), shader(shaderPtr) {}

bool Triangle::intersect(const ray& r, float ray_tmin, float ray_tmax, hit_record& rec) const
{
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
    ray_tmax = t; // update ray_tmax to the closest hit so far
    rec.t = t;
    rec.p = r.at(rec.t);
    rec.shape = this;

    // compute normal using cross product of two edges
    rec.normal = unit_vector(cross(b-a, c-a));

    // ensure the normal faces against the ray direction
    if (dot(rec.normal, r.direction()) > 0.0) {
        rec.normal = -rec.normal;
    }

    rec.beta = beta;
    rec.gamma = gamma;
    rec.alpha = 1.0 - beta - gamma;
    rec.shaderPointer = shader;
    rec.r = r;

    return true;
}