/*
* Used to create new object after the object, which is created previously, is dead 
*
* @author: Kai Yang
*/

#ifndef OBJECT_GENERATOR_H
#define OBJECT_GENERATOR_H

class ObjectGenerator : public GameObjectComponent
{
public:
	ObjectGenerator(void);
	virtual ~ObjectGenerator(void);

	static const char* ClassName();
	virtual const char* GetName();

	virtual void PostLogicUpdate();

	void SetObjectName(const char* pszName);
	void SetModelName(const char* pszName);
	void SetMaterialName(const char* pszName);
	void SetColliderName(const char* pszName);

	void SetInertiaType(InertiaType eType);

	void SetRotation(Vector3 vRotation);
	void SeScale(Vector3 vScale);

	void SetDelay(float32 fDelay);

private:
	const char* m_pszObjectName;
	const char* m_pszModelName;
	const char* m_pszMaterialName;
	const char* m_pszColliderName;
	InertiaType m_eInertiaType;
	Vector3		m_vRotation;
	Vector3		m_vScale;

	GameObject* m_pGeneratedObject;

	float32 m_fDelay;
	float32 m_fCD;
};

#endif