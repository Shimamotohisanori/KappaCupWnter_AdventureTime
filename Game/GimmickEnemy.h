#pragma once
class Player;
class Game;
class GimmickEnemy : public IGameObject
{
public:
	GimmickEnemy();
	~GimmickEnemy();
	bool Start();
	void Update();
	void GimmickEnemyMove();
	void GimmickEnemyRotation();
	void GimmickEnemyManageState();
	void PlayGimmickEnemyAnimation();
	void GimmickEnemyAttack();
	void StepOnGimmickEnemy();
	void GimmickEnemyGhostObjectManager();
	void Respawn();
	void Render(RenderContext& rc);

	float AddGameFrame(float time)
	{
		return time += 20 * g_gameTime->GetFrameDeltaTime();
	}

	Vector3 m_gimmickEnemyPos = Vector3::Zero;
	Vector3 m_gimmickEnemyInitalPos = Vector3::Zero;

private:
	ModelRender p_gimmickEnemyModelRender;//敵のモデルレンダラー
	ModelRender p_gimmickEnemyShellModelRender;//敵の殻だけのモデルレンダラー
	Vector3 p_gimmickEnemyScale = { 7.0f,7.0f,7.0f };//敵の大きさ
	Vector3 p_gimmickEnemyShellScale = { 2.0f,2.0f,2.0f };//敵の空の大きさ
	Vector3 p_gimmickEnemyMoveSpeed = Vector3::Zero;//敵の移動速度
	Quaternion p_gimmickEnemyRotation;//敵の回転
	CharacterController p_gimmickEnemyCharacterController;//敵のキャラコン
	PhysicsGhostObject p_gimmickEnemyBodyGhostObject;//敵の体のゴーストオブジェクト
	PhysicsGhostObject p_gimmickEnemyHeadGhostObject;//敵の頭のゴーストオブジェクト
	PhysicsGhostObject p_gimmickEnemyAttackGhostObject;//敵の攻撃用のゴーストオブジェクト
	SoundSource* p_playerHitDamageSE;//プレイヤーが敵から攻撃を食らったときに流れる音
	SoundSource* p_hitAndDieSE;//プレイヤーの攻撃がヒットしたもしくは当たって死んだときの音

	Player* p_player;
	Game* p_game;

	bool p_isDead = false;//敵が死んでいるかのフラグ
	bool p_isMove = false;//敵が動いてるかのフラグ
	bool p_isAttack = false;//敵が攻撃するかのフラグ
	bool p_isIdle = false;//敵が待機するフラグ
	bool p_isDamage = false;//敵が攻撃を受けたかどうかのフラグ
	bool p_isPlayAttackAnimation = false;//攻撃アニメーションが再生されてるかどうかのフラグ
	bool p_hasAlreadyDamaged = false;//攻撃アニメーション中にすでに一度ダメージを与えたかどうかのフラグ

	enum GimmickEnemyAnimationClip
	{
		enGimmickEnemyAnimationClip_Idle,
		enGimmickEnemyAnimationClip_Walk,
		enGimmickEnemyAnimationClip_FirstHit,
		enGimmickEnemyAnimationClip_Die,
		enGimmickEnemyAnimationClip_Attack,
		enGimmickEnemyAnimationClip_Num,
	};
	AnimationClip p_gimmickEnemyAnimationClips[enGimmickEnemyAnimationClip_Num];

	enum GimmickEnemyState
	{
		enGimmickEnemyState_Idle,//待機
		enGimmickEnemyState_Walk,//歩き
		enGimmickEnemyState_FirstHit,//プレイヤーから一回攻撃を受けた
		enGimmickEnemyState_Die,//死亡
		enGimmickEnemyState_Attack,//攻撃
	};
	int p_gimmickEnemyState = enGimmickEnemyState_Idle;

	int p_damageCount = 0;

	//敵とプレイヤーの距離
	float p_distance = 0.0f;
	//敵が死んだときのアニメーション時間
	float p_deadAnimationTime = 0.0f;
	//敵が攻撃したときのアニメーション時間
	float p_attackAnimationTime = 0.0f;
	//重力を加算するためのフレーム時間。
	float p_gravityFrameTime = 0.0f; 
};

