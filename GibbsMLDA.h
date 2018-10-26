#pragma once
#include <vector>
#include "Rand.h"


class CGibbsMLDA
{

public:
	CGibbsMLDA();
	~CGibbsMLDA();

	/////////////////////////////////////////
	// メモリの確保．必ず最初に実行．
	// numTopic : カテゴリの数
	// numDocument : 物体の数
	// numModal : 使用するモダリティの数
	// modalDim : 各モーダルの次元
	void Initilize( int numTopic , int numDocument , int numModal , int *modalDim );

	/////////////////////////////////////////
	// データのセット
	//  data : 各物体のデータ
	//		　data[i][j][k] ⇒ i番目の物体の、j番目のモーダルの、ヒストグラムのk次元目が格納されている

	void SetNumSamplingIteration(int num){ m_numIteration = num; }
	void SetData( double **data[] );
	double Learn();
	double Recog( const char *modelDir );
	void SaveModel( const char *dir );
	std::vector<int> GetClassificationResult();
	std::vector< std::vector<double> > GetPw_dw( int modality );
	std::vector< std::vector<double> > GetPz_dz();
	bool LoadModel( const char *dir );


protected:
	void ReleaseData();
	void ReleaseBuffer();										// メモリの開放．

	void RandomZ( bool fixed=false );												// zの初期化．ランダムでカテゴリを割り当てる
	void CalcPhi();
	void CalcTheta();
	double CalcLiklihood( int m );								// モダリティmの尤度を計算

	inline int Sampling( int d, int m, int l, bool fixed=false );
	unsigned int random();										// 乱数生成関数
	double random0_1();
	
	void InitN();

	double Update( bool fixed=false );

	long double SumExp( long double e1 , long double e2 );


	/* 
	接尾辞
	 m : モダリティのインデックス
	 d : ドキュメントのインデックス（物体数）
	 z : カテゴリのインデックス
	 w : 単語のインデックス
	 l : 文書の長さ（単語数）のインデックス
	*/

	// ハイパパラメタ
	double *m_beta_m;					// phiの事前分布（モダリティ分だけ存在） 
	double m_alpha;					// tehtaの事前分布

	// 推定する確率
	double **m_theta_dz;			// θ[document][topic]　		P(z|d)
	double ***m_phi_mzw;			// φ[modality][topic][word^w]	P(w^m|d)

	// リサンプリングに必要な整数
	int ***m_N_mwz;					// N[modalty][word][topic]	単語w^mのうちトピックがzとなった数
	int **m_N_dz;					// N[document][topic]	文書dの中のトピックzが割り当てられた単語数
	int **m_N_mz;					// N[modality][topic]	モダリティm内の単語のうち、トピックがzとなった数
	int *m_N_z;						// N[topic]				トピックがzとなった単語数
	int *m_N_d;						// N[documents]			各文書内の単語の総数

	double *m_P_z;					// サンプリング用のバッファ

	double *m_P_dmwz;

	struct Document{
		int lenght;			// 文書の長さ
		int *wordID_l;		// 各単語のID
		int *z_l;			// 各単語に割り当てられたカテゴリ
	}**m_documents_dm;

	
	double m_lik;
	double *m_modalLik;				//各モーダルの尤度

	int m_numDoc;					//ドキュメントの数
	int m_numModal;					//使用するモーダルの数
	int m_numTopic;
	int *m_dataDim;					//書くモーダルのデータの次元数
	double ***m_data_mdw;			//入力されたデータ

	int m_numIteration;			//繰り返

	CRand m_rand;
};