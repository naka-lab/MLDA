// GibbsLDATest.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//
#define _CRT_SECURE_NO_WARNINGS 1
#include "array.h"
#include "GibbsMLDA.h"
#include <vector>
#include <algorithm>
#include "mystl.h"
#include "dir.h"
#include "config_parser.h"

using namespace std;


// カテゴリ分類の精度を計算
double CalcCategorizationAcc( std::vector<int> result , std::vector<int> correct , std::vector<int> *confusionMat = NULL )
{
	// 0からスタートするように変更
	while( !EXIST(result , 0 ) )
	{
		for(int i=0 ; i<result.size() ; i++ ) result[i] -= 1;
	}

	while( !EXIST(correct , 0 ) )
	{
		for(int i=0 ; i<correct.size() ; i++ ) correct[i] -= 1;
	}

	int catMax = *std::max_element( correct.begin() , correct.end() )+1;	// 正しいカテゴリ数
	std::vector<int> orderedResult = result;
	int maxAcc = 0;
	int resMax = std::max( *std::max_element( result.begin() , result.end() )+1 , catMax );


	for(int i=0 ; i<result.size() ; i++ )
	{
		if( result[i] == correct[i] ) maxAcc++;
	}

	int numChange = 0;
	do
	{
		numChange = 0;
		for(int c1=0 ; c1<resMax ; c1++ )
		{
			// c1とc2を入れ替える
			for(int c2=0 ; c2<resMax ; c2++ )
			{
				if( c1!=c2 )
				{
					int acc = 0;

					for(int i=0 ; i<result.size() ; i++ )
					{
						if( result[i] == c1 ) orderedResult[i] = c2;
						else if( result[i] == c2 ) orderedResult[i] = c1;
						else orderedResult[i] = result[i];

						// 精度計算
						if( orderedResult[i] == correct[i] ) acc++;
					}

					if( acc > maxAcc )
					{
						maxAcc = acc;
						result = orderedResult;
						numChange++;
					}
				}
			}
		}
	}while( numChange );


	if( confusionMat ) *confusionMat = result;

	return (double)maxAcc / result.size();
}



std::string MakeConfusionMatrix( std::vector<int> confutionVec, std::vector<int> correct )
{
	// 0からスタートするように変更
	while( !EXIST(correct , 0 ) )
	{
		for(int i=0 ; i<correct.size() ; i++ ) correct[i] -= 1;
	}

	// カテゴリの最大数
	int numCat = std::max( *std::max_element( ALL(confutionVec) ) , *std::max_element( ALL(correct) ) );

	std::string confStr;
	for(int o=0 ; o<confutionVec.size() ; o++ )
	{
		//confStr += _fmt( "%d\t" , correct[o] );
		char tmp[1024];
		sprintf(tmp, "%d\t" , correct[o] );
		confStr +=  tmp;
		for(int c=0 ; c<numCat+1 ; c++ )
		{
			if( confutionVec[o] == c )
			{
				//fprintf( fp , "□" );
				confStr += "□";
			}
			else
			{
				//fprintf( fp , "■" );
				confStr += "■";
			}
		}
		confStr += "\n";
		//fprintf( fp , "\n" );
	}
	return confStr;
}



void Learn(int numObject, int numModal, int numCategories,
		   std::string saveDir, int modaldim[],
		   double **data[], vector<int> correct, int numIter , int numSampling )
{
	CGibbsMLDA lda;
	double maxLik = -DBL_MAX;

	//CreateDirectory( saveDir.c_str() , NULL );
	_mkdir( saveDir.c_str() );

	lda.SetNumSamplingIteration( numSampling );

	for(int i=0 ; i<numIter ; i++ )
	{
		lda.Initilize( numCategories , numObject , numModal , modaldim );
		lda.SetData( data );
		double lik = lda.Learn();

		printf("%d回目...lik = %lf\r" , i , lik );


		if( lik > maxLik )
		{
			printf("\n");
			printf("    -> モデル更新\n");
			maxLik = lik;

			lda.SaveModel( saveDir.c_str() );

			if ( correct.size() )
			{
				std::vector<int> res = lda.GetClassificationResult();
				std::vector<int> confutionMat;
				double acc = CalcCategorizationAcc( res , correct , &confutionMat );

				SaveArray( confutionMat , confutionMat.size() , (saveDir+"ClassResult2.txt").c_str() );

				FILE *fp = fopen( (saveDir+"confutionmat.txt").c_str() , "w" );
				fprintf( fp , "%lf\n" , acc );
				for(int o=0 ; o<numObject ; o++ )
				{
					fprintf( fp , "%d	" , correct[o] );
					for(int c=0 ; c<numCategories ; c++ )
					{
						if( confutionMat[o] == c )
						{
							fprintf( fp , "□" );
						}
						else
						{
							fprintf( fp , "■" );
						}
					}
					fprintf( fp , "\n" );
				}
				fclose(fp);
			}
		}
	}
}




