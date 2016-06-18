#include "HelloWorldScene.h"
#include "ui/CocosGUI.h"
#include "ui/UILayout.h"
#include "Agent.h"
#include "AOI/Aoi.h"
#include "AOI/QuadTree.h"

#include <time.h>
#include <random>

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

static const int GAP = 10;
static const std::string gSprFile = "slider_bar_button.png";
static int gId = 1;
static int IdGenerator() { return gId++; }
static int gBigMapWidth = 600;

//--- test
vector<Vec2> testPoint;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

	InitDisplay();

	this->scheduleUpdate();
    return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::InitDisplay()
{
	mAoi = new CAoi();
	mSelAgent = nullptr;
	testPoint.push_back(Vec2(200, 200));
	testPoint.push_back(Vec2(250, 250));

	//--- draw node init
	mDrawRange = DrawNode::create();
	mDrawRange->setVisible(false);
	this->addChild(mDrawRange, 100);

	mDrawTree = DrawNode::create();
	this->addChild(mDrawTree, 100);

	mAoi->Init(GAP, GAP + 600, GAP + 600, GAP, 100);
	_layout = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("ui/Display.json"));
	this->addChild(_layout);

	Size screenSize = Director::getInstance()->getWinSize();
	Size rootSize = _layout->getContentSize();
	this->setPosition(Vec2((screenSize.width - rootSize.width) / 2,
		(screenSize.height - rootSize.height) / 2));

	if (_layout)
	{

		mBigEntitiesNum = static_cast<TextField*>(Helper::seekWidgetByName(_layout, "TextField_big_entities"));
		mBigEntitiesNum->setTextHorizontalAlignment(TextHAlignment::LEFT);
		mAoiId = static_cast<TextField*>(Helper::seekWidgetByName(_layout, "TextField_aoi_id"));
		mAoiId->setTextHorizontalAlignment(TextHAlignment::LEFT);
		mAoiRange = static_cast<TextField*>(Helper::seekWidgetByName(_layout, "TextField_aoi_range"));
		mAoiRange->setTextHorizontalAlignment(TextHAlignment::LEFT);
		mAoiEntitiesNum = static_cast<TextField*>(Helper::seekWidgetByName(_layout, "TextField_aoi_entities"));
		mAoiEntitiesNum->setTextHorizontalAlignment(TextHAlignment::LEFT);

		mBigMap = static_cast<ImageView*>(Helper::seekWidgetByName(_layout, "BigMap"));
		mAoiMap = static_cast<ImageView*>(Helper::seekWidgetByName(_layout, "AoiMap"));

		//---------------- ctrl panel
		// aoi range
		Slider* rangeSl = static_cast<Slider*>(Helper::seekWidgetByName(_layout, "Slider_aoiRange"));
		rangeSl->addEventListener([&](Ref* sender, ui::Slider::EventType type)->void {
			Slider* self = (Slider*)sender;
			CCLOG("--- rangeSl, per:%d", self->getPercent());

			this->ChangeRange(self->getPercent() * 0.01 * (float)gBigMapWidth * 0.5f);
		});

		// move speed
		Slider* moveSl = static_cast<Slider*>(Helper::seekWidgetByName(_layout, "Slider_moveSpeed"));
		moveSl->addEventListener([&](Ref* sender, ui::Slider::EventType type)->void {
			Slider* self = (Slider*)sender;
			CCLOG("--- moveSl, per:%d", self->getPercent());
		});

		//visible range
		CheckBox* visibleRange = static_cast<CheckBox*>(Helper::seekWidgetByName(_layout, "CheckBox_visbleRange"));
		visibleRange->addEventListener([](Ref* sender, ui::CheckBox::EventType type)->void {
			switch (type)
			{
			case CheckBox::EventType::SELECTED:
				CCLOG("visibleRange SELECTED");
				//_displayValueLabel->setString(StringUtils::format("Selected"));
				break;

			case CheckBox::EventType::UNSELECTED:
				//_displayValueLabel->setString(StringUtils::format("Unselected"));
				CCLOG("visibleRange UNSELECTED");
				break;

			default:
				break;
			}
		});

		//is move
		CheckBox* isMove = static_cast<CheckBox*>(Helper::seekWidgetByName(_layout, "CheckBox_isMove"));
		isMove->addEventListener([](Ref* sender, ui::CheckBox::EventType type)->void {
			switch (type)
			{
			case CheckBox::EventType::SELECTED:
				//_displayValueLabel->setString(StringUtils::format("Selected"));
				CCLOG("isMove SELECTED");
				break;

			case CheckBox::EventType::UNSELECTED:
				//_displayValueLabel->setString(StringUtils::format("Unselected"));
				CCLOG("isMove UNSELECTED");
				break;

			default:
				break;
			}
		});

		// select
		mTFSelId = static_cast<TextField*>(Helper::seekWidgetByName(_layout, "TextField_select"));
		mTFSelId->setTextHorizontalAlignment(TextHAlignment::LEFT);
		mTFSelId->addEventListener([&](Ref* sender, ui::TextField::EventType type)->void {
			TextField* self = (TextField*)sender;
			CCLOG("editSelect %s", self->getString().c_str());
			mBigEntitiesNum->setString(self->getString());

		});
		Button* btnSelect = static_cast<Button*>(Helper::seekWidgetByName(_layout, "Button_select"));
		btnSelect->addClickEventListener([&](Ref* sender)->void {
			CCLOG("btnSelect");
		});

		// add
		mTFAdd = static_cast<TextField*>(Helper::seekWidgetByName(_layout, "TextField_add"));
		mTFAdd->setTextHorizontalAlignment(TextHAlignment::LEFT);
		mTFAdd->addEventListener([&](Ref* sender, ui::TextField::EventType type)->void {
			CCLOG("editAdd");
		});
		Button* btnAdd = static_cast<Button*>(Helper::seekWidgetByName(_layout, "Button_add"));
		btnAdd->addClickEventListener([&](Ref* sender)->void {
			int num = atoi(mTFAdd->getString().c_str());
			CCLOG("--- add num:%d", num);
			this->AddAgents(num);
		});

		// del
		mTFDel = static_cast<TextField*>(Helper::seekWidgetByName(_layout, "TextField_del"));
		mTFDel->setTextHorizontalAlignment(TextHAlignment::LEFT);
		mTFDel->addEventListener([&](Ref* sender, ui::TextField::EventType type)->void {

			CCLOG("editDel");
		});
		Button* btnDelete = static_cast<Button*>(Helper::seekWidgetByName(_layout, "Button_del"));
		btnDelete->addClickEventListener([&](Ref* sender)->void {
			int num = atoi(mTFDel->getString().c_str());
			CCLOG("--- del num:%d", num);
			this->DelAgents(num);
		});
	}
}

