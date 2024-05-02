/** *********************************************************************************
 *	
 ************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include "opcode.h"
#include "gr.h"
#include "led.h"

#define	TEST	(1)

OPCODE opcode_init_tab[]={
 //ニモ,意味					,機械語		 ,f_emu,d_dis  //z,c,m,動作 --------------------
 {"JMP","JMP always"            ,"00xxdd00"  ,f_JMP,d_JMP},//-,-,-,無条件に分岐する
 {"JBP","Jump if Not Blanking"  ,"00xxdd01"  ,f_JBP,d_JBP},//-,-,-,帰線期間でなければ分岐
 {"JNF","Jump if Not Full"      ,"00xxdd10"  ,f_JNF,d_JNF},//-,-,-,0xffでなけれは分岐
 {"JF" ,"Jump if Full"          ,"00xxdd11"  ,f_JF ,d_JF },//-,-,-,0xffなら分岐
 {"JNC","Jump if Not Carry"     ,"01xxdd00"  ,f_JNC,d_JNC},//-,-,-,キャリー無(ボロー有)なら分岐
 {"JC" ,"Jump if Carry"         ,"01xxdd01"  ,f_JC ,d_JC },//-,-,-,キャリー有(ボロー無)なら分岐
 {"JNM","Jump if Not Minus"     ,"01xxdd10"  ,f_JNM,d_JNM},//-,-,-,0またはプラスなら分岐
 {"JM" ,"Jump if Minus"         ,"01xxdd11"  ,f_JM ,d_JM },//-,-,-,マイナスなら分岐
 {"LD" ,"LoaD"                  ,"10xx0000"  ,f_LD ,d_LD },//?,0,?,ロード
 {"LDT","LoaD Test"             ,"10xx0001"  ,f_LD ,d_LD },//?,0,?,上記の試験（フラグ更新のみ）
 {"SBC","SuB. with Borrow"      ,"10xx0010"  ,f_SBB,d_SBB},//?,?,?,ボロー付減算
 {"SCT","Sub. with Borrow Test" ,"10xx0011"  ,f_SBB,d_SBB},//?,?,?,上記の試験（フラグ更新のみ）
 {"SUB","SUBtract"              ,"10xx0100"  ,f_SUB,d_SUB},//?,?,?,減算
 {"SBT","SUBtract Test"         ,"10xx0101"  ,f_SUB,d_SUB},//?,?,?,上記の試験（フラグ更新のみ）
 {"OR" ,"OR"                    ,"10xx0110"  ,f_OR ,d_OR },//?,0,?,論理和
 {"ORT","OR Test"               ,"10xx0111"  ,f_OR ,d_OR },//?,0,?,上記の試験（フラグ更新のみ）
 {"ADD","ADD"                   ,"10xx1000"  ,f_ADD,d_ADD},//?,?,?,加算
 {"ADT","ADd Test"              ,"10xx1001"  ,f_ADD,d_ADD},//?,?,?,上記の試験（フラグ更新のみ）
 {"AND","AND"                   ,"10xx1010"  ,f_AND,d_AND},//?,0,?,論理積
 {"ANT","ANd Test"              ,"10xx1011"  ,f_AND,d_AND},//?,0,?,上記の試験（フラグ更新のみ）
 {"ADC","ADd with Carry"        ,"10xx1100"  ,f_ADC,d_ADC},//?,?,?,キャリー付加算
 {"ACT","Add with Carry Test"   ,"10xx1101"  ,f_ADC,d_ADC},//?,?,?,上記の試験（フラグ更新のみ）
 {"XOR","eXcusive OR"           ,"10xx1110"  ,f_XOR,d_XOR},//?,0,?,排他的論理和
 {"XOT","eXcusive Or Test"      ,"10xx1111"  ,f_XOR,d_XOR},//?,0,?,上記の試験（フラグ更新のみ）
 {"LDV","LoaD from Vram"        ,"11xx000-"  ,f_LDV,d_LDV},//?,0,?,ビデオメモリからのロード
 {"LDP","LoaD from PC"          ,"11xx001-"  ,f_LDP,d_LDP},//?,0,?,PC（次の命令のアドレス）+「値」=>Acc
 {"ST" ,"STore"                 ,"11xx010-"  ,f_ST ,d_ST },//-,-,-,ストア
 {"SR" ,"ShiFt Right"           ,"11--011-"  ,f_SFR,d_SFR},//?,?,?,右シフト（オペランドは無効）
 {"SL" ,"ShiFt Left"            ,"11--100-"  ,f_SFL,d_SFL},//?,?,?,左シフト（オペランドは無効）
 {"IN" ,"IN"                    ,"11xx101-"  ,f_IN ,d_IN },//-,-,-,トグルスイッチの状態を入力
 {"OUT","OUT  "                 ,"11xx110-"  ,f_OUT,d_OUT},//-,-,-,Accと指定メモリ値をLEDに表示
 {"STV","STore to Vram"         ,"11xx1110"  ,f_STV,d_STV},//-,-,-,ビデオメモリへのストア
 {"HALT","HALT"                 ,"11xx1111"  ,f_HLT,d_HLT},//-,-,-,停止
// {"SCN","SCaN JOY stick port"   ,"11xx1111"  ,f_SCN,d_SCN},//-,-,-,ジョイスティック入力のスキャン
// {"STP","STore Program counter" ,"11xx100-"  ,f_STP,d_STP},//-,-,-,プログラムカウンタ値を保存
 //ニモ,意味					,機械語		 ,f_emu,d_dis  //z,c,m,動作 --------------------
 { NULL,NULL,NULL}
};
OPCODE UNDEFINED_OPCODE=
 {"???","???","--------",f_und,d_und};
//	xx	アドレッシング
//	00	Imm8
//	01	Reg8
//	10	[Reg8]
//	11	Imm16

//	dd	ディスプレイスメント
//	00	Abs
//	01	fwd
//	10	back
//	11	fwd+carry

FILE *ifp;

#define Ropen(name) {ifp=fopen(name,"rb");if(ifp==NULL) \
{ printf("Fatal: can't open file:%s\n",name);exit(1);}  \
}
#define Read(buf,siz)   fread (buf,1,siz,ifp)
#define Rclose()  fclose(ifp)


void memdump(int adr,int len);
void VRAM_output(int adrs,int data);
extern	int	led_trace;

/** *********************************************************************************
 *	命令コード(code) が、OPCODE表の１要素(*s)  にマッチするか判定.
 ************************************************************************************
 *	マッチしたら 0 を返す.
 */
