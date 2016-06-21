#include "OrthList.h"
#include <algorithm>
#include <assert.h>

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

void COrthList::Insert(int _id, int _x, int _y, std::map<int, int>& _notiList)
{
	bool isPre = mRoot->mX >= _x ? true : false;
	SListObj* newObj = new SListObj(_id, _x, _y);
	_insertStep(mRoot, newObj, isPre);
	mObjMap.insert(std::make_pair(_id, newObj));

	std::vector<int> result;
	Query(_id, result);

	for (int id : result)
	{
		auto iter = mObjMap.find(id);
		if (iter != mObjMap.end())
		{
			_notiList.insert(std::make_pair(id, id)); //�����Լ��Ŀ����б�����ˢ�±���
			iter->second->mList.insert(std::make_pair(_id, _id));
		}
	}

	newObj->mList = _notiList;
}

void COrthList::Remove(int _id, std::map<int, int>& _notiList)
{
	auto iter = mObjMap.find(_id);
	if (iter == mObjMap.end())
		return;

	SListObj* listObj = iter->second;
	_notiList = listObj->mList; //�����Լ��Ŀ����б�����ˢ�±���

	//��������
	if (listObj->preObj != nullptr)
		listObj->preObj->nextObj = listObj->nextObj;

	if (listObj->nextObj != nullptr)
		listObj->nextObj->preObj = listObj->preObj;

	//�����Լ������б��Ƴ����˿����б��е��Լ�
	for (auto iter = listObj->mList.begin(); iter != listObj->mList.end(); ++iter)
	{
		int id = iter->second;
		auto it = mObjMap.find(id);
		if (it != mObjMap.end())
		{
			std::map<int, int>& list = it->second->mList;
			auto listIt = list.find(_id);
			if (listIt != list.end())
				list.erase(listIt);
		}
	}

	delete iter->second;
	mObjMap.erase(iter);
}

void COrthList::Update(int _id, int _x, int _y, std::map<int, int>& _aList, std::map<int, int>& _uList, std::map<int, int>& _rList)
{
	auto iter = mObjMap.find(_id);
	if (iter == mObjMap.end())
		return;

	SListObj* aoiObj = iter->second;
	_rList = aoiObj->mList; //ԭ�п����б�



	_adjustStep(aoiObj, _x, _y); //��������

	std::vector<int> result;
	Query(aoiObj->mId, result);

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
}

void COrthList::Query(int _id, std::vector<int>& _result)
{
	auto iter = mObjMap.find(_id);
	if (iter == mObjMap.end())
		return;

	SListObj* listObj = iter->second; //���ҿ���
	_queryStep(listObj->preObj, listObj, true, _result);
	_queryStep(listObj->nextObj, listObj, false, _result);
}

SListObj* COrthList::GetAoiObj(int _id)
{
	auto iter = mObjMap.find(_id);
	return iter != mObjMap.end() ? iter->second : nullptr;
}

bool COrthList::_insertStep(SListObj* _dstObj, SListObj* _srcObj, bool isPre)
{
	if (_dstObj == nullptr)
		return false;

	if (isPre)
	{
		if (_srcObj->mX <= _dstObj->mX)
		{
			if (_dstObj->preObj == nullptr) //�ɲ���
			{
				_srcObj->nextObj = _dstObj;
				_dstObj->preObj = _srcObj;
				return true;
			}
			else if (_dstObj->preObj->mX <= _srcObj->mX) //�ɲ���
			{
				_dstObj->preObj->nextObj = _srcObj;
				_srcObj->preObj = _dstObj->preObj;

				_dstObj->preObj = _srcObj;
				_srcObj->nextObj = _dstObj;
				return true;
			}
			else //������ǰ����
			{
				return _insertStep(_dstObj->preObj, _srcObj, isPre);
			}
		}
		
		assert(false, "--- COrthList::StepMove, _srcObj->mX > _dstObj->mX");
	}
	else
	{
		if (_srcObj->mX >= _dstObj->mX)
		{
			if (_dstObj->nextObj == nullptr) //�ɲ���
			{
				_srcObj->preObj = _dstObj;
				_dstObj->nextObj = _srcObj;
				return true;
			}
			else if (_dstObj->nextObj->mX >= _srcObj->mX) //�ɲ���
			{
				_dstObj->nextObj->preObj = _srcObj;
				_srcObj->nextObj = _dstObj->nextObj;

				_dstObj->nextObj = _srcObj;
				_srcObj->preObj = _dstObj;
				return true;
			}
			else //����������
			{
				return _insertStep(_dstObj->nextObj, _srcObj, isPre);
			}
		}

		assert(false, "--- COrthList::StepMove, _srcObj->mX < _dstObj->mX");
	}
}

void COrthList::_queryStep(SListObj* _dstObj, SListObj* _srcObj, bool isPre, std::vector<int>& _result)
{
	if (_dstObj == nullptr)
		return ;

	if (_dstObj->mId > 0) //���˸��ڵ�
	{
		if (abs(_dstObj->mX - _srcObj->mX) <= mRadius) //x �ڷ�Χ��
		{
			if (abs(_dstObj->mY - _srcObj->mY) <= mRadius) //y �ڷ�Χ��
				_result.push_back(_dstObj->mId);
		}
		else //������Χ�򲻼�������
			return;
	}

	SListObj* nextObj = isPre ? _dstObj->preObj : _dstObj->nextObj;
	_queryStep(nextObj, _srcObj, isPre, _result);
}

//�ƶ��ǵ�������
void COrthList::_adjustStep(SListObj* _srcObj, int _newX, int _newY)
{
	_srcObj->mY = _newY;
	if (_newX == _srcObj->mX) //x ����
		return;

	bool isPre = _newX - _srcObj->mX < 0 ? true : false;
	_srcObj->mX = _newX;

	if (isPre)
	{
		if (_srcObj->preObj == nullptr)
			return;
		else if (_srcObj->preObj->mX < _srcObj->mX) //�ƶ�û�����ڽ��ڵ�
			return;
		else
		{
			//�ϵ���ǰ���ӣ����²���
			_srcObj->preObj->nextObj = _srcObj->nextObj;
			if (_srcObj->nextObj != nullptr)
				_srcObj->nextObj->preObj = _srcObj->preObj;

			SListObj* preObj = _srcObj->preObj;
			_srcObj->preObj = nullptr;
			_srcObj->nextObj = nullptr;
			_insertStep(preObj, _srcObj, isPre);
		}
	}
	else
	{
		if (_srcObj->nextObj == nullptr)
			return;
		else if (_srcObj->nextObj->mX > _srcObj->mX) //�ƶ�û�����ڽ��ڵ�
			return;
		else
		{
			//�ϵ���ǰ���ӣ����²���
			_srcObj->nextObj->preObj = _srcObj->preObj;
			if (_srcObj->preObj != nullptr)
				_srcObj->preObj->nextObj = _srcObj->nextObj;

			SListObj* nextObj = _srcObj->nextObj;
			_srcObj->preObj = nullptr;
			_srcObj->nextObj = nullptr;
			_insertStep(nextObj, _srcObj, isPre);
		}
	}
}
