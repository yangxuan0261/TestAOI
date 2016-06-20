#ifndef __Agent__H_
#define __Agent__H_

#include "cocos2d.h"

class CAgent;
typedef std::function<void(CAgent*)> AgentCallback;

class CAgent : public cocos2d::Sprite
{
public:
	CAgent();
	virtual ~CAgent();
	static CAgent* Create(const std::string& filename, int _id);
	void	MyInit();
	void	SetClamp(const cocos2d::Rect& _clamp) { mClampBox = _clamp; }
	int		GetRange() const { return mRange; }
	void	SetRange(int _range) { mRange = _range; }
	void	SetCallback(AgentCallback _sel, AgentCallback _move);
	int		GetId() const { return mId; }

private:
	int		mId;
	int		mRange;
	cocos2d::Rect mClampBox;
	AgentCallback	mSelCallback;
	AgentCallback	mMoveCallback;
};

#endif
