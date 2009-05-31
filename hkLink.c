/*
 *  �͂Ăȃ_�C�A���[�L�[���[�h���������NAPI ���p�v���O����
 *      -- �W�����͂܂��̓t�@�C������ǂݍ��񂾕�����ɃL�[���[�h�����N��
 *         ���ߍ��݁A�W���o�͂ɏ����o��
 *
 *      written by H.Tsujimura      17 Jun 2004
 *      Copyleft 2004 by H.Tsujimura (tsupo@na.rim.or.jp)
 *
 *      �g����:
 *          hkLink [-p] [-c] [�t�@�C���� [...]]
 *              -p: proxy �T�[�o�o�R�� XML-RPC API �����s����
 *                   (1) �J�����g�f�B���N�g���� proxy.txt �Ƃ�����
 *                       �O�̃t�@�C��������΁Aproxy.txt ����ݒ��
 *                       �ǂݍ���
 *                   (2) proxy.txt �����݂��Ȃ��ꍇ�Aproxy �̐ݒ��
 *                       Internet Explorer �̐ݒ�𗬗p���� [Windows
 *                       �̏ꍇ�̂�]
 *
 *                          proxy.txt �̃t�H�[�}�b�g
 *                            �ȉ���2�s����Ȃ�e�L�X�g�t�@�C��
 *                              proxy �T�[�o��(�܂��� IP�A�h���X)
 *                              �|�[�g�ԍ�
 *              -c: �u�͂ĂȃL�[���[�h���������NAPI�v���g������ɁA
 *                  �u�͂ĂȃL�[���[�h�ꗗ�擾API�v+���O�̃L�[���[�h
 *                  �����N�ϊ��������g�p����
 *
 *      ����:
 *      (1) �u�͂ĂȃL�[���[�h���������NAPI�v���Ԃ����ʂ͕����������Ă�
 *          �邱�Ƃ�����B(2004�N6��17������)
 *              ��) ���͕�����
 *                      �Ă�
 *                      ��������
 *                      �Ȃ��͂�܂�
 *                  �̂悤�ɂЂ炪�Ȃ݂̂������A�����镶����̏ꍇ�A
 *                  ������������悤�ł���B�������A�u�A�v��u�B�v����
 *                  �ޏꍇ�͕����������Ȃ��B
 *      (2) 2004�N6��18��14�����݁A���͕�����Ɋ֌W�Ȃ��A��ɋ󕶎���
 *          �Ԃ��Ă���悤�ɂȂ��� (���C���e�i���X�� or �T�[�r�X��~�� ?)
 *
 *      ���l:
 *          �u�͂ĂȃL�[���[�h���������NAPI�v���Ԃ����ʂ��u�󕶎���v��
 *          �ꍇ�́A�u�͂ĂȃL�[���[�h�ꗗ�擾API�v���g���Ď擾�����L�[
 *          ���[�h�e�[�u�������Ɏ��O�ŃL�[���[�h�����N�𐶐�����悤�ɂ�
 *          �� (2004�N6��18��)
 *
 * History:
 * $Log: /comm/hkLink/hkLink.c $
 * 
 * 1     09/05/14 3:51 tsupo
 * (1) �r���h���̃f�B���N�g���\���𐮗�
 * (2) VSS�T�[�o���_��ύX
 * 
 * 3     07/03/10 2:47 Tsujimura543
 * setKeywordLink() �̈����ǉ��ɔ����C��
 * 
 * 2     04/07/07 11:35 Tsujimura543
 * �ŏ��̈�ʌ��J��
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
            /* �u�͂ĂȃL�[���[�h���������NAPI�v���Ăяo�� */
            /*  �� setKeywordLink() �͓��́A�o�͂Ƃ� UTF-8 */
            q = setKeywordLink( sjis2utf( buf ),
                                result, sz,
                                0, CNAME_GENERAL, "_blank", "hatena" );
            if ( q && *q ) {
                fputs( utf2sjis( q ), stdout );
                fputc( '\n', stdout );
                continue;
            }

            /* �ϊ����ʎ��s�̂Ƃ��A�������͕ϊ����ʂ��󕶎��̏ꍇ */
            cacheVer = 1;
        }

        /* ���O(xmlRPC���C�u������)�ŃL�[���[�h�����N�ϊ������s����      */
        /*   [�L�[���[�h�ꗗ���擾������ёO��̎擾����72���Ԉȏ�o�߂� */
        /*    ���Ƃ��ɁA�u�͂ĂȃL�[���[�h�ꗗ�擾API�v�ɂ��A�L�[���[  */
        /*    �h�ꗗ���擾����]                                          */
        /*  �� changeHatenaKeyword() �͓��͂� Shift_JIS�A�o�͂� EUC-JP   */
        q = changeHatenaKeyword( buf, result, MODE_HTML );
        if ( q && *q )
            fputs( euc2sjis( q ), stdout );
        else
            fputs( buf, stdout );
                            /* �ϊ����s���͓��͕���������̂܂܃G�R�[�o�b�N */
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
