#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../renderlib/vec3.h"
#include "../src/ray.h"
#include "../src/Sphere.h"

// People I talked to and worked with: Maddie, Sara

// Tests implemented: Sphere construction, Sphere ray intersection (direct hit, grazing hit, and miss)

// Sphere Construction
// Expected behavior: Sphere stores the given center and radius correctly
// Verification: Compare stored center and radius to known values
TEST_CASE(" Sphere construction test ") 
{
    point3 center(0.0, 0.0, 0.0);
    double radius = 1.0;

    Sphere s(center, radius);

    float eps = 1.0e-3; // a tolerance for how close the answer needs to be

    REQUIRE_THAT(s.get_center().x(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(s.get_center().y(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(s.get_center().z(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(s.get_radius(), Catch::Matchers::WithinAbs(1.0f, eps));
}

// Sphere Ray Intersection Tests
// Test 1: Ray misses the sphere
// Expected behavior: Ray that does not hit the sphere returns false
// Verification: Check that hit() is false and hit_record remains unchainged
TEST_CASE(" Sphere ray intersection test 1 - miss")
{
    point3 center(0.0, 0.0, 0.0);
    double radius = 1.0;

    Sphere s(center, radius);

    // A ray that should not intersect with the sphere
    vec3 origin(30.0, 0.0, 0.0);
    vec3 direction(0.0, 1.0, 0.0);

    ray r(origin, direction);

    hit_record rec;

    bool hit = s.intersect(r, 0.001, INFINITY, rec);

    float eps = 1.0e-3; // a tolerance for how close the answer needs to be

    REQUIRE(hit == false);

    REQUIRE_THAT(rec.t, Catch::Matchers::WithinAbs(-1.0f, eps));
    REQUIRE_THAT(rec.p.x(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(rec.p.y(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(rec.p.z(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(rec.normal.x(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(rec.normal.y(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(rec.normal.z(), Catch::Matchers::WithinAbs(0.0f, eps));
}

// Test 2: Ray directly hits the sphere
// Expected behavior: Ray intersects sphere and produces correct hit data
// Verifcaiton: hit() is true and t, hit point, and normal match expected values
TEST_CASE(" Sphere ray intersection test 2 - direct hit ")
{
    point3 center(0.0, 0.0, 0.0);
    double radius = 1.0;

    Sphere s(center, radius);

    // A ray that should intersect with the sphere
    vec3 origin(30.0, 0.0, 0.0);
    vec3 direction(-1.0, 0.0, 0.0);

    ray r(origin, direction);

    hit_record rec;

    bool hit = s.intersect(r, 0.001, INFINITY, rec);

    float eps = 1.0e-3; // a tolerance for how close the answer needs to be

    REQUIRE(hit == true);

    REQUIRE_THAT(rec.t, Catch::Matchers::WithinAbs(29.0f, eps));
    REQUIRE_THAT(rec.p.x(), Catch::Matchers::WithinAbs(1.0f, eps));
    REQUIRE_THAT(rec.p.y(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(rec.p.z(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(rec.normal.x(), Catch::Matchers::WithinAbs(1.0f, eps));
    REQUIRE_THAT(rec.normal.y(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(rec.normal.z(), Catch::Matchers::WithinAbs(0.0f, eps));
}

// Test 3: Ray grazes the sphere
// Expected behavior: Tangent ray still registers a valid hit
// Verification: hit() is true and t, hit point, and normal match expected values
TEST_CASE(" Sphere ray intersection test 3 - grazing hit ")
{
    point3 center(0.0, 0.0, 0.0);
    double radius = 1.0;

    Sphere s(center, radius);

    vec3 origin(30.0, 1.0, 0.0);
    vec3 direction(-1.0, 0.0, 0.0);

    ray r(origin, direction);

    hit_record rec;

    bool hit = s.intersect(r, 0.001, INFINITY, rec);

    float eps = 1.0e-3; // a tolerance for how close the answer needs to be

    REQUIRE(hit == true);
    REQUIRE_THAT(rec.t, Catch::Matchers::WithinAbs(30.0f, eps));
    REQUIRE_THAT(rec.p.x(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(rec.p.y(), Catch::Matchers::WithinAbs(1.0f, eps));
    REQUIRE_THAT(rec.p.z(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(rec.normal.x(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(rec.normal.y(), Catch::Matchers::WithinAbs(1.0f, eps));
    REQUIRE_THAT(rec.normal.z(), Catch::Matchers::WithinAbs(0.0f, eps));
}