//--------------------------------- ctrl agents
void HelloWorld::AddAgents(int _num)
{
	if (_num < 1 || _num > 65535)
		return;

	Size bigMapSize = mBigMap->getContentSize();
	Vec2 bigMapPos = mBigMap->getPosition();

	// 绑定回调
	auto selFunc = std::bind(&HelloWorld::SelectAgent, this, std::placeholders::_1);
	auto moveFunc = std::bind(&HelloWorld::MoveAgent, this, std::placeholders::_1);

	std::default_random_engine generator1(time(NULL)); // mersenne_twister_engine 梅森旋转法
	std::uniform_int_distribution<int> distInt(0, bigMapSize.width);

	CAgent* agent = nullptr;
	for (int i = 0; i < _num; ++i)
	{
		agent = CAgent::Create(gSprFile, ::IdGenerator());
		if (agent)
		{
			mAgentBigVec.push_back(agent);
			_layout->addChild(agent); //添加到根节点，move是根据egl获取坐标，设置的是相对坐标
			agent->setColor(Color3B::BLACK);

			agent->SetCallback(selFunc, moveFunc);
			agent->SetRange(100);
			float x = (float)distInt(generator1); //testPoint[i].x;
			float y = (float)distInt(generator1); //testPoint[i].y;
			agent->setPosition(Vec2(x, y));
			
			//set clamp box
			agent->SetClamp(Rect(bigMapPos.x, bigMapPos.y, bigMapSize.width, bigMapSize.height));

			mAoi->Insert(agent->GetId(), x, y);
		}
	}

	RefreshBigMapInfo();
}

