#pragma once
class Game;
class Player : public IGameObject
{
public:
	Player();
	~Player();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	void Move();
	void Rotation();
	void ManageState();
	void PlayAnimation();
	void PlayerHPManager();
	void ResetPosition();

	float AddGameFrame(float time)
	{
		return time += 20 * g_gameTime->GetFrameDeltaTime();
	}
	ModelRender m_playerModelRender;//モデルレンダー
	Vector3 m_playerPosition;			//座標。
	CharacterController m_playerCharacterController;  //キャラクターコントローラー。
	Vector3 m_playerMoveSpeed = Vector3::Zero; //移動速度。
	Vector3 m_playerScale = { 1.5f,1.5f,1.5f };//キャラのスケール
	Vector3 m_playerInitialPosition; //初期位置
	Quaternion m_playerRotation;  //クォータニオン。
	SoundSource* m_jumpSE;

	float m_gravityFrameTime = 0.0f; //重力を加算するためのフレーム時間。

	enum EnAnimationClip {		//アニメーション。
		enAnimationClip_Idle,
		enAnimationClip_Walk,
		enAnimationClip_Run,
		enAnimationClip_Jump,
		enAnimationClip_Num,
	};
	AnimationClip m_playerAnimationClips[enAnimationClip_Num];		//アニメーションクリップ。

	enum playerState //プレイヤーのステート(状態)を表す変数。
	{
		enPlayerState_Idle, //待機
		enPlayerState_Walk, //歩き
		enPlayerState_Run, //走り
		enPlayerState_Jump, //ジャンプ中

	};
	int m_playerState = enPlayerState_Idle; //プレイヤーの状態を表す変数。

	int m_clearCount = 0;
	int m_playerHP = 3;//プレイヤーのHP

	bool m_isPlayerDead = false;

private:
	Game* p_game = nullptr;
};

