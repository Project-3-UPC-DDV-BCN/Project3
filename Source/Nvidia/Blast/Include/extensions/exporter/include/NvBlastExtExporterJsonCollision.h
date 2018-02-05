// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2017 NVIDIA Corporation. All rights reserved.


#ifndef NVBLASTEXTEXPORTERJSONCOLLISION_H
#define NVBLASTEXTEXPORTERJSONCOLLISION_H

#include "NvBlastTypes.h"

namespace Nv
{
namespace Blast
{


struct CollisionHull;

/**
	Serializes collision geometry to JSON format. 
*/
class JsonCollisionExporter
{
public: 
	JsonCollisionExporter(){};

	/**
		Method creates file with given path and serializes given array of arrays of convex hulls to it in JSON format.
		\param[in] path		Output file path 
		\param[in] hulls	Array of arrays of convex hull descriptors. Each array contain array of convex hulls for chunk (hulls[0] - convexes for chunk 0, etc.)
	*/
	bool writeCollision(const char* path, uint32_t meshCount, const uint32_t* meshOffsets, const CollisionHull* hulls);
};


} // namespace Blast
} // namespace Nv

#endif //NVBLASTEXTEXPORTERJSONCOLLISION_H