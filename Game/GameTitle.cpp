#include "stdafx.h"
#include "GameTitle.h"
#include "Game.h"
#include "SoundManager.h"
GameTitle::GameTitle()
{
}

GameTitle::~GameTitle()
{
}

bool GameTitle::Start()
{
	m_titleSpriteRender.Init("Assets/sprite/GameTitle1.dds", 1920.0f, 1080.0f);
	
	//PlayingSound()を使用するため、SoundManagerのインスタンスを取得。
	SoundManager* sound = FindGO<SoundManager>("soundmanager");
	//BGMの再生
	p_titleBGM = sound->PlayingSound(enSound_GameTitleBGM,true,0.5f);
	return true;
}

void GameTitle::Update()
{
	TitleManager();
}

void GameTitle::TitleManager()
{
	//何かのボタンが押されていたら
	if (g_pad[0]->IsPressAnyKey() && !p_isNewGame)
	{
		SoundManager* sound = FindGO<SoundManager>("soundmanager");
		p_titleDecisionSE = sound->PlayingSound(enSound_DecisionSE, false);

		p_isNewGame = true;
	}

	if (p_isNewGame)
	{
		p_deltatime += g_gameTime->GetFrameDeltaTime();
	}

	if (p_deltatime > 3.0f)
	{
		p_isNewGame = false;
		p_deltatime = 0.0f;
		m_game = NewGO<Game>(0, "game");
		DeleteGO(p_titleBGM);
		DeleteGO(p_titleDecisionSE);
		DeleteGO(this);
	}
}

void GameTitle::Render(RenderContext& rc)
{
	m_titleSpriteRender.Draw(rc);
}