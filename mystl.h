#pragma once 
/*******************************************************************

MySTL.h		developed by naka_t	2010.04.15

STL関係の関数とか

　＊プログラム作成					naka_t	2010.04.15

 *******************************************************************/
#include <vector>
#include <stdio.h>
#include <map>


// マクロ
#define ALL(a)  (a).begin(),(a).end()
#define RALL(a) (a).rbegin(), (a).rend()
#define EXIST(s,e) (std::find(ALL(s), (e))!=(s).end())
#define SORT(c) sort((c).begin(),(c).end())

#define FOR(i,a,b) for(int i=(a);i<(b);++i)
#define REP(i,n)  FOR(i,0,n)


