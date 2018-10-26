#define _CRT_SECURE_NO_WARNINGS 1
#include "GibbsMLDA.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include "array.h"
#include <float.h>
#include <algorithm>
#include "picojson.h"


using namespace std;

#define CHECK_NULL( var ) if( !var ){ printf( "Error 関数 [ %s ] 内の変数 [ %s ] がNULL!\n\n" , __FUNCTION__ , #var ); }

// 乱数
unsigned int CGibbsMLDA::random()
{
	return rand();
}

double CGibbsMLDA::random0_1()
{
	return (double)rand()/RAND_MAX;
}



CGibbsMLDA::CGibbsMLDA()
{

	// 初期化
	m_beta_m = NULL;
	m_alpha = 0;

	m_theta_dz = NULL;
	m_phi_mzw = NULL;

	m_N_mwz = NULL;
	m_N_dz = NULL;
	m_N_mz = NULL;
	m_N_z = NULL;
	m_N_d = NULL;

	m_documents_dm = NULL;

	m_lik = 0;
	m_modalLik = NULL;

	m_numDoc = 0;
	m_numModal = 0;
	m_dataDim = NULL;

	m_P_z = NULL;
	m_data_mdw = NULL;

	m_numIteration = 400;
}

CGibbsMLDA::~CGibbsMLDA()
{
	ReleaseBuffer();
	ReleaseData();
}

void CGibbsMLDA::Initilize( int numTopic , int numDocument , int numModal , int *modalDim )
{
	ReleaseBuffer();

	m_numDoc = numDocument;
	m_numModal = numModal;
	m_numTopic = numTopic;

	m_dataDim = new int[numModal];
	for(int m=0 ; m<numModal ; m++ )
		m_dataDim[m] = modalDim[m];

	m_beta_m = new double[ numModal ];
	for(int i=0 ; i<numModal ; i++ ) m_beta_m[i] = 0.1;

	m_alpha = 50.0 / numTopic;

	m_theta_dz = AllocMatrix<double>( numDocument , numTopic );
	Zeros( m_theta_dz , numDocument , numTopic );

	m_phi_mzw = new double**[numModal];
	for(int m=0 ; m<numModal ; m++ )
	{
		m_phi_mzw[m] = AllocMatrix<double>( numTopic , modalDim[m] );
		Zeros( m_phi_mzw[m] , numTopic , modalDim[m] );
	}

	m_N_mwz = new int**[numModal];
	for(int m=0 ; m<numModal ; m++ )
	{
		m_N_mwz[m] = AllocMatrix<int>( modalDim[m] , numTopic );
		Zeros( m_N_mwz[m] , modalDim[m] , numTopic );
	}

	m_N_dz = AllocMatrix<int>( numDocument , numTopic );
	Zeros( m_N_dz , numDocument , numTopic );
	/*
	m_N_mz = new int*[numModal];
	for(int m=0 ; m<numModal ; m++ )
	{
	m_N_mz[m] = new int [numTopic];
	Zeros( m_N_mz[m] , numTopic );
	}
	*/

	m_N_mz = AllocMatrix<int>( numModal , numTopic );
	Zeros( m_N_mz , numModal , numTopic );

	m_N_z = new int[numTopic];
	Zeros( m_N_z , numTopic );

	m_P_z = new double[numTopic];
	Zeros( m_P_z , numTopic );

	m_modalLik = new double[numModal];
	Zeros( m_modalLik , numModal );

	m_N_d = new int [numDocument];
	Zeros( m_N_d , numDocument );
}

void CGibbsMLDA::InitN()
{
	// 初期化
	for(int m=0 ; m<m_numModal ; m++ )
	{
		Zeros( m_N_mwz[m] , m_dataDim[m] , m_numTopic );
		Zeros( m_N_mz[m] , m_numTopic );
	}
	Zeros( m_N_dz , m_numDoc , m_numTopic );
	Zeros( m_N_z , m_numTopic );
}

void CGibbsMLDA::ReleaseBuffer()
{

	delete [] m_dataDim;	m_dataDim = NULL;
	delete [] m_beta_m;		m_beta_m = NULL;
	Free( m_theta_dz );		m_theta_dz = NULL;
	Free( m_N_dz );			m_N_dz = NULL;
	Free( m_N_mz );			m_N_mz = NULL;

	for(int m=0 ; m<m_numModal ; m++ )
	{
		Free( m_phi_mzw[m] );
		Free( m_N_mwz[m] );
	}

	delete [] m_phi_mzw;	m_phi_mzw = NULL;
	delete [] m_N_mwz;		m_N_mwz = NULL;	
	delete [] m_N_mz;		m_N_mz = NULL;
	delete [] m_N_z;		m_N_z = NULL;
	delete [] m_N_d;		m_N_d = NULL;
	delete [] m_modalLik;	m_modalLik = NULL;
	delete [] m_P_z;		m_P_z = NULL;

}



