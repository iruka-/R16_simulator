/** *********************************************************************************
 *	オペコードの解釈実行.
 ************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define Extern /* */

#include "opcode.h"
#include "hash.h"

/** *********************************************************************************
 *
 ************************************************************************************
 */
int	 set_label(char *label,int val);
void dw(int);
void Error(char *format, ...);
int	 op_exec(char *inst,char *operand);
extern int	 s_pass;

char *op1,*op2;
char opbuf[256];

#define	SPLIT_MAX 256
char *split_result[SPLIT_MAX];
char  split_string[1024];

//
//	文字列(s)に文字(a)が含まれていれば、その位置を文字数(>=0)で返す.
//	含まれていなければ -1 を返す.
int	str_index(char *s,int a)
{
	int c;
	int idx=0;
	while(1) {
		c = *s;
		if(c==0) return -1;
		if(c==a) return idx;
		s++;idx++;
	}
}
//
//	文字列(s)をカンマ(,)で複数に分解する.
//	-----------------------------------------------
//	分解された個数(最大256個) を戻り値にする.
//		分解後の文字列バッファは split_result[]
int	 split_str(char *s)
{
	strcpy(split_string,s);
	char *p=split_string;
	int cnt=0;
	int c;

	split_result[cnt++]=p;	//最初の位置.
	while(1) {
		c = *p;
		if(c==0) break;
		if(c==',') {
			*p=0;
			split_result[cnt++]=(p+1);	// ','の次の位置.
		}
		p++;
	}
	return cnt;
}

//
//	文字列(s)をカンマ(,)で２つまでに分解する.
//	-----------------------------------------------
//	分解された個数(最大2個) を戻り値にする.
//	分解された文字列は op1とop2になる.
int	 split_str2(char *s)
{
	char *p=opbuf;
	int c;
	if(str_index(s,',')<0) return 1;

	strcpy(opbuf,s);
	op1=opbuf;
	while(1) {
		c = *p;
		if(c==0) break;
		if(c==',') {
			*p=0;op2=&p[1];return 2;
		}
		p++;
	}
	op2=p;
	return 1;
}

//
//	asm命令(op) を cnt個生成する.
//
void op_execN(char *op,char *cnt)
{
	int n=1;
	int i;
	int rc = sscanf(cnt,"%d",&n);
	if(rc==0) {
		Error("OP cnt(%s)",cnt);
	}
	for(i=0;i<n;i++) {
		op_exec(op,"1");
	}
}
/** *********************************************************************************
 *	１文字(c)が１６進数なら、数値に変換する.
 ************************************************************************************
 *	エラーの場合は (-1)
 */
int	is_hex(int c)
{
	if((c>='0')&(c<='9')) return c-'0';
	if((c>='A')&(c<='F')) return c-'A'+10;
	if((c>='a')&(c<='f')) return c-'a'+10;
	return -1;
}
/** *********************************************************************************
 *	文字列(src)を数値、もしくは１６進数値に変換して(*val)に入れる.
 ************************************************************************************
 *	最後に'H'が付いているものだけ１６進数値に変換する.
 *
 *	成功すれば(1) 失敗すれば(0)を返す.
 */
int	is_numhex(char *src,int *val)
{
	int d=0,hex=0;
	int c,x,hexf=0;

	c = *src++;
	d = is_hex(c);hex=d;
	if(d>=10) hexf=1;

	if(d==(-1)) return 0;	// Error
	while(1) {
		c = *src++;
		if(c==0) {
			if(hexf==0) {
				*val = d;	// 10進.
				return 1;	// OK
			}
			return 0;		// Error
		}
		if((c=='h')||(c=='H')) {
			if(*src==0) {
				*val = hex;
				return 1;	// OK
			}
		}
		x = is_hex(c);
		if(x == (-1)) {
			return 0;		// Error
		}
		if(x>=10) hexf=1;
		d=d*10+x;
		hex=hex*16+x;
	}
}
/** *********************************************************************************
 *	オペランドを解析して、実効アドレス(*val) を求める.
 ************************************************************************************
 *	modeはアドレスモードを入れる.
 *	成功すれば真(非ゼロ)を返す.
 */
