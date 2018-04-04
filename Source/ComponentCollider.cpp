#include "ComponentCollider.h"
#include "GameObject.h"
#include "ComponentRigidBody.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ComponentMeshRenderer.h"
#include "MathGeoLib/AABB.h"
#include "Mesh.h"
#include "MathGeoLib/Quat.h"
#include "PhysicsMaterial.h"
#include "ModuleResources.h"

ComponentCollider::ComponentCollider(GameObject* attached_gameobject, ColliderType type)
{
	SetActive(true);
	SetGameObject(attached_gameobject);
	collider_type = type;
	std::string name;
	capsule_direction = CapsuleDirection::XAxis;
	collider_shape = nullptr;
	convex_mesh = nullptr;
	triangle_mesh = nullptr;
	geo_triangle_mesh = nullptr;
	geo_convex_mesh = nullptr;
	is_convex = false;
	phys_material = nullptr;
	rb_is_released = false;

	rigidbody = (ComponentRigidBody*)attached_gameobject->GetComponent(Component::CompRigidBody);
	ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)attached_gameobject->GetComponent(Component::CompMeshRenderer);
	AABB box;
	box.SetFromCenterAndSize({ 0,0,0 }, { 1,1,1 });
	if (mesh_renderer != nullptr)
	{
		box = mesh_renderer->bounding_box;
	}

	if (rigidbody != nullptr)
	{
		collider_material = App->physics->CreateMaterial(0.6, 0.6, 0);
		switch (type)
		{
		case ComponentCollider::BoxCollider:
		{
			physx::PxBoxGeometry geo_box(box.HalfSize().x, box.HalfSize().y, box.HalfSize().z);
			collider_shape = App->physics->CreateShape(*rigidbody->GetRigidBody(), geo_box, *collider_material);
			SetColliderCenter(box.CenterPoint());
			SetType(ComponentType::CompBoxCollider);
			name += "Box_";
		}
			break;
		case ComponentCollider::SphereCollider:
		{
			physx::PxSphereGeometry geo_sphere(box.MinimalEnclosingSphere().r);
			collider_shape = App->physics->CreateShape(*rigidbody->GetRigidBody(), geo_sphere, *collider_material);
			SetColliderCenter(box.CenterPoint());
			SetType(ComponentType::CompSphereCollider);
			name += "Sphere_";
		}
			break;
		case ComponentCollider::CapsuleCollider:
		{
			physx::PxCapsuleGeometry geo_capsule(box.MinimalEnclosingSphere().r, box.HalfSize().y);
			collider_shape = App->physics->CreateShape(*rigidbody->GetRigidBody(), geo_capsule, *collider_material);
			SetColliderCenter(box.CenterPoint());
			SetType(ComponentType::CompCapsuleCollider);
			name += "Capsule_";
		}
			break;
		case ComponentCollider::MeshCollider:
		{
			rigidbody->SetKinematic(true);
			Mesh* collider_mesh = mesh_renderer->GetMesh();
			triangle_mesh = App->physics->CreateTriangleMesh(collider_mesh);
			geo_triangle_mesh = new physx::PxTriangleMeshGeometry(triangle_mesh);
			convex_mesh = App->physics->CreateConvexMesh(collider_mesh);
			geo_convex_mesh = new physx::PxConvexMeshGeometry(convex_mesh);
			geo_convex_mesh->meshFlags.set(physx::PxConvexMeshGeometryFlag::eTIGHT_BOUNDS);
			ChangeMeshToConvex(false);
			SetType(ComponentType::CompMeshCollider);
			name += "Mesh_";
			collider_shape = App->physics->CreateShape(*rigidbody->GetRigidBody(), *geo_triangle_mesh, *collider_material);
		}
			break;
		case ComponentCollider::TerrainCollider:
		{
			name += "Terrain_";
		}
			break;
		default:
			break;
		}

		name += "Collider ";
		SetName(name);
	}

	SetTrigger(false);
	if(collider_shape != nullptr)
		collider_shape->userData = this;
}