void CGibbsMLDA::SetData( double **data[] )
{
	ReleaseData();

	m_data_mdw = data;

	// メモリ確保
	m_documents_dm = AllocMatrix<Document>( m_numDoc , m_numModal );
	Zeros( m_documents_dm , m_numDoc , m_numModal );

	Zeros( m_N_d , m_numDoc );

	for(int d=0 ; d<m_numDoc ; d++ )
	{
		for(int m=0 ; m<m_numModal ; m++ )
		{
			if( data[m] == NULL ) continue;

			int length = 0;

			// 合計を計算
			for(int w=0 ; w<m_dataDim[m] ; w++ ) length += (int)data[m][d][w];

			m_documents_dm[d][m].lenght = length;
			m_documents_dm[d][m].wordID_l = new int[length];
			m_documents_dm[d][m].z_l = new int[length];

			// 単語を代入
			for(int w=0, l=0 ; w<m_dataDim[m] ; w++ )
			{
				for(int j=0 ; j<(int)data[m][d][w] ; j++ , l++ )
				{
					m_documents_dm[d][m].wordID_l[l] = w;
				}
			}

			m_N_d[d] += length;		
		}
	}
}

void CGibbsMLDA::ReleaseData()
{
	if( m_documents_dm )
	{

		for(int d=0 ; d<m_numDoc ; d++ )
		{
			for(int m=0 ; m<m_numModal ; m++ )
			{
				if( m_documents_dm[d][m].wordID_l ) delete [] m_documents_dm[d][m].wordID_l;
				if( m_documents_dm[d][m].z_l ) delete [] m_documents_dm[d][m].z_l;
			}
		}

		Free( m_documents_dm );	m_documents_dm = NULL;

	}

	m_data_mdw = NULL;
}



void CGibbsMLDA::RandomZ( bool fixed )
{
	if( m_numDoc==0 || !m_documents_dm )
	{
		printf("Error : Documentがセットされていません\n");
		return;
	}


	// ランダムでトピックを割り当て
	for(int d=0 ; d<m_numDoc ; d++ )
	{
		for(int m=0 ; m<m_numModal ; m++ )
		{
			int *z_l =  m_documents_dm[d][m].z_l;
			int *wordID_l = m_documents_dm[d][m].wordID_l;
			int L = m_documents_dm[d][m].lenght;
			for(int l=0 ; l<L ; l++ )
			{
				int z = random() % m_numTopic;
				int w = wordID_l[l];

				z_l[l] = z;

				// 総数をカウント
				m_N_dz[d][z]++;

				if( !fixed )
				{
					m_N_mwz[m][w][z]++;
					m_N_mz[m][z]++;
				}
				m_N_z[z]++;
			}
		}
	}

}

void CGibbsMLDA::CalcPhi()
{

	for(int z=0 ; z<m_numTopic ; z++ )
	{
		for(int m=0 ; m<m_numModal ; m++ )
		{
			double W = m_dataDim[m];
			double beta = m_beta_m[m];
			double cons = 1 / (m_N_mz[m][z] + W * beta);

			for(int w=0 ; w<W ; w++ )
			{
				m_phi_mzw[m][z][w] = ( m_N_mwz[m][w][z] + beta ) * cons;
			}
		}
	}
}

void CGibbsMLDA::CalcTheta()
{
	for(int d=0 ; d<m_numDoc ; d++ )
	{
		// 定数部分を計算
		double cons = 1/(m_N_d[d] + m_numTopic * m_alpha);
		for(int z=0 ; z<m_numTopic ; z++ )
		{
			m_theta_dz[d][z] = (m_N_dz[d][z] + m_alpha) * cons;
		}
	}


}

double CGibbsMLDA::CalcLiklihood( int m )
{
	int W = m_dataDim[m];
	double lik = 0;

	if( !m_data_mdw[m] ) return 0;

	for(int d=0 ; d<m_numDoc ; d++)
	{
		for (int w=0 ; w<W ; w++)
		{
			double sum = 0.0000000001;

			for(int z=0 ; z<m_numTopic ; z++ )
			{
				sum += m_phi_mzw[m][z][w] * m_theta_dz[d][z];
			}

			lik += m_data_mdw[m][d][w] * log( sum );
		}
	}

	return lik;
}



