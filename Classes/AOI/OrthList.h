#ifndef __OrthList__H_
#define __OrthList__H_

#include <vector>
#include <map>

struct SListObj
{
	SListObj(int _id, int _x, int _y)
	{
		mId = _id;
		mX = _x;
		mY = _y;
		preObj = nullptr;
		nextObj = nullptr;
	}
	int mId;
	int mX;
	int mY;
	SListObj* preObj;
	SListObj* nextObj;
	std::map<int, int> mList;
};

class COrthList
{
public:
	COrthList(int _centerX, int _centerY, int _raduis);
	virtual ~COrthList();

	void		Insert(int _id, int _x, int y);
	void		Remove(int _id);
	void		Query(int _id, int _left, int _top, int _right, int _bottom, std::vector<int>& _result);

	SListObj*		StepMove(SListObj* _dstObj, SListObj* _srcObj, bool isPre);
public:
	SListObj*		mRoot;
	int				mRadius;
	std::map<int, SListObj*>	mObjMap;

};

#endif
