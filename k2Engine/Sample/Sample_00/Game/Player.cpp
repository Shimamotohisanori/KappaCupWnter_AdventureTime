#include "stdafx.h"
#include "Player.h"

Player::Player()
{
	
}

Player::~Player()
{

}

bool Player::Start()
{
	//モデルを初期化する
	m_modelRender.Init("Assets/modelData/unityChan.tkm");
	return true;
}

void Player::Update()
{
	//モデルを更新する
	m_modelRender.Update();
}

void Player::Render(RenderContext& renderContext)
{
	//モデルを表示する
	m_modelRender.Draw(renderContext);
}