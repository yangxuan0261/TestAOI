#ifndef __Aoi__H_
#define __Aoi__H_

#include <vector>
#include <map>

class CQuadTree;

struct SAoiObj
{
	SAoiObj() : mTree(nullptr) {}
	int mId;
	int mX;
	int mY;
	CQuadTree* mTree;
	std::map<int, int> mList;
};

class CAoi
{
public:
	CAoi();
	virtual ~CAoi();

	void	Init(int _left, int _top, int _right, int _bottom, int _radius);
	void	Insert(int _id, int _x, int _y, std::map<int, int>& _notiList);
	void	Remove(int _id, std::map<int, int>& _notiList);
	void	Update(int _id, int _x, int _y, std::map<int, int>& _nList, std::map<int, int>& _uList, std::map<int, int>& _oList);
	SAoiObj*	GetAoiObj(int _id);
	CQuadTree*	GetTree() const { return mTree; }
	int			GetDepth();
	void		CaculDepth(int& _depth, CQuadTree* _tree);

private:
	CQuadTree*	mTree;
	std::map<int, SAoiObj*>	mObjMap;
	int			mRadius;
};

#endif
