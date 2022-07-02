# AviUtl プラグイン - スクリプト並び替えパッチ

スクリプト並び替えプラグインの不具合と競合を解消します。
[最新バージョンをダウンロード](../../releases/latest/)

- スクリプトを設定ダイアログにドラッグアンドドロップしたとき、「震える」になってしまう問題を解決します。
	- この問題は『設定ダイアログにスクロールを付ける』＆『設定ダイアログを高速描画する』＆『スクリプト選択コンボボックスがウィンドウ外に出現する』の条件が重なったときに発生します。
- スクリプトを設定ダイアログにドラッグアンドドロップしたあと、rikky_module などのプラグインが正常に動作しなくなる問題を解決します。
	- rikky_module の colordialog などのボタンが作成されなくなります。

## 導入方法

以下のファイルを AviUtl の Plugins フォルダに入れてください。
* script_sort_patch.auf

## 更新履歴

* 2.0.0 - 2022/07/02 rikky_module の colordialog が重くなる問題を修正
* 1.0.0 - 2022/06/24 初版

## 動作確認

* (必須) AviUtl 1.10 & 拡張編集 0.92 http://spring-fragrance.mints.ne.jp/aviutl/
* (共存確認) patch.aul r41 https://scrapbox.io/ePi5131/patch.aul

## クレジット

* Microsoft Research Detours Package https://github.com/microsoft/Detours
* aviutl_exedit_sdk https://github.com/ePi5131/aviutl_exedit_sdk
* Common Library https://github.com/hebiiro/Common-Library

## 作成者情報
 
* 作成者 - 蛇色 (へびいろ)
* GitHub - https://github.com/hebiiro
* Twitter - https://twitter.com/io_hebiiro

## 免責事項

このプラグインおよび同梱物を使用したことによって生じたすべての障害・損害・不具合等に関しては、私と私の関係者および私の所属するいかなる団体・組織とも、一切の責任を負いません。各自の責任においてご使用ください。
