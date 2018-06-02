#pragma once
#include "Component.h"
#include "Nvidia/PhysX/Include/PxShape.h"
#include "Nvidia/PhysX/Include/PxMaterial.h"
#include "Nvidia/PhysX/Include/PxSimulationEventCallback.h"

class ComponentRigidBody;
class PhysicsMaterial;

class ComponentCollider :
	public Component
{
public:

	enum ColliderType
	{
		BoxCollider, SphereCollider, CapsuleCollider, MeshCollider, TerrainCollider
	};

	enum CapsuleDirection
	{
		XAxis, YAxis, ZAxis
	};

	ComponentCollider(GameObject* attached_gameobject, ColliderType type);
	~ComponentCollider();

	ColliderType GetColliderType() const;
	void SetColliderMaterial(PhysicsMaterial* mat);
	PhysicsMaterial* GetColliderMaterial() const;
	void SetColliderShape(physx::PxShape* shape);
	physx::PxShape* GetColliderShape() const;
	void SetTrigger(bool trigger);
	bool IsTrigger() const;
	void SetColliderCenter(float3 center);
	float3 GetColliderCenter() const;
	void SetBoxSize(float3 size);
	float3 GetBoxSize() const;
	void SetSphereRadius(float radius);
	float GetSphereRadius() const;
	void SetCapsuleRadius(float radius);
	float GetCapsuleRadius() const;
	void SetCapsuleHeight(float height);
	float GetCapsuleHeight() const;
	void SetColliderActive(bool active);
	void SetCapsuleDirection(CapsuleDirection direction);
	CapsuleDirection GetCapsuleDirection() const;
	void ChangeMeshToConvex(bool set_convex);
	bool IsConvex() const;

	float3 GetColliderWorldPos() const;

	void Save(Data& data) const;
	void Load(Data& data);

	ComponentRigidBody* GetRigidBody() const;

	bool rb_is_released;

	void OnEnable();
	void OnDisable();

private:
	ColliderType collider_type;
	physx::PxMaterial* collider_material;
	physx::PxShape* collider_shape;
	bool is_trigger;
	CapsuleDirection capsule_direction;
	bool is_convex;
	physx::PxConvexMesh* convex_mesh;
	physx::PxTriangleMesh* triangle_mesh;
	physx::PxTriangleMeshGeometry* geo_triangle_mesh;
	physx::PxConvexMeshGeometry* geo_convex_mesh;
	ComponentRigidBody* rigidbody;
	PhysicsMaterial* phys_material;
};