void Recog(int numObject, int numModal, int numCategories,
		   std::string saveDir, std::string loadModel, int modaldim[],
		   double **data[], vector<int> correct, int numIter , int numSampling )
{
	CGibbsMLDA lda;
	double maxLik = -DBL_MAX;

	//CreateDirectory( saveDir.c_str() , NULL );
	_mkdir( saveDir.c_str() );
	lda.SetNumSamplingIteration( numSampling );

	lda.Initilize( numCategories , numObject , numModal , modaldim );
	lda.SetData( data );
	double lik = lda.Recog( loadModel.c_str() );

	printf("lik = %lf\n" , lik );
	lda.SaveModel( saveDir.c_str() );

	if ( correct.size() )
	{
		std::vector<int> res = lda.GetClassificationResult();
		std::vector<int> confutionMat;
		double acc = CalcCategorizationAcc( res , correct , &confutionMat );

		SaveArray( confutionMat , confutionMat.size() , (saveDir+"ClassResult2.txt").c_str() );

		FILE *fp = fopen( (saveDir+"confutionmat.txt").c_str() , "w" );
		fprintf( fp , "%lf\n" , acc );
		for(int o=0 ; o<numObject ; o++ )
		{
			fprintf( fp , "%d	" , correct[o] );
			for(int c=0 ; c<numCategories ; c++ )
			{
				if( confutionMat[o] == c )
				{
					fprintf( fp , "□" );
				}
				else
				{
					fprintf( fp , "■" );
				}
			}
			fprintf( fp , "\n" );
		}
		fclose(fp);
	}
}

//int _tmain(int argc, char* argv[])
int main(int argc, char* argv[])
{
	int numModal;
	int numIter;
	int numSampling;
	int numCategory;
	std::string categoryFileName;
	std::string saveDir;
	std::string loadDir;
	double **data[10] = {NULL};
	int modaldim[10] = {0};
	int numObj;
	vector<int> correctCategory;


	// コマンドライン引数，設定ファイル解析用
	config_parser cp(argc, argv);

	cp.get_and_assert("num_modal", numModal);
	cp.get_and_assert("num_trial", numIter);
	cp.get_and_assert("num_samiter", numSampling);
	cp.get_and_assert("num_cat", numCategory);
	cp.get_and_assert("save_dir", saveDir);

	printf("カテゴリ数		%d\n" , numCategory );
	printf("モダリティ数		%d\n" , numModal );
	printf("保存先			%s\n" , saveDir.c_str() );
	printf("サンプリング回数	%d\n" , numSampling );
	printf("繰り返し回数		%d\n" , numIter );

	if ( saveDir.size()==0 || (*(saveDir.end()-1)!='/' && *(saveDir.end()-1)!='\\' ) ) saveDir.push_back('/');

	if( cp.get("category", categoryFileName ) )
	{
		if ( categoryFileName != "NULL" )
		{
			correctCategory = LoadArray<int>( categoryFileName.c_str() );
			printf("正解カテゴリ		%s\n" , categoryFileName.c_str() );
		}
	}

	for (int i=0 ; i<numModal ; i++ )
	{
		std::string filename;
		char varname[256];
		int weight;

		sprintf( varname , "data%d" , i );
		cp.get_and_assert(varname, filename);

		if ( filename == "NULL" )
		{
			data[i] = NULL;

			sprintf( varname , "dim%d" , i );
			cp.get_and_assert(varname, modaldim[i]);
		}
		else
		{
			int dim;
			data[i] = LoadMatrix<double>( dim , numObj , filename.c_str() );



			sprintf( varname , "dim%d" , i );
			//TRY_LODAVAR( modaldim[i] , varname , argv[2] );
			cp.get_and_assert(varname, modaldim[i]);

			if ( dim != modaldim[i] )
			{
				printf("%sと%sの次元数が一致しません。\n" , filename.c_str() , argv[2] );
				return -1;
			}

			weight = 0;
			sprintf( varname , "weight%d" , i );
			//TRY_LODAVAR( weight , varname , argv[2] );
			if( cp.get(varname, weight) )
			{
				NormalizeX( data[i] , modaldim[i] , numObj , weight , data[i] );
			}

			printf("Data%d			%s / dim:%d / weight:%d\n" , i , filename.c_str() , modaldim[i] , weight );
		}
	}

	printf("物体数			%d\n" , numObj );

	if ( cp.contains("recog") )
	{
		//TRY_LODAVAR( loadDir , "LoadModel" , argv[2] );
		cp.get_and_assert("load_dir", loadDir);
		if ( loadDir.size()==0 || (*(loadDir.end()-1)!='/' && *(loadDir.end()-1)!='\\' ) ) loadDir.push_back('/');
		printf("学習済みモデル		%s\n" , loadDir.c_str() );
		Recog( numObj , numModal , numCategory , saveDir , loadDir, modaldim , data , correctCategory , numIter , numSampling );
	}
	else if(cp.contains("learn"))
	{
		Learn( numObj , numModal , numCategory , saveDir , modaldim , data , correctCategory , numIter , numSampling );
	}

	for(int i=0 ; i<10 ; i++ )
	{
		Free( data[i] );
	}

	return 0;
}