int match_table(OPCODE *s,int code)
{
	char *pat = s->pattern;
	int i,bit,sbit;
	int m=0x80;
	for(i=0;i<8;i++,m>>=1) {
		sbit = *pat++;
		bit = code & m;
		if(sbit == '1') {
			if(bit == 0) return -1;
		}
		if(sbit == '0') {
			if(bit != 0) return -1;
		}
	}
	return 0;	// Matching OK!
}

/** *********************************************************************************
 *	命令コード code がマッチするOPCODEをopcode_init_tab[]から探す. 
 ************************************************************************************
 *	見つかったら、それを *table に丸っとコピーする.
 *	見つからなかったら、UNDEFINED_OPCODEを *table に丸っとコピーする.
 */
void make_table(OPCODE *table,int code)
{
	OPCODE *s = opcode_init_tab;
	while(s->mnemonic) {
		if(match_table(s,code)==0) {
			*table = *s;
			return;
		}
		s++;
	}
	*table = UNDEFINED_OPCODE;
}

/** *********************************************************************************
 *	opcode_init_tab[]の情報をもとに、早引き表の code_table[256] を構築する.
 ************************************************************************************
 */
void init_table()
{
	OPCODE *table = code_table;		// 構築したい表 code_table[256]; の先頭アドレス.
	int code;
	for(code=0;code<256;code++,table++) {
		make_table(table,code);
	}
}

/** *********************************************************************************
 *	後続する16bit即値が存在する命令なら 1 を返す.
 ************************************************************************************
 */
int	is_imm16(int code)
{
	code &= XX_MASK;
	if( code == XX_IMM16 ) return 1;	//後続する IMM16 即値を持つ.
	return 0;
}
/** *********************************************************************************
 *	16bitの命令を１つフェッチして実行する.
 ************************************************************************************
 */
int	execute_pc()
{
	int rc=0;
	int opcode = memory[ reg.pc++ ];
	OPCODE *table = &code_table[ (opcode >> 8) & 0xff ];
	reg.pc_bak = reg.pc;

	rc = table->emufunc(opcode,table);	//命令実行.
	if(led_trace) {
		table->disfunc(opcode,table);	//逆アセンブル.
	}
	if(rc) return rc;
	return LED_draw(reg.pc,led_trace);

}

//	JOY入力.
int	 JOY_input()
{
	return 0;
}

void randomize_mem()
{
	int i,j=0;
	for(i=0;i<MEMSIZE;i++) {
		vram[i]=j;
		VRAM_output(i,j);
		j*=0x3d09;j++;
	}
}

int	load_binary(char *fname)
{
	Ropen(fname);
	int rc = Read(memory,sizeof(memory));
	Rclose();
	
	(void)rc;
	return 0;
}
/** *********************************************************************************
 *	メインルーチン.
 ************************************************************************************
 */
int main(int argc,char **argv)
{
	int rc;
	init_table();

	gr_init(SCREEN_W,SCREEN_H,32,0);

	randomize_mem();
	load_binary(argv[1]);

	reg.pc = 0;

	while(1) {
		rc = execute_pc();
		if(rc) break;
	}
	gr_close();
	return 0;
}


/** *********************************************************************************
 *
 ************************************************************************************
 */
