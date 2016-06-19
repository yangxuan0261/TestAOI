#include "OrthList.h"
#include <algorithm>

COrthList::COrthList(int _centerX, int _centerY, int _raduis)
{
	mRoot = new SListObj(-1, _centerX, _centerY);
	mRadius = _raduis;
}

COrthList::~COrthList()
{
	if (mRoot)
		delete mRoot;
}

void COrthList::Insert(int _id, int _x, int y)
{
	
}

void COrthList::Remove(int _id)
{

}

void COrthList::Query(int _id, int _left, int _top, int _right, int _bottom, std::vector<int>& _result)
{

}

SListObj* COrthList::StepMove(SListObj* _dstObj, SListObj* _srcObj, bool isPre)
{
	if (_dstObj == nullptr)
		return nullptr;


	if (isPre)
	{
		if (_srcObj->mX <= _dstObj->mX)
		{
			if (_dstObj->preObj == nullptr) //可插入
			{
				_srcObj->nextObj = _dstObj;
				_dstObj->preObj = _srcObj;
				return _srcObj;
			}
			else if (_dstObj->preObj->mX <= _srcObj->mX) //可插入
			{
				_dstObj->preObj->nextObj = _srcObj;
				_srcObj->preObj = _dstObj->preObj;

				_dstObj->preObj = _srcObj;
				_srcObj->nextObj = _dstObj;
				return _srcObj;
			}
			else //继续向前查找
			{
				return StepMove(_dstObj->preObj, _srcObj, isPre);
			}
		}

		return false; //不会走到行代码
	}
	else
	{
		if (_dstObj->nextObj != nullptr)
		{
			return StepMove(_dstObj->nextObj, _srcObj, isPre); //向后查找
		}
		return false; //不会走到行代码
	}
}

