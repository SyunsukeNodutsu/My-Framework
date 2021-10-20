//-----------------------------------------------------------------------------
// File: SpatialPartitioning.h
//
// 空間分割による衝突判定の高速化テスト
// 参考：http://marupeke296.com/COL_3D_No15_Octree.html
//-----------------------------------------------------------------------------
#pragma once
#define CLINER8TREEMANAGER_MAXLEVEL 7

template <class T>
class CCell;

//=============================================================================
// 分木登録オブジェクト(OFT)
//=============================================================================
template<class T>
class ObjectForTree
{
public:

	// コンストラクタ
	ObjectForTree();

	// デストラクタ
	virtual ~ObjectForTree() { }

	// リストから外れる
	bool Remove();

	// 空間を登録
	void RegistCell(CCell<T>* pCell) { m_pCell = pCell; }

	// 次のオブジェクトへのスマートポインタを返す
	std::shared_ptr<ObjectForTree<T>>& GetNextObj() {
		return m_spNext;
	}

public:

	// 登録空間
	CCell<T>* m_pCell;

	// 判定対象オブジェクト
	T* m_pObject;

	// 連続する木構造
	std::shared_ptr<ObjectForTree<T>> m_spPre;	// 前のObjectForTree構造体
	std::shared_ptr<ObjectForTree<T>> m_spNext;	// 次のObjectForTree構造体

};



//=============================================================================
// 線形8分木空間管理クラス
//=============================================================================
template <class T>
class CLiner8TreeManager
{
public:

	// コンストラクタ
	CLiner8TreeManager();

	// デストラクタ
	virtual ~CLiner8TreeManager();

	// 初期化(線形8分木配列を構築する)
	bool Initialize(unsigned int level, float3& min, float3& max);

	// オブジェクトを登録
	bool Regist(float3* min, float3* max, std::shared_ptr<ObjectForTree<T>>& spOFT);

	// 衝突判定リストを作成しサイズを返す
	DWORD GetAllCollisionList(std::vector<T*>& colVec);

protected:

	unsigned int m_uiDim;
	CCell<T>** ppCellAry;	// 線形空間ポインタ配列
	unsigned int m_iPow[CLINER8TREEMANAGER_MAXLEVEL + 1];// べき乗数値配列
	float3 m_W;				// 領域の幅
	float3 m_RgnMin;		// 領域の最小値
	float3 m_RgnMax;		// 領域の最大値
	float3 m_Unit;			// 最小領域の辺の長さ
	DWORD m_dwCellNum;		// 空間の数
	unsigned int m_uiLevel;	// 最下位レベル

protected:

	// 空間内で衝突リストを作成
	bool GetCollisionList(DWORD index, std::vector<T*>& colVec, std::list<T*>& colStac);

	// 空間を生成
	bool CreateNewCell(DWORD index);

	// 座標から空間番号を算出
	DWORD GetMortonNumber(float3* min, float3* max);

	// ビット分割関数
	DWORD BitSeparateFor3D(BYTE n);

	// 3Dモートン空間番号算出関数
	DWORD Get3DMortonNumber(BYTE x, BYTE y, BYTE z);

	// 座標→線形8分木要素番号変換関数
	DWORD GetPointElem(float3& p);

};



//=============================================================================
// 空間クラス
//=============================================================================
template <class T>
class CCell
{
public:

	// コンストラクタ
	CCell() { }

	// デストラクタ
	virtual ~CCell();

	// リンクを全てリセットする
	void ResetLink(std::shared_ptr<ObjectForTree<T>>& spOFT);

	// OFTをプッシュ
	bool Push(std::shared_ptr<ObjectForTree<T> >& spOFT);

	// 削除されるオブジェクトをチェック
	bool OnRemove(ObjectForTree<T>* pRemoveObj);

	// 最新OFTへのポインタを返す
	std::shared_ptr<ObjectForTree<T> >& GetFirstObj() {
		return m_spLatest;
	}

protected:

	// 最新OFLへのスマートポインタ
	std::shared_ptr<ObjectForTree<T>> m_spLatest;

};
