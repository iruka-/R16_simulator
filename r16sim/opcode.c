/** *********************************************************************************
 *	�I�y�R�[�h�̉��ߎ��s.
 ************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include "opcode.h"
#include "led.h"

void VRAM_output(int adrs,int data);
extern	int	SW_inp ;
/** *********************************************************************************
 *	Acc���ʂ���A�t���O z,c,m��S���X�V����.
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
 *	Acc���ʂ���A�t���O z,m���X�V����. c=0 �Œ�.
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
 *	�I�y�R�[�h����͂��āA�����A�h���X(ea) �����߂�.
 ************************************************************************************
 *	�Ƃ����A�����A�h���X�̕]�����ʁi�܂�I�y�����h�j��Ԃ�.
 */
int	opadrs(int code)
{
	int ea = 0;
	int xx = code & XX_MASK;
	// �A�h���b�V���O�C���q(xx)
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
//	�X�g�A�n���߂̂��߂̃X�g�A�Ԓn�̕]�������s����.	opadrs�Ɠ������.
int	stadrs(int code)
{
	return opadrs(code);
}

#if	0
//	�X�g�A�n���߂̂��߂̃X�g�A�Ԓn�̕]�������s����.
//	�߂�l���X�g�A���ׂ��Ԓn(0..0x0000ffff) �������� (-1)�X�g�A�o���Ȃ����߃p�^��.
int	stadrs(int code)
{
	int ea = 0;
	int xx = code & XX_MASK;
	// �A�h���b�V���O�C���q(xx)
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
 *	�I�y�R�[�h����͂��āA���������A�h���X(ea) �����߂�.
 ************************************************************************************
 *	����n�� XX �����߂�����Add�����߂��邱�ƂɂȂ��Ă���.
 */
int	jpadrs(int code)
{
	int ea = opadrs(code);
	int dd = code & DD_MASK;
	// �f�B�X�v���C�X�����g�C���q(dd)
	switch(dd) {
	 case DD_ABS :	break;
	 case DD_FWD :	ea = reg.pc + ea; break;
	 case DD_BACK:	ea = reg.pc - ea; break;
	 case DD_FWDC:	ea = reg.pc + ea + reg.c; break;
	 default:	break;
	}
	return ea & 0xffff;	// �A�h���X��16bit�𒴂��Ȃ��悤�ɂ���.
}
/** *********************************************************************************
 *	�I�y�R�[�h����͂��āAVRAM�����A�h���X(ea) �����߂�.
 ************************************************************************************
 *	�Ƃ����AVRAM�����A�h���X�̕]�����ʁi�܂�I�y�����h�j��Ԃ�.
 */
int	vramadrs(int code)
{
	int ea = 0;
	int xx = code & XX_MASK;
	// �A�h���b�V���O�C���q(xx)
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
 *	�������牺�́A���߂̎��s���s�Ȃ�.
 ************************************************************************************
 {"JMP","JMP always"  ,"00xxdd00"  ,f_JMP,d_JMP},//-,-,-,�������ɕ��򂷂�
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
	int blank = 1;	// ��.	���Ԃ�u�����L���O�҂����Ǝv���̂ŁA�u�����L���O�ɂ��Ă��܂��B
	if(	blank ) {
		reg.pc = ea;	//�����I�ɂ� JNB=JMP�ɂȂ��Ă��܂�.
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
		// PC�̒l���X�g�A.
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
//	int acc = reg.acc - opadrs(code) - reg.c;		//���ʂ�SBC���߂Ȃ�...
	int acc = reg.acc - opadrs(code) - (1-reg.c);	//�����Z����carry�̘_�����t�ɂȂ�.
	flag_update_zcm(acc);
	// SC/MP-II�̃{���[���G�~�����[�g.
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
		// VRAM�X�g�A�����s.
		vram[ea] = reg.acc;
		// ���łɁAVRAM�X�V.
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
		// ��L���X�g�A�����s.
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
	int acc = reg.acc;		// �V�t�g�O�̒l��������ƃ���.
	reg.acc >>= 1;			// �E�V�t�g.

	flag_update_zm(reg.acc);
	reg.c = ( acc & 1 ) ;	// �L�����[�t���O�̓V�t�g�O�̒l��LSB
	return 0;
}
int f_SFL (int code,OPCODE *tab)
{
	int acc = reg.acc;
	acc <<= 1;				// ���V�t�g.
	reg.acc =acc;

	flag_update_zcm(acc);	// �t���O�X�V�̂��߂ɂ͌��ʂ�32bit�l��^����.
	return 0;
}
int f_IN  (int code,OPCODE *tab)
{
	int ea = stadrs(code);
//	if(	ea >= 0 ) 
	{
		// �X�C�b�`�l�̃X�g�A�����s.
		memory[ea] = SW_inp;	//SW_input();
	}
	return 0;
}
int f_SCN (int code,OPCODE *tab)
{
	int ea = stadrs(code);
//	if(	ea >= 0 ) 
	{
		// JOY�l�̃X�g�A�����s.
		memory[ea] = JOY_input();
	}
	return 0;
}
int f_HLT(int code,OPCODE *tab)
{
#if	0
	return 1;	//�I��.
#else
	int ea = reg.pc_bak - 1;
	reg.pc = ea;
	return 0;
#endif
}
//	����`����.
int f_und (int code,OPCODE *tab)
{
	return 0;
}

/** *********************************************************************************
 *
 ************************************************************************************
 */
