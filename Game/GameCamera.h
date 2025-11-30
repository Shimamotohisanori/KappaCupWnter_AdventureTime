#pragma once
class Game;
class Player;
//ゲーム中のカメラを制御する。
class GameCamera : public IGameObject
{
public:
	GameCamera();
	~GameCamera();
	bool Start() override;
	void Update()override;
	void CameraPosManager();
	/////////////////////////////////////
	//メンバ変数
	/////////////////////////////////////
	Player* m_player;	//プレイヤー。
	Vector3 m_toCameraPos;	//注視点から視点に向かうベクトル。
	Quaternion m_cameraRotation; //カメラの回転
private:
	Game* p_game;
};