ComponentCollider::~ComponentCollider()
{
	/*if (triangle_mesh)
	{
		triangle_mesh->release();
	}
	if (convex_mesh)
	{
		convex_mesh->release();
	}*/
	//collider_material->release();
	//collider_shape->release();

	if (rigidbody != nullptr && !rb_is_released)
	{
		rigidbody->RemoveShape(*GetColliderShape());
	}
	else
	{
		rigidbody = nullptr;
	}
	RELEASE(phys_material);
	RELEASE(geo_triangle_mesh);
	RELEASE(geo_convex_mesh);
}

ComponentCollider::ColliderType ComponentCollider::GetColliderType() const
{
	return collider_type;
}

void ComponentCollider::SetColliderMaterial(PhysicsMaterial * mat)
{
	collider_material->setStaticFriction(mat->GetStaticFriction());
	collider_material->setDynamicFriction(mat->GetDynamicFriction());
	collider_material->setRestitution(mat->GetRestitution());
	collider_material->setFrictionCombineMode((physx::PxCombineMode::Enum)mat->GetFrictionMode());
	collider_material->setRestitutionCombineMode((physx::PxCombineMode::Enum)mat->GetRestitutionMode());
	phys_material = mat;
}

PhysicsMaterial * ComponentCollider::GetColliderMaterial() const
{
	return phys_material;
}

void ComponentCollider::SetColliderShape(physx::PxShape * shape)
{
	collider_shape = shape;
}

physx::PxShape * ComponentCollider::GetColliderShape() const
{
	return collider_shape;
}

void ComponentCollider::SetTrigger(bool trigger)
{
	bool can_change = true;

	if (trigger)
	{
		if (collider_shape != nullptr && IsActive() && collider_shape->getGeometryType() == physx::PxGeometryType::eTRIANGLEMESH)
		{
			can_change = false;
			CONSOLE_ERROR("Non-Convex mesh collider can't be trigger. If you need to set it as trigger, set it convex");
		}
	}

	if (can_change)
	{
		is_trigger = trigger;
		if (is_trigger)
		{
			if (collider_shape != nullptr && collider_shape->getFlags().isSet(physx::PxShapeFlag::eSIMULATION_SHAPE))
			{
				collider_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !trigger);
				collider_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, trigger);
			}
		}
		else
		{
			if (collider_shape != nullptr && collider_shape->getFlags().isSet(physx::PxShapeFlag::eTRIGGER_SHAPE))
			{
				collider_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, trigger);
				collider_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !trigger);
			}
		}
	}
}

bool ComponentCollider::IsTrigger() const
{
	return is_trigger;
}

void ComponentCollider::SetColliderCenter(float3 center)
{
	if (collider_shape != nullptr)
	{
		physx::PxTransform transform = collider_shape->getLocalPose();
		transform.p.x = center.x;
		transform.p.y = center.y;
		transform.p.z = center.z;
		collider_shape->setLocalPose(transform);
	}
}

float3 ComponentCollider::GetColliderCenter() const
{
	if (collider_shape != nullptr)
	{
		physx::PxTransform transform = collider_shape->getLocalPose();
		float3 pos(transform.p.x, transform.p.y, transform.p.z);
		return pos;
	}
	else
		return float3::zero;
}

void ComponentCollider::SetBoxSize(float3 size)
{
	if (collider_shape != nullptr)
	{
		if (collider_type == ColliderType::BoxCollider)
		{
			physx::PxBoxGeometry geo_box;
			if (collider_shape->getBoxGeometry(geo_box))
			{
				geo_box.halfExtents.x = size.x * 0.5f;
				geo_box.halfExtents.y = size.y * 0.5f;
				geo_box.halfExtents.z = size.z * 0.5f;
				collider_shape->setGeometry(geo_box);
			}
		}
	}
}

float3 ComponentCollider::GetBoxSize() const
{
	if (collider_shape != nullptr)
	{
		float3 ret;
		if (collider_type == ColliderType::BoxCollider)
		{
			physx::PxBoxGeometry geo_box;
			if (collider_shape->getBoxGeometry(geo_box))
			{
				ret.x = geo_box.halfExtents.x * 2;
				ret.y = geo_box.halfExtents.y * 2;
				ret.z = geo_box.halfExtents.z * 2;
			}
		}
		return ret;
	}
	else
		return float3::zero;
}