int CGibbsMLDA::Sampling( int d, int m, int l, bool fixed  )
{
	int topic = m_documents_dm[d][m].z_l[l];
	int w = m_documents_dm[d][m].wordID_l[l];
	double *P_z = m_P_z;						// バッファ


	if( !fixed )
	{
		// カウントから減らす
		m_N_mwz[m][w][topic]--;
		m_N_mz[m][topic]--;
	}

	m_N_dz[d][topic]--;
	m_N_z[topic]--;


	// Gibbs Sampling
	// あらかじめ計算しておく
	double beta = m_beta_m[m];
	double WxBeta = m_dataDim[m] * beta;


	// 多項分布を計算
	for(int z=0 ; z<m_numTopic ; z++ )
	{
		P_z[z] = (m_N_dz[d][z]+m_alpha) * (m_N_mwz[m][w][z] + beta) / (m_N_mz[m][z] + WxBeta);
	}

	// 累積確率密度にする
	for(int z=1 ; z<m_numTopic ; z++ )
	{
		P_z[z] += P_z[z-1];
	}


	// サンプリングするための乱数を発生
	double rand = m_rand.GetRandF() * P_z[m_numTopic-1];


	// 計算した確率に従って新たなトピックを選択
	for(topic=0 ; topic<m_numTopic-1 ; topic++ )
	{
		if( P_z[topic] > rand ) break;
	}


	if( !fixed )
	{
		// 新たなトピックを追加
		m_N_mwz[m][w][topic]++;
		m_N_mz[m][topic]++;
	}

	m_N_dz[d][topic]++;
	m_N_z[topic]++;

	return topic;
}


double CGibbsMLDA::Update( bool fixed )
{
	double plik = 0;
	clock_t start = clock();

	m_rand.Init();

	// Zに初期値をセット
	RandomZ( fixed );

	for(int it=0 ; it<m_numIteration ; it++ )
	{
		// 新たなzを順次サンプリング
		for (int d=0 ; d<m_numDoc ; d++)	// ドキュメント
		{
			for (int m=0 ; m<m_numModal ; m++)	// モダリティ
			{
				if( m_data_mdw[m] )
				{
					int L = m_documents_dm[d][m].lenght;
					int *z_l = m_documents_dm[d][m].z_l;
					for (int l=0 ; l<L ; l++)	// 文書内の単語数
					{
						int z = Sampling( d , m , l , fixed );
						z_l[l] = z;				// 新たなトピックを割り当て
					}
				}
			}
		}


		/*
		// パラメタ計算
		CalcTheta();
		CalcPhi();

		// 尤度計算
		m_lik = 0;
		for (int m=0 ; m<m_numModal ; m++)
		{
			m_modalLik[m] = CalcLiklihood( m );
			m_lik += m_modalLik[m];
		}

		// 判定
		if( it>1 && (fabs((m_lik - plik)/m_lik) < 1e-5 ) )
		{
			break;
		}

		plik = m_lik;
		*/
	}

	// パラメタ計算
	CalcTheta();
	CalcPhi();

	// 尤度計算
	m_lik = 0;
	for (int m=0 ; m<m_numModal ; m++)
	{
		m_modalLik[m] = CalcLiklihood( m );
		m_lik += m_modalLik[m];
	}



	return m_lik;
}

double CGibbsMLDA::Learn()
{
	InitN();
	return Update();
}

double CGibbsMLDA::Recog( const char *modelDir )
{

	InitN();
	LoadModel( modelDir );
	return Update(true);
}

vector<int> CGibbsMLDA::GetClassificationResult()
{
	vector<int> topics;
	for(int d=0 ; d<m_numDoc ; d++ )
	{
		int maxZ = -1;
		double max = -DBL_MAX;
		for(int z=0 ; z<m_numTopic ; z++ )
		{
			if( max < m_theta_dz[d][z] )
			{
				max = m_theta_dz[d][z];
				maxZ = z;
			}
		}

		topics.push_back( maxZ+1 );
	}

	return topics;
}

std::vector< std::vector<double> > CGibbsMLDA::GetPw_dw( int m )
{
	std::vector< std::vector<double> > Pw;

	Pw.resize( m_numDoc );
	for(int d=0 ; d<m_numDoc ; d++ )
	{
		Pw[d].resize( m_dataDim[m] );

		for(int w=0 ; w<m_dataDim[m] ; w++ )
		{
			Pw[d][w] = 0;
			for(int z=0 ; z<m_numTopic ; z++ )
			{
				Pw[d][w] += m_theta_dz[d][z] * m_phi_mzw[m][z][w];
			}
		}
	}

	return Pw;
}

