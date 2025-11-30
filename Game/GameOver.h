#pragma once
class GameTitle;
class GameOver : public IGameObject
{
public:
	GameOver();
	~GameOver();
	bool Start();
	void Update();
	void GameOverManager();
	void Render(RenderContext& rc);

	SpriteRender m_gameOverSpriteRender;
	GameTitle* m_title;


private:
	SoundSource* p_gameOverBGM;
	SoundSource* p_gameOverDecisionSE;

	float p_deltatime = 0.0f;//画面の移行に使う時間

	bool p_isNewTitle = false;//ゲーム画面に移行できるかどうかのフラグ
};