void ComponentCollider::SetSphereRadius(float radius)
{
	if (collider_shape != nullptr)
	{
		if (collider_type == ColliderType::SphereCollider)
		{
			physx::PxSphereGeometry geo_sphere;
			if (collider_shape->getSphereGeometry(geo_sphere))
			{
				geo_sphere.radius = radius;
				collider_shape->setGeometry(geo_sphere);
			}
		}
	}
}

float ComponentCollider::GetSphereRadius() const
{
	if (collider_shape != nullptr)
	{
		float ret;
		if (collider_type == ColliderType::SphereCollider)
		{
			physx::PxSphereGeometry geo_sphere;
			if (collider_shape->getSphereGeometry(geo_sphere))
			{
				ret = geo_sphere.radius;
			}
		}
		return ret;
	}
	else
		return 0;
}

void ComponentCollider::SetCapsuleRadius(float radius)
{
	if (collider_shape != nullptr)
	{
		if (collider_type == ColliderType::CapsuleCollider)
		{
			physx::PxCapsuleGeometry geo_capsule;
			if (collider_shape->getCapsuleGeometry(geo_capsule))
			{
				geo_capsule.radius = radius;
				collider_shape->setGeometry(geo_capsule);
			}
		}
	}
}

float ComponentCollider::GetCapsuleRadius() const
{
	if (collider_shape != nullptr)
	{
		float ret;
		if (collider_type == ColliderType::CapsuleCollider)
		{
			physx::PxCapsuleGeometry geo_capsule;
			if (collider_shape->getCapsuleGeometry(geo_capsule))
			{
				ret = geo_capsule.radius;
			}
		}
		return ret;
	}
	else
		return 0;
}

void ComponentCollider::SetCapsuleHeight(float height)
{
	if (collider_shape != nullptr)
	{
		if (collider_type == ColliderType::CapsuleCollider)
		{
			physx::PxCapsuleGeometry geo_capsule;
			if (collider_shape->getCapsuleGeometry(geo_capsule))
			{
				geo_capsule.halfHeight = height * 0.5f;
				collider_shape->setGeometry(geo_capsule);
			}
		}
	}
}

float ComponentCollider::GetCapsuleHeight() const
{
	if (collider_shape != nullptr)
	{
		float ret;
		if (collider_type == ColliderType::CapsuleCollider)
		{
			physx::PxCapsuleGeometry geo_capsule;
			if (collider_shape->getCapsuleGeometry(geo_capsule))
			{
				ret = geo_capsule.halfHeight * 2;
			}
		}
		return ret;
	}
	else
		return 0;
}

void ComponentCollider::SetColliderActive(bool active)
{
	if (collider_shape != nullptr)
	{
		SetActive(active);
		ComponentRigidBody* rigidbody = (ComponentRigidBody*)GetGameObject()->GetComponent(Component::CompRigidBody);
		if (rigidbody)
		{
			if (active)
			{
				rigidbody->GetRigidBody()->attachShape(*collider_shape);
			}
			else
			{
				rigidbody->GetRigidBody()->detachShape(*collider_shape);
			}
		}
	}
}

void ComponentCollider::SetCapsuleDirection(CapsuleDirection direction)
{
	float3 axis_direction;

	switch (direction)
	{
	case 0:
	{
		axis_direction.x = 0;
		axis_direction.y = 0;
		axis_direction.z = 0;
	}
	break;
	case 1:
	{
		axis_direction.x = 0;
		axis_direction.y = 0;
		axis_direction.z = 90;
	}
	break;
	case 2:
	{
		axis_direction.x = 0;
		axis_direction.y = 90;
		axis_direction.z = 0;
	}
	break;
	default:
		break;
	}

	if (collider_shape != nullptr)
	{
		physx::PxTransform t = collider_shape->getLocalPose();
		Quat rotation = Quat::FromEulerXYZ(axis_direction.x * DEGTORAD, axis_direction.y * DEGTORAD, axis_direction.z * DEGTORAD);
		physx::PxQuat q(rotation.x, rotation.y, rotation.z, rotation.w);
		t.q = q;
		collider_shape->setLocalPose(t);
		capsule_direction = direction;
	}
}

