/*********************************************************************

MyIO.h     Copyright 2009.12.02 by naka_t


  last modified by naka_t 2009.12.02

  ＊入出力関数						naka_t 2008.04.30
  ＊ファイルからの入出力を追加		naka_t 2009.12.02
  ＊std::string版のfgetsを追加		naka_t 2009.12.02
*********************************************************************/
#pragma once
#include <time.h>
#include <stdio.h>
#include <string>


#ifndef ULLINT
#define ULLINT unsigned long long int
#endif

// 入出力関数の定義
#define IO_FUNC( type , format ) \
	static int fprintfVar( type val , FILE *fp ){ return fprintf( fp , format , val ); }	\
	static int fscanfVar( type *var , FILE *fp ){ return fscanf( fp, format , var ); }		\
	static int sscanfVar( type *var , const char  *str ){ return sscanf( str, format , var ); }

IO_FUNC( int , "%d" )
IO_FUNC( short , "%hd" )
IO_FUNC( ULLINT , "%llu" )
IO_FUNC( double , "%lf" )
IO_FUNC( float , "%f" )
IO_FUNC( bool , "%d" )
IO_FUNC( unsigned int , "%u" )
IO_FUNC( char* , "%s" )

static int fprintfVar( std::string &val , FILE *fp ){ return fprintf( fp , "%s" , val.c_str() ); }
static int fscanfVar( std::string *var , FILE *fp ){
	char c = 0;
	var->clear();
	while ( ( c = fgetc( fp ) ) != EOF && isspace(c) );						// 前方区切り文字削除
	while ( c != EOF && !isspace(c) ){ var->push_back(c); c=fgetc(fp);}		// 文字を読み込む
	ungetc( c , fp );														// 読み取り過ぎた文字を戻す
	return var->length() ? 1 : 0;
}
static int sscanfVar( std::string *var , const char  *str  ){
	var->clear();
	while ( *str != 0 && isspace(*str) ) str++;									// 前方区切り文字削除
	while ( *str != 0 && !isspace(*str) ){ var->push_back(*str); str++;}		// 文字を読み込む
	return var->length() ? 1 : 0;
}

static int fprintfVar( const char *&val , FILE *fp ){ return fprintf( fp , "%s" , val ); }


// std::string版のfgets
// 戻り値：読み込んだ文字数
//		   ファイルの終端の際は、string::nposを返します
static size_t fgets( std::string &buff , FILE *fp ){

	char c;
	buff.clear();
	while ( ( c = fgetc( fp ) ) != EOF && c != '\n' )
		buff.push_back( c );

	if( c==EOF && buff.size()==0 ) return std::string::npos;

	return buff.size();
}