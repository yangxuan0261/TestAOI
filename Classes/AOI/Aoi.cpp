#include "Aoi.h"
#include "QuadTree.h"
#include <utility>

CAoi::CAoi()
{
	mTree = nullptr;
}

CAoi::~CAoi()
{
	delete mTree;
}

void CAoi::Init(int _left, int _top, int _right, int _bottom, int _radius)
{
	mTree = new CQuadTree(_left, _top, _right, _bottom, nullptr);
	mRadius = _radius;
}

void CAoi::Insert(int _id, int _x, int _y, std::map<int, int>& _notiList)
{
	if (mObjVec.find(_id) != mObjVec.end()) //已存在，直接返回
		return;

	CQuadTree* tree = mTree->Insert(_id, _x, _y); //返回自己所在的树
	if (tree == nullptr)
		return;

	std::vector<int> result;
	mTree->Query(_id, _x - mRadius, _y + mRadius, _x + mRadius, _y - mRadius, result);

	//相同range情况下，自己能看到别人，别人自然能看到自己，把自己添加到别人的可视列表中
	for (int id : result) 
	{
		auto iter = mObjVec.find(id);
		if (iter != mObjVec.end())
		{
			_notiList.insert(std::make_pair(id, id)); //返回自己的可视列表，用来刷新别人
			iter->second->mList.insert(std::make_pair(_id, _id));
		}
	}

	SAoiObj* aoiObj = new SAoiObj();
	aoiObj->mId = _id;
	aoiObj->mX = _x;
	aoiObj->mY = _y;
	aoiObj->mTree = tree; //自己所在的树，为了删除是能更快删除
	aoiObj->mList = _notiList; //自己可视的列表
	mObjVec.insert(std::make_pair(_id, aoiObj));
}

void CAoi::Remove(int _id, std::map<int, int>& _notiList)
{
	auto iter = mObjVec.find(_id);
	if (iter == mObjVec.end())
		return;

	SAoiObj* aoiObj = iter->second;
	if (aoiObj->mTree)
		aoiObj->mTree->Remove(_id);

	//遍历自己可视列表，移除别人可视列表中的自己
	for (auto iter = aoiObj->mList.begin(); iter != aoiObj->mList.end(); ++iter)
	{
		int id = iter->second;
		auto it = mObjVec.find(id);
		if (it != mObjVec.end())
		{
			std::map<int, int>& list = it->second->mList;
			auto listIt = list.find(_id);
			if (listIt != list.end())
				list.erase(listIt);
		}
	}

	_notiList = aoiObj->mList; //返回自己的可视列表，用来刷新别人
	delete aoiObj;
	mObjVec.erase(_id);
}

void CAoi::Update(int _id, int _x, int _y, std::map<int, int>& _aList, std::map<int, int>& _uList, std::map<int, int>& _rList)
{
	auto iter = mObjVec.find(_id); //TODO: Update待优化
	if (iter == mObjVec.end())
		return;

	SAoiObj* aoiObj = iter->second;
	if (aoiObj->mTree)
		aoiObj->mTree->Remove(_id);
	else
		mTree->Remove(_id);

	_rList = aoiObj->mList;
	CQuadTree* tree = mTree->Insert(_id, _x, _y);
	if (tree == nullptr)
		return;

	aoiObj->mX = _x;
	aoiObj->mY = _y;
	aoiObj->mTree = tree; //更新自己所在的树

	std::vector<int> result;
	mTree->Query(_id, _x - mRadius, _y + mRadius, _x + mRadius, _y - mRadius, result);

	for (int id : result)
	{
		_aList.insert(std::make_pair(id, id));

		auto iter = _rList.find(id);
		if (iter != _rList.end())
		{
			_uList.insert(std::make_pair(id, id));
			_rList.erase(iter);
		}
	}

	for (auto iter = _uList.begin(); iter != _uList.end(); ++iter)
	{
		auto it = _aList.find(iter->second);
		if (it != _aList.end())
			_aList.erase(it);
	}

	aoiObj->mList.clear();
	for (auto iter = _aList.begin(); iter != _aList.end(); ++iter)
		aoiObj->mList.insert(std::make_pair(iter->second, iter->second));

	for (auto iter = _uList.begin(); iter != _uList.end(); ++iter)
		aoiObj->mList.insert(std::make_pair(iter->second, iter->second));

	int a = 1;
}

SAoiObj* CAoi::GetAoiObj(int _id)
{
	auto iter = mObjVec.find(_id);
	return iter != mObjVec.end() ? iter->second : nullptr;
}

int CAoi::GetDepth()
{
	int depth = 0;
	CaculDepth(depth, mTree);
	return depth;
}

void CAoi::CaculDepth(int& _depth, CQuadTree* _tree)
{
	if (_tree == nullptr)
		return;

	_depth = _tree->mDepth > _depth ? _tree->mDepth : _depth;
	for (CQuadTree* tree : _tree->GetChildren())
		CaculDepth(_depth, tree);
}

