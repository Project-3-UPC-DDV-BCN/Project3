#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include <iostream>
#include <list>
#include <string>
#include "MathGeoLib\float2.h"
#include "MathGeoLib\float4.h"
#include "MathGeoLib\float4x4.h"
#include "MathGeoLib\Quat.h"

// -----------------------------------------
// -----------------------------------------

// USEFUL FUNCTIONS //

// -----------------------------------------
// -----------------------------------------

// Returns the angle between two points in degrees
float AngleFromTwoPoints(float x1, float y1, float x2, float y2);

float NormalizeAngle(float angle);

float4x4 RotateArround(float4x4 to_rotate, float3 center, float angle_x, float angle_y);

// Returns the distance from two points(can be a negative distance)
float DistanceFromTwoPoints(float x1, float y1, float x2, float y2);

// Compares two char*, returns true if equal
bool TextCmp(const char* text1, const char* text2);

// Copies a string to another
void TextCpy(char * destination, const char * origen);

// Separate a string into tokens using the indicated iterator
void Tokenize(std::string string, const char separator, std::list<std::string>& tokens);

// Returns chars to upper case
std::string ToUpperCase(std::string str);

// Returns chars to lower case
std::string ToLowerCase(std::string str);

#endif //__FUNCTIONS_H__