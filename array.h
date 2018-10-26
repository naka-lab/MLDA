#pragma once
/*******************************************************************
Array.cpp		developed by naka_t 2009.12.02


多次元配列を扱うための関数

＊オーバーロードで作成していたものを
＊テンプレート関数にしてみました							naka_t	2008.07.08
＊fprintfVar,fscanfVarをMyIO.hへ移行
　SaveVariableとLoadVariableの変数上書き保存機能追加		naka_t	2009.12.02
 ＊行列保存・読み込み関数（Load/SaveVariableMatrix)追加		naka_t	2011.01.31
 ＊ソースコードの整理										naka_t	2011.02.01
 ＊配列のスライス関数を追加									naka_t  2012.07.09

 *******************************************************************/
#include <string>
#include <vector>
#include <map>
#include <float.h>
#include "myio.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


#define MessageBox(mess) { printf( "%s\n", mess ); exit(0); }
#define MessageBoxA(mess) { printf( "%s\n", mess ); exit(0); }
#define AfxMessageBox(mess) { printf( "%s\n", mess ); exit(0); }

// 行数をカウント
static int CountLine( FILE *fp )
{
	int num = 0;
	int c;

	while(1)
	{
		c = fgetc(fp);

		if( c == '\0' || c == EOF ) break;
		else if( c == '\n' ) num++;
	}

	rewind(fp);

	return num;
}

// 行内のタブの数を数える
static int CountCols( FILE *fp )
{
	int num = 0;
	int c , old;

	while(1)
	{
		c = fgetc(fp);

		if( c == '\n' || c == EOF ) break;
		else if( c == '\t' ) num++;

		old = c;
	}

	//最後の列にタブが入っていない可能性もあるので
	if( old != '\t' && c == '\n' ) num++;

	rewind(fp);

	return num;
}

// 二次元配列確保
template<typename T> T** AllocMatrix( int ysiz , int xsiz)
{
	//配列の実態は1次元で確保していることに注意！
	T **mptr = NULL;
	T *data = NULL;	//配列の実態

	try
	{
		mptr = new T*[ysiz];
		data = new T[ysiz*xsiz];

		for (int i=0;i<ysiz;i++)
		{
			mptr[i] = data + i*xsiz;
		}

		return mptr;
	}
	catch( std::bad_alloc &ba )
	{
		std::string mess = "AllocMatrix : メモリの確保に失敗！\r\n";
		mess += ba.what();
		AfxMessageBox(mess.c_str());
		return NULL;
	}
}

// 三次元配列確保
template<typename T> T*** AllocMatrix(int zsize,  int ysiz, int xsiz)
{
	//配列の実態は1次元で確保していることに注意！
	T ***mptr;	//points to allocated matrix
	T **zyptr;	//z-yのポインタ
	T *data;	//データの実態
	int i,j;	//counter

	data = new T[xsiz*ysiz*zsize];
	zyptr = new T*[zsize*ysiz];
	mptr = new T**[zsize];

	for( i=0 ; i<zsize ; i++)
	{
		mptr[i] = zyptr + i*ysiz;
		for(j=0 ; j<ysiz ; j++)
		{
			mptr[i][j] = data+ i*xsiz*ysiz + j*xsiz;
		}
	}

	return mptr;
}

// 一次元配列開放
template <typename T> void Free(T *&mptr, T* newPtr=NULL)
{
	delete [] mptr;
	mptr = newPtr;
}

// 二次元配列開放
template <typename T> void Free(T **&mptr, T** newPtr=NULL)
{
	if( mptr )
	{
		delete [] mptr[0];
		delete [] mptr;
	}
	mptr = newPtr;
}

// 三次元配列開放
template <typename T> void Free(T ***&mptr, T*** newPtr=NULL)
{

	if( mptr )
	{
		delete [] mptr[0][0];
		delete [] mptr[0];
		delete [] mptr;
	}
	mptr = newPtr;
}

// 配列初期化
template <typename T> void Zeros( T mptr , int xsize )
{
	memset( mptr , 0 , sizeof( mptr[0] )*xsize );
};

// 二次元配列初期化
template <typename T> void Zeros( T mptr , int ysize , int xsize )
{
	memset( mptr[0] , 0 , sizeof( mptr[0][0] )*ysize*xsize );
};

