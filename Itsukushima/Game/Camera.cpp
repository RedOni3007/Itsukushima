#include <Game/Camera.h>

Camera::Camera(void)
{
	m_vUP = Vector3(0.0f,1.0f,0.0f);

	m_fFOV = 60.0f;
	m_fRatio = 16.0f/9.0f;
	m_fNearClip = 0.1f;
	m_fFarClip = 1000.0f;
	m_mProjectionMatrix = glm::perspective( m_fFOV, m_fRatio, m_fNearClip, m_fFarClip);
}


Camera::~Camera(void)
{
}

void Camera::RefreshViewMatrix()
{
	if(m_pGameObject != nullptr)
	{
		Vector3 vPos = m_pGameObject->GetWorldPos();
		Vector3 vForward = MFD_Normalize(-m_pGameObject->GetWorldForward());
		
		//camera looking -z, object default facing z, so opposite direction
		m_mViewMatrix = glm::lookAt( vPos, vPos + vForward, m_vUP);
	}
}

void 
Camera::SetFOV(float32 fFOV)
{
	m_fFOV = fFOV;
	m_mProjectionMatrix = glm::perspective( m_fFOV, m_fRatio, m_fNearClip, m_fFarClip);
}

void
Camera::SetRatio(float32 fRatio)
{
	m_fRatio = fRatio;
	m_mProjectionMatrix = glm::perspective( m_fFOV, m_fRatio, m_fNearClip, m_fFarClip);
}

void
Camera::SetNearClip(float32 fNearClip)
{
	m_fNearClip = fNearClip;
	m_mProjectionMatrix = glm::perspective( m_fFOV, m_fRatio, m_fNearClip, m_fFarClip);
}

void
Camera::SetFarClip(float32 fFarClip)
{
	m_fFarClip = fFarClip;
	m_mProjectionMatrix = glm::perspective( m_fFOV, m_fRatio, m_fNearClip, m_fFarClip);
}

Vector3 
Camera::GetUp()
{
	return m_vUP;
}

float32 
Camera::GetNearClip()
{
	return m_fNearClip;
}

float32 
Camera::GetFarClip()
{
	return m_fFarClip;
}

const Matrix44*
Camera::GetViewMat()
{	
	return &m_mViewMatrix;
}


const Matrix44*
Camera::GetProjectMat()
{
	return &m_mProjectionMatrix;
}

void 
Camera::PreLogicUpdate()
{

}

void 
Camera::LogicUpdate()
{
	RefreshViewMatrix();
}

void 
Camera::PostLogicUpdate()
{
	RefreshViewMatrix();
}

Vector3 
Camera::GetPos()
{
	return m_pGameObject->GetWorldPos();
}

Vector3 
Camera::GetDir()
{
	return -m_pGameObject->GetForward();
}

const char* 
Camera::GetName()
{
	return "Camera";
}

const char* 
Camera::ClassName()
{
	return "Camera";
}

void 
Camera::CalculateScreenToWorldRay(float32 fSX, float32 fSY, uint32 uWidth, uint32 uHeight,Vector3& vPos, Vector3 &vDir)
{
	float32 x = (2.0f * fSX) / uWidth - 1.0f;
	float32 y = 1.0f - (2.0f * fSY) / uHeight;
	float32 z = 1.0f;
	Vector3 ray_screen = Vector3 (x, y, z);
	Vector4 ray_clip = Vector4 (ray_screen.x, ray_screen.y, -1.0f, 1.0f);

	Vector4 ray_camera = MFD_Inverse(m_mProjectionMatrix) * ray_clip;
	ray_camera = Vector4 (ray_camera.x,ray_camera.y, -1.0f, 0.0f);

	Vector3 ray_world = Vector3(MFD_Inverse(m_mViewMatrix) * ray_camera);
	// don't forget to normalise the vector at some point
	ray_world = MFD_Normalize(ray_world);

	vPos = m_pGameObject->GetWorldPos();
	vDir = ray_world;
}