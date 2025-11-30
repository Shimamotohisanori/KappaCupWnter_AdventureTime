#pragma once
class Game;
class GameTitle : public IGameObject
{
public:
	GameTitle();
	~GameTitle();
	bool Start();
	void Update();
	void TitleManager();
	void Render(RenderContext& rc);

	SpriteRender m_titleSpriteRender;
	Game* m_game;


private:
	SoundSource* p_titleBGM;
	SoundSource* p_titleDecisionSE;

	float p_deltatime = 0.0f;//画面の移行に使う時間

	bool p_isNewGame = false;//ゲーム画面に移行できるかどうかのフラグ
};

