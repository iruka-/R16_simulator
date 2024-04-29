/************************************************************************************
 *
 ************************************************************************************
 */
#ifndef	opcode_h_
#define	opcode_h_


typedef	unsigned short WORD;

struct _OPCODE ;		// ちょこっと宣言.

//	命令実行関数のプロトタイプ型.
typedef int (*EMUFUNC) (int code,struct _OPCODE *tab);
//	命令逆アセンブル関数のプロトタイプ型.
typedef int (*DISFUNC) (int code,struct _OPCODE *tab);


typedef	struct _OPCODE {
	char *mnemonic;			// ニモ
	char *comment;			// 意味
	char *pattern;			// 機械語
	EMUFUNC emufunc;		// 命令実行関数
	DISFUNC disfunc;		// 命令逆アセンブル関数
} OPCODE;


#define	MEMSIZE		0x10000			// 実装メモリーサイズ (WORD)
#define	VRAMSIZE	0x10000			// 実装VRAMサイズ (WORD)

OPCODE  code_table[256];		// 命令表(8bit)

/** *********************************************************************************
 *	メモリーコンテキスト
 ************************************************************************************
 */
WORD    memory[MEMSIZE];

/** *********************************************************************************
 *	ＶＲＡＭコンテキスト
 ************************************************************************************
 */
WORD    vram[VRAMSIZE];

/** *********************************************************************************
 *	レジスタコンテキスト
 ************************************************************************************
 */
typedef	struct tag_R16_CONTEXT {
	WORD	pc;
	WORD	acc;
	char	f;	// full
	char	z;	// zero
	char	c;	// carry
	char	m;	// minus
//
	WORD	pc_bak;	// jumpする前のPC.	(逆アセンブル時に必要)
} R16_CONTEXT;

R16_CONTEXT	reg;


#define	EMUFUNC_(x_)	int x_(int code,struct _OPCODE *tab)
#define	DISFUNC_(x_)	int x_(int code,struct _OPCODE *tab)

//	命令実行関数のプロトタイプ宣言.
EMUFUNC_( f_JMP );
EMUFUNC_( f_JBP );
EMUFUNC_( f_JM  );
EMUFUNC_( f_JNM );
EMUFUNC_( f_JF  );
EMUFUNC_( f_JNF );
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
EMUFUNC_( f_STV );
EMUFUNC_( f_ST  );
EMUFUNC_( f_OUT );
EMUFUNC_( f_STP );
EMUFUNC_( f_SFR );
EMUFUNC_( f_SFL );
EMUFUNC_( f_IN  );
EMUFUNC_( f_SCN );
EMUFUNC_( f_HLT );
               
//	命令逆アセンブル関数のプロトタイプ宣言.
DISFUNC_( d_JMP );
DISFUNC_( d_JBP );
DISFUNC_( d_JM  );
DISFUNC_( d_JNM );
DISFUNC_( d_JF  );
DISFUNC_( d_JNF );
DISFUNC_( d_JE  );
DISFUNC_( d_JNE );
DISFUNC_( d_JC  );
DISFUNC_( d_JNC );
DISFUNC_( d_LD  );
DISFUNC_( d_ADD );
DISFUNC_( d_SUB );
DISFUNC_( d_ADC );
DISFUNC_( d_SBB );
DISFUNC_( d_AND );
DISFUNC_( d_OR  );
DISFUNC_( d_XOR );
DISFUNC_( d_LDV );
DISFUNC_( d_LDP );
DISFUNC_( d_STV );
DISFUNC_( d_ST  );
DISFUNC_( d_OUT );
//DISFUNC_( d_STP );
DISFUNC_( d_SFR );
DISFUNC_( d_SFL );
DISFUNC_( d_IN  );
DISFUNC_( d_SCN );
DISFUNC_( d_HLT );

EMUFUNC_( f_und );	//未定義.
DISFUNC_( d_und );


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

//
//	入出力関数.
//
void LED_output(int acc , int ea);
int	 SW_input();
int	 JOY_input();

#endif	//opcode_h_
