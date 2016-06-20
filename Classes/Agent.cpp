#include "Agent.h"

USING_NS_CC;

CAgent::CAgent()
{
	mRange = 10;
	mSelCallback = nullptr;
	mMoveCallback = nullptr;
}

CAgent::~CAgent()
{

}

CAgent * CAgent::Create(const std::string & filename, int _id)
{
	CAgent *sprite = new (std::nothrow) CAgent();
	if (sprite && sprite->initWithFile(filename))
	{
		sprite->mId = _id;
		sprite->autorelease();
		sprite->MyInit();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

void CAgent::MyInit()
{
	Size size = getContentSize();
	mLblId = LabelTTF::create();
	mLblId->setString(String::createWithFormat("%d", mId)->getCString());
	mLblId->setPosition(Vec2(size.width * 0.5, size.height * 0.5));
	this->addChild(mLblId);


	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = [&](Touch *touch, Event *unusedEvent)->bool {

		Vec2 p = touch->getLocation();
		Rect rect = this->getBoundingBox();

		if (rect.containsPoint(p))
		{
			CCLOG("---- agent onTouchBegan, id:%d", mId);
			if (mSelCallback)
				mSelCallback(this);
			return true; 
		}

		return false; 
	};

	listener->onTouchMoved = [&](Touch *touch, Event *unusedEvent)->void {

		Vec2 p = touch->getLocation();
		if (mClampBox.containsPoint(p))
		{
			CCLOG("---- agent onTouchMoved, id:%d", mId);
			this->setPosition(touch->getLocation());
			if (mMoveCallback)
				mMoveCallback(this);
		}
	};

	listener->onTouchEnded = [&](Touch *touch, Event *unusedEvent)->void {
		
	};

	listener->onTouchCancelled = [&](Touch *touch, Event *unusedEvent)->void {
		
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	//_eventDispatcher->addEventListenerWithFixedPriority(listener, 30);

}

void CAgent::SetCallback(AgentCallback _sel, AgentCallback _move)
{
	mSelCallback = _sel;
	mMoveCallback = _move;
}
