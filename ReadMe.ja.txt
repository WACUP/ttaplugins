ttaplugins-winamp v3.72
=====================================================

*このプラグインは
本家(http://www.true-audio.com/)配布のwinamp用プラグインver3.5に
taglib(https://github.com/taglib/taglib)をベースとした
メディアライブラリ読み込み対応を付け加え、ID3v2周りを改変したものです。
またアルバムアートなどメディアライブラリのメタデータ読み込み機能を付加しました。
新たにttaファイルへのエンコードプラグインも同梱しています。
Winamp Ver.2.9以降に対応しており，Winamp Ver.5.9.1にて動作確認しています。


*インストール方法
 WinampのフォルダにあるPluginsフォルダにin_tta.dllとenc_tta.dllをコピーしてください。

*更新履歴
2025-10-28 3.72       taglibを最新master(based on 2025/10/25 20:00:24 Commit hash:49be371caab517a6e2c35a6038b8eeb12c9b82ae)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2022(バージョン 17.14.18)に変更(in_tta.dll, enc_tta.dll)
2025-06-18 3.71       taglibを最新master(based on 2025/06/14 15:53:30 Commit hash:6563ceaafadf6b2f1997e5582a6bd57cc7b80a57)ベースに変更(in_tta.dll)
                      コードの見直し(in_tta.dll, enc_tta.dll)
                      コンパイラをVisual Studio 2022(バージョン 17.14.6)に変更(in_tta.dll, enc_tta.dll)
2025-05-18 3.70       taglibを最新master(based on 2025/05/17 00:01:23 Commit hash:3ccc390155a30532d25301839c0b8b134681d7a5)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2022(バージョン 17.14.0)に変更(in_tta.dll, enc_tta.dll)
2024-10-13 3.69       taglibを最新master(based on 2024/08/24 13:40:41 Commit hash:e3de03501ff66221d1f1f971022b248d5b38ba06)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2022(バージョン 17.11.5)に変更(in_tta.dll, enc_tta.dll)
                      AlbumArt.cpp中のTTA_AlbumArtProvider::IsMine() 関数を修正(in_tta.dll)
2023-07-04 3.68       taglibを最新master(based on 2023/07/01 14:43:27 Commit hash:c840222a391439285478820b4477d5fa6b78d63d)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2022(バージョン 17.6.4)に変更(in_tta.dll, enc_tta.dll)
2023-06-03 3.67       taglibを最新master(based on 2023/05/26 20:12:19 Commit hash:39e712796f2dd7336188ae690bc3837049f29aac)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2022(バージョン 17.6.2)に変更(in_tta.dll, enc_tta.dll)
2023-02-23 3.66       taglibを最新master(based on 2022/12/21 22:14:35 Commit hash:a31356e330674640a07bef7d71d08242cae8e9bf)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2022(バージョン 17.5.0)に変更(in_tta.dll, enc_tta.dll)
2022-05-17 3.65       taglibを最新master(based on 2022/03/16  1:52:44 Commit hash:0470c2894d07523af11f1ac4e2fa7ce85ced26fe)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2022(バージョン 17.2.0)に変更(in_tta.dll, enc_tta.dll)
2022-02-24 3.64       taglibを最新master(based on 2022/02/10  4:07:13 Commit hash:8ab618da186f8890cbb8b20f708ddd1f985c6d6b)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2022(バージョン 17.0.6)に変更(in_tta.dll, enc_tta.dll)
2021-12-20 3.63       コンパイラをVisual Studio 2022(バージョン 17.0.4)に変更(in_tta.dll, enc_tta.dll)
2021-08-18 3.62       taglibを最新master(based on 2021/07/10  4:47:36 Commit hash:f58161511050ac6c060d28e75dcd5c5a3af7279c)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2019(バージョン 16.11.1)に変更(in_tta.dll, enc_tta.dll)
2021-05-09 3.61       taglibを最新master(based on 2021/05/02 19:14:27 Commit hash:4971f8fb032f0cde8c2437cea44b6217b0f37269)ベースに変更(in_tta.dll)
2021-04-29 3.60       コンパイラをVisual Studio 2019(バージョン 16.9.4)に変更(in_tta.dll, enc_tta.dll)
2021-04-16 3.59       taglibを最新master(based on 2021/03/30 03:21:59 Commit hash:1644c0dd87f40a2492bde4011fe72257fc72ee0e)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2019(バージョン 16.9.3)に変更(in_tta.dll, enc_tta.dll)
2021-03-16 3.58       taglibを最新master(based on 2021/02/17 16:20:05 Commit hash:b5cd4c40e25c8026b42c4e71b4df63cbeb7cfe6a)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2019(バージョン 16.9.1)に変更(in_tta.dll, enc_tta.dll)
                      NSIS スクリプトをUnicode targetに変更
2021-03-01 3.57       taglibを最新master(based on 2021/02/16 21:00:49 Commit hash:340ec9932a7c782a2f876c0efea233a43502c88e)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2019(バージョン 16.8.6)に変更(in_tta.dll, enc_tta.dll)
2019-06-27 3.55       taglibを最新master(based on 2019/05/31 20:21:16 Commit hash:ba7adc2bc261ed634c2a964185bcffb9365ad2f4)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2019(バージョン 16.1.4)に変更(in_tta.dll, enc_tta.dll)
2019-02-01 3.54       taglibを最新master(based on 2018/10/28 22:43:45 Commit hash:5cb589a5b82c13ba8f0542e5e79629da7645cb3c)ベースに変更(in_tta.dll)
                      インストーラーを NSIS verion 3.04 ベースに変更し，インストールスクリプトをプロキシ内からでも VC2017 redistributable をダウンロード可能なように変更
                      コンパイラをVisual Studio 2017(バージョン 15.9.6)に変更(in_tta.dll, enc_tta.dll)
2018-01-01 3.53       taglibを最新master(based on 2017/11/20 8:03:52 Commit hash:a80093167f93f4147ec153693d871780874f9747)ベースに変更(in_tta.dll)
2017-12-31 3.52       taglibを最新master(based on 2017/10/01 0:15:41 commit Hash:cb9f07d9dcd791b63e622da43f7b232adaec0a9a)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2017(バージョン 15.5.2)に変更(in_tta.dll, enc_tta.dll)
                      タグデータをセーブ時，再読み込みするように変更(in_tta.dll)
2017-08-30 3.51       taglibを最新master(based on 2017/06/13 17:22:00 commit Hash:46483948413c1330c036240ed89429a1c6905843)ベースに変更(in_tta.dll)
                      コンパイラをVisual Studio 2017(バージョン 15.3)に変更(in_tta.dll, enc_tta.dll)
2017-06-29 3.50       taglibを最新master(based on 2017/06/09 17:52:56 commit Hash:c8bcd153fe4a1c9c792dd8cd404226b19a3fc1c7)ベースに変更(in_tta.dll)
                      コンパイラをVC2017に変更(in_tta.dll, enc_tta.dll)
					  SSE2拡張を使用するようコンパイルオプションを変更(in_tta.dll)
2017-06-06 3.49       デコードエンジンをttaplugin-winamp-3.5ベースからlibtta-cpp-2.3ベースに変更(in_tta.dll)
2017-04-19 3.48       再生用デコードエンジンをlibtta-cpp-2.3ベースからttaplugin-winamp-3.5 ベースに変更(in_tta.dll)
2017-04-07 3.47       Windows7での実行を可能にするためにコンパイラをVC2010に変更(in_tta.dll,enc_tta.dll)
                      zlibを動的リンクに変更(in_tta.dll)
2017-02-25 3.46       taglibを最新master(based on 2017/02/24 15:47:30 commit Hash:4891ee729d5bd98fbe8ff82b89e9758f00d18815)ベースに変更(in_tta.dll)
                      libttaを静的リンクに変更(in_tta.dll)
2016-12-14 3.45       メモリリークを修正(libtta.dll)
                      メモリリークを修正(enc_tta.dll)
2016-12-12 3.44       コードの見直し(in_tta.dll)
                      コンフィグダイアログを削除(in_tta.dll)
2016-12-10 3.43       taglibを最新stable(based on 2016/12/05 11:02:59 commit Hash:36ccad2bd4b5b8aec1e547faef3bfe0269316ae9)ベースに変更(in_tta.dll)
                      トランスコーディング時の使用メモリを削減(in_tta.dll)
                      コードの見直し(in_tta.dll)  
2016-06-07 3.42       taglibを最新stable(based on 2016/05/14 10:46:42 commit Hash:3e47a036fbbe5117377018fbbe4cbdd49319196e)ベースに変更(in_tta.dll)
2016-06-06 3.41       デコードエンジンをttaplugin-winamp-3.5ベースからlibtta-cpp-2.3ベースに変更(in_tta.dll)
                      メタデータ中のbitrateが正しくなかった問題を修正(in_tta.dll)
					  エンコード時の一時ファイル保管場所を出力ディレクトリに修正(enc_tta.dll)
2016-05-17 3.40       マイナーバグを修正．(enc_tta.dll)
2016-05-15 3.39       マイナーバグを修正．対応メタデータ種類を増加(in_tta.dll)
2015-12-30 3.38		  VC2015 ランタイム再配布可能パッケージのダウンロードURIを修正
2015-10-10 3.37       エンコード時に再生時間がリセットされる問題を修正(in_tta.dll)
2015-09-04 3.36       バージョン番号付番規則を変更
                      ライブラリのコンパイルオプションを変更(tag.dll)
                      ライブラリのコンパイルオプションを変更(in_tta.dll)
2015-08-27 3.5 Beta31 タグデータ編集時正常に保存されない問題を修正(in_tta)
                      再生時に瞬時ビットレートを表示するよう変更(in_tta)
2015-08-27 3.5 Beta30 トランスコーディングの動作不能を修正(in_tta)
2015-08-24 3.5 Beta29 再生用デコードエンジンをlibtta-cpp-2.3ベースからttaplugin-winamp-3.5 ベースに変更(in_tta.dll)
2015-08-24 3.2 Beta28 同梱のlibtta.dllをSSE4対応版に変更(libtta.dll)
2015-08-24 3.2 Beta27 メタデータ読み込み時のファイルアクセス回数をさらに削減(in_tta.dll)
2015-08-20 3.2 Beta26 メタデータ読み込み時のファイルアクセス回数を削減(in_tta.dll)
2015-08-13 3.2 Beta25 エンコード時ファイルの末尾が不正となっている問題を修正(enc_tta.dll)
                      正しいファイル名を取得するためにトランスコーディング用関数名を修正(in_tta.dll)
		    		  コードの見直し(in_tta.dll)
2015-08-12 3.2 Beta24 コードの見直し(in_tta.dll)
                      コードの見直し(enc_tta.dll)
2015-08-11 3.2 Beta23 ネイティブUnicode版に変更(in_tta.dll)
                      管理上各プラグインを分離
2015-08-11 3.2 Beta22 アルバムアート保存時の問題を修正(in_tta.dll)
2015-08-11 3.2 Beta21 エンコード機能の追加(enc_tta.dll)
                      タグ情報の保存時の問題を修正(in_tta.dll)
				      libttaを外部DLL化(in_tta.dll)
                      taglibを外部DLL化(in_tta.dll)
                      taglibを最新stable(2015/08/10 0:50:13 commit Hash:bc106ad81e1015c896b32ae1fec6cb3c3894ac84)ベースに変更
2015-08-04 3.2 Beta20 安定性に関する修正
2015-08-04 3.2 Beta19 トランスコーディングの動作不能を修正
2015-08-02 3.2 Beta18 24bitファイルの再生時にフリーズする問題を修正
                      タイトル表示出力を廃止しWinamp本体に任せるように変更
				      メタデータ読み込み時のファイルアクセス回数を削減
				      プラグインをUNICODEベースモードに変更
				      zlibを本体同梱のdllに変更
                      taglibを最新stable(2015/06/19 16:12:32 commit Hash:e90b5e5f2faded598688c48dd3bdd2fc6cd5cbab)ベースに変更
                      コンパイラをVC2015に変更
2015-05-27 3.2 Beta17 ベースライブラリをlibtta++2.3に変更
                      taglibを最新stable(2015/05/25 21:41:37 commit Hash:c5f2e9342dfdf9a94dbcbf70fc3861b2221d1b39)ベースに変更
2014-11-28 3.2 Beta16 ベースライブラリをlibtta++2.2に変更
                      taglibを最新stable(2014/10/23 2:31:14 commit Hash:072851869a2980dab4aefd471ad7dd707993d74f)ベースに変更
                      コンパイラをVC2013に変更
2013-08-22 3.2 Beta15 taglibを最新stable(commit Hash:fddf3ed51b705d69e12df4a0cba01984ce82c638)ベースに変更
				      taglibをスタティックリンクに再変更
2011-12-10 3.2 Beta14 Revert multibyte version
                      libttaからの例外チェックを追加
2011-12-03 3.2 Beta13 Unicode Plugin version
                      デコーダー例外処理を追加
2011-11-27 3.2 Beta12 不正なファイル名を渡された際にフリーズする問題を修正
                      アルバムアートなしのファイルを読み込むとフリーズすることがある問題を修正
2011-11-21 3.2 Beta11 ベースライブラリをlibtta++2.1に変更
                      taglibを同梱から外部dllを使う方法に変更
                      アルバムアート読み込み時のメモリリークを修正
2011-11-17 3.2 Beta10 taglibを1.7.0ベースに変更
                      コンパイラをVC2010に変更
2010-11-29 3.2 Beta9  アルバムアート読み込みに対応
2009-04-09 3.2 Beta8  コンパイラをVC2008に変更
                      ID3 Ver2.4においてYearタグをTDRL -> TDRCに変更
2007/10/23 3.2 Beta7  シーク位置ずれの修正
	                  Format Conversionに対応
2006/07/03 3.2 Beta6  メディアライブラリにyear情報を付加するために小改正
2006/02/23 3.2 Beta5  Beta4での修正点が実際には修正されておらず,再修正
2005/12/19 3.2 Beta4  再生中に他のプログラムからttaファイルにアクセスできない問題点を修正
2005/12/16 3.2 Beta3  ID3v2 Ver2.3の読み込み不具合修正
	                  (フレームサイズを正しくデコードしていない問題)
2005/12/14 3.2 Beta2

*配布
配布規定はLGPL2.1に従います。

*その他
この改変は私が勝手に行ったものであり、この改変版に関して
本家に問い合わせを行わないでください。

*謝辞
メディアライブラリ読み込み関連では
 おたちゃん さん作成のin_mpg123.dll 改悪版
のソースコードを参照しました。
  Web: http://www3.cypress.ne.jp/otachan/

ID3v2関連では
はせた さん作成のSTEP(SuperTagEditor改 Plugin Version (Nightmare))
のソースコードを参考にしました。
  Web: http://haseta2003.hp.infoseek.co.jp/

シーク関連の修正は
h0shu さん作成のパッチを参考にしました。
  Web: http://hoshustep.hp.infoseek.co.jp/dust.html

この場をお借りしてお礼申し上げます。

Yamagata Fumihiro
mailto:bunbun042000@gmail.com
