// ============================================================
// Jolt Physics 最小動作確認(HelloWorld相当)
//
// 床(静的な箱)の上に、球(動的な剛体)を1つ落として、
// 毎フレームのY座標をコンソールに出力する。
//
// 確認が終わったら、このファイルはプロジェクトから外し、
// 本来の main.cpp(WinMainを含むもの)に戻すこと。
// ============================================================

#include <iostream>
#include <cstdarg>
#include <thread>

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

using namespace JPH;
using namespace JPH::literals;

// ------------------------------------------------------------
// Joltが内部で使うレイヤーの定義。
// ここでは「静止物(NON_MOVING)」と「動く物(MOVING)」の
// 2種類だけを使う、一番シンプルな構成にしている。
// ------------------------------------------------------------
namespace Layers
{
	static constexpr ObjectLayer NON_MOVING = 0;
	static constexpr ObjectLayer MOVING = 1;
	static constexpr ObjectLayer NUM_LAYERS = 2;
};

// どのレイヤー同士が衝突判定を行うかを定義するクラス
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
public:
	bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
	{
		switch (inObject1)
		{
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING; // 静止物は動く物とだけ衝突
		case Layers::MOVING:
			return true; // 動く物は何とでも衝突
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

// ブロードフェーズ(大まかな衝突判定)用のレイヤー定義。
// 小規模なシーンなので、ObjectLayerと1対1に対応させている
namespace BroadPhaseLayers
{
	static constexpr BroadPhaseLayer NON_MOVING(0);
	static constexpr BroadPhaseLayer MOVING(1);
	static constexpr uint NUM_LAYERS(2);
};

class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl()
	{
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	uint GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}

private:
	BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

// ObjectLayer と BroadPhaseLayer の組み合わせが衝突判定対象かどうかを判定するクラス
class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
public:
	bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

// Joltのトレース(ログ)出力を、標準出力に流すための関数
static void TraceImpl(const char* inFMT, ...)
{
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);
	std::cout << buffer << std::endl;
}

int main()
{
	// --- 1. Joltの初期化(この4行は、Joltを使う前に必ず一度だけ呼ぶ) ---
	RegisterDefaultAllocator();
	Trace = TraceImpl;
	Factory::sInstance = new Factory();
	RegisterTypes();

	// --- 2. 物理演算に必要な補助オブジェクトの用意 ---
	// フレームごとの一時的なメモリ確保に使うアロケータ(10MB)
	TempAllocatorImpl tempAllocator(10 * 1024 * 1024);

	// 物理演算をマルチスレッドで回すためのジョブシステム
	JobSystemThreadPool jobSystem(
		JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers,
		std::thread::hardware_concurrency() - 1);

	BPLayerInterfaceImpl broadPhaseLayerInterface;
	ObjectVsBroadPhaseLayerFilterImpl objectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImpl objectLayerPairFilter;

	// --- 3. 物理ワールドの作成 ---
	PhysicsSystem physicsSystem;
	physicsSystem.Init(
		1024,	// 最大剛体数
		0,		// ボディミューテックスの数(0で自動)
		1024,	// 最大ボディペア数
		1024,	// 最大コンタクトコンストレイント数
		broadPhaseLayerInterface,
		objectVsBroadPhaseLayerFilter,
		objectLayerPairFilter);

	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

	// --- 4. 床(静的な箱)を作る ---
	// 中心(0,-1,0)、半径(100, 1, 100)の巨大な箱を床として使う
	BoxShapeSettings floorShapeSettings(Vec3(100.0f, 1.0f, 100.0f));
	ShapeRefC floorShape = floorShapeSettings.Create().Get();

	BodyCreationSettings floorSettings(
		floorShape,
		RVec3(0.0_r, -1.0_r, 0.0_r),
		Quat::sIdentity(),
		EMotionType::Static,
		Layers::NON_MOVING);

	Body* floor = bodyInterface.CreateBody(floorSettings);
	bodyInterface.AddBody(floor->GetID(), EActivation::DontActivate);

	// --- 5. 球(動的な剛体)を作る。高さ2の位置から落とす ---
	BodyCreationSettings sphereSettings(
		new SphereShape(0.5f),
		RVec3(0.0_r, 2.0_r, 0.0_r),
		Quat::sIdentity(),
		EMotionType::Dynamic,
		Layers::MOVING);

	BodyID sphereId = bodyInterface.CreateAndAddBody(sphereSettings, EActivation::Activate);

	// 球に初速を与えてみる(任意。無くても重力で落ちる)
	bodyInterface.SetLinearVelocity(sphereId, Vec3(0.0f, -1.0f, 0.0f));

	// --- 6. シミュレーションループ ---
	// PhysicsSystem::Update を呼ぶ前に、一度だけ最適化しておくと少し速くなる
	physicsSystem.OptimizeBroadPhase();

	const float deltaTime = 1.0f / 60.0f;
	const int stepsPerUpdate = 1; // 60fps駆動ならcollisionStepsは1でよい

	for (int step = 0; step < 120; ++step) // 2秒分(60fps × 120)だけ回す
	{
		physicsSystem.Update(deltaTime, stepsPerUpdate, &tempAllocator, &jobSystem);

		RVec3 position = bodyInterface.GetCenterOfMassPosition(sphereId);
		std::cout << "Step " << step << ": Y = " << position.GetY() << std::endl;
	}

	// --- 7. 後片付け ---
	bodyInterface.RemoveBody(sphereId);
	bodyInterface.DestroyBody(sphereId);
	bodyInterface.RemoveBody(floor->GetID());
	bodyInterface.DestroyBody(floor->GetID());

	UnregisterTypes();
	delete Factory::sInstance;
	Factory::sInstance = nullptr;

	std::cout << "Jolt Physics HelloWorld: 完了。何かキーを押すと終了します。" << std::endl;
	std::cin.get();

	return 0;
}