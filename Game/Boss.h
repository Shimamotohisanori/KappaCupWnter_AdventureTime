#pragma once
class Player;
class Game;
class Boss : public IGameObject
{
public:
	Boss();
	~Boss();
	bool Start();
	void Update();
	void BossMove();
	void BossRotation();
	void BossManageState();
	void Respawn();
	void StepOnBoss();
	void Attack();
	void ChangeBossAnimation();
	void Render(RenderContext& rc);

	float AddGameFrame(float time)
	{
		return time + 20 * g_gameTime->GetFrameDeltaTime();
	}

	Vector3 m_bossPos = Vector3::Zero;//ボスの現在の位置
	Vector3 m_bossInitalPos = Vector3::Zero;//ボスの初期位置

private:
	/// <summary>
	/// アニメーションイベント用の関数。
	/// </summary>
	/// <param name="clipName">アニメーションの名前。</param>
	/// <param name="eventName">アニメーションイベントのキーの名前。</param>
	void OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName);
	Player* p_player = nullptr;
	Game* p_game = nullptr;

	ModelRender p_bossModelRender;//モデルレンダラー
	Quaternion p_bossRotation;//ボスの回転
	Vector3 p_bossScale = {5.5f,5.5f,5.5f};//ボスのスケール
	Vector3 p_bossMoveSpeed = Vector3::Zero;//ボスの移動速度
	CharacterController p_bossCharacterController;//ボスのキャラコン
	PhysicsGhostObject p_bossBodyGhostObject;//敵の体のゴーストオブジェクト
	PhysicsGhostObject p_bossHeadGhostObject;//敵の頭のゴーストオブジェクト
	PhysicsGhostObject p_bossAttackGhostObject;//敵の攻撃用のゴーストオブジェクト
	SoundSource* p_playerHitDamageSE;//プレイヤーが敵から攻撃を食らったときに流れる音
	SoundSource* p_bossShakeUpSE;//ボスが振りかぶった時の音
	SoundSource* p_bossHitAndDieSE;//ボスが踏まれるもしくは死ぬときの音

	enum EnBossAnimationClip {		//アニメーション。
		enBossAnimationClip_Idle,
		enBossAnimationClip_Walk,
		enBossAnimationClip_Attack,
		enBossAnimationClip_Die,
		enBossAnimationClip_Hit,
		enBossAnimationClip_Num,
	};
	AnimationClip p_bossAnimationClips[enBossAnimationClip_Num];		//アニメーションクリップ。

	enum bossState //ボスのステート(状態)を表す変数。
	{
		enBossState_Idle, //待機
		enBossState_Walk, //歩き
		enBossState_Attack,//攻撃
		enBossState_Die,//死ぬ
		enBossState_Hit,//当たる
	};
	int p_bossState = enBossState_Idle; //ボスの状態を表す変数。

	int p_damageCount = 0;

	//敵が動いてるかのフラグ
	bool p_isMove = false;
	//敵が死んでいるかのフラグ
	bool p_isDead = false;
	//敵が攻撃するフラグ
	bool p_isAttack = false;
	//敵が待機するかどうかのフラグ
	bool p_isIdle = false;
	//敵が攻撃を受けたかどうかのフラグ
	bool p_isDamage = false;
	//敵に攻撃がヒットしたかどうかのフラグ
	bool p_isHitAnimation = false;
	//攻撃アニメーションが再生されてるかどうかのフラグ
	bool p_isPlayAttackAnimation = false;
	//攻撃アニメーション中にすでに一度ダメージを与えたかどうかのフラグ
	bool p_hasAlreadyDamaged = false;

	//重力を加算するためのフレーム時間。
	float p_gravityFrameTime = 0.0f;
	//敵が死んだときのアニメーション時間
	float p_deadAnimationTime = 0.0f;
	//敵が攻撃したときのアニメーション時間
	float p_attackAnimationTime = 0.0f;
	//被ダメアニメーションから待機モーションに戻すための変数
	float p_changeAnimationTime = 0.0f;
};