std::vector< std::vector<double> >  CGibbsMLDA::GetPz_dz()
{
	vector< vector<double> > P_dz;

	P_dz.resize( m_numDoc );

	for(int d=0 ; d<m_numDoc ; d++ )
	{
		for(int z=0 ; z<m_numTopic ; z++ )
		{
			P_dz[d].push_back( m_theta_dz[d][z] );
		}
	}

	return P_dz;
}


void CGibbsMLDA::SaveModel( const char *directory )
{
	char filename[256];
	FILE *fp;
	vector<int> topics;
	vector<vector<double> > Pw_dw;

	//CreateDirectory( directory , NULL );

	topics = GetClassificationResult();
	strcpy( filename , directory );
	strcat( filename , "ClassResult.txt" );
	SaveArray( topics , topics.size() , filename );

	for(int m=0 ; m<m_numModal ; m++ )
	{
		sprintf( filename , "%sphi%03d.txt" , directory , m );
		SaveMatrix( m_phi_mzw[m] , m_dataDim[m] , m_numTopic , filename );

		Pw_dw = GetPw_dw(m);
		sprintf( filename , "%sPw%03d.txt" , directory , m );
		SaveMatrix( Pw_dw , m_dataDim[m] , m_numDoc , filename );
	}

	strcpy( filename , directory );
	strcat( filename , "theta.txt" );
	SaveMatrix( m_theta_dz , m_numTopic , m_numDoc , filename );


	picojson::object jobj;
	jobj.insert(make_pair("num_iteration",	(double)m_numIteration));
	jobj.insert(make_pair("num_modalities", (double)m_numModal));
	jobj.insert(make_pair("num_topics", (double)m_numTopic));
	jobj.insert(make_pair("total_liklihood", m_lik));
	jobj.insert(make_pair("modality_liklihoods", picojson::array(m_modalLik, m_modalLik+m_numModal)));

	strcpy(filename, directory);
	strcat(filename, "settings.json");
	fp = fopen(filename, "w");
	fputs(picojson::value(jobj).serialize(true).c_str(), fp);
	fclose(fp);

	// モデルを保存
	for(int m=0 ; m<m_numModal ; m++ )
	{
		sprintf( filename , "%sNmwz%03d.txt" , directory , m );
		SaveMatrix( m_N_mwz[m] , m_numTopic , m_dataDim[m] , filename );
	}

	sprintf( filename , "%sNmz.txt" , directory );
	SaveMatrix( m_N_mz , m_numTopic , m_numModal , filename );
}


bool CGibbsMLDA::LoadModel( const char *directory )
{
	char filename[256];
	int x,y;

	// モデルを読み込み
	for(int m=0 ; m<m_numModal ; m++ )
	{
		sprintf( filename , "%sNmwz%03d.txt" , directory , m );
		Free( m_N_mwz[m] );
		m_N_mwz[m] = LoadMatrix<int>( x , y , filename );

		if( !m_N_mwz[m] ) 
		{
			strcat( filename , " が不正です！" );
			MessageBox( filename );
			return false;
		}

		if( x != m_numTopic || y != m_dataDim[m] )
		{
			sprintf( filename , "%sのデータの書式が不正です。\r\nカテゴリ数=%d, モダリティ%dの次元=%d" , filename,  x , m , y );
			MessageBox( filename );
			return false;
		}
	}

	sprintf( filename , "%sNmz.txt" , directory );
	Free( m_N_mz );
	m_N_mz = LoadMatrix<int>( x , y , filename );
	if( !m_N_mz ) 
	{
		strcat( filename , " が不正です！" );
		MessageBox( filename );
		return false;
	}

	if( x != m_numTopic || y != m_numModal )
	{
		sprintf( filename , "%sのデータの書式が不正です。\r\nカテゴリ数=%d, モダリティ数=%d" , filename , x , y );
		MessageBox( filename );
		return false;
	}

	return true;
}


// exp(e1) + exp(e2) = exp( A )
// のAを返す
long double CGibbsMLDA::SumExp( long double e1 , long double e2 )
{
	if( e1 < e2 )
	{
		long double tmp = e1;
		e1 = e2;
		e2 = tmp;
	}

	//printf( "%g %g %g\n" , e1 , e2 , log( 1 + exp(e2-e1) ) );

	return e1 + log( 1 + exp(e2-e1) );
}