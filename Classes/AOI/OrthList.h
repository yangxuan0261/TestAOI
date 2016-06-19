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

	void		Insert(int _id, int _x, int _y, std::map<int, int>& _notiList);
	void		Remove(int _id, std::map<int, int>& _notiList);
	void		Update(int _id, int _x, int _y, std::map<int, int>& _aList, std::map<int, int>& _uList, std::map<int, int>& _rList);

	void		Query(int _id, std::vector<int>& _result);
	SListObj*	GetAoiObj(int _id);
	int			GetRange() const { return mRadius; }

	bool		_insertStep(SListObj* _dstObj, SListObj* _srcObj, bool isPre);
	void		_queryStep(SListObj* _dstObj, SListObj* _srcObj, bool isPre, std::vector<int>& _result);
	void		_adjustStep();
public:
	SListObj*		mRoot;
	int				mRadius;
	std::map<int, SListObj*>	mObjMap;

};

#endif
