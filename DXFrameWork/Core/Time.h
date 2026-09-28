#pragma once
#include <windows.h>

//フレーム間の経過時間、起動からの経過時間、FPSを管理する
class Time
{
public:

	//周波数の取得と標準時刻の記録
	void Initialize();
	
	//毎フレーム先頭で呼ぶ
	void Tick();

	// 前フレームからの経過時間(秒)
	float GetDeltaTime() const { return static_cast<float>(m_deltaTime); }

	// Initialize からの経過時間(秒)
	float GetTotalTime() const { return static_cast<float>(m_totalTime); }

	// 直近の平均FPS(一定間隔ごとに更新される)
	float GetFPS() const { return m_fps; }

	// このフレームでFPSの値が更新されたか(タイトルバー表示の更新タイミングに使う)
	bool IsFpsUpdated() const { return m_fpsUpdated; }

private:
	LARGE_INTEGER m_frequency{};		// 1秒あたりのカウント数
	LARGE_INTEGER m_startCounter{};		// Initialize 時のカウンタ
	LARGE_INTEGER m_prevCounter{};		// 前フレームのカウンタ

	double m_deltaTime = 0.0;
	double m_totalTime = 0.0;

	// FPS計測用
	double m_fpsTimer = 0.0;			// 計測開始からの経過時間
	int    m_fpsFrameCount = 0;			// 計測中のフレーム数
	float  m_fps = 0.0f;
	bool   m_fpsUpdated = false;
};