// 三次元配列初期化
template <typename T> void Zeros( T mptr , int zsize , int ysize , int xsize )
{
	memset( mptr[0][0] , 0 , sizeof( mptr[0][0][0] )*zsize*ysize*xsize );
};

// 配列の保存
template <typename T> void SaveArray( T array, int size, const char *filename)
{
	int n;
	FILE *fp;

	fp = fopen( filename , "w" );
	for( n=0 ; n<size ; n++)
	{
		fprintfVar( array[n] , fp );
		fprintf( fp , "\n" );
	}
	fclose( fp );
};

// 二次元配列の保存
template <typename T> void SaveMatrix( T array , int xsize, int ysize, const char *filename , const char *mode = "w" )
{
	FILE *fp = fopen( filename , mode );
	SaveMatrix( array , xsize , ysize , fp );
	fclose( fp );
};

// 二次元配列の保存
template <typename T> void SaveMatrix( T array , int xsize, int ysize, FILE *fp )
{
	for(int y=0 ; y<ysize ; y++)
	{
		for(int x=0 ; x<xsize ; x++)
		{
			fprintfVar( array[y][x] , fp );
			fprintf( fp , "	" );
		}
		fprintf(fp , "\n");
	}
};

// 三次元配列の保存
template <typename T> void SaveMatrix( T array , int xsize, int ysize, int zsize, const char *filename , const char *mode = "w" )
{
	FILE *fp = fopen( filename , mode );
	SaveMatrix( array , xsize , ysize , zsize, fp );
	fclose( fp );
};

// 三次元配列の保存
template <typename T> void SaveMatrix( T array , int xsize, int ysize , int zsize , FILE *fp )
{
	fprintf( fp , "%d %d %d\n" , xsize , ysize , zsize );	//サイズを保存
	for(int z=0 ; z<zsize ; z++ )
	{
		fprintf( fp , "\n## %d\n" , z );
		SaveMatrix( array[z] , xsize , ysize , fp );
	}
};


// 名前つきで変数を保存
template <typename T> bool SaveVariable( T value , const char *valname , const char *filename , const char *mode="a" )
{
	FILE *fp;
	std::vector<std::string> vecLines;
	std::string line;
	bool bFind = false;

	if( *mode=='a')	// 追記モードならば現在の内容を一時保存
	{
		fp = fopen( filename , "r" );
		if( fp )	// ファイルがあれば読み込む
		{
			while( fgets( line , fp ) != std::string::npos ) { vecLines.push_back(line); }
			fclose(fp);
		}
	}


	// 書き込み開始
	fp = fopen( filename , "w" );

	if( !fp )
	{
		char mess[1024];
		sprintf( mess , "SaveVariable : %sがオープンできません" , filename );
		AfxMessageBox( mess );
		return false;
	}

	for(int i=0 ; i<vecLines.size() ; i++ )
	{
		char tmpName[256];
		sscanf( vecLines[i].c_str() , "%s" , tmpName );
		if( strcmp(tmpName,valname)==0 )
		{
			// 変数が見つかったら、上書き
			fprintf( fp , "%s		" , valname );
			fprintfVar( value , fp );
			fprintf( fp , "\n" );
			bFind = true;
		}
		else
		{
			// 見つからなかったら、そのまま保存
			fprintf( fp , "%s\n" , vecLines[i].c_str() );
		}
	}

	if( !bFind )
	{
		fprintf( fp , "%s		" , valname );
		fprintfVar( value , fp );
		fprintf( fp , "\n" );
	}

	fclose(fp);
	return true;
}

