#pragma once
class Player;
class Game;
class Enemy : public IGameObject
{
public:
	Enemy();
	~Enemy();
	bool Start();
	void Update();
	void EnemyMove();
	void EnemyRotation();
	void EnemyManageState();
	void PlayEnemyAnimation();
	void StepOnEnemy();
	void Attack();
	void Respawn();
	void Render(RenderContext& rc);

	float AddGameFrame(float time)
	{
		return time + 20 * g_gameTime->GetFrameDeltaTime();
	}

	Vector3 m_enemyPos = Vector3::Zero; //敵の位置
	Vector3 m_enemyInitalPos;//敵の初期位置
private:
	Player* p_player;
	Game* p_game;

	ModelRender p_enemyModelRender;//敵のモデルレンダラー
	Vector3 p_enemyScale = { 7.0f,7.0f,7.0f };//敵の大きさ
	Vector3 p_enemyMoveSpeed = Vector3::Zero;//敵の移動速度
	Quaternion p_enemyRotation;//敵の回転
	CharacterController p_enemyCharacterController;//敵のキャラクターコントローラー
	PhysicsGhostObject p_enemyBodyGhostObject;//敵の体のゴーストオブジェクト
	PhysicsGhostObject p_enemyHeadGhostObject;//敵の頭のゴーストオブジェクト
	PhysicsGhostObject p_enemyAttackGhostObject;//敵の攻撃用のゴーストオブジェクト
	SoundSource* p_dieSE;//敵が死んだときの音
	SoundSource* p_playerHitDamageSE;//プレイヤーにダメージを与えた時の音

	enum EnemyAnimationClip
	{
		enEnemyAnimationClip_Idle,
		enEnemyAnimationClip_Walk,
		enEnemyAnimationClip_Die,
		enEnemyAnimationClip_Attack,
		enEnemyAnimationClip_Num,
	};
	AnimationClip p_enemyAnimationClips[enEnemyAnimationClip_Num];

	enum EnemyState
	{
		enEnemyState_Idle,//待機
		enEnemyState_Walk,//歩き
		enEnemyState_Die,//死亡
		enEnemyState_Attack,//攻撃
	};
	int p_enemyState = enEnemyState_Idle;

	//敵が動いてるかのフラグ
	bool p_isMove = false;
	//敵が死んでいるかのフラグ
	bool p_isDead = false;
	//敵が攻撃するフラグ
	bool p_isAttack = false;
	//敵が待機するかどうかのフラグ
	bool p_isIdle = false;
	//攻撃アニメーションが再生されてるかどうかのフラグ
	bool p_isPlayAttackAnimation = false;
	//攻撃アニメーション中にすでに一度ダメージを与えたかどうかのフラグ
	bool p_hasAlreadyDamaged = false;


	//敵とプレイヤーの距離
	float p_distance = 0.0f;
	//敵が死んだときのアニメーション時間
	float p_deadAnimationTime = 0.0f;
	//敵が攻撃したときのアニメーション時間
	float p_attackAnimationTime = 0.0f;
	//重力を加算するためのフレーム時間。
	float p_gravityFrameTime = 0.0f;
};