int eval(char *src,int *val,int *mode)
{
	int rc;
	static char srcbuf[256];
	int	len;
	char *s = srcbuf;
	int c;
//	int rp=0;
	strcpy(srcbuf,src);
	len=strlen(srcbuf);

	*mode = XX_IMM8;

	if(*s=='*') {
		*mode = XX_IND8;
		s++;
	}

	if(*s=='[') {
//		*mode = XX_REG8;
		*mode = XX_IND8;
		if(	srcbuf[len-1]==']') {
			srcbuf[len-1]=0;
		}
		s++;
	}

	c = tolower(*s);
	// $ffff
	if(c=='$') {
		rc = sscanf(s+1,"%x",val);
		return rc;
	}
	// r10h
	if((c=='r') && (is_numhex(s+1,val))){
		*val |= IS_REG;
		rc=1;
		if(	*mode != XX_IND8) {
			*mode  = XX_REG8;
		}
		return rc;
	}
	// 0xaaaa
	if((c=='0') && (s[1]=='x')) {
		rc = sscanf(s+2,"%x",val);
		return rc;
	}
	// 1234 || aaffh
	if(is_numhex(s,val)){
		rc=1;
		return rc;
	}
	//	label
	if( hash_search(sym,s,val) ) {
		if(	*val & IS_REG) {		// r16 などと等価なレジスタラベル.
			if(	*mode == XX_IMM8) {	// IMMのままだったら REGに書き換え.
				*mode  = XX_REG8;
			}
		}//						そうでない場合は定数ラベル.
		return 1;
	}else{
		if(s_pass) {
			Error("Symbol Not Found(%s)",s);	//未定義ラベル.
		}
		return 0;
	}
	return 0;
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
	 case XX_IMM8 :	ea = code & 0xff;		break;
	 case XX_REG8 :	ea = memory[code & 0xff];break;
	 case XX_IND8 :	ea = memory[code & 0xff];
	 				ea = memory[ea];		 break;
	 case XX_IMM16:	ea = memory[ reg.pc++ ]; break;
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
	int val=0,mode,rc,offset=0,rel=DD_ABS;
	rc = eval(operand,&val,&mode);
	//定数アドレスへの分岐.
	//常に相対分岐にする.
	offset = val - (reg.pc+1);
	if(offset>=0) {
		rel = DD_FWD;
	}else{
		rel = DD_BACK;
		offset = -offset;
	}
	if(offset < 256) {
		dw(code|rel|offset);
	}else{
		dw(code|DD_ABS|XX_IMM16);
		dw(val);
	}
	(void)rc;
	return 0;
}
int f_LD  (int code,OPCODE *tab)
{
	int val=0,mode,rc;
	rc = eval(operand,&val,&mode);
//	if(rc>0) 
	{
		if(val & 0xff00) {	// 16bit address
			code |= XX_IMM16;
			dw(code);dw(val);
		}else{
			code |= mode;
			dw(code|val);
		}
	}
	(void)rc;
	return 0;
}
int f_LDP (int code,OPCODE *tab)
{
	return f_LD(code,tab);
}
int f_LDV (int code,OPCODE *tab)
{
	return f_LD(code,tab);
}
int f_ST  (int code,OPCODE *tab)
{
	int val=0,mode,rc;
	rc = eval(operand,&val,&mode);
#if	1
	if(mode == 0x1000) mode = 0x0000;	// ST reg  -> imm
	if(mode == 0x2000) mode = 0x1000;	// ST *reg -> reg
#endif
	if(val & 0xff00) {	// 16bit address
//		Error("address size over 256:%s",operand);
		// IMM16
		dw(code);dw(val);
//		code |= mode;
//		dw(code|val);
	}else{				//  8bit address
		code |= mode;
		dw(code|val);
	}
	
	(void)rc;
	return 0;
}
int f_STV (int code,OPCODE *tab)
{
	return f_ST(code,tab);
}
int f_HLT (int code,OPCODE *tab)
{
	return f_ST(code,tab);
}

int f_OUT (int code,OPCODE *tab)
{
	return f_LD(code,tab);
}
int f_SR (int code,OPCODE *tab)
{
	return f_OUT(code,tab);
}
int f_SL (int code,OPCODE *tab)
{
	return f_OUT(code,tab);
}
int f_IN  (int code,OPCODE *tab)
{
	return f_OUT(code,tab);
}
int f_SCN (int code,OPCODE *tab)
{
	return f_OUT(code,tab);
}



