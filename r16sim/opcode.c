/** *********************************************************************************
 *	オペコードの解釈実行.
 ************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include "opcode.h"
#include "led.h"

void VRAM_output(int adrs,int data);
extern	int	SW_inp ;
/** *********************************************************************************
 *	Acc結果から、フラグ z,c,mを全部更新する.
 ************************************************************************************
 */
void flag_update_zcm(int acc)
{
	if(acc & 0xffff) reg.z = 0;
	else		     reg.z = 1;

	if((acc & 0xffff)!=0xffff) reg.f = 0;
	else		     reg.f = 1;

	if(acc & 0x8000) reg.m = 1;
	else		     reg.m = 0;

	if(acc & 0x10000) reg.c = 1;
	else		      reg.c = 0;

}
/** *********************************************************************************
 *	Acc結果から、フラグ z,mを更新する. c=0 固定.
 ************************************************************************************
 */
void flag_update_zm(int acc)
{
	if(acc & 0xffff) reg.z = 0;
	else		     reg.z = 1;

	if((acc & 0xffff)!=0xffff) reg.f = 0;
	else		     reg.f = 1;

	if(acc & 0x8000) reg.m = 1;
	else		     reg.m = 0;

	reg.c  = 0;
}
/** *********************************************************************************
 *	オペコードを解析して、実効アドレス(ea) を求める.
 ************************************************************************************
 *	といいつつ、実効アドレスの評価結果（つまりオペランド）を返す.
 */
int	opadrs(int code)
{
	int ea = 0;
	int xx = code & XX_MASK;
	// アドレッシング修飾子(xx)
	switch(xx) {
	 case XX_IMM8 :	ea = code & 0xff;		 break;
	 case XX_REG8 :	ea = memory[code & 0xff];break;
	 case XX_IND8 :	ea = memory[code & 0xff];
	 				ea = memory[ea];		 break;
	 case XX_IMM16:	ea = memory[ reg.pc++ ]; break;
	 default:	break;
	}
	return ea;
}
//	ストア系命令のためのストア番地の評価を実行する.	opadrsと同一実装.
int	stadrs(int code)
{
	return opadrs(code);
}

#if	0
//	ストア系命令のためのストア番地の評価を実行する.
//	戻り値＝ストアすべき番地(0..0x0000ffff) もしくは (-1)ストア出来ない命令パタン.
int	stadrs(int code)
{
	int ea = 0;
	int xx = code & XX_MASK;
	// アドレッシング修飾子(xx)
	switch(xx) {
	 case XX_IMM8 :	return -1;			// ST Imm error!
	 case XX_REG8 :	ea =        code & 0xff ;break;
	 case XX_IND8 :	ea = memory[code & 0xff];break;
	 case XX_IMM16:	return -1;			// ST Imm error!
	 default:	return -1;
	}
	return ea;
}
#endif


/** *********************************************************************************
 *	オペコードを解析して、分岐先実効アドレス(ea) を求める.
 ************************************************************************************
 *	分岐系は XX を解釈した後、ddを解釈することになっている.
 */
int	jpadrs(int code)
{
	int ea = opadrs(code);
	int dd = code & DD_MASK;
	// ディスプレイスメント修飾子(dd)
	switch(dd) {
	 case DD_ABS :	break;
	 case DD_FWD :	ea = reg.pc + ea; break;
	 case DD_BACK:	ea = reg.pc - ea; break;
	 case DD_FWDC:	ea = reg.pc + ea + reg.c; break;
	 default:	break;
	}
	return ea & 0xffff;	// アドレスは16bitを超えないようにする.
}
/** *********************************************************************************
 *	オペコードを解析して、VRAM実効アドレス(ea) を求める.
 ************************************************************************************
 *	といいつつ、VRAM実効アドレスの評価結果（つまりオペランド）を返す.
 */
