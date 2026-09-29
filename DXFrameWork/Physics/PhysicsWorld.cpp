#include "PhysicsWorld.h"

#include <cstdarg>
#include <cstdio>
#include <thread>
#include <windows.h>

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

using namespace JPH;

namespace
{
	// Joltのトレース(ログ)出力を、出力ウィンドウに流す
	void TraceImpl(const char* inFMT, ...)
	{
		va_list list;
		va_start(list, inFMT);
		char buffer[1024];
		vsnprintf(buffer, sizeof(buffer), inFMT, list);
		va_end(list);

		OutputDebugStringA(buffer);
		OutputDebugStringA("\n");
	}

	constexpr uint kMaxBodies = 1024;
	constexpr uint kMaxBodyPairs = 1024;
	constexpr uint kMaxContactConstraints = 1024;
	constexpr uint kNumBodyMutexes = 0; // 0で自動
}

bool PhysicsWorld::Initialize()
{
	// --- Joltの初期化(プロセス内で最初に一度だけ呼ぶ) ---
	// すでに他の場所で初期化済みの場合は、ここは呼ばないようにする
	if (Factory::sInstance == nullptr)
	{
		RegisterDefaultAllocator();
		Trace = TraceImpl;
		Factory::sInstance = new Factory();
		RegisterTypes();
	}

	// フレームごとの一時的なメモリ確保用(10MB)
	m_tempAllocator = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);

	// 物理演算をマルチスレッドで回すためのジョブシステム
	const unsigned int threadCount =
		(std::thread::hardware_concurrency() > 1)
		? std::thread::hardware_concurrency() - 1
		: 1;

	m_jobSystem = std::make_unique<JobSystemThreadPool>(
		JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, threadCount);

	m_physicsSystem.Init(
		kMaxBodies, kNumBodyMutexes, kMaxBodyPairs, kMaxContactConstraints,
		m_broadPhaseLayerInterface, m_objectVsBroadPhaseLayerFilter, m_objectLayerPairFilter);

	m_initialized = true;
	return true;
}

void PhysicsWorld::Shutdown()
{
	if (!m_initialized)
	{
		return;
	}

	m_jobSystem.reset();
	m_tempAllocator.reset();

	// Factoryの解放は、アプリ全体で1回だけでよい。
	// 複数のPhysicsWorldを作らない前提なので、ここで行う
	if (Factory::sInstance != nullptr)
	{
		UnregisterTypes();
		delete Factory::sInstance;
		Factory::sInstance = nullptr;
	}

	m_initialized = false;
}

void PhysicsWorld::Update(float deltaTime)
{
	if (!m_initialized || deltaTime <= 0.0f)
	{
		return;
	}

	m_physicsSystem.Update(deltaTime, kCollisionSteps, m_tempAllocator.get(), m_jobSystem.get());
}

BodyID PhysicsWorld::CreateStaticBox(RVec3Arg position, Vec3Arg halfExtent)
{
	BodyInterface& bodyInterface = m_physicsSystem.GetBodyInterface();

	BodyCreationSettings settings(
		new BoxShape(halfExtent),
		position,
		Quat::sIdentity(),
		EMotionType::Static,
		Layers::NON_MOVING);

	return bodyInterface.CreateAndAddBody(settings, EActivation::DontActivate);
}

BodyID PhysicsWorld::CreateDynamicSphere(RVec3Arg position, float radius)
{
	BodyInterface& bodyInterface = m_physicsSystem.GetBodyInterface();

	BodyCreationSettings settings(
		new SphereShape(radius),
		position,
		Quat::sIdentity(),
		EMotionType::Dynamic,
		Layers::MOVING);

	return bodyInterface.CreateAndAddBody(settings, EActivation::Activate);
}

void PhysicsWorld::RemoveAndDestroyBody(BodyID bodyId)
{
	BodyInterface& bodyInterface = m_physicsSystem.GetBodyInterface();
	bodyInterface.RemoveBody(bodyId);
	bodyInterface.DestroyBody(bodyId);
}

RVec3 PhysicsWorld::GetBodyPosition(BodyID bodyId) const
{
	return m_physicsSystem.GetBodyInterface().GetCenterOfMassPosition(bodyId);
}