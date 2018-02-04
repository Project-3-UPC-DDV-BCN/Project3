#ifndef __SKELETON_H__
#define __SKELETON_H__

#include "Globals.h"
#include "MathGeoLib\MathGeoLib.h"

#include "Resource.h"

class JointPose
{
public:
	JointPose();
	JointPose(const JointPose& pose);
	JointPose(Quat& rotation, float3& position, float3& scale);

	// Set rotation, position and scale for this joint pose if not set previously
	void SetPose(Quat& rot, float3& pos, float3& scale);

	// Returns the Transform matrix of this joint for MatGeoLib calculations
	float4x4 GetTransformMatrix() const;
	//Returns the Transformation matrix of this joint for OpenGL calculations
	float4x4 GetOpenGLTransformMatrix() const;
	// Returns rotation quaternion
	Quat GetRotation() const;
	// Returns position vector
	float3 GetPosition() const;
	// Return scale vector
	float3 GetScale() const;

private:
	Quat rotation;
	float3 position;
	float3 scale;

	bool set = false;
};

class Joint
{
public:
	Joint();
	Joint(const char* name, JointPose& initial_pose);

	// Sets the parent index for this joint if it wasn't set previously
	void SetParentIndex(int index);
	// Sets the name
	void SetName(const char* name);

	// Returns the parent_index at the skeleton joint array
	int GetParentIndex() const;

	// Returns the local transformation matrix for OpenGL
	float4x4 GetOpenGLTransformMatrix() const;
	//Return local transformation matrix for MatGeoLib calculations
	float4x4 GetLocalTransformationMatrix() const;

	void SetPose(Quat& rot, float3& pos, float3& scale);

	JointPose GetPose()const;
	const char* GetName() const;

private:
	int parent_index = -2; // -2 unset, -1 no parent, 0...* index 
	std::string name;
	JointPose initial_pose;
};

class Skeleton : public Resource
{
public:
	Skeleton();
	Skeleton(uint num_joints, Joint* joints);

	~Skeleton();

	// Set the number of joints for this skeleton if unset
	void SetNumJoints(int num);
	// Set tht joint array pointer if unset
	void SetJoints(Joint* joints);

	// Return the number of joints this skeleton has
	int GetNumJoints() const;
	// Return the joints pointer
	Joint* const GetJoints() const;
	//Return world transformation for index joint
	float4x4 GetJointWorldTransformMatrix(uint joint);
	//Return an openGL ready world transformation for index joint
	float* GetOpenGLJointWorldTransformMatrix(uint joint);

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

private:
	int num_joints = -1; // -1 = unset
	Joint* joints = nullptr;
};

#endif
