# Mutimodal Latent Dirichlet Allocation
Tomoaki Nakamura, Takaya Araki, Takayuki Nagai and Naoto Iwahashi, “Grounding of Word Meanings in LDA-Based Multimodal Concepts”, Advanced Robotics, Vol.25, pp. 2189-2206, Apr.2012

## インストール
Windowsでは`msvc/mlda.sln`をVisual Studio 2015 (Release, x86)でコンパイルできることを確認しています．
Mac, Ubuntuではmakeでコンパイルで確認しています．

## 実行方法
### 学習

```
./mlda -learn -config lda_config.json
```

- -learn : 学習モードで実行
- -config file_name：設定ファイルの指定


### 認識
```
./mlda -recog -config lda_config.json
```


- -recog : 認識モードで実行
- -config file_name：設定ファイルの指定

### 設定ファイル

設定ファイルはjson形式で以下のように記述してください．

```
{
    "num_modal": 3,
    "num_samiter": 100,
    "num_cat": 10,
    "data0": "VisionHistogram.txt",
    "data1": "AudioHistogram_new.txt",
    "data2": "TactileHistogram.txt",
    "dim0": 500,
    "dim1": 50,
    "dim2": 15,
    "weight0": 280,
    "weight1": 340,
    "weight2": 160,
    "category": "Category.txt",

    "#######": "以下学習で利用",
    "save_dir": "model",
    "num_trial": 100,

    "#######": "以下認識で利用",
    "load_dir": "model",
}
```

学習で使用する設定値の意味は以下の通りです．

|設定値|意味|
|:----:|:------|
|num_modal     | 入力するモダリティ数 |
|num_smiter    | Gibssサンプリングを繰り返す回数 |
|num_cat       | カテゴリ数 |
|data*         | *番目のデータ（タブ区切りテキスト）のファイル名．データがない場合や，予測させたい場合には`"NULL"`を指定 |
|dim*          | *番目のデータの次元数 |
|weight*       | *番目のデータに対する重み（分類する際の重要度）|
|category      | 正解カテゴリが記述されたテキストのファイル名．これが指定されると精度が自動で計算されます．もしなければ `"NULL"` を指定します． その場合，精度は計算されません．|
|save_dir      | 学習されたモデルの保存先 **（学習時のみに使用）** |
|num_trial     | 初期値を変えてサンプリングを繰り返す回数 **（学習時のみに使用）** |
|load_dir      | 学習済みモデルの格納ディレクトリ **（認識時のみに使用）** |

さらに，これらの設定値は設定ファイルに記述しなくとも，以下のようにコマンドライン引数で渡すこともできます．

```
mlda -learn -config lda_config.json -num_cat 15
```

設定ファイルとコマンドライン引数で異なる値が指定された場合，コマンドライン引数の値が優先されます．

### 出力

学習すると以下のようなファイルが出力されます．

|ファイル名 | 内容｜
|:-------:|:----|
|ClassResult.txt     | 分類結果 |
|ClassResult2.txt    | 正解カテゴリが与えられた場合に，正解に最も近くなるようにカテゴリ番号を入れ替えた分類結果．設定ファイルで `category` が指定された場合のみ出力されます．|
|confutionmat.txt    | 分類の精度と混同行列．設定ファイルで `category` が指定された場合のみ出力されます． |
|Nmwz*.txt           | 学習の結果，*番目のモダリティの特徴wにカテゴリzが割り当てられた回数．|
|Nmz.txt             | 学習の結果，モダリティmにカテゴリzが割り当てられた回数．|
|phi*.txt            | *番目のモダリティにおいて，カテゴリzで特徴wが発生する確率P(w&#124;z) |
|Pw*.txt             | d番目の物体から*番目のモダリティの特徴wが発生する確率P(w&#124;d)．data*に`"NULL"`を指定した場合は，予測値となる．|
|theta.txt           | d番目の物体がカテゴリzに分類される確率P(z&#124;d) |




