#include <ctype.h>
#include "UsefulFunctions.h"
#include <stdio.h>
#include <cmath>
#include <random>
#include <limits>
#include <stdlib.h>
#include "Globals.h"
#include "MathGeoLib\TransformOps.h"

// Returns the angle between two points in degrees
float AngleFromTwoPoints(float x1, float y1, float x2, float y2)
{
	float deltaY = y2 - y1;
	float deltaX = x2 - x1;

	float angle = atan2(deltaY, deltaX) * RADTODEG;

	if (angle <= 180)
		return -angle;
	if (angle > 180)
		return angle / 2;
}

float NormalizeAngle(float angle)
{
	if (angle > 360)
	{
		int multiplers = angle / 360;
		angle -= (360 * multiplers);
	}

	if (angle < -360)
	{
		int multiplers = abs(angle) / 360;
		angle += (360 * multiplers);
	}

	return angle;
}

float4x4 RotateArround(float4x4 to_rotate, float3 center, float angle_x, float angle_y)
{
	float3 to_rotate_pos;
	Quat to_rotate_rot;
	float3 to_rotate_scal;

	to_rotate.Decompose(to_rotate_pos, to_rotate_rot, to_rotate_scal);

	float3 distance = to_rotate_pos - center;

	Quat X(to_rotate.WorldX(), angle_x * DEGTORAD);
	Quat Y(to_rotate.WorldY(), angle_y * DEGTORAD);

	distance = X.Transform(distance);
	distance = Y.Transform(distance);

	float4x4 ret = to_rotate;

	ret[0][3] = distance.x + center.x;
	ret[1][3] = distance.y + center.y;
	ret[2][3] = distance.z + center.z;

	return ret;
}

float DistanceFromTwoPoints(float x1, float y1, float x2, float y2)
{
	int distance_x = x2 - x1;
	int distance_y = y2 - y1;
	float sign = ((distance_x * distance_x) + (distance_y * distance_y));
	float dist = abs((distance_x * distance_x) + (distance_y * distance_y));

	if (sign > 0)
		return sqrt(dist);
	else
		return -sqrt(dist);
}

bool TextCmp(const char * text1, const char * text2)
{
	bool ret = false;

	if (text1 == nullptr || text2 == nullptr)
		return false;

	if (strcmp(text1, text2) == 0)
		ret = true;

	return ret;
}

void TextCpy(char* destination, const char * origen)
{
	if (origen != nullptr)
	{
		strcpy_s(destination, strlen(origen), origen);
	}
}

void Tokenize(std::string string, const char separator, std::list<std::string>& tokens)
{
	int i = 0;
	const char* str = string.c_str();
	while (*(str + i) != 0)
	{
		std::string temporal;
		while (*(str + i) != separator && *(str + i) && *(str + i) != '\n')
		{
			temporal.push_back(*(str + i));
			i++;
		}
		tokens.push_back(temporal);
		if (*(str + i)) i++;
	}
}

std::string ToUpperCase(std::string str)
{
	for (uint i = 0; i < str.size(); i++)
	{
		str[i] = toupper(str[i]);
	}

	return str;
}

std::string ToLowerCase(std::string str)
{
	for (uint i = 0; i < str.size(); i++)
	{
		str[i] = tolower(str[i]);
	}

	return str;
}
