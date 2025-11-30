#include "stdafx.h"
#include "GameClear.h"
#include "GameTitle.h"
#include "SoundManager.h"
GameClear::GameClear()
{
}

GameClear::~GameClear()
{
}

bool GameClear::Start()
{
	m_GameClearRender.Init("Assets/sprite/GameClear1.dds", 1920.0f, 1080.0f);

	SoundManager* sound = FindGO<SoundManager>("soundmanager");
	p_gameClearBGM = sound->PlayingSound(enSound_GameClearBGM);

	return true;
}

void GameClear::Update()
{
	GameClearManager();
}

void GameClear::GameClearManager()
{
	//‰½‚©‚Ìƒ{ƒ^ƒ“‚ª‰Ÿ‚³‚ê‚Ä‚¢‚½‚ç
	if (g_pad[0]->IsPressAnyKey() && !p_isNewTitle)
	{
		SoundManager* sound = FindGO<SoundManager>("soundmanager");
		p_gameClearDecisionSE = sound->PlayingSound(enSound_DecisionSE, false);

		p_isNewTitle = true;
	}

	if (p_isNewTitle)
	{
		p_deltatime += g_gameTime->GetFrameDeltaTime();
	}

	if (p_deltatime > 3.0f)
	{
		p_isNewTitle = false;
		p_deltatime = 0.0f;
		m_gameTitle = NewGO<GameTitle>(0, "gametitle");
		DeleteGO(p_gameClearBGM);
		DeleteGO(p_gameClearDecisionSE);
		DeleteGO(this);
	}
}

void GameClear::Render(RenderContext& rc)
{
	m_GameClearRender.Draw(rc);
}
