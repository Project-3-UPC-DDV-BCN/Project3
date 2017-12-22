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
// Copyright (c) 2016-2017 NVIDIA Corporation. All rights reserved.


#ifndef NVBLASTPROFILER_H
#define NVBLASTPROFILER_H

#include "NvBlastPreprocessor.h"


namespace Nv
{
namespace Blast
{


/**
Custom Blast profiler interface.
*/
class ProfilerCallback
{
protected:
	virtual ~ProfilerCallback() {}

public:
	/**
	Called when a nested profile zone starts.
	*/
	virtual void zoneStart(const char* name) = 0;

	/**
	Called when the current profile zone ends.
	*/
	virtual void zoneEnd() = 0;
};


/**
Profiler detail to be reported. The higher setting is used, the more details are reported.
*/
struct ProfilerDetail
{
	enum Level
	{
		LOW,
		MEDIUM,
		HIGH
	};
};


} // namespace Blast
} // namespace Nv


/**
Profiler features are only active in checked, debug and profile builds.
*/

/**
Set a custom profiler callback. May be nullptr (the default).
*/
NVBLAST_API void NvBlastProfilerSetCallback(Nv::Blast::ProfilerCallback* pcb);


/**
Sets the depth of reported profile zones.
Higher levels (more nesting) of instrumentation can have a significant impact.
Defaults to Nv::Blast::ProfilerDetail::Level::LOW.
*/
NVBLAST_API void NvBlastProfilerSetDetail(Nv::Blast::ProfilerDetail::Level);


#endif
