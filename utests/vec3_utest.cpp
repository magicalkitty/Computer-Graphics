#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../renderlib/vec3.h"

// People I talked to and worked with: Sara, Maddie, Breta, Ethan

// Operators: addition, dot product, cross product, scalar multiplication, scalar division

// Addition
// Expected behavior: Component-wise addition
// Verification: Add two known vectors and independently check each component against expected values
TEST_CASE( "vec3 addition test" )
{
    vec3 a(1, 1, 1);
    vec3 b(1, 1, 1);

    vec3 c = a + b;

    float eps = 1.0e-3; // a tolerance for how close the answer needs to be
    float actualAnswer = 2.0f;
    REQUIRE_THAT( c.x(), Catch::Matchers::WithinAbs(actualAnswer, eps));
    REQUIRE_THAT( c.y(), Catch::Matchers::WithinAbs(actualAnswer, eps));
    REQUIRE_THAT( c.z(), Catch::Matchers::WithinAbs(actualAnswer, eps));
}

// Dot Product
// Expected behavior: Scalar result a*b = a.x*b.x + a.y*b.y + a.z*b.z
// Verification: Compute dot product of two known vectors with mixed signs and compare against independently calculated expected value
TEST_CASE( "vec3 dot product test" )
{
    vec3 a(1, 2, 3);
    vec3 b(4, -5, 6);

    double result = dot(a, b);

    float eps = 1.0e-3; // a tolerance for how close the answer needs to be
    float actualAnswer = 12.0f; // 1*4+2*-5+3*6
    REQUIRE_THAT( result, Catch::Matchers::WithinAbs(actualAnswer, eps));

    // Don't use this since we might have floating point precision issues
    // REQUIRE( result  == actualAnswer);
}

// Cross Product
// Expected behavior: Vector perpendicult to both inputs, with components following the right-hand rule
// Verification: Cross two non-parallel vectors and check each resulting components against independently calculated expected values
TEST_CASE( "vec3 cross product test" )
{
    vec3 a(1, 2, 3);
    vec3 b(4, -5, 6);

    vec3 c = cross(a, b);

    float eps = 1.0e-3; // a tolerance for how close the answer needs to be
    REQUIRE_THAT( c.x(), Catch::Matchers::WithinAbs(27.0f, eps));
    REQUIRE_THAT( c.y(), Catch::Matchers::WithinAbs(6.0f, eps));
    REQUIRE_THAT( c.z(), Catch::Matchers::WithinAbs(-13.0f, eps));
    // REQUIRE( c.x() == 27.0);
    // REQUIRE( c.y() == 6.0);
    // REQUIRE( c.z() == -13.0);
}

// Scalar Multiplication
// Expected behavior: Each component is multiplied by the scalar
// Verification: Multiply a vector by a scalar and validate each componennts independently
TEST_CASE( "vec3 scalar multiplication test" )
{
    vec3 a(1, -2, 3);
    double t = 3.0;

    vec3 c = a * t;

    float eps = 1.0e-3; // a tolerance for how close the answer needs to be
    REQUIRE_THAT( c.x(), Catch::Matchers::WithinAbs(3.0f, eps));
    REQUIRE_THAT( c.y(), Catch::Matchers::WithinAbs(-6.0f, eps));
    REQUIRE_THAT( c.z(), Catch::Matchers::WithinAbs(9.0f, eps));
//     REQUIRE( c.x() == 3.0 );
//     REQUIRE( c.y() == -6.0 );
//     REQUIRE( c.z() == 9.0 );
}

// Scalar Division
// Expected behavior: Each componenet is divided by the scalar (scalar != 0)
// Verification: Divide by a scalar and check each component against expected fractional values
TEST_CASE( "vec3 scalar division test" )
{
    vec3 a(1, -2, 3);
    double t = 2.0;

    vec3 c = a / t;

    float eps = 1.0e-3; // a tolerance for how close the answer needs to be
    REQUIRE_THAT( c.x(), Catch::Matchers::WithinAbs(0.5f, eps));
    REQUIRE_THAT( c.y(), Catch::Matchers::WithinAbs(-1.0f, eps));
    REQUIRE_THAT( c.z(), Catch::Matchers::WithinAbs(1.5f, eps));
    // REQUIRE( c.x() == 0.5 );
    // REQUIRE( c.y() == -1.0 );
    // REQUIRE( c.z() == 1.5 );
}

