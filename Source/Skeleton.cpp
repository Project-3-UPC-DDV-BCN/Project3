#include "Skeleton.h"
#include "Data.h"

// ----- JointPose -----

JointPose::JointPose()
{
	rotation = Quat::identity;
	position = float3::zero;
	scale = { 1.0f,1.0f,1.0f };
}

JointPose::JointPose(const JointPose & pose)
{
	rotation = pose.rotation;
	position = pose.position;
	scale = pose.scale;
	set = pose.set;
}

JointPose::JointPose(Quat& rotation, float3& position, float3& scale) : rotation(rotation), position(position), scale(scale), set(true)
{
}

void JointPose::SetPose(Quat& rot, float3& pos, float3& scale)
{
	if (!set)
	{
		rotation = rot;
		position = pos;
		this->scale = scale;
		set = true;
	}
}

float4x4 JointPose::GetTransformMatrix() const
{
	return float4x4::FromTRS(position,rotation,scale);
}

float4x4 JointPose::GetOpenGLTransformMatrix() const
{
	return float4x4::FromTRS(position, rotation, scale).Transposed();
}

Quat JointPose::GetRotation() const
{
	return rotation;
}

float3 JointPose::GetPosition() const
{
	return position;
}

float3 JointPose::GetScale() const
{
	return scale;
}

// -----

// ----- Joint -----

Joint::Joint()
{
}

Joint::Joint(const char * name, JointPose & initial_pose) : initial_pose(initial_pose)
{
	this->name = name;
}

void Joint::SetParentIndex(int index)
{
	if (parent_index == -2) // -2 = unset
	{
		parent_index = index;
	}
}

void Joint::SetName(const char * name)
{
	this->name = name;
}

int Joint::GetParentIndex() const
{
	return parent_index;
}

float4x4 Joint::GetOpenGLTransformMatrix() const
{
	return initial_pose.GetOpenGLTransformMatrix();
}

float4x4 Joint::GetLocalTransformationMatrix() const
{
	return initial_pose.GetTransformMatrix();
}

void Joint::SetPose(Quat & rot, float3 & pos, float3 & scale)
{
	initial_pose.SetPose(rot, pos, scale);
}

JointPose Joint::GetPose() const
{
	return initial_pose;
}

const char * Joint::GetName() const
{
	return name.c_str();
}

// -----

// ----- Skeleton -----

Skeleton::Skeleton()
{
	SetType(Resource::SkeletonResource);
}

Skeleton::Skeleton(uint num_joints, Joint * joints) : num_joints(num_joints), joints(joints)
{
	SetType(Resource::SkeletonResource);
}

Skeleton::~Skeleton()
{
	RELEASE_ARRAY(joints);
}

void Skeleton::SetNumJoints(int num)
{
	if (num_joints == -1)
		num_joints = num;
}

void Skeleton::SetJoints(Joint * joints)
{
	if (this->joints == nullptr)
		this->joints = joints;
}

int Skeleton::GetNumJoints() const
{
	return num_joints;
}

Joint * const Skeleton::GetJoints() const
{
	return joints;
}

float4x4 Skeleton::GetJointWorldTransformMatrix(uint joint)
{
	if ((joints + joint)->GetParentIndex() == -1) // root joint returns its transformation 
	{
		return (joints + joint)->GetLocalTransformationMatrix();
	}
	else // calculate transform from parent transform
	{
		return GetJointWorldTransformMatrix((joints + joint)->GetParentIndex()) * (joints + joint)->GetLocalTransformationMatrix();
	}
}

float * Skeleton::GetOpenGLJointWorldTransformMatrix(uint joint)
{
	if ((joints + joint)->GetParentIndex() == -1) // root joint returns its transformation 
	{
		return (joints + joint)->GetLocalTransformationMatrix().Transposed().ptr();
	}
	else // calculate transform from parent transform
	{
		return (GetJointWorldTransformMatrix((joints + joint)->GetParentIndex()) * (joints + joint)->GetLocalTransformationMatrix()).Transposed().ptr();
	}
}

int Skeleton::GetJointIndex(const char * joint_name) const
{
	std::string name = joint_name;
	for (int i = 0; i < num_joints; ++i)
	{
		if (name == joints[i].GetName())
			return i;
	}
	return -1;
}

void Skeleton::Save(Data & data) const
{
	if (num_joints != -1)
	{
		data.AddString("name", GetName());
		data.AddString("lib_path", GetLibraryPath());
		data.AddInt("UID", GetUID());

		data.AddInt("num_joints", num_joints);

		for (int i = 0; i < num_joints; ++i)
		{
			data.CreateSection("joint_" + std::to_string(i));

			data.AddString("name", (joints + i)->GetName());
			data.AddInt("parent_index", (joints + i)->GetParentIndex());

			float4 rot_quat;
			rot_quat.x = (joints + i)->GetPose().GetRotation().x;
			rot_quat.y = (joints + i)->GetPose().GetRotation().y;
			rot_quat.z = (joints + i)->GetPose().GetRotation().z;
			rot_quat.w = (joints + i)->GetPose().GetRotation().w;
			data.AddVector4("rotation", rot_quat);

			data.AddVector3("position", (joints + i)->GetPose().GetPosition());

			data.AddVector3("scale", (joints + i)->GetPose().GetScale());

			data.CloseSection();
		}
	}
}

bool Skeleton::Load(Data & data)
{
	bool ret = true;

	SetName(data.GetString("name"));
	SetUID(data.GetInt("UID"));
	SetLibraryPath(data.GetString("lib_path"));

	num_joints = data.GetInt("num_joints");

	if (num_joints < 1)
		ret = false;
	else
	{
		joints = new Joint[num_joints];

		for (int i = 0; i < num_joints; ++i)
		{
			data.EnterSection("joint_" + std::to_string(i));

			(joints + i)->SetName(data.GetString("name").c_str());
			(joints + i)->SetParentIndex(data.GetInt("parent_index"));

			Quat rot;
			float4 quat_rot = data.GetVector4("rotation");
			rot.x = quat_rot.x;
			rot.y = quat_rot.y;
			rot.z = quat_rot.z;
			rot.w = quat_rot.w;

			float3 pos = data.GetVector3("position");
			float3 scale = data.GetVector3("scale");

			(joints + i)->SetPose(rot, pos, scale);

			data.LeaveSection();
		}
	}

	return ret;
}

void Skeleton::CreateMeta() const
{
	time_t now = time(0);
	char* dt = ctime(&now);

	Data data;
	data.AddInt("Type", GetType());
	data.AddUInt("UUID", GetUID());
	data.AddString("Time_Created", dt);
	data.AddString("Library_path", GetLibraryPath());
	data.AddString("Name", GetName());

	data.SaveAsMeta(GetAssetsPath());
}

void Skeleton::LoadToMemory()
{
}

void Skeleton::UnloadFromMemory()
{
}

// ----- 