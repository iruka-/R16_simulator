/************************************************************************************
 *
 ************************************************************************************
 */
#ifndef	opcode_h_
#define	opcode_h_

#include "hash.h"

typedef	unsigned short WORD;

struct _OPCODE ;		// ちょこっと宣言.

//	命令実行関数のプロトタイプ型.
typedef int (*EMUFUNC) (int code,struct _OPCODE *tab);
//	命令逆アセンブル関数のプロトタイプ型.
//typedef int (*DISFUNC) (int code,struct _OPCODE *tab);


typedef	struct _OPCODE {
	char *mnemonic;			// ニモ
	char *comment;			// 意味
	char *pattern;			// 機械語
	EMUFUNC emufunc;		// 命令実行関数
	int	  data;				// 
} OPCODE;


#define	MEMSIZE		0x10000			// 実装メモリーサイズ (WORD)
#define	VRAMSIZE	0x10000			// 実装VRAMサイズ (WORD)

//OPCODE  code_table[256];		// 命令表(8bit)

/** *********************************************************************************
 *	メモリーコンテキスト
 ************************************************************************************
 */
WORD    memory[MEMSIZE];

/** *********************************************************************************
 *	ＶＲＡＭコンテキスト
 ************************************************************************************
 */
//WORD    vram[VRAMSIZE];

/** *********************************************************************************
 *	レジスタコンテキスト
 ************************************************************************************
 */
typedef	struct _CONTEXT {
	WORD	pc;
	WORD	acc;
	char	z;	// zero
	char	c;	// carry
	char	m;	// minus
//
	WORD	pc_bak;	// jumpする前のPC.	(逆アセンブル時に必要)
} CONTEXT;

CONTEXT	reg;


#define	EMUFUNC_(x_)	int x_(int code,struct _OPCODE *tab)
#define	DISFUNC_(x_)	int x_(int code,struct _OPCODE *tab)

//	命令実行関数のプロトタイプ宣言.
EMUFUNC_( f_JMP );
EMUFUNC_( f_JNB );
EMUFUNC_( f_JM  );
EMUFUNC_( f_JNM );
EMUFUNC_( f_JE  );
EMUFUNC_( f_JNE );
EMUFUNC_( f_JC  );
EMUFUNC_( f_JNC );
EMUFUNC_( f_LD  );
EMUFUNC_( f_ADD );
EMUFUNC_( f_SUB );
EMUFUNC_( f_ADC );
EMUFUNC_( f_SBB );
EMUFUNC_( f_AND );
EMUFUNC_( f_OR  );
EMUFUNC_( f_XOR );
EMUFUNC_( f_LDV );
EMUFUNC_( f_LDP );
EMUFUNC_( f_ST  );
EMUFUNC_( f_OUT );
//EMUFUNC_( f_STP );
EMUFUNC_( f_SR  );
EMUFUNC_( f_SL  );
EMUFUNC_( f_IN  );
//EMUFUNC_( f_SCN );
EMUFUNC_( f_STV );
EMUFUNC_( f_HLT );

EMUFUNC_( f_ORG );
EMUFUNC_( f_EQU );
EMUFUNC_( f_DW  );
EMUFUNC_( f_DOT );
EMUFUNC_( f_END );

EMUFUNC_( f_MOV );
EMUFUNC_( f_INC );
EMUFUNC_( f_DEC );
EMUFUNC_( f_CP );
EMUFUNC_( f_CALL);
EMUFUNC_( f_ENT );
EMUFUNC_( f_RET );
EMUFUNC_( f_SFR );
EMUFUNC_( f_SFL );
               
//	xx	アドレッシング
//	00	Imm8
//	01	Reg8
//	10	[Reg8]
//	11	Imm16

enum {
	XX_IMM8 =0x0000,
	XX_REG8 =0x1000,
	XX_IND8 =0x2000,
	XX_IMM16=0x3000,	//後続する IMM16 即値を持つ.
	XX_MASK =0x3000,

	IS_REG	=0x10000,
};
//	dd	ディスプレイスメント
//	00	Abs
//	01	fwd
//	10	back
//	11	fwd+carry
enum {
	DD_ABS  =0x0000,
	DD_FWD  =0x0400,
	DD_BACK =0x0800,
	DD_FWDC =0x0C00,
	DD_MASK =0x0C00,
};
//
//	演算命令で、結果をAccに戻すかどうか.
//
enum {
	OP_TEST	    =0x0100,	//テストのみでAccを更新しない.
	OP_TEST_MASK=0x0100,
};

char symbuf[128];
char label[128];
char mnemonic[128];
char operandbuf[128];
char comment[512];

char *operand;

int	save_ptr;	//memory[]の有効サイズ.
HASH *sym;		//シンボル表.

#define	ZZ	printf("%s:%d: ZZ\n",__FILE__,__LINE__);

#endif	//opcode_h_