// 名前つきで配列を保存
template <typename T> bool SaveVariableArray( T value , int num , const char *valname , const char *filename , const char *mode="a" )
{
	FILE *fp;
	std::vector<std::string> vecLines;
	std::string line;
	bool bFind = false;

	if( *mode=='a')	// 追記モードならば現在の内容を一時保存
	{
		fp = fopen( filename , "r" );
		if( fp )	// ファイルがあれば読み込む
		{
			while( fgets( line , fp ) != std::string::npos ) { vecLines.push_back(line); }
			fclose(fp);
		}
	}


	// 書き込み開始
	fp = fopen( filename , "w" );

	if( !fp )
	{
		char mess[1024];
		sprintf( mess , "SaveVariable : %sがオープンできません" , filename );
		AfxMessageBox( mess );
		return false;
	}

	for(int i=0 ; i<vecLines.size() ; i++ )
	{
		char tmpName[256];
		sscanf( vecLines[i].c_str() , "%s" , tmpName );
		if( strcmp(tmpName,valname)==0 )
		{
			// 変数が見つかったら、上書き
			fprintf( fp , "%s	" , valname );
			for(int j=0 ; j<num ; j++ )
			{
				fprintfVar( value[j] , fp );
				fprintf( fp , "	" );
			}
			fprintf( fp , "\n" );
			bFind = true;
		}
		else
		{
			// 見つからなかったら、そのまま保存
			fprintf( fp , "%s\n" , vecLines[i].c_str() );
		}
	}

	if( !bFind )
	{
		fprintf( fp , "%s	" , valname );
		for(int j=0 ; j<num ; j++ )
		{
			fprintfVar( value[j] , fp );
			fprintf( fp , "	" );
		}
		fprintf( fp , "\n" );
	}

	fclose(fp);
	return true;
}

// 名前つきで二次元配列を保存
template <typename T> bool SaveVariableMatrix( T value , int ysize , int xsize , const char *valname , const char *filename , const char *mode="a" )
{
	FILE *fp;
	std::vector<std::string> vecLines;
	std::string line;
	bool bFind = false;

	if( *mode=='a')	// 追記モードならば現在の内容を一時保存
	{
		fp = fopen( filename , "r" );
		if( fp )	// ファイルがあれば読み込む
		{
			while( fgets( line , fp ) != std::string::npos ) { vecLines.push_back(line); }
			fclose(fp);
		}
	}


	// 書き込み開始
	fp = fopen( filename , "w" );

	if( !fp )
	{
		char mess[1024];
		sprintf( mess , "SaveVariable : %sがオープンできません" , filename );
		AfxMessageBox( mess );
		return false;
	}

	for(int i=0 ; i<vecLines.size() ; i++ )
	{
		char tmpName[256];
		sscanf( vecLines[i].c_str() , "%s" , tmpName );
		if( strcmp(tmpName,valname)==0 )
		{
			fprintf( fp , "%s			%d	%d\n" , valname , ysize , xsize );

			// 変数が見つかったら、上書き
			for(int y=0 ; y<ysize ; y++ )
			{
				fprintf( fp , "*	" );
				for(int x=0 ; x<xsize ; x++ )
				{
					fprintfVar( value[y][x] , fp );
					fprintf( fp , "	" );
				}
				fprintf( fp , "\n" );
			}

			// "*"が行列の続きを表すので読み飛ばす
			for( i = i+1 ; i<vecLines.size() ; i++ )
			{
				sscanf( vecLines[i].c_str() , "%s" , tmpName );
				if( strcmp(tmpName,"*")!=0 ) 
				{
					i--;
					break;
				}
			}
			bFind = true;
		}
		else
		{
			// 見つからなかったら、そのまま保存
			fprintf( fp , "%s\n" , vecLines[i].c_str() );
		}
	}

	if( !bFind )
	{
		fprintf( fp , "%s			%d	%d\n" , valname , ysize , xsize );
		for(int y=0 ; y<ysize ; y++ )
		{
			fprintf( fp , "*	" );
			for(int x=0 ; x<xsize ; x++ )
			{
				fprintfVar( value[y][x] , fp );
				fprintf( fp , "	" );
			}
			fprintf( fp , "\n" );
		}

	}

	fclose(fp);
	return true;
}

static bool SaveLine( const char *filename , const char* Format, ... )
{
	va_list args;
	FILE *fp = fopen( filename , "a" );

	if( !fp )
	{
		char mess[1024];
		sprintf( mess , "SaveVariable : %sがオープンできません" , filename );
		AfxMessageBox( mess );
		return false;
	}

	va_start(args, Format );
	vfprintf( fp , Format , args );
	va_end(args);

	fclose( fp );

	return true;
}

