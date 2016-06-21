#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class CAgent;
class CAoi;
class CQuadTree;
class COrthList;

enum class ENotifyType : int
{
	None = 0,
	Add,
	Update,
	Remove,
	Reset,
};

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
	void RefreshAoiInfo();
	void RefreshBigMapInfo();
	void MoveAgent(CAgent* _agent);
	void ChangeRange(int _range);
	void SimulateBorn();

	//-------- draw line and point
	void DrawRange();
	void DrawAoiTree();
	void DrawTree(CQuadTree* _tree);
	void DrawNotifyTarget(ENotifyType _type, std::map<int, int>& _notiList, CAgent* _target = nullptr);

	//--- auto move
	void AutoMove(bool _b);
	void Move(CAgent* _agent, bool _move);

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

	std::map<int, CAgent*>	mAgentBigVec;
	std::map<int, CAgent*>	mAgentAoiVec;

	bool	mShowTree;

	CAgent*	mSelAgent;
	cocos2d::DrawNode*		mDrawRange;
	cocos2d::DrawNode*		mDrawTree;

	CAoi*		mAoi;

	//COrthList*	mOrthList;
};

#endif // __HELLOWORLD_SCENE_H__
