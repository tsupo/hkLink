/*
 *  はてなダイアリーキーワード自動リンクAPI 利用プログラム
 *      -- 標準入力またはファイルから読み込んだ文字列にキーワードリンクを
 *         埋め込み、標準出力に書き出す
 *
 *      written by H.Tsujimura      17 Jun 2004
 *      Copyleft 2004 by H.Tsujimura (tsupo@na.rim.or.jp)
 *
 *      使い方:
 *          hkLink [-p] [-c] [ファイル名 [...]]
 *              -p: proxy サーバ経由で XML-RPC API を実行する
 *                   (1) カレントディレクトリに proxy.txt という名
 *                       前のファイルがあれば、proxy.txt から設定を
 *                       読み込む
 *                   (2) proxy.txt が存在しない場合、proxy の設定は
 *                       Internet Explorer の設定を流用する [Windows
 *                       の場合のみ]
 *
 *                          proxy.txt のフォーマット
 *                            以下の2行からなるテキストファイル
 *                              proxy サーバ名(または IPアドレス)
 *                              ポート番号
 *              -c: 「はてなキーワード自動リンクAPI」を使う代わりに、
 *                  「はてなキーワード一覧取得API」+自前のキーワード
 *                  リンク変換処理を使用する
 *
 *      注意:
 *      (1) 「はてなキーワード自動リンクAPI」が返す結果は文字化けしてい
 *          ることがある。(2004年6月17日現在)
 *              例) 入力文字列が
 *                      てへ
 *                      こうさぎ
 *                      なかはらまい
 *                  のようにひらがなのみが偶数個連続する文字列の場合、
 *                  文字化けするようである。ただし、「、」や「。」を含
 *                  む場合は文字化けしない。
 *      (2) 2004年6月18日14時現在、入力文字列に関係なく、常に空文字列が
 *          返ってくるようになった (メインテナンス中 or サービス停止中 ?)
 *
 *      備考:
 *          「はてなキーワード自動リンクAPI」が返す結果が「空文字列」の
 *          場合は、「はてなキーワード一覧取得API」を使って取得したキー
 *          ワードテーブルを元に自前でキーワードリンクを生成するようにし
 *          た (2004年6月18日)
 *
 * History:
 * $Log: /comm/hkLink/hkLink.c $
 * 
 * 1     09/05/14 3:51 tsupo
 * (1) ビルド環境のディレクトリ構造を整理
 * (2) VSSサーバ拠点を変更
 * 
 * 3     07/03/10 2:47 Tsujimura543
 * setKeywordLink() の引数追加に伴う修正
 * 
 * 2     04/07/07 11:35 Tsujimura543
 * 最初の一般公開版
 *
 */

#include "xmlRPC.h"

#ifndef	lint
static char	*rcs_id =
"$Header: /comm/hkLink/hkLink.c 1     09/05/14 3:51 tsupo $";
#endif


void
makeKeywordLink( FILE * fp, int cacheVer )
{
    char    *p, *q, buf[BUFSIZ * 4], result[BUFSIZ * 16];
    size_t  sz = BUFSIZ * 16;

    while ( ( p = fgets( buf, BUFSIZ - 1, fp ) ) != NULL ) {
        if ( (*p == '\r') || (*p == '\n' ) ) {
            fputs( p, stdout );
            continue;
        }

        if ( !cacheVer ) {
            /* 「はてなキーワード自動リンクAPI」を呼び出す */
            /*  ※ setKeywordLink() は入力、出力とも UTF-8 */
            q = setKeywordLink( sjis2utf( buf ),
                                result, sz,
                                0, CNAME_GENERAL, "_blank", "hatena" );
            if ( q && *q ) {
                fputs( utf2sjis( q ), stdout );
                fputc( '\n', stdout );
                continue;
            }

            /* 変換結果失敗のとき、もしくは変換結果が空文字の場合 */
            cacheVer = 1;
        }

        /* 自前(xmlRPCライブラリ内)でキーワードリンク変換を実行する      */
        /*   [キーワード一覧未取得時および前回の取得から72時間以上経過し */
        /*    たときに、「はてなキーワード一覧取得API」により、キーワー  */
        /*    ド一覧を取得する]                                          */
        /*  ※ changeHatenaKeyword() は入力は Shift_JIS、出力は EUC-JP   */
        q = changeHatenaKeyword( buf, result, MODE_HTML );
        if ( q && *q )
            fputs( euc2sjis( q ), stdout );
        else
            fputs( buf, stdout );
                            /* 変換失敗時は入力文字列をそのままエコーバック */
    }
}

int
main( int argc, char *argv[] )
{
    int     ret = 0;
    int     useProxy = 0;
    int     cacheVer = 0;

    if ( argc >= 2 ) {
        int     i, j;
        FILE    *fp;

        for ( i = 1; i < argc; i++ ) {
            if ( argv[i][0] == '-' ) {
                for ( j = 0; argv[i][j] != '\0'; j++ ) {
                    switch ( argv[i][j] ) {
                    case 'p':
                        useProxy = !useProxy;
                        setUseProxy( useProxy );
                        break;
                    case 'c':
                        cacheVer = !cacheVer;
                        break;
                    }
                }

                continue;
            }

            fp = fopen( argv[i], "r" );
            if ( fp ) {
                makeKeywordLink( fp, cacheVer );
                fclose( fp );
                ret++;
            }
        }
    }

    if ( ret == 0 ) {
        makeKeywordLink( stdin, cacheVer );
        ret++;
    }

    return ( ret );
}