ComponentCollider::CapsuleDirection ComponentCollider::GetCapsuleDirection() const
{
	return capsule_direction;
}

void ComponentCollider::ChangeMeshToConvex(bool set_convex)
{
	if (collider_shape != nullptr)
	{
		if (collider_type != MeshCollider) return;
		ComponentRigidBody* rigidbody = (ComponentRigidBody*)GetGameObject()->GetComponent(Component::CompRigidBody);
		if (rigidbody)
		{
			if (set_convex)
			{
				if (collider_shape)
				{
					rigidbody->RemoveShape(*collider_shape);
				}
				collider_shape = App->physics->CreateShape(*rigidbody->GetRigidBody(), *geo_convex_mesh, *collider_material);
				if (collider_shape != nullptr)
				{
					collider_shape->userData = this;
				}
			}
			else
			{
				if (collider_shape)
				{
					rigidbody->RemoveShape(*collider_shape);
				}
				rigidbody->SetKinematic(true);
				collider_shape = App->physics->CreateShape(*rigidbody->GetRigidBody(), *geo_triangle_mesh, *collider_material);
				if (collider_shape != nullptr)
				{
					collider_shape->userData = this;
					SetTrigger(false);
				}
			}
		}

		is_convex = set_convex;
	}
}

bool ComponentCollider::IsConvex() const
{
	return is_convex;
}

void ComponentCollider::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.AddBool("IsConvex", is_convex);
	data.AddBool("IsTrigger", is_trigger);
	data.AddInt("CapsuleDir", capsule_direction);
	if (phys_material)
	{
		data.AddBool("HaveMaterial", true);
		data.CreateSection("PhysMaterial");
		data.AddInt("UUID", phys_material->GetUID());
		data.CloseSection();
	}
	else
	{
		data.AddBool("HaveMaterial", false);
	}
	data.AddVector3("ColliderCenter", GetColliderCenter());
	switch (collider_type)
	{
	case ComponentCollider::BoxCollider:
		data.CreateSection("Box");
		data.AddVector3("BoxSize", GetBoxSize());
		data.CloseSection();
		break;
	case ComponentCollider::SphereCollider:
		data.CreateSection("Sphere");
		data.AddFloat("SphereRadius", GetSphereRadius());
		data.CloseSection();
		break;
	case ComponentCollider::CapsuleCollider:
		data.CreateSection("Capsule");
		data.AddFloat("CapsuleRadius", GetCapsuleRadius());
		data.AddFloat("CapsuleHeight", GetCapsuleHeight());
		data.CloseSection();
		break;
	default:
		break;
	}
	
}

void ComponentCollider::Load(Data & data)
{
	SetType((ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	ChangeMeshToConvex(data.GetBool("IsConvex"));
	SetTrigger(data.GetBool("IsTrigger"));
	SetCapsuleDirection((CapsuleDirection)data.GetInt("CapsuleDir"));
	if (data.GetBool("HaveMaterial"))
	{
		data.EnterSection("PhysMaterial");
		uint mat_id = data.GetUInt("UUID");
		phys_material = App->resources->GetPhysMaterial(mat_id);
		data.LeaveSection();
	}
	SetColliderCenter(data.GetVector3("ColliderCenter"));
	if (data.EnterSection("Box"))
	{
		SetBoxSize(data.GetVector3("BoxSize"));
		data.LeaveSection();
	}
	if (data.EnterSection("Sphere"))
	{
		SetSphereRadius(data.GetFloat("SphereRadius"));
		data.LeaveSection();
	}
	if (data.EnterSection("Capsule"))
	{
		SetCapsuleRadius(data.GetFloat("CapsuleRadius"));
		SetCapsuleHeight(data.GetFloat("CapsuleHeight"));
		data.LeaveSection();
	}
}

ComponentRigidBody * ComponentCollider::GetRigidBody() const
{
	return rigidbody;
}