// 配列の読み込み
template <typename T> T* LoadArray( int &size, const char * filename)
{
	FILE *fp = NULL;
	T *ptr = NULL;
	int n;

	try
	{
		fp = fopen( filename , "r");

		if( fp==NULL ) throw "ファイルが開けません";

		size = CountLine( fp );
		ptr = new T[size];

		if( !fp ) throw "new:失敗";

		for(n=0 ; n<size ; n++)
		{
			if( fscanfVar( &ptr[n] ,  fp ) != 1 ) throw "データの書式が不正です";
			fscanf( fp , "\n" );
		}
		fclose( fp );

		return ptr;
	}
	catch( const char *errStr )
	{
		char mess[1024];

		sprintf( mess, "%sの%s" , filename , errStr );

		AfxMessageBox( mess );

		if( !fp ) fclose( fp );
		if( !ptr ) free(ptr);

		return NULL;
	}
}

// 配列の読み込み
template <typename T> std::vector<T> LoadArray( const char * filename)
{
	FILE *fp = NULL;
	std::vector<T> v;
	int n;

	try
	{
		fp = fopen( filename , "r");

		if( fp==NULL ) throw "ファイルが開けません";

		int size = CountLine( fp );
		v.resize( size );

		if( !fp ) throw "new:失敗";

		for(n=0 ; n<size ; n++)
		{
			if( fscanfVar( &v[n] ,  fp ) != 1 ) throw "データの書式が不正です";
			fscanf( fp , "\n" );
		}
		fclose( fp );

		return v;
	}
	catch( const char *errStr )
	{
		char mess[1024];

		sprintf( mess, "%sの%s" , filename , errStr );

		AfxMessageBox( mess );

		if( fp ) fclose( fp );

		return v;
	}
}

static std::vector<std::string> split(std::string str, std::string delim)
{
	std::vector<std::string> vecRes;

	int cutAt;
	while( (cutAt = str.find_first_of(delim)) != str.npos )
	{
		if(cutAt > 0)
		{
			vecRes.push_back(str.substr(0, cutAt));
		}
		str = str.substr(cutAt + 1);
	}
	if(str.length() > 0)
	{
		vecRes.push_back(str);
	}
}


// 二次元配列の読み込み
template <typename T> std::vector<std::vector<T> > LoadMatrix( const char *filename )
{
	FILE *fp = NULL;
	T data;
	std::string strLine;
	std::vector<std::string> vecStrLine;
	std::vector<T> vecLine;
	std::vector<std::vector<T> > vecData;
	char mess[1024];

	fp = fopen( filename , "r");

	if( fp==NULL )
	{
		sprintf( mess, "%s がありません．" , filename );
		AfxMessageBox( mess );
	}


	while( fgets( strLine , fp ) != std::string::npos )
	{
		vecStrLine = split( strLine , "\t" );
		vecLine.clear();

		for(int i=0 ; i<vecStrLine.size() ;  i++ )
		{
			if( sscanfVar( &data , vecStrLine[i].c_str() ) != 1 )
			{
				sprintf( mess, "%s の書式が不正です．" , filename );
				AfxMessageBox( mess );
				return std::vector<std::vector<T> >();
			} 
			vecLine.push_back( data );
		}

		vecData.push_back( vecLine );
	}

	return vecData;
}

// 二次元配列の読み込み
template <typename T> T **LoadMatrix(int &xsize, int &ysize , const char *filename )
{
	FILE *fp = NULL;
	T **array = NULL;
	char mess[1024];

	fp = fopen( filename , "r");

	if( fp==NULL )
	{
		sprintf( mess, "%s がありません．" , filename );
		AfxMessageBox( mess );
		return NULL;
	}

	//行数取得
	ysize = CountLine(fp);
	xsize = CountCols(fp);

	array = (T **)AllocMatrix<T>( ysize , xsize );

	if( !LoadMatrix( array , xsize, ysize , fp ) )
	{
		if( !fp ) fclose( fp );
		if( !array ) Free( array );

		sprintf( mess, "%s の書式が不正です．" , filename );
		AfxMessageBox( mess );

		return NULL;
	}

	fclose(fp);

	return array;
}