int	vramadrs(int code)
{
	int ea = 0;
	int xx = code & XX_MASK;
	// アドレッシング修飾子(xx)
	switch(xx) {
	 case XX_IMM8 :	ea = code & 0xff;		break;
	 case XX_REG8 :	ea = memory[code & 0xff];break;
	 case XX_IND8 :	ea = memory[code & 0xff];
	 				ea = vram[ea];		 break;
	 case XX_IMM16:	ea = vram[ reg.pc ]; break;
	 default:	break;
	}
	return ea;
}
/** *********************************************************************************
 *	こっから下は、命令の実行を行なう.
 ************************************************************************************
 {"JMP","JMP always"  ,"00xxdd00"  ,f_JMP,d_JMP},//-,-,-,無条件に分岐する
 */
int f_JMP (int code,OPCODE *tab)
{
	int ea = jpadrs(code);
	reg.pc = ea;
	return 0;
}
int f_JBP (int code,OPCODE *tab)
{
	int ea = jpadrs(code);
	int blank = 1;	// 仮.	たぶんブランキング待ちだと思うので、ブランキングにしてしまう。
	if(	blank ) {
		reg.pc = ea;	//実装的には JNB=JMPになってしまう.
	}
	return 0;
}
int f_JM  (int code,OPCODE *tab)
{
	int ea = jpadrs(code);
	if(	reg.m ) {
		reg.pc = ea;
	}
	return 0;
}
int f_JNM (int code,OPCODE *tab)
{
	int ea = jpadrs(code);
	if(	reg.m == 0) {
		reg.pc = ea;
	}
	return 0;
}
int f_JF  (int code,OPCODE *tab)
{
	int ea = jpadrs(code);
//	if(	reg.acc == 0xffff ) {
	if(	reg.f != 0) {
		reg.pc = ea;
	}
	return 0;
}
int f_JNF (int code,OPCODE *tab)
{
	int ea = jpadrs(code);
//	if(	reg.acc != 0xffff ) {
	if(	reg.f == 0) {
		reg.pc = ea;
	}
	return 0;
}
int f_JE  (int code,OPCODE *tab)
{
	int ea = jpadrs(code);
	if(	reg.z ) {
		reg.pc = ea;
	}
	return 0;
}
int f_JNE (int code,OPCODE *tab)
{
	int ea = jpadrs(code);
	if(	reg.z == 0 ) {
		reg.pc = ea;
	}
	return 0;
}
int f_JC  (int code,OPCODE *tab)
{
	int ea = jpadrs(code);
	if(	reg.c ) {
		reg.pc = ea;
	}
	return 0;
}
int f_JNC (int code,OPCODE *tab)
{
	int ea = jpadrs(code);
	if(	reg.c == 0 ) {
		reg.pc = ea;
	}
	return 0;
}
int f_LD  (int code,OPCODE *tab)
{
	int ea = opadrs(code);
	flag_update_zm(ea);
	if( (code & OP_TEST)==0 ) {
		reg.acc = ea;
	}
	return 0;
}
int f_LDP (int code,OPCODE *tab)
{
	int acc = reg.pc + opadrs(code);	// PCrel
//	printf("LDP code=%x pc=%x a=%x\n",code,reg.pc,acc);
	flag_update_zm(acc);
	reg.acc = acc;
	return 0;
}
int f_STP (int code,OPCODE *tab)
{
	int ea = stadrs(code);
//	if(	ea >= 0 ) 
	{
		// PCの値をストア.
		memory[ea] = reg.pc;
	}
	return 0;
}
int f_ADD (int code,OPCODE *tab)
{
	int acc = reg.acc + opadrs(code);
	flag_update_zcm(acc);
	if( (code & OP_TEST)==0 ) {
		reg.acc = acc;
	}
	return 0;
}
int f_SUB (int code,OPCODE *tab)
{
	int acc = reg.acc - opadrs(code);
	flag_update_zcm(acc);
	if( (code & OP_TEST)==0 ) {
		reg.acc = acc;
	}
	return 0;
}
int f_ADC (int code,OPCODE *tab)
{
	int acc = reg.acc + opadrs(code) + reg.c;
	flag_update_zcm(acc);
	if( (code & OP_TEST)==0 ) {
		reg.acc = acc;
	}
	return 0;
}
int f_SBB (int code,OPCODE *tab)
{
//	int acc = reg.acc - opadrs(code) - reg.c;		//普通のSBC命令なら...
	int acc = reg.acc - opadrs(code) - (1-reg.c);	//引き算時はcarryの論理が逆になる.
	flag_update_zcm(acc);
	// SC/MP-IIのボローをエミュレート.
	reg.c ^= 1;

	if( (code & OP_TEST)==0 ) {
		reg.acc = acc;
	}
	return 0;
}
int f_AND (int code,OPCODE *tab)
{
	int acc = reg.acc & opadrs(code);
	flag_update_zm(acc);
	if( (code & OP_TEST)==0 ) {
		reg.acc = acc;
	}
	return 0;
}
int f_OR  (int code,OPCODE *tab)
{
	int acc = reg.acc | opadrs(code);
	flag_update_zm(acc);
	if( (code & OP_TEST)==0 ) {
		reg.acc = acc;
	}
	return 0;
}
int f_XOR (int code,OPCODE *tab)
{
	int acc = reg.acc ^ opadrs(code);
	flag_update_zm(acc);
	if( (code & OP_TEST)==0 ) {
		reg.acc = acc;
	}
	return 0;
}
int f_LDV (int code,OPCODE *tab)
{
	int ea = vramadrs(code);
	flag_update_zm(ea);
	reg.acc = ea;
	return 0;
}


