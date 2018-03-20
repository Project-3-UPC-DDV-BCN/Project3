#include "Vertex.h"
#include "Globals.h"
Vertex::Vertex()
{
}

Vertex::~Vertex()
{
}

void Vertex::SetPosition(float x, float y, float z)
{
	position[0] = x;
	position[1] = y;
	position[2] = z;
}

void Vertex::SetPosition(float * position)
{
	if (position == nullptr)
		return;

	position = this->position;
}
