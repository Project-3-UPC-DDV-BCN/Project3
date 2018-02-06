#ifndef _H_DEBUG_DRAW__
#define _H_DEBUG_DRAW__

#include "Component.h"
#include "MathGeoLib\Math\float3.h"
#include "Glew\include\glew.h"

void DrawLine(float3 start, float3 end);
void Draw(float* vertices, uint num_vertices, uint* indices, uint num_indices);

#endif // !_H_DEBUG_DRAW__