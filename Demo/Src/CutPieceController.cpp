#include "Itsukushima_include.h"
#include "CutPieceController.h"


CutPieceController::CutPieceController(void)
{
	m_fDestroyDelay = 1.0f;
}


CutPieceController::~CutPieceController(void)
{
}

const char* 
CutPieceController::ClassName()
{
	return "CutPieceController";
}

const char* 
CutPieceController::GetName()
{
	return "CutPieceController";
}

void 
CutPieceController::LogicUpdate()
{
	if(m_pGameObject->IsDead())
		return;

	if(m_fDestroyDelay > 0)
	{
		m_fDestroyDelay -= Timer::Instance()->GetDeltaTime();
		if(m_fDestroyDelay < 0)
		{
			m_pGameObject->GetModelComponent()->SetRenderPass(RenderPass::GBUFFER, false);
			m_pGameObject->GetModelComponent()->SetRenderPass(RenderPass::TRANSPARENT, true);
			m_pGameObject->GetColliderComponent()->Enable(false);
			m_pGameObject->GetRigidBodyComponent()->Enable(false);
		}
	}
	else if(m_fDestroyDelay < 0)
	{
		m_fDestroyDelay -= Timer::Instance()->GetDeltaTime();
		if(m_fDestroyDelay > -1.0f)
		{
			m_pGameObject->GetModelComponent()->SetTransparency((1.0f + m_fDestroyDelay) * 0.5f);
		}
		else
		{
			m_pGameObject->Destroy();
		}
	}
}

void
CutPieceController::SetDestroyDelay(float32 fDelay)
{
	m_fDestroyDelay = fDelay;
}

float32 
CutPieceController::GetDestroyCountDown()
{
	return m_fDestroyDelay;
}