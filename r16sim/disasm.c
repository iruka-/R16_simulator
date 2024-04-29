/** *********************************************************************************
 *	オペコードの逆アセンブル
 ************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include "opcode.h"

//	後続する16bit即値が存在する命令なら 1 を返す.
int	is_imm16(int code);


/** *********************************************************************************
//	ディスティネーション（オペランド）を文字列で出力.
 ************************************************************************************
 */
int	gen_dst_string(int code,char *dst)
{
	int ea = 0;
	int xx = code & XX_MASK;
	// アドレッシング修飾子(xx)
	switch(xx) {
	 case XX_IMM8 :	ea = code & 0xff;		 
	//	sprintf(dst,"#$%02x",ea);
		sprintf(dst,"%d",ea);
	 	break;
	 case XX_REG8 :	ea = code & 0xff;
	//	sprintf(dst,"$%02x",ea);
		sprintf(dst,"R%d",ea);
	 	break;
	 case XX_IND8 :	ea = code & 0xff;
	//	sprintf(dst,"[$%02x]",ea);
		sprintf(dst,"*R%d",ea);
	 	break;
	 case XX_IMM16:	ea = memory[ reg.pc_bak ];
	//	sprintf(dst,"$%04x",ea);
		sprintf(dst,"0x%04x",ea);
	 	break;
	 default:	break;
		sprintf(dst,"???");
	 	break;
	}
	return ea;
}

//
//	LoaDの実効アドレス はそのままAccに代入されるが、
//	Storeの実効アドレスはアドレスとして使用されるらしい・・・。
//
int	gen_store_string(int code,char *dst)
{
	int ea = 0;
	int xx = code & XX_MASK;
	// アドレッシング修飾子(xx)
	switch(xx) {
//	 case XX_IMM8 :	ea = code & 0xff;		 
//		sprintf(dst,"%d",ea);
//	 	break;

//	 case XX_REG8 :	ea = code & 0xff;
	 case XX_IMM8 :	ea = code & 0xff;
		sprintf(dst,"R%d",ea);
	 	break;
//	 case XX_IND8 :	ea = code & 0xff;
	 case XX_REG8 :	ea = code & 0xff;
		sprintf(dst,"*R%d",ea);
	 	break;
	 case XX_IND8 :	ea = code & 0xff;
		sprintf(dst,"**R%d",ea);
	 	break;
	 case XX_IMM16:	ea = memory[ reg.pc_bak ];
		sprintf(dst,"*0x%04x",ea);
	 	break;
	 default:	break;
		sprintf(dst,"???");
	 	break;
	}
	return ea;
}

/** *********************************************************************************
//	ディスティネーション（分岐先）を文字列で出力.
 ************************************************************************************
 */
int	gen_jmp_string(int code,char *dst)
{
	int ea = 0;
	int xx = code & XX_MASK;
	char jadrs[32]="";
	// アドレッシング修飾子(xx)
	switch(xx) {
	 case XX_IMM8 :	ea = code & 0xff;		 
	//	sprintf(jadrs,"$%02x",ea);
		sprintf(jadrs,"0x%02x",ea);
	 	break;
	 case XX_REG8 :	ea = code & 0xff;
	//	sprintf(jadrs,"[$%02x]",ea);
		sprintf(jadrs,"R%d",ea);
	 	break;
	 case XX_IND8 :	ea = code & 0xff;
	//	sprintf(jadrs,"[[$%02x]]",ea);
		sprintf(jadrs,"*R%d",ea);
	 	break;
	 case XX_IMM16:	ea = memory[ reg.pc_bak ];
	//	sprintf(jadrs,"$%04x",ea);
		sprintf(jadrs,"0x%04x",ea);
	 	break;
	 default:	break;
		sprintf(jadrs,"???");
	 	break;
	}
/** *********************************************************************************
 *	オペコードを解析して、分岐先実効アドレス(ea) を求める.
 ************************************************************************************
 *	分岐系は XX を解釈した後、ddを解釈することになっている.
 */
	int dd = code & DD_MASK;
	// ディスプレイスメント修飾子(dd)
	switch(dd) {
	 case DD_ABS :
	 	sprintf(dst,"%s",jadrs);break;
	 case DD_FWD :
	 	sprintf(dst,"+%s",jadrs);break;
	 case DD_BACK:
	 	sprintf(dst,"-%s",jadrs);break;
	 case DD_FWDC:
	 	sprintf(dst,"+%s+c",jadrs);break;
	 default:
	 	sprintf(dst,"%s",jadrs);break;
	}
	return ea;
}


int disasm(int code,OPCODE *tab)
{
	int pc = ( reg.pc_bak - 1 ) & 0xffff;
	char f='F';
//	char z='Z';
	char c='C';
	char m='M';

	char code2[40];
	char dst[80];
	
	code2[0] = 0;
	dst[0] = 0;
	
	if(is_imm16(code)) {	//後続IMM16フィールド有り?
		sprintf(code2,"%04x",memory[ reg.pc_bak ]);
	}
	if(code & 0x8000) {
		gen_dst_string(code,dst);	//JMP以外
	}else{
		gen_jmp_string(code,dst);	//JMP系
	}
//	if(reg.z==0) z = '_';
	if(reg.f==0) f = '_';
	if(reg.c==0) c = '_';
	if(reg.m==0) m = '_';
	
	printf("%04x: %04x %4s %-5s %-9s :%c%c%c A:0x%04x %s\n"
		,pc
		,code
		,code2
		,tab->mnemonic
		,dst

		,f,c,m
		,reg.acc
		,tab->comment
	);
	return 0;
}

int disStore(int code,OPCODE *tab)
{
	int pc = ( reg.pc_bak - 1 ) & 0xffff;
	char f='F';
	char c='C';
	char m='M';

	char code2[40];
	char dst[80];
	
	code2[0] = 0;
	dst[0] = 0;
	
	if(is_imm16(code)) {	//後続IMM16フィールド有り?
		sprintf(code2,"%04x",memory[ reg.pc_bak ]);
	}
	gen_store_string(code,dst);	// SToreのみ.

	if(reg.f==0) f = '_';
	if(reg.c==0) c = '_';
	if(reg.m==0) m = '_';
	
	printf("%04x: %04x %4s %-5s %-9s :%c%c%c A:0x%04x %s\n"
		,pc
		,code
		,code2
		,tab->mnemonic
		,dst

		,f,c,m
		,reg.acc
		,tab->comment
	);
	return 0;
}
/** *********************************************************************************
 *	
 ************************************************************************************
 */
int d_JMP (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_JBP (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_JM  (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_JNM (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_JF  (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_JNF (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_JE  (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_JNE (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_JC  (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_JNC (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_LD  (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_ADD (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_SUB (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_ADC (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_SBB (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_AND (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_OR  (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_XOR (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_LDP (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_LDV (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_STV (int code,OPCODE *tab)
{
	return disStore(code,tab);
}
int d_ST  (int code,OPCODE *tab)
{
	return disStore(code,tab);
}
int d_OUT (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
/*
int d_STP (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
*/
int d_SFR (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_SFL (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_IN  (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_SCN (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_HLT (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
int d_und (int code,OPCODE *tab)
{
	return disasm(code,tab);
}
/** *********************************************************************************
 *
 ************************************************************************************
 */
