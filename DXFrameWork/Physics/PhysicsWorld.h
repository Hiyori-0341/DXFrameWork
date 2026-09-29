#pragma once

#include <memory>
#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyID.h>

#include "PhysicsLayers.h"

//Jolt Physicsの初期化、終了、物理ワールドの更新をまとめるクラス
//GameObject/RigitBodyができるまで
//このクラスを直接使用して剛体を追加する
class PhysicsWorld
{
public:
	bool Initialize();
	void Shutdown();

	void Update(float deltaTime);

	//剛体の生成(最小限)
	//後でRigidBodyに置き換える

	JPH::BodyID CreateStaticBox(JPH::RVec3Arg position, JPH::RVec3Arg halfExtent);

	//動的な球を生成
	JPH::BodyID CreateDynamicSphere(JPH::RVec3Arg position, float radius);

	void RemoveAndDestroyBody(JPH::BodyID bodyID);

	JPH::RVec3 GetBodyPosition(JPH::BodyID bodyID)const;

	JPH::PhysicsSystem& GetPhysicsSystem() { return m_physicsSystem; }

private:
	//物理演算シュミレーションを何回に分けて進めるか（通常１）
	static constexpr int kCollisionSteps = 1;

	std::unique_ptr<JPH::TempAllocatorImpl> m_tempAllocator;
	std::unique_ptr<JPH::JobSystemThreadPool> m_jobSystem;

	BPLayerInterfaceImpl m_broadPhaseLayerInterface;
	ObjectVsBroadPhaseLayerFilter m_objectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImpl m_objectLayerPairFilter;

	JPH::PhysicsSystem m_physicsSystem;
	bool m_initialized = false;
};