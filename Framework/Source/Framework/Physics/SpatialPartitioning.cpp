#include "SpatialPartitioning.h"

//=============================================================================
// 
// 分木登録オブジェクト(OFT)
// 
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
template<class T>
ObjectForTree<T>::ObjectForTree()
	: m_pCell(nullptr)
	, m_pObject(nullptr)
{
}

//-----------------------------------------------------------------------------
// リストから外れる
//-----------------------------------------------------------------------------
template<class T>
bool ObjectForTree<T>::Remove()
{
	if (!m_pCell) return false;// すでに逸脱している時は処理終了
	if (!m_pCell->OnRemove(this)) return false;// 自分を登録している空間に自身を通知

	// 逸脱処理
	// 前後のオブジェクトを結びつける
	if (m_spPre.get() != nullptr)
	{
		m_spPre->m_spNext = m_spNext;
		m_spPre = nullptr;
	}
	if (m_spNext.get() != nullptr)
	{
		m_spNext->m_spPre = m_spPre;
		m_spNext = nullptr;
	}
	m_pCell = nullptr;
	return true;
}



//=============================================================================
// 
// 線形8分木空間管理クラス
// 
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
template<class T>
CLiner8TreeManager<T>::CLiner8TreeManager()
	: m_uiDim(0)
	, ppCellAry(nullptr)
	, m_iPow()
	, m_W(float3::One)
	, m_RgnMin(float3::Zero)
	, m_RgnMax(float3::One)
	, m_Unit(float3::One)
	, m_dwCellNum(0)
	, m_uiLevel()
{
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
template<class T>
CLiner8TreeManager<T>::~CLiner8TreeManager()
{
	DWORD i;
	for (i = 0; i < m_dwCellNum; i++)
	{
		if (ppCellAry[i] != nullptr)
			delete ppCellAry[i];
	}
	delete[] ppCellAry;
}

//-----------------------------------------------------------------------------
// 初期化(線形8分木配列を構築する)
//-----------------------------------------------------------------------------
template<class T>
bool CLiner8TreeManager<T>::Initialize(unsigned int level, float3& min, float3& max)
{
	if (level >= CLINER8TREEMANAGER_MAXLEVEL) return false;

	// 各レベルでの空間数を算出
	int i;
	m_iPow[0] = 1;
	for (i = 1; i < CLINER8TREEMANAGER_MAXLEVEL + 1; i++)
		m_iPow[i] = m_iPow[i - 1] * 8;

	// レベル(0基点)の配列作成
	m_dwCellNum = (m_iPow[level + 1] - 1) / 7;
	ppCellAry = new CCell<T>*[m_dwCellNum];
	ZeroMemory(ppCellAry, sizeof(CCell<T>*) * m_dwCellNum);

	// 領域を登録
	m_RgnMin = min;
	m_RgnMax = max;
	m_W = m_RgnMax - m_RgnMin;
	m_Unit = m_W / ((float)(1 << level));

	m_uiLevel = level;

	return true;
}

//-----------------------------------------------------------------------------
// オブジェクトを登録
//-----------------------------------------------------------------------------
template<class T>
bool CLiner8TreeManager<T>::Regist(float3* min, float3* max, std::shared_ptr<ObjectForTree<T>>& spOFT)
{
	// オブジェクトの境界範囲から登録モートン番号を算出
	DWORD index = GetMortonNumber(min, max);
	if (index < m_dwCellNum)
	{
		// 空間が無い場合は新規作成
		if (!ppCellAry[index])
			CreateNewCell(index);

		return ppCellAry[index]->Push(spOFT);
	}
	return false;// 登録失敗
}

//-----------------------------------------------------------------------------
// 衝突判定リストを作成しサイズを返す
//-----------------------------------------------------------------------------
template<class T>
DWORD CLiner8TreeManager<T>::GetAllCollisionList(std::vector<T*>& colVec)
{
	colVec.clear();

	// ルート空間の存在をチェック
	if (ppCellAry[0] == nullptr) return 0;

	// ルート空間を処理
	std::list<T*> ColStac;
	GetCollisionList(0, colVec, ColStac);

	return (DWORD)colVec.size();
}

//-----------------------------------------------------------------------------
// 空間内で衝突リストを作成
//-----------------------------------------------------------------------------
template<class T>
bool CLiner8TreeManager<T>::GetCollisionList(DWORD index, std::vector<T*>& ColVect, std::list<T*>& ColStac)
{
	std::list<T*, std::iterator> it;
	// ① 空間内のオブジェクト同士の衝突リスト作成
	std::shared_ptr<ObjectForTree<T>> spOFT1 = ppCellAry[index]->GetFirstObj();
	while (spOFT1.get() != nullptr)
	{
		std::shared_ptr<ObjectForTree<T>> spOFT2 = spOFT1->m_spNext;
		while (spOFT2 != nullptr)
		{
			// 衝突リスト作成
			ColVect.push_back(spOFT1->m_pObject);
			ColVect.push_back(spOFT2->m_pObject);
			spOFT2 = spOFT2->m_spNext;
		}
		// ② 衝突スタックとの衝突リスト作成
		for (it = ColStac.begin(); it != ColStac.end(); it++)
		{
			ColVect.push_back(spOFT1->m_pObject);
			ColVect.push_back(*it);
		}
		spOFT1 = spOFT1->m_spNext;
	}

	bool ChildFlag = false;
	// ③ 子空間に移動
	DWORD ObjNum = 0;
	DWORD i, Nextindex;
	for (i = 0; i < 8; i++)
	{
		Nextindex = index * 8 + 1 + i;
		if (Nextindex < m_dwCellNum && ppCellAry[index * 8 + 1 + i])
		{
			if (!ChildFlag)
			{
				// ④ 登録オブジェクトをスタックに追加
				spOFT1 = ppCellAry[index]->GetFirstObj();
				while (spOFT1.get())
				{
					ColStac.push_back(spOFT1->m_pObject);
					ObjNum++;
					spOFT1 = spOFT1->m_spNext;
				}
			}
			ChildFlag = true;
			GetCollisionList(index * 8 + 1 + i, ColVect, ColStac);	// 子空間へ
		}
	}

	// ⑤ スタックからオブジェクトを外す
	if (ChildFlag)
	{
		for (i = 0; i < ObjNum; i++)
			ColStac.pop_back();
	}

	return true;
}

//-----------------------------------------------------------------------------
// 空間を生成
//-----------------------------------------------------------------------------
template<class T>
bool CLiner8TreeManager<T>::CreateNewCell(DWORD index)
{
	// 引数の要素番号
	while (!ppCellAry[index])
	{
		// 指定の要素番号に空間を新規作成
		ppCellAry[index] = new CCell<T>;

		// 親空間にジャンプ
		index = (index - 1) >> 3;
		if (index >= m_dwCellNum) break;
	}
	return true;
}

//-----------------------------------------------------------------------------
// 座標から空間番号を算出
//-----------------------------------------------------------------------------
template<class T>
DWORD CLiner8TreeManager<T>::GetMortonNumber(float3* Min, float3* Max)
{
	// 最小レベルにおける各軸位置を算出
	DWORD LT = GetPointElem(*Min);
	DWORD RB = GetPointElem(*Max);

	// 空間番号を引き算して
	// 最上位区切りから所属レベルを算出
	DWORD Def = RB ^ LT;
	unsigned int HiLevel = 1;
	unsigned int i;
	for (i = 0; i < m_uiLevel; i++)
	{
		DWORD Check = (Def >> (i * 3)) & 0x7;
		if (Check != 0)
			HiLevel = i + 1;
	}
	DWORD SpaceNum = RB >> (HiLevel * 3);
	DWORD AddNum = (m_iPow[m_uiLevel - HiLevel] - 1) / 7;
	SpaceNum += AddNum;

	if (SpaceNum > m_dwCellNum)
		return 0xffffffff;

	return SpaceNum;
}

//-----------------------------------------------------------------------------
// ビット分割関数
//-----------------------------------------------------------------------------
template<class T>
DWORD CLiner8TreeManager<T>::BitSeparateFor3D(BYTE n)
{
	DWORD s = n;
	s = (s | s << 8) & 0x0000f00f;
	s = (s | s << 4) & 0x000c30c3;
	s = (s | s << 2) & 0x00249249;
	return s;
}

//-----------------------------------------------------------------------------
// 3Dモートン空間番号算出関数
//-----------------------------------------------------------------------------
template<class T>
DWORD CLiner8TreeManager<T>::Get3DMortonNumber(BYTE x, BYTE y, BYTE z)
{
	return BitSeparateFor3D(x) |
		BitSeparateFor3D(y) << 1 |
		BitSeparateFor3D(z) << 2;
}

//-----------------------------------------------------------------------------
// 座標→線形8分木要素番号変換関数
//-----------------------------------------------------------------------------
template<class T>
DWORD CLiner8TreeManager<T>::GetPointElem(float3& p)
{
	return Get3DMortonNumber(
		(BYTE)((p.x - m_RgnMin.x) / m_Unit.x),
		(BYTE)((p.y - m_RgnMin.y) / m_Unit.y),
		(BYTE)((p.z - m_RgnMin.z) / m_Unit.z)
	);
}



//=============================================================================
// 
// 空間クラス
// 
//=============================================================================

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
template<class T>
CCell<T>::~CCell()
{
	if (m_spLatest.get() != nullptr)
		ResetLink(m_spLatest);
}

//-----------------------------------------------------------------------------
// リンクを全てリセットする
//-----------------------------------------------------------------------------
template<class T>
void CCell<T>::ResetLink(std::shared_ptr<ObjectForTree<T>>& spOFT)
{
	if (spOFT->m_spNext.get() != nullptr)
		ResetLink(spOFT->m_spNext);
	spOFT = nullptr;
}

//-----------------------------------------------------------------------------
// OFTをプッシュ
//-----------------------------------------------------------------------------
template<class T>
bool CCell<T>::Push(std::shared_ptr<ObjectForTree<T> >& spOFT)
{
	if (spOFT.get() == nullptr) return false;// 無効オブジェクトは登録しない
	if (spOFT->m_pCell == this) return false;	// 2重登録チェック

	if (m_spLatest.get() == nullptr)
	{
		// 空間に新規登録
		m_spLatest = spOFT;
	}
	else
	{
		// 最新OFTオブジェクトを更新
		spOFT->m_spNext = m_spLatest;
		m_spLatest->m_spPre = spOFT;
		m_spLatest = spOFT;
	}
	spOFT->RegistCell(this);// 空間を登録
	return true;
}

//-----------------------------------------------------------------------------
// 削除されるオブジェクトをチェック
//-----------------------------------------------------------------------------
template<class T>
bool CCell<T>::OnRemove(ObjectForTree<T>* pRemoveObj)
{
	if (m_spLatest.get() == pRemoveObj)
	{
		// 次のオブジェクトに挿げ替え
		if (m_spLatest != nullptr)
			m_spLatest = m_spLatest->GetNextObj();
	}
	return true;
}