int f_STV (int code,OPCODE *tab)
{
	int ea = stadrs(code);
//	if(	ea >= 0 ) 
	{
		// VRAMストアを実行.
		vram[ea] = reg.acc;
		// ついでに、VRAM更新.
		VRAM_output(ea,reg.acc);
//		printf("STV	0x%x,0x%x\n",ea,reg.acc);
	}
	return 0;
}
int f_ST  (int code,OPCODE *tab)
{
	int ea = stadrs(code);
//	if(	ea >= 0 ) 
	{
		// 主記憶ストアを実行.
		memory[ea] = reg.acc;
	}
	return 0;
}
int f_OUT (int code,OPCODE *tab)
{
	int ea = opadrs(code);
	LED_output(reg.acc , ea);
//	printf("f_OUT(%x %x)\n",reg.acc , ea);
	LED_draw(reg.pc,1);
	return 0;
}
int f_SFR (int code,OPCODE *tab)
{
	int acc = reg.acc;		// シフト前の値をちょっとメモ.
	reg.acc >>= 1;			// 右シフト.

	flag_update_zm(reg.acc);
	reg.c = ( acc & 1 ) ;	// キャリーフラグはシフト前の値のLSB
	return 0;
}
int f_SFL (int code,OPCODE *tab)
{
	int acc = reg.acc;
	acc <<= 1;				// 左シフト.
	reg.acc =acc;

	flag_update_zcm(acc);	// フラグ更新のためには結果の32bit値を与える.
	return 0;
}
int f_IN  (int code,OPCODE *tab)
{
	int ea = stadrs(code);
//	if(	ea >= 0 ) 
	{
		// スイッチ値のストアを実行.
		memory[ea] = SW_inp;	//SW_input();
	}
	return 0;
}
int f_SCN (int code,OPCODE *tab)
{
	int ea = stadrs(code);
//	if(	ea >= 0 ) 
	{
		// JOY値のストアを実行.
		memory[ea] = JOY_input();
	}
	return 0;
}
int f_HLT(int code,OPCODE *tab)
{
#if	0
	return 1;	//終了.
#else
	int ea = reg.pc_bak - 1;
	reg.pc = ea;
	return 0;
#endif
}
//	未定義命令.
int f_und (int code,OPCODE *tab)
{
	return 0;
}

/** *********************************************************************************
 *
 ************************************************************************************
 */