void HelloWorld::DelAgents(int _num)
{
	if (_num < 1 || _num > 65535 || _num > mAgentBigVec.size())
		return;

	CAgent* agent = nullptr;
	for (int i = 0; i < _num; ++i)
	{
		agent = mAgentBigVec[0];
		mAoi->Remove(agent->GetId());
		agent->removeFromParent();

		if (mSelAgent == agent) //删除选中对象
		{
			mSelAgent = nullptr;
			mDrawRange->setVisible(false);
		}

		mAgentBigVec.erase(mAgentBigVec.begin());
	}

	RefreshBigMapInfo();
}

void HelloWorld::update(float delta)
{
	Layer::update(delta);

	if (mSelAgent)
		DrawRange();
	
	DrawAoiTree();
}

//选中变色，绘制aoi范围
void HelloWorld::SelectAgent(CAgent* _agent)
{
	if (mSelAgent == _agent)
		return;

	if (mSelAgent != nullptr)
		mSelAgent->setColor(Color3B::BLACK);

	mSelAgent = _agent;
	mSelAgent->setColor(Color3B::RED);
	mDrawRange->setVisible(true);

	RefreshAoiInfo(_agent);
}

void HelloWorld::MoveAgent(CAgent* _agent)
{
	CCLOG("--- HelloWorld::MoveAgent, moving, id:%d", _agent->GetId());
	RefreshAoiInfo(_agent);
}

void HelloWorld::ChangeRange(int _range)
{
	if (_range < 0 || (float)_range > gBigMapWidth * 0.5)
		return;

	for (CAgent* agent : mAgentBigVec)
		agent->SetRange(_range);

	RefreshAoiInfo(mSelAgent);
}

void HelloWorld::RefreshAoiInfo(CAgent* _agent)
{
	if (_agent == nullptr)
		return;

	SAoiObj* aoiObj = mAoi->GetAoiObj(_agent->GetId());

	String* str = String::createWithFormat("id:%d", aoiObj->mId);
	mAoiId->setString(str->getCString());

	str = String::createWithFormat("range:%d", _agent->GetRange()); //TODO: 范围应该在SAoiObj中
	mAoiRange->setString(str->getCString());

	str = String::createWithFormat("entities:%d", aoiObj->mList.size());
	mAoiEntitiesNum->setString(str->getCString());

	Vec2 pos = _agent->getPosition();
	str = String::createWithFormat("pos:(%03.0f, %03.0f)", aoiObj->mX, aoiObj->mY);
	//mAoiEntitiesNum->setString(str->getCString());
}

void HelloWorld::RefreshBigMapInfo()
{
	String* str = String::createWithFormat("entities:%d, depth:%d", mAgentBigVec.size(), mAoi->GetDepth());
	mBigEntitiesNum->setString(str->getCString());
}

HelloWorld::~HelloWorld()
{
	if (mAoi)
		delete mAoi;
}

void HelloWorld::DrawRange()
{
	CAgent* agent = mSelAgent;
	Vec2 pos = agent->getPosition();
	int range = agent->GetRange();
	Vec2 lt(pos.x - range, pos.y + range);
	Vec2 rb(pos.x + range, pos.y - range);
	mDrawRange->clear();
	mDrawRange->drawRect(lt, rb, Color4F::RED);
}

void HelloWorld::DrawAoiTree()
{
	mDrawTree->clear();
	DrawTree(mAoi->GetTree());
}

void HelloWorld::DrawTree(CQuadTree* _tree)
{
	if (_tree == nullptr)
		return;

	const auto children = _tree->GetChildren();
	if (children.size() > 0) //有四个节点的才绘制十字架，先绘制自己，后绘制节点
	{
		int centerX = (_tree->ml + _tree->mr) >> 1;
		int centerY = (_tree->mt + _tree->mb) >> 1;

		mDrawTree->drawLine(Vec2(_tree->ml, centerY), Vec2(_tree->mr, centerY), Color4F::BLACK); //H
		mDrawTree->drawLine(Vec2(centerX, _tree->mt), Vec2(centerX, _tree->mb), Color4F::BLACK); //v

		for (auto ch : children)
			DrawTree(ch);
	}
}


