#include "DiffuseShader.h"
#include <cmath>
#include <random>
#include "Scene.h"
#include "Random.h"

static float floatMax = INFINITY;

DiffuseShader::DiffuseShader(const color& diffuseColor)
    : diffuseColor(diffuseColor) {}

vec3 DiffuseShader::random_in_unit_sphere() const
{
  static sivelab::Random rand;
  vec3 randomDir;
  while (true) {
    randomDir = vec3(
      2.0 * rand.uniform() - 1.0,
      2.0 * rand.uniform() - 1.0,
      2.0 * rand.uniform() - 1.0
    );
    if (randomDir.length() < 1.0) break;
  }
  return unit_vector(randomDir);
}

color DiffuseShader::rayColor(const Scene& world, const hit_record &hit, const Light& light, int depth) const
{
  if (depth <= 0) {
    return vec3(0, 0, 0);
  }

  vec3 randomDir = random_in_unit_sphere();

  vec3 scatterDirection = hit.normal + randomDir;

  if (scatterDirection.length_squared() < 1e-8) {
    scatterDirection = hit.normal;
  }

  ray scatteredRay(hit.p, scatterDirection);

  vec3 scatteredColor = world.computeRayColor(scatteredRay, 0.001, floatMax, light, depth);

  return diffuseColor * scatteredColor;
}