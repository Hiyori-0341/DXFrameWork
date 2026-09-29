#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

using namespace JPH;

//エンジンで使う衝突レイヤーの定義
//動かないもの、動くものの最小構成
//増やす場合、ここに追加する(プレイヤーなど)
namespace Layers
{
	static constexpr ObjectLayer NON_MOVING = 0;
	static constexpr ObjectLayer MOVING = 1;
	static constexpr ObjectLayer NUM_LAYERS = 2;
}

//どのレイヤー同士が衝突判定を行うかを定義するクラス
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
public:
	bool ShouldCollide(ObjectLayer inObject1,ObjectLayer inObject2)const override
	{
		switch (inObject1)
		{
			case Layers::NON_MOVING:
				return inObject2 == Layers::MOVING;
			case Layers::MOVING:
				return true;
			default:
				return false;
		}
	}
};

//ブロードフェーズ用レイヤー定義
namespace BroadPhaseLayers
{
	static constexpr BroadPhaseLayer NON_MOVING(0);
	static constexpr BroadPhaseLayer MOVING(1);
	static constexpr uint			 NUM_LAYERS(2);
}

class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:
    BPLayerInterfaceImpl()
    {
        mObjectToBroadPhase[Layers::NON_MOVING] =
            BroadPhaseLayers::NON_MOVING;

        mObjectToBroadPhase[Layers::MOVING] =
            BroadPhaseLayers::MOVING;
    }

    uint GetNumBroadPhaseLayers() const override
    {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    BroadPhaseLayer GetBroadPhaseLayer(
        ObjectLayer inLayer) const override
    {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

    const char* GetBroadPhaseLayerName(
        BroadPhaseLayer inLayer) const override
    {
        switch ((BroadPhaseLayer::Type)inLayer)
        {
        case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
            return "NON_MOVING";

        case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
            return "MOVING";

        default:
            JPH_ASSERT(false);
            return "INVALID";
        }
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