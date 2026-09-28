#include "Time.h"

namespace
{
	// deltaTimeの上限(秒)。ウィンドウのドラッグ中やデバッガ停止後に、
	// 巨大なdeltaTimeでゲームが飛ぶのを防ぐ
	constexpr double kMaxDeltaTime = 0.1;

	// FPSを更新する間隔(秒)
	constexpr double kFpsUpdateInterval = 0.5;
}

void Time::Initialize()
{
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_startCounter);
	m_prevCounter = m_startCounter;

	m_deltaTime = 0.0;
	m_totalTime = 0.0;
	m_fpsTimer = 0.0;
	m_fpsFrameCount = 0;
	m_fps = 0.0f;
	m_fpsUpdated = false;
}

void Time::Tick()
{
	LARGE_INTEGER now{};
	QueryPerformanceCounter(&now);

	// カウンタの差を秒に変換
	m_deltaTime = static_cast<double>(now.QuadPart - m_prevCounter.QuadPart)
		/ static_cast<double>(m_frequency.QuadPart);
	m_totalTime = static_cast<double>(now.QuadPart - m_startCounter.QuadPart)
		/ static_cast<double>(m_frequency.QuadPart);
	m_prevCounter = now;

	// 経過時間の暴走を防ぐ
	if (m_deltaTime > kMaxDeltaTime)
	{
		m_deltaTime = kMaxDeltaTime;
	}

	// FPS計測: 一定時間ごとに「フレーム数 ÷ 経過時間」で平均を出す
	m_fpsUpdated = false;
	m_fpsTimer += m_deltaTime;
	++m_fpsFrameCount;

	if (m_fpsTimer >= kFpsUpdateInterval)
	{
		m_fps = static_cast<float>(m_fpsFrameCount / m_fpsTimer);
		m_fpsTimer = 0.0;
		m_fpsFrameCount = 0;
		m_fpsUpdated = true;
	}
}