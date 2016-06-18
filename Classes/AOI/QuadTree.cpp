#include "QuadTree.h"

CQuadTree::CQuadTree(int _l, int _t, int _r, int _b, CQuadTree* _parent)
{
	ml = _l;
	mt = _t;
	mr = _r;
	mb = _b;
	mDepth = 1;
	if (_parent != nullptr)
	{
		mDepth = _parent->mDepth + 1;
	}
}


CQuadTree::~CQuadTree()
{
	for (CQuadTree* tree : mChildren)
		delete tree;

	for (STreeObj* obj : mObjVec)
		delete obj;
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
	}
	else
	{
		mObjVec.push_back(new STreeObj(_id, _x, _y));

		if (mObjVec.size() > 1)
		{
			return Subdivide(_id);
		}

		return this;
	}
}

CQuadTree* CQuadTree::Subdivide(int last)
{
	int centerX = (ml + mr) / 2;
	int centerY = (mt + mb) / 2;

	if (mChildren.size() == 0)
	{
		mChildren.push_back(new CQuadTree(ml, mt, centerX, centerY, this));
		mChildren.push_back(new CQuadTree(centerX, mt, mr, centerY, this));
		mChildren.push_back(new CQuadTree(ml, centerY, centerX, mb, this));
		mChildren.push_back(new CQuadTree(centerX, centerY, mr, mb, this));
	}

	CQuadTree* tmp = nullptr;
	CQuadTree* ret = nullptr;
	for (STreeObj* obj : mObjVec)
	{
		for (CQuadTree* tree : mChildren)
		{
			tmp = tree->Insert(obj->mId, obj->mX, obj->mY);
			if (tmp)
			{
				if (obj->mId == last)
				{
					ret = tmp;
					break;
				}
			}
		}

		delete obj;
	}
	mObjVec.clear();

	return ret;
}

bool CQuadTree::Remove(int _id)
{
	if (mObjVec.size() > 0)
	{
		if (mObjVec[0]->mId == _id)
		{
			delete mObjVec[0];
			mObjVec.clear();
			return true;
		}
	}
	else if (mChildren.size() > 0)
	{
		for (CQuadTree* tree : mChildren)
		{
			if (tree->Remove(_id))
				return true;
		}
	}
	return false;
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
		if (mObjVec.size() > 0)
		{
			STreeObj* obj = mObjVec[0];
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
