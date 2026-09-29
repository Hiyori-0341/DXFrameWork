#pragma once
#include <windows.h>
#include "Window.h"
#include "Time.h"
#include "Graphics/GraphicsDevice.h"
#include "Input/InputSystem.h"
#include "Physics/PhysicsWorld.h"

// アプリケーション全体の流れ(初期化 → ループ → 終了)を管理するクラス
class Application
{
public:
	bool Initialize(HINSTANCE hInstance, int nCmdShow);
	int  Run();

private:
	void Update();
	void Render();
	void UpdateWindowTitle();

	Window         m_window;
	Time           m_time;
	GraphicsDevice m_graphics;
	InputSystem    m_input;
	PhysicsWorld   m_physics;

	//動作確認用
	JPH::BodyID m_debugFloorId;
	JPH::BodyID m_debugSphereId;
};