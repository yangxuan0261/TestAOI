#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class CAgent;
class CAoi;
class CQuadTree;

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

	virtual ~HelloWorld();

    virtual bool init();
	virtual void update(float delta);
    void menuCloseCallback(cocos2d::Ref* pSender);
	void InitDisplay();
    CREATE_FUNC(HelloWorld);

	void AddAgents(int _num);
	void DelAgents(int _num);
	void SelectAgent(CAgent* _agent);
	void RefreshAoiInfo(CAgent* _agent);
	void RefreshBigMapInfo();
	void MoveAgent(CAgent* _agent);
	void ChangeRange(int _range);
	void DrawRange();
	void DrawAoiTree();
	void DrawTree(CQuadTree* _tree);

private:
	cocos2d::ui::Layout*	_layout;

	cocos2d::ui::ImageView*	mBigMap;
	cocos2d::ui::ImageView*	mAoiMap;

	cocos2d::ui::TextField*	mBigEntitiesNum;
	cocos2d::ui::TextField*	mAoiId;
	cocos2d::ui::TextField*	mAoiRange;
	cocos2d::ui::TextField*	mAoiEntitiesNum;

	cocos2d::ui::TextField*	mTFSelId;
	cocos2d::ui::TextField*	mTFAdd;
	cocos2d::ui::TextField*	mTFDel;

	std::vector<CAgent*>	mAgentBigVec;
	std::vector<CAgent*>	mAgentAoiVec;

	CAgent*	mSelAgent;
	cocos2d::DrawNode*		mDrawRange;
	cocos2d::DrawNode*		mDrawTree;

	CAoi*		mAoi;
};

#endif // __HELLOWORLD_SCENE_H__