// 三次元配列の読み込み
template <typename T> T ***LoadMatrix(int &xsize, int &ysize , int &zsize , const char *filename )
{
	FILE *fp = NULL;
	T data;
	T ***array = NULL;
	char line[1024];
	int cnt;

	fp = fopen( filename , "r");

	try
	{
		if( fp==NULL ) throw "ファイルがありません";

		//行数取得
		if( fscanf( fp , "%d %d %d\n" , &xsize , &ysize , &zsize ) != 3 ) throw "書式エラー0";
		array = AllocMatrix<T>( zsize , ysize , xsize );

		for(int z=0 ; z<zsize ; z++ )
		{
			int cnt;
			if( fscanf( fp , "\n## %d\n" , &cnt )!=1 || cnt!=z ) throw "書式エラー1";
			if( !LoadMatrix( array[z] , xsize, ysize , fp ) )  throw "書式エラー2";
		}

		if( !fp ) fclose( fp );
		if( !array ) Free( array );

		return array;
	}
	catch(const char *str)
	{
		char mess[1024];
		sprintf( mess, "%s %s" , filename , str );
		AfxMessageBox( mess );

		if( !fp ) fclose( fp );
		if( !array ) Free( array );

		return NULL;
	}

}

// 二次元配列の読み込み
template <typename T> bool LoadMatrix( T **array , int xsize, int ysize , FILE *fp )
{
	T data;

	for(int m=0 ; m<ysize ; m++)
	{
		for(int n=0 ; n<xsize ; n++)
		{
			if( fscanfVar( &data , fp ) != 1 )
			{
				return false;
			}
			fscanf( fp , "	" );

			array[m][n]=data;
		}

		fscanf( fp , "\n");
	}
	return true;
}


// 名前付で保存された変数をロード
template <typename T> T LoadVariable( const char *valname , const char *filename)
{
	//同じ変数があれば上書き
	FILE *fp = fopen( filename , "r" );
	char namebuff[1024];
	char mess[1024];
	bool bFind = false;
	T var;

	if( !fp )
	{
		sprintf( mess , "LoadVariable : %sを開けません。" , filename);
		AfxMessageBox( mess );

		return 0;
	}

	while( fscanf( fp , "%s		" , namebuff ) != EOF )
	{
		if( strcmp( valname , namebuff )==0 )
		{
			if( fscanfVar( &var , fp ) == 1 )
			{
				bFind = true;
				break;
			}
		}
	}


	if( !bFind )
	{
		sprintf( mess , "LoadVariable : %sの変数%sの読み込みに失敗。" , filename , valname );
		AfxMessageBox( mess );
	}

	fclose(fp);

	return var;
}


// 名前付で保存された変数をロード
template <typename T> bool LoadVariable( T &var , const char *valname , const char *filename)
{
	//同じ変数があれば上書き
	FILE *fp = fopen( filename , "r" );
	char namebuff[1024];
	char mess[1024];
	bool bFind = false;

	if( !fp )
	{
		return false;
	}

	while( fscanf( fp , "%s		" , namebuff ) != EOF )
	{
		if( strcmp( valname , namebuff )==0 )
		{
			if( fscanfVar( &var , fp ) == 1 )
			{
				bFind = true;
				break;
			}
		}
	}


	if( !bFind )
	{
		return false;
	}

	fclose(fp);
	return true;
}


// 一次元配列のデータを読み込む
template <typename T> bool LoadVariable( T pVar , int count , const char *valname , const char *filename)
{
	FILE *fp = fopen( filename , "r" );
	char namebuff[1024];
	char mess[1024];
	bool bFind = false;


	if( !fp )
	{
		sprintf( mess , "LoadVariable : %sを開けません。" , filename);
		AfxMessageBox( mess );

		return 0;
	}

	while( fscanf( fp , "%s		" , namebuff ) != EOF )
	{
		if( strcmp( valname , namebuff )==0 )
		{
			bFind = true;
			break;
		}
	}

	if( !bFind )
	{
		sprintf( mess , "LoadVariable : %sの変数%sの読み込みに失敗。0" , filename , valname );
		AfxMessageBox( mess );
		fclose(fp);
		return false;
	}
	else
	{
		for(int i=0 ; i<count ; i++)
		{
			if( fscanfVar( &pVar[i] , fp ) != 1 )
			{
				sprintf( mess , "LoadVariable : [%s]の変数[%s]の読み込みに失敗。1" , filename , valname );
				AfxMessageBox( mess );
				fclose(fp);
				return false;
			}
		}
	}

	fclose(fp);
	return true;
}


