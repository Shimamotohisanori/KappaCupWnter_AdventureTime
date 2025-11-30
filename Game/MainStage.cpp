#include "stdafx.h"
#include "MainStage.h"

MainStage::MainStage()
{
}

MainStage::~MainStage()
{
}

bool MainStage::Start()
{
	//m_mainStageRender.Init("Assets/modelData/mainStage.tkm");
	m_mainStageRender.Init("Assets/modelData/stage2.tkm");
	m_mainStagePhysicsStaticObject.CreateFromModel(m_mainStageRender.GetModel(),
		m_mainStageRender.GetModel().GetWorldMatrix());

	return true;
}

void MainStage::Update()
{

}

void MainStage::Render(RenderContext& rc)
{
	m_mainStageRender.Draw(rc);
}