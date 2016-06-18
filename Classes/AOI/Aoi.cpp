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

std::map<int, int> CAoi::Insert(int _id, int _x, int _y)
{
	std::map<int, int> ret;
	if (mObjVec.find(_id) != mObjVec.end()) //�Ѵ��ڣ�ֱ�ӷ���
		return ret;

	CQuadTree* tree = mTree->Insert(_id, _x, _y); //�����Լ����ڵ���
	if (tree == nullptr)
		return ret;

	std::vector<int> result;
	mTree->Query(_id, _x - mRadius, _y + mRadius, _x + mRadius, _y - mRadius, result);

	//��ͬrange����£��Լ��ܿ������ˣ�������Ȼ�ܿ����Լ������Լ���ӵ����˵Ŀ����б���
	for (int id : result) 
	{
		auto iter = mObjVec.find(id);
		if (iter != mObjVec.end())
		{
			ret.insert(std::make_pair(id, id));
			iter->second->mList.insert(std::make_pair(_id, _id));
		}
	}

	SAoiObj* aoiObj = new SAoiObj();
	aoiObj->mId = _id;
	aoiObj->mX = _x;
	aoiObj->mY = _y;
	aoiObj->mTree = tree; //�Լ����ڵ���
	aoiObj->mList = ret; //�Լ����ӵ��б�
	mObjVec.insert(std::make_pair(_id, aoiObj));

	return ret; //�����Լ��Ŀ����б�����ˢ�±���
}

std::map<int, int> CAoi::Remove(int _id)
{
	std::map<int, int> ret; 
	auto iter = mObjVec.find(_id);
	if (iter == mObjVec.end())
		return ret;

	SAoiObj* aoiObj = iter->second;
	if (aoiObj->mTree)
		aoiObj->mTree->Remove(_id);
	else
		mTree->Remove(_id);

	//�����Լ������б��Ƴ����˿����б��е��Լ�
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

	ret = aoiObj->mList;
	delete aoiObj;
	mObjVec.erase(_id);
	return ret; //�����Լ��Ŀ����б�����ˢ�±���
}

void CAoi::Update(int _id, int _x, int _y, std::map<int, int>& _nList, std::map<int, int>& _uList, std::map<int, int>& _oList)
{
	auto iter = mObjVec.find(_id);
	if (iter == mObjVec.end())
		return;

	SAoiObj* aoiObj = iter->second;
	if (aoiObj->mTree)
		aoiObj->mTree->Remove(_id);
	else
		mTree->Remove(_id);

	_oList = aoiObj->mList;
	CQuadTree* tree = mTree->Insert(_id, _x, _y);
	if (tree == nullptr)
		return;

	aoiObj->mX = _x;
	aoiObj->mY = _y;

	std::vector<int> result;
	mTree->Query(_id, _x - mRadius, _y - mRadius, _x + mRadius, _y + mRadius, result);

	for (int id : result)
	{
		_nList.insert(std::make_pair(id, id));

		auto iter = _oList.find(id);
		if (iter != _oList.end())
		{
			_uList.insert(std::make_pair(id, id));
			_oList.erase(iter);
		}
	}

	for (auto iter = _uList.begin(); iter != _uList.end(); ++iter)
	{
		auto it = _nList.find(iter->second);
		if (it != _nList.end())
			_nList.erase(it);
	}

	aoiObj->mList.clear();
	for (auto iter = _nList.begin(); iter != _nList.end(); ++iter)
		aoiObj->mList.insert(std::make_pair(iter->second, iter->second));

	for (auto iter = _uList.begin(); iter != _uList.end(); ++iter)
		aoiObj->mList.insert(std::make_pair(iter->second, iter->second));
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

