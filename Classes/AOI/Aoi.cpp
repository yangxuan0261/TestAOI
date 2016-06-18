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
	if (mObjVec.find(_id) != mObjVec.end()) //�Ѵ��ڣ�ֱ�ӷ���
		return;

	CQuadTree* tree = mTree->Insert(_id, _x, _y); //�����Լ����ڵ���
	if (tree == nullptr)
		return;

	std::vector<int> result;
	mTree->Query(_id, _x - mRadius, _y + mRadius, _x + mRadius, _y - mRadius, result);

	//��ͬrange����£��Լ��ܿ������ˣ�������Ȼ�ܿ����Լ������Լ���ӵ����˵Ŀ����б���
	for (int id : result) 
	{
		auto iter = mObjVec.find(id);
		if (iter != mObjVec.end())
		{
			_notiList.insert(std::make_pair(id, id)); //�����Լ��Ŀ����б�����ˢ�±���
			iter->second->mList.insert(std::make_pair(_id, _id));
		}
	}

	SAoiObj* aoiObj = new SAoiObj();
	aoiObj->mId = _id;
	aoiObj->mX = _x;
	aoiObj->mY = _y;
	aoiObj->mTree = tree; //�Լ����ڵ�����Ϊ��ɾ�����ܸ���ɾ��
	aoiObj->mList = _notiList; //�Լ����ӵ��б�
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

	_notiList = aoiObj->mList; //�����Լ��Ŀ����б�����ˢ�±���
	delete aoiObj;
	mObjVec.erase(_id);
}

void CAoi::Update(int _id, int _x, int _y, std::map<int, int>& _aList, std::map<int, int>& _uList, std::map<int, int>& _rList)
{
	auto iter = mObjVec.find(_id); //TODO: Update���Ż�
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
	aoiObj->mTree = tree; //�����Լ����ڵ���

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