//	擬似命令.
int f_ORG (int code,OPCODE *tab)
{
	int val=0,mode,rc;
	rc = eval(operand,&val,&mode);
	if(rc>0) {
		reg.pc = val;
		reg.pc_bak = val;
	}
	return 0;
}
int f_EQU (int code,OPCODE *tab)
{
	int val=0,mode,rc;
	rc = eval(operand,&val,&mode);
	if(rc>0) {
//		set_label(label,val);
		hash_insert(sym,label,val,1);
	}
	(void)rc;
	return 0;
}
int f_DW (int code,OPCODE *tab)
{
	int i,val,mode,rc;
	int cnt=split_str(operand);
	for(i=0;i<cnt;i++) {
		rc = eval(split_result[i],&val,&mode);
		if(rc>0) {
			dw(val);
		}else{
			Error("DW value(%s)",split_result[i]);
		}
	}
	return 0;
}
int f_DOT (int code,OPCODE *tab)
{
	int i,val,mode,rc,high=0;
	int cnt=split_str(operand);
	for(i=0;i<cnt;i++) {
		rc = eval(split_result[i],&val,&mode);
		if(rc>0) {
			if(i & 1) {
				dw(high | val);
			}else{
				high=(val<<4);
			}
		}else{
			Error("DW value(%s)",split_result[i]);
		}
	}
	if(cnt & 1) {
		dw(high | 0);
	}
	return 0;
}
int f_END (int code,OPCODE *tab)
{
	return 0;
}
int f_MOV (int code,OPCODE *tab)
{
	int n = split_str2(operand);
	if(n!=2) Error("Need 2 operand!");
	op_exec("ld",op2);		// src
	op_exec("st",op1);		// dst
	return 0;
}
int f_CP (int code,OPCODE *tab)
{
	int n = split_str2(operand);
	if(n!=2) Error("Need 2 operand!");
//	op2-op1にしているのは Borrowになっているから.
	op_exec("ld" ,op2);		// src
	op_exec("sub",op1);		// dst
#if	0	// Z判定出来ないので...
	op_exec("sub","1");		// dst
#endif
	return 0;
}
int f_INC (int code,OPCODE *tab)
{
	op_exec("ld" ,operand);		// dst
	op_exec("add","1");
	op_exec("st" ,operandbuf);		// dst
	return 0;
}
int f_DEC (int code,OPCODE *tab)
{
	split_str2(operand);
	op_exec("ld" ,operand);		// dst
	op_exec("sub","1");
	op_exec("st" ,operandbuf);		// dst
	return 0;
}

//	関数呼び出し:
int f_CALL (int code,OPCODE *tab)
{
#if	0
	op_exec("stp","r14");		// ra
#else
	op_exec("ldp","2");			// jmpの次.
	op_exec("st","r14");		// ra
#endif
	op_exec("jmp",operandbuf);
	return 0;
}

//	関数の先頭で行なう処理:
int f_ENT (int code,OPCODE *tab)
{
	// r15--;
	op_exec("ld","r15");
	op_exec("sub","1");
	op_exec("st","r15");

	// *r15 = ra;
	op_exec("ld","r14");
//	op_exec("add","1");
	op_exec("st","*r15");

	return 0;
}

//	関数の戻りで行なう処理:
int f_RET (int code,OPCODE *tab)
{
	// r14 = *r15;
	op_exec("ld","*r15");
	op_exec("st","r14");

	// r15++;
	op_exec("ld","r15");
	op_exec("add","1");
	op_exec("st","r15");

	// pc = ra;
#if	0
	op_exec("jmp","*r14");
#else
	op_exec("ldp","4");			// jmpの次の次.

	op_exec("st","r13");		// jmp書き換えptr.
	op_exec("ld","r14");		// ra --> acc
	op_exec("st","*r13");		// jmpオペランド書き換え.
	op_exec("jmp","65536");		// long jmp
#endif

	return 0;
}
int f_ADD (int code,OPCODE *tab)
{
	int n=split_str2(operand);
	if(n>=2) {
		char *mne = tab->mnemonic;
		op_exec("ld",op1);
		op_exec(mne ,op2);
		op_exec("st",op1);
	}else{
		f_LD(code,tab);
	}
	return 0;
}
int f_SFR (int code,OPCODE *tab)
{
	int n=split_str2(operand);
	if(n>=2) {
//		char *mne = tab->mnemonic;
		op_exec("ld",op1);
		op_execN("sr",op2);
		op_exec("st",op1);
	}else{
		f_LD(code,tab);
	}
	return 0;
}
int f_SFL (int code,OPCODE *tab)
{
	int n=split_str2(operand);
	if(n>=2) {
//		char *mne = tab->mnemonic;
		op_exec("ld",op1);
		op_execN("sl",op2);
		op_exec("st",op1);
	}else{
		f_LD(code,tab);
	}
	return 0;
}

/** *********************************************************************************
 *
 ************************************************************************************
 */
