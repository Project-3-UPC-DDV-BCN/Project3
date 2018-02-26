#include "myassert.h"
#include "MathGeoLib.h"
#include "TestRunner.h"
#include "TestData.h"

using namespace TestData;

BENCHMARK(Triangle_intersects_AABB, "Triangle::Intersects(AABB)")
{
	dummyResultInt += Triangle(ve[i], ve[i+1], ve[i+2]).Intersects(aabb[i]) ? 1 : 0;
}
BENCHMARK_END
