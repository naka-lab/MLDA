# Mutimodal Latent Dirichlet Allocation
Tomoaki Nakamura, Takaya Araki, Takayuki Nagai and Naoto Iwahashi, “Grounding of Word Meanings in LDA-Based Multimodal Concepts”, Advanced Robotics, Vol.25, pp. 2189-2206, Apr.2012

## インストール
Windowsでは`msvc/mlda.sln`をVisual Studio 2015 (Release, x86)でコンパイルを確認しています．
Mac, Ubuntuではmakeでコンパイルできます．

## 実行方法
### 学習

```
./mlda -learn -config lda_config.json
```

- -learn : 学習するためのオプション
- -config file_name：設定ファイルを読み込み


### 認識
```
./mlda -recog -config lda_config.json
```


- -recog : 学習するためのオプション
-- config file_name：設定ファイルを読み込み

### 設定ファイル

設定ファイルのjsonで以下のように記述してください．

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
|data*         | *番目のデータ（タブ区切りテキスト）のファイル名 |
|dim*          | *番目のデータの次元数 |
|weight*       | *番目のデータに対する重み（分類する際の重要度）|
|category      | 正解カテゴリが記述されたテキストのファイル名．もしなければ `NULL` を指定 |
|save_dir      | 学習されたモデルの保存先（学習時のみに使用） |
|num_trial     | 初期値を変えてサンプリングを繰り返す回数（学習時のみに使用） |
|load_dir      | 学習済みモデルの格納ディレクトリ（認識時のみに使用） |

さらに，これらの設定値は設定ファイルに記述しなくとも，以下のようにコマンドライン引数で渡すこともできます．

```
mlda -learn -config lda_config.json -num_cat 15
```
この場合，コマンドライン引数で与えたパラメータが優先されます．

### 出力

学習すると以下のようなファイルが出力されます．

|ファイル名 | 内容｜
|:-------:|:----|
|ClassResult.txt     | 分類結果 |
|ClassResult2.txt    | 正解カテゴリが与えられた場合に，正解に最も近くなるように数値を入れ替えた分類結果．|
|confutionmat.txt    | 分類の精度と混同行列 |
|Nmwz*.txt           | 学習の結果，*番目のモダリティの特徴wにカテゴリzが割り当てられた回数．|
|Nmz.txt             | 学習の結果，モダリティmにカテゴリzが割り当てられた回数．|
|phi*.txt            | モダリティ*においてカテゴリzで特徴wが発生する確率P(w&#124;z) |
|Pw*.txt             | d番目の物体から*番目のモダリティの特徴wが発生する確率P(w&#124;d) |
|theta.txt           | d番目の物体がカテゴリzに分類される確率P(z&#124;d) |




