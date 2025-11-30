#include "stdafx.h"
#include "GameOver.h"
#include "GameTitle.h"
#include "SoundManager.h"
GameOver::GameOver()
{
}

GameOver::~GameOver()
{
}

bool GameOver::Start()
{
	m_gameOverSpriteRender.Init("Assets/sprite/GameOver1.dds", 1920.0f, 1080.0f);

	SoundManager* sound = FindGO<SoundManager>("soundmanager");
	p_gameOverBGM = sound->PlayingSound(enSound_GameOverBGM);

	return true;
}

void GameOver::Update()
{
	GameOverManager();
}

void GameOver::GameOverManager()
{
	//‰½‚©‚Ìƒ{ƒ^ƒ“‚ª‰Ÿ‚³‚ê‚Ä‚¢‚½‚ç
	if (g_pad[0]->IsPressAnyKey() && !p_isNewTitle)
	{
		SoundManager* sound = FindGO<SoundManager>("soundmanager");
		p_gameOverDecisionSE = sound->PlayingSound(enSound_DecisionSE, false);

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
		m_title = NewGO<GameTitle>(0, "gametitle");
		DeleteGO(p_gameOverBGM);
		DeleteGO(p_gameOverDecisionSE);
		DeleteGO(this);
	}
}

void GameOver::Render(RenderContext& rc)
{
	m_gameOverSpriteRender.Draw(rc);
}