#pragma once
class GameTitle;
class GameClear : public IGameObject
{
public:
	GameClear();
	~GameClear();
	bool Start();
	void Update();
	void GameClearManager();
	void Render(RenderContext& rc);

	SpriteRender m_GameClearRender;
	GameTitle* m_gameTitle;


private:
	SoundSource* p_gameClearBGM;
	SoundSource* p_gameClearDecisionSE;

	float p_deltatime = 0.0f;//画面の移行に使う時間

	bool p_isNewTitle = false;//ゲーム画面に移行できるかどうかのフラグ
};

