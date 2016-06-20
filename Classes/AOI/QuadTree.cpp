#include "QuadTree.h"
#include <algorithm>

const static int gDepthMax = 5;

CQuadTree::CQuadTree(int _l, int _t, int _r, int _b, CQuadTree* _parent)
{
	ml = _l;
	mt = _t;
	mr = _r;
	mb = _b;
	mDepth = 0;
	if (_parent != nullptr)
	{
		mDepth = _parent->mDepth + 1;
	}

	genChildren();
}


CQuadTree::~CQuadTree()
{
	for (CQuadTree* tree : mChildren)
		delete tree;

	for (STreeObj* obj : mObjVec)
		delete obj;
}

//默认生成所有树，不实时生成
void CQuadTree::genChildren()
{
	if (mDepth < gDepthMax)
	{
		int centerX = (ml + mr) >> 1;
		int centerY = (mt + mb) >> 1;
		mChildren.push_back(new CQuadTree(ml, mt, centerX, centerY, this));
		mChildren.push_back(new CQuadTree(centerX, mt, mr, centerY, this));
		mChildren.push_back(new CQuadTree(ml, centerY, centerX, mb, this));
		mChildren.push_back(new CQuadTree(centerX, centerY, mr, mb, this));
	}
}

CQuadTree* CQuadTree::Insert(int _id, int _x, int _y)
{
	if (_x < ml || _x > mr || _y > mt || _y < mb)
		return nullptr;

	if (mChildren.size() > 0)
	{
		CQuadTree* ret = nullptr;
		for (CQuadTree* tree : mChildren)
		{
			ret = tree->Insert(_id, _x, _y);
			if (ret)
				return ret;
		}
		return nullptr;
	}
	else
	{
		mObjVec.push_back(new STreeObj(_id, _x, _y));
		return this;
	}
}

bool CQuadTree::Remove(int _id)
{
	if (mObjVec.size() > 0)
	{	
		auto iter = std::find_if(std::begin(mObjVec), std::end(mObjVec), [&](const STreeObj* _treeObj)->bool {
			return _treeObj->mId == _id;
		});
		if (iter != mObjVec.end())
		{
			delete *iter;
			mObjVec.erase(iter);
			return true;
		}
		return false;
	}
	else if (mChildren.size() > 0)
	{
		for (CQuadTree* tree : mChildren)
		{
			if (tree->Remove(_id))
				return true;
		}
		return false;
	}
}

void CQuadTree::Query(int _id, int _left, int _top, int _right, int _bottom, std::vector<int>& _result)
{
	if (_left > mr || _right < ml || _top < mb || _bottom > mt)
		return;

	if (mChildren.size() > 0)
	{
		for (CQuadTree* tree : mChildren)
			tree->Query(_id, _left, _top, _right, _bottom, _result);
	}
	else
	{
		for (STreeObj* obj : mObjVec)
		{
			//判断 其他点 是否在 选中点 的范围框内，是则在可视化列表内
			if (_id != obj->mId 
				&& obj->mX > _left 
				&& obj->mX < _right 
				&& obj->mY < _top
				&& obj->mY > _bottom)
			{
				_result.push_back(obj->mId);
			}
		}
	}
}
