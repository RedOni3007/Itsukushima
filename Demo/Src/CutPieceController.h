/*
* Used to timed-destroy the object which is created by cutting
*
* @author: Kai Yang
*/

#ifndef CUT_PIECE_CONTROLLER_H
#define CUT_PIECE_CONTROLLER_H

class CutPieceController : public GameObjectComponent
{
public:
	CutPieceController(void);
	virtual ~CutPieceController(void);

	static const char* ClassName();
	virtual const char* GetName();

	virtual void LogicUpdate();

	void SetDestroyDelay(float32 fDelay);
	float32 GetDestroyCountDown();

private:
	float32 m_fDestroyDelay;
};

#endif