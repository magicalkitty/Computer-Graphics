#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../renderlib/vec3.h"
#include "../src/ray.h"

// People I talked to and worked with: Maddie, Breta

// Tests implemented: Construction, Evalutation of Parametric Line, Immutability, and Numerical Robustness

// Ray Construction
// Expected behavior: Ray constructed, stored with given origin and direction
// Verification: Construct ray with known origin and direction, verify stored values match inputs
TEST_CASE(" Ray construction test ") 
{
    point3 origin(1.0, 2.0, 3.0);
    vec3 direction(4.0, 5.0, 6.0);

    ray r(origin, direction);

    float eps = 1.0e-3; // a tolerance for how close the answer needs to be

    REQUIRE_THAT(r.origin().x(), Catch::Matchers::WithinAbs(1.0f, eps));
    REQUIRE_THAT(r.origin().y(), Catch::Matchers::WithinAbs(2.0f, eps));
    REQUIRE_THAT(r.origin().z(), Catch::Matchers::WithinAbs(3.0f, eps));

    REQUIRE_THAT(r.direction().x(), Catch::Matchers::WithinAbs(4.0f, eps));
    REQUIRE_THAT(r.direction().y(), Catch::Matchers::WithinAbs(5.0f, eps));
    REQUIRE_THAT(r.direction().z(), Catch::Matchers::WithinAbs(6.0f, eps));

}

// Evaluation of Parametric Line
// Expected behavior: at(t) returns point along ray at parameter t
// Verification: Evaluate at multiple t values, compare against independently calculated expected points
TEST_CASE(" Evaluation of Parametric Line ")
{
    point3 origin(1.0, 2.0, 3.0);
    vec3 direction(4.0, 5.0, 6.0);

    ray r(origin, direction);

    float eps = 1.0e-3; // a tolerance for how close the answer needs to be

    // Test at t = 0.0
    double t0 = 0.0;
    point3 p0 = r.at(t0);

    REQUIRE_THAT(p0.x(), Catch::Matchers::WithinAbs(1.0f, eps)); // 1 + 0 * 4
    REQUIRE_THAT(p0.y(), Catch::Matchers::WithinAbs(2.0f, eps)); // 2 + 0 * 5
    REQUIRE_THAT(p0.z(), Catch::Matchers::WithinAbs(3.0f, eps)); // 3 + 0 * 6

    // Test at t = 1.0
    double t1 = 1.0;
    point3 p1 = r.at(t1);

    REQUIRE_THAT(p1.x(), Catch::Matchers::WithinAbs(5.0f, eps)); // 1 + 1 * 4
    REQUIRE_THAT(p1.y(), Catch::Matchers::WithinAbs(7.0f, eps)); // 2 + 1 * 5
    REQUIRE_THAT(p1.z(), Catch::Matchers::WithinAbs(9.0f, eps)); // 3 + 1 * 6

    // Test at t = 2.0
    double t2 = 2.0;
    point3 p2 = r.at(t2);

    REQUIRE_THAT(p2.x(), Catch::Matchers::WithinAbs(9.0f, eps)); // 1 + 2 * 4
    REQUIRE_THAT(p2.y(), Catch::Matchers::WithinAbs(12.0f, eps)); // 2 + 2 * 5
    REQUIRE_THAT(p2.z(), Catch::Matchers::WithinAbs(15.0f, eps)); // 3 + 2 * 6

    // Test at t = -2.0
    double t3 = -2.0;
    point3 p3 = r.at(t3);

    REQUIRE_THAT(p3.x(), Catch::Matchers::WithinAbs(-7.0f, eps)); // 1 + (-2) * 4
    REQUIRE_THAT(p3.y(), Catch::Matchers::WithinAbs(-8.0f, eps)); // 2 + (-2) * 5
    REQUIRE_THAT(p3.z(), Catch::Matchers::WithinAbs(-9.0f, eps)); // 3 + (-2) * 6

}

// Immutability of Ray Components
// Expected behavior: origin() and direction() return const references that cannot be used to modify internal state
// Verification: Attempt to modify returned origin and direction, verify internal state remains unchanged
TEST_CASE(" Immutability of Ray Components ")
{
    point3 origin(1.0, 2.0, 3.0);
    vec3 direction(4.0, 5.0, 6.0);

    ray r(origin, direction);

    float eps = 1.0e-3; // a tolerance for how close the answer needs to be

    // Attempt to modify origin and direction
    point3 orig = r.origin();
    vec3 dir = r.direction();

    orig[0] = 10.0;
    dir[1] = 20.0;

    REQUIRE_THAT(r.origin().x(), Catch::Matchers::WithinAbs(1.0f, eps));
    REQUIRE_THAT(r.origin().y(), Catch::Matchers::WithinAbs(2.0f, eps));
    REQUIRE_THAT(r.origin().z(), Catch::Matchers::WithinAbs(3.0f, eps));

    REQUIRE_THAT(r.direction().x(), Catch::Matchers::WithinAbs(4.0f, eps));
    REQUIRE_THAT(r.direction().y(), Catch::Matchers::WithinAbs(5.0f, eps));
    REQUIRE_THAT(r.direction().z(), Catch::Matchers::WithinAbs(6.0f, eps));
}

// Numerical Robustness Test
// Expected behavior: Ray computations remain accurate with large/small values
// Verification: Construct ray with large origin and direction values, evaluate at large positive and negative t, verify results against expected values
TEST_CASE(" Ray Numerical Robustness Test ")
{
    point3 origin(1e8, -1e8, 1e-8);
    vec3 direction(1e4, 1e4, -1e4);

    ray r(origin, direction);

    float eps = 1.0e-3; // a tolerance for how close the answer needs to be

    double t1 = 10000.0;
    point3 p1 = r.at(t1);

    REQUIRE_THAT(p1.x(), Catch::Matchers::WithinAbs(2e8f, eps));
    REQUIRE_THAT(p1.y(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(p1.z(), Catch::Matchers::WithinAbs(-1e8f, eps));

    double t2 = -10000.0;
    point3 p2 = r.at(t2);

    REQUIRE_THAT(p2.x(), Catch::Matchers::WithinAbs(0.0f, eps));
    REQUIRE_THAT(p2.y(), Catch::Matchers::WithinAbs(-2e8f, eps));
    REQUIRE_THAT(p2.z(), Catch::Matchers::WithinAbs(1e8f, eps));    

}