#ifndef __QuadTree__H_
#define __QuadTree__H_

#include <vector>

struct STreeObj
{
	STreeObj(int _id, int _x, int _y)
	{
		mId = _id;
		mX = _x;
		mY = _y;
	}
	int mId;
	int mX;
	int mY;
};

class CQuadTree
{
public:
	CQuadTree(int _l, int _t, int _r, int _b, CQuadTree* _parent);
	virtual ~CQuadTree();

	void			genChildren();
	CQuadTree*		Insert(int _id, int _x, int _y);
	bool			Remove(int _id);
	void			Query(int _id, int _left, int _top, int _right, int _bottom, std::vector<int>& _result);
	const std::vector<CQuadTree*>&	GetChildren() { return mChildren; }

public:
	int		ml;
	int		mt;
	int		mr;
	int		mb;
	std::vector<STreeObj*>	mObjVec;
	std::vector<CQuadTree*>	mChildren;

	//just for caculate depth
	CQuadTree*	mParent; 
	int		mDepth;
};

#endif