// 二次元配列のデータを読み込む
template <typename T> bool LoadVariable( T pVar , int ysize, int xsize, const char *valname , const char *filename)
{
	FILE *fp = fopen( filename , "r" );
	char namebuff[1024];
	char mess[1024];
	bool bFind = false;


	if( !fp )
	{
		sprintf( mess , "LoadVariable : %sを開けません。" , filename);
		AfxMessageBox( mess );

		return 0;
	}

	while( fscanf( fp , "%s		" , namebuff ) != EOF )
	{
		if( strcmp( valname , namebuff )==0 )
		{
			bFind = true;
			break;
		}
	}

	if( !bFind )
	{
		sprintf( mess , "LoadVariable : %sの変数%sの読み込みに失敗。0" , filename , valname );
		AfxMessageBox( mess );
		fclose(fp);
		return false;
	}
	else
	{
		for(int y=0 ; y<ysize ; y++)
		{
			fscanf( fp , "%s		" , namebuff );
			if( strcmp( "*" , namebuff )!=0 )
			{
				sprintf( mess , "LoadVariable : [%s]の変数[%s]の読み込みに失敗。行数が一致しましせん。" , filename , valname );
				AfxMessageBox( mess );
				fclose(fp);
				return false;
			}
			for(int x=0 ; x<xsize ; x++ )
			{
				if( fscanfVar( &pVar[y][x] , fp ) != 1 )
				{
					sprintf( mess , "LoadVariable : [%s]の変数[%s]の読み込みに失敗。1" , filename , valname );
					AfxMessageBox( mess );
					fclose(fp);
					return false;
				}
			}
		}
	}

	fclose(fp);
	return true;
}


// 二次元配列のX方向の合計を計算
template <typename T>bool SumX( T *src , T dst , int ysize , int xsize )
{
	if( !src || !dst ) return false;
	for(int y=0 ; y<ysize ; y++ )
	{
		dst[y] = SumX( src[y] , xsize );
	}
	return true;
}

// 一次元配列の合計を計算
template <typename T>double SumX( T data , int xsize )
{
	double sum = 0;

	for(int i=0 ; i<xsize ; i++)
	{
		sum += data[i];
	}

	return sum;
};

// data[][xpos]のy方向の合計を取る
template <typename T>double SumY( T data , int ysize , int xpos )
{
	double sum = 0;

	for(int i=0 ; i<ysize ; i++)
	{
		sum += data[i][xpos];
	}

	return sum;
};

// 配列の正規化
template <typename T1 , typename T2> void Normalize( T1 &src , int dimension , double sum , T2 &dst )
{
	double dataSum = SumX( src , dimension );

	if(dataSum!=0)for(int i=0 ; i<dimension ; i++)
	{
		dst[i] = src[i] * sum / dataSum;
	}
}

// 二次元配列のX方向への正規化
template <typename T1 , typename T2> void NormalizeX( T1 &src , int dimension , int dataNum , double sum , T2 &dst)
{
	for(int i=0 ; i<dataNum ; i++)
	{
		Normalize( src[i] , dimension , sum , dst[i] );
	}
}

// 二次元配列のX方向への正規化
template <typename T1 , typename T2> void NormalizeY( T1 &src , int xsize , int ysize , double sum , T2 &dst)
{
	double datasum;

	for(int x=0 ; x<xsize ; x++ )
	{
		datasum = SumY( src , ysize , x );

		if(datasum!=0)for(int y=0 ; y<ysize ; y++ ) dst[y][x] = src[y][x] / datasum * sum;
	}
}

//二次元配列のコピー
template <typename T1 , typename T2> void Copy( T1 &src , T2 &dst , int ysize , int xsize )
{
	for(int y=0 ; y<ysize ; y++)
	{
		for(int x=0 ; x<xsize ; x++)
		{
			dst[y][x] = src[y][x];
		}
	}
};
