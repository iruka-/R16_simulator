/** *********************************************************************************
 *	R16 �A�Z���u��.
 ************************************************************************************
 */

/*
�s���c��

[O]	-	�s����
	-	���x��
	-	�j���j�b�N
	-	�I�y�����h
	-	���X�g�o��
	-	�o�C�i���o��

����:
	�Q�p�X�A�Z���u���Ȃ̂ŁA
	-	�P�p�X�ڂ̓A�h���X�m��̂ݍs�Ȃ�.
	-	�Q�p�X�ڂŊ��S�ȃo�C�i���[���m�肷��.
	
	-	�܂�A�P�p�X�ڂƂQ�p�X�ڂŁA���߂𗎂Ƃ��������ς���Ă͂Ȃ�Ȃ�
		�i�A�h���X������Ă͂Ȃ�Ȃ��j
	-	���ɁA�P�p�X�ڂ͖���`���x�����̂��߂ɃA�h���X�s�m��ƂȂ邯��ǂ�
		�Q�p�X�ڂƓ���̐U�镑�������Ȃ���΂Ȃ�Ȃ��i�����R�[�h������Ă��ǂ����A�h���X�������̂͂��߁j

 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "opcode.h"
#include "hash.h"

OPCODE opcode_init_tab[]={
 //�j��,�Ӗ�					,�@�B��		 ,f_emu,d_dis  //z,c,m,���� --------------------
 {"JMP","JMP always"            ,"00xxdd00"  ,f_JMP,0},//-,-,-,�������ɕ��򂷂�
 {"JBP","Jump if Not Blanking"  ,"00xxdd01"  ,f_JMP,0},//-,-,-,�A�����ԂłȂ���Ε���
 {"JNF","Jump if Not Full"      ,"00xxdd10"  ,f_JMP,0},//-,-,-,0xff�łȂ���͕���
 {"JF" ,"Jump if Full"          ,"00xxdd11"  ,f_JMP,0},//-,-,-,0xff�Ȃ番��
 {"JNC","Jump if Not Carry"     ,"01xxdd00"  ,f_JMP,0},//-,-,-,�L�����[��(�{���[�L)�Ȃ番��
 {"JC" ,"Jump if Carry"         ,"01xxdd01"  ,f_JMP,0},//-,-,-,�L�����[�L(�{���[��)�Ȃ番��
 {"JNM","Jump if Not Minus"     ,"01xxdd10"  ,f_JMP,0},//-,-,-,0�܂��̓v���X�Ȃ番��
 {"JM" ,"Jump if Minus"         ,"01xxdd11"  ,f_JMP,0},//-,-,-,�}�C�i�X�Ȃ番��
 {"LD" ,"LoaD"                  ,"10xx0000"  ,f_LD ,0},//?,0,?,���[�h
 {"LDT","LoaD Test"             ,"10xx0001"  ,f_LD ,0},//?,0,?,��L�̎����i�t���O�X�V�̂݁j
 {"SBC","SuB. with Borrow"      ,"10xx0010"  ,f_ADD,0},//?,?,?,�{���[�t���Z
 {"SCT","Sub. with Borrow Test" ,"10xx0011"  ,f_ADD,0},//?,?,?,��L�̎����i�t���O�X�V�̂݁j
 {"SUB","SUBtract"              ,"10xx0100"  ,f_ADD,0},//?,?,?,���Z
 {"SBT","SUBtract Test"         ,"10xx0101"  ,f_ADD,0},//?,?,?,��L�̎����i�t���O�X�V�̂݁j
 {"OR" ,"OR"                    ,"10xx0110"  ,f_ADD,0},//?,0,?,�_���a
 {"ORT","OR Test"               ,"10xx0111"  ,f_ADD,0},//?,0,?,��L�̎����i�t���O�X�V�̂݁j
 {"ADD","ADD"                   ,"10xx1000"  ,f_ADD,0},//?,?,?,���Z
 {"ADT","ADd Test"              ,"10xx1001"  ,f_ADD,0},//?,?,?,��L�̎����i�t���O�X�V�̂݁j
 {"AND","AND"                   ,"10xx1010"  ,f_ADD,0},//?,0,?,�_����
 {"ANT","ANd Test"              ,"10xx1011"  ,f_ADD,0},//?,0,?,��L�̎����i�t���O�X�V�̂݁j
 {"ADC","ADd with Carry"        ,"10xx1100"  ,f_ADD,0},//?,?,?,�L�����[�t���Z
 {"ACT","Add with Carry Test"   ,"10xx1101"  ,f_ADD,0},//?,?,?,��L�̎����i�t���O�X�V�̂݁j
 {"XOR","eXcusive OR"           ,"10xx1110"  ,f_ADD,0},//?,0,?,�r���I�_���a
 {"XOT","eXcusive Or Test"      ,"10xx1111"  ,f_ADD,0},//?,0,?,��L�̎����i�t���O�X�V�̂݁j
 {"LDV","LoaD from Vram"        ,"11xx000-"  ,f_LDV,0},//?,0,?,�r�f�I����������̃��[�h
 {"LDP","LoaD from PC"          ,"11xx001-"  ,f_LDP,0},//?,0,?,PC�i���̖��߂̃A�h���X�j+�u�l�v=>Acc
 {"ST" ,"STore"                 ,"11xx010-"  ,f_ST ,0},//-,-,-,�X�g�A
 {"SR" ,"ShiFt Right"           ,"11--011-"  ,f_SR ,0},//?,?,?,�E�V�t�g�i�I�y�����h�͖����j
 {"SL" ,"ShiFt Left"            ,"11--100-"  ,f_SL ,0},//?,?,?,���V�t�g�i�I�y�����h�͖����j
 {"IN" ,"IN"                    ,"11xx101-"  ,f_IN ,0},//-,-,-,�g�O���X�C�b�`�̏�Ԃ����
 {"OUT","OUT  "                 ,"11xx110-"  ,f_OUT,0},//-,-,-,Acc�Ǝw�胁�����l��LED�ɕ\��
 {"STV","STore to Vram"         ,"11xx1110"  ,f_STV,0},//-,-,-,�r�f�I�������ւ̃X�g�A
 {"HALT","HALT"                 ,"11xx1111"  ,f_HLT,0},//-,-,-,��~
// {"SCN","SCaN JOY stick port"   ,"11xx1111"  ,f_SCN,0},//-,-,-,�W���C�X�e�B�b�N���͂̃X�L����
// {"STP","STore Program counter" ,"11xx100-"  ,f_STP,0},//-,-,-,�v���O�����J�E���^�l��ۑ�
 //�[������
 {"ORG",".Set Origin"   ,"--------",f_ORG,0},
 {"EQU",".EQU" 		    ,"--------",f_EQU,0},
 {"DW" ,".DW" 		    ,"--------",f_DW ,0},
 {"DOT" ,".DOT" 	    ,"--------",f_DOT,0},
 {"END",".END" 		    ,"--------",f_END,0},
 {"MOV",".MOV"          ,"--------",f_MOV,0},
 {"CALL",".CALL"        ,"--------",f_CALL,0},
 {"CP", ".CP"           ,"--------",f_CP,0},
 {"INC",".INC"          ,"--------",f_INC,0},
 {"DEC",".DEC"          ,"--------",f_DEC,0},
 {"ENTER",".ENT"        ,"--------",f_ENT,0},
 {"LEAVE",".RET"        ,"--------",f_RET,0},
 {"SFR" ,".ShiFtRight"  ,"--------",f_SFR,0},//?,?,?,�E�V�t�g�i�I�y�����h�͖����j
 {"SFL" ,".ShiFtLeft"   ,"--------",f_SFL,0},//?,?,?,���V�t�g�i�I�y�����h 
//�j��,�Ӗ�				,�@�B��	   ,f_emu,d_dis  //z,c,m,���� --------------------
 { NULL,NULL,NULL}
};

FILE *ifp;
FILE *ofp;
FILE *lfp;

char  *s_infile;	//	�\�[�X�t�@�C����.
int	   lnum;		//	�s�ԍ�.
char   line[1024];
char  *lptr;
int		s_pass;
int		s_lopt;		// '-l' ?

#define Ropen(name) {ifp=fopen(name,"rb");if(ifp==NULL) \
{ printf("Fatal: can't open file:%s\n",name);exit(1);}  \
}
#define Wopen(name) {ofp=fopen(name,"wb");if(ofp==NULL) \
{ printf("Fatal: can't create file:%s\n",name);exit(1);}  \
}
#define Lopen(name) {lfp=fopen(name,"wb");if(lfp==NULL) \
{ printf("Fatal: can't create file:%s\n",name);exit(1);}  \
}

#define Write(buf,siz)  fwrite(buf,1,siz,ofp)

#define Wclose()  fclose(ofp)
#define Lclose()  fclose(lfp)
#define Rclose()  fclose(ifp)


void dw(int data)
{
	memory[reg.pc++]=data;
	
	if( save_ptr < reg.pc ) {
		save_ptr = reg.pc;
	}
}

void Wsave(void)
{
	Write(memory,save_ptr*2);
}

/** *********************************************************************************
 *	OPCODE�\���̂� pattern������ ���� data(���߃R�[�h)�����.
 ************************************************************************************
 */
void make_data(OPCODE *t)
{
	char *pat = t->pattern;
	int i,sbit;
	int m=0x8000;
	int code=0;
	for(i=0;i<8;i++,m>>=1) {
		sbit = *pat++;
		if(sbit == '1') {
			code |= m;
		}
	}
	t->data = code;

}
/** *********************************************************************************
 *
 ************************************************************************************
 */
void opcode_init()
{
	OPCODE *t = opcode_init_tab;
	while(t->mnemonic) {
		make_data(t);
		t++;
	}
}

/** *********************************************************************************
 *
 ************************************************************************************
 */
int	str_cmpi(char *t,char *s)
{
	while(*t) {
		if(tolower(*t)!=tolower(*s)) return 1;
		t++;
		s++;
	}
	if(*s) return 1;
	return 0;
}

/** *********************************************************************************
 *	
 ************************************************************************************
 */
void Error(char *format, ...)
{
	va_list argptr;
	char buffer[256];

	if(s_pass == 0) return;
	
	va_start(argptr, format);
	vsnprintf(buffer, sizeof(buffer), format, argptr);

	fprintf(stderr,"FATAL:%s\n",buffer);
	fprintf(stderr,"%s:%d:%s\n",s_infile,lnum,line);

	fprintf(lfp,"FATAL:%s\n",buffer);
	fprintf(lfp,"%s:%d:%s\n",s_infile,lnum,line);

	va_end(argptr);

//	exit(1);
}

/** *********************************************************************************
 *
 ************************************************************************************
 */
OPCODE *search_code(char *mne)
{
	OPCODE *t = opcode_init_tab;
	while(t->mnemonic) {
		if(str_cmpi(mne,t->mnemonic)==0) return t;
		t++;
	}
	return NULL;
}

//OPCODE UNDEFINED_OPCODE=
// {"???","???","--------",f_und,d_und};
//	xx	�A�h���b�V���O
//	00	Imm8
//	01	Reg8
//	10	[Reg8]
//	11	Imm16

//	dd	�f�B�X�v���C�X�����g
//	00	Abs
//	01	fwd
//	10	back
//	11	fwd+carry



/** *********************************************************************************
 *	�t�@�C��(fp)����P�s���̕������(buf)�ɓǂݍ���.
 ************************************************************************************
 *	���������0
 *	�������I�[�o�[=1
 *	EOF�ɒB�����EOF��Ԃ�.
 */
int getln(char *buf,FILE *fp)
{
	int c;
	int l;
	l=0;
	while(1) {
		c=getc(fp);
		if(c==EOF)  return(EOF);/* EOF */
		if(c==0x0d) continue;
		if(c==0x0a) {
			*buf = 0;	/* EOL */
			return(0);	/* OK  */
		}
		*buf++ = c;l++;
		if(l>=255) {
			*buf = 0;
			return(1);	/* Too long line */
		}
	}
}

/** *********************************************************************************
 *
 ************************************************************************************
 */
int	is_spc(int c)
{
	if(c == '\t') return 1;
	if(c == ' ') return 1;
	return 0;
}
/** *********************************************************************************
 *
 ************************************************************************************
 */
char *spskip(void)
{
	while(*lptr) {
		if(is_spc(*lptr)==0) break;
		lptr++;
	}
	return lptr;
}
/** *********************************************************************************
 *	�s�o�b�t�@����A�P����P�擾����.
 ************************************************************************************
 */
char *getsym(char *buf)
{
	char *t = buf;
	*t=0;
	spskip();
	while(*lptr) {
		if(is_spc(*lptr)) break;
		*t++ = *lptr++;
	}
	*t=0;
	return buf;
}
int is_comment(char *symbuf)
{
	int c = *symbuf;
	if((c==0)||(c==';')||(c=='#')) return 1;	//�R�����g.
	if((c=='/')&&(symbuf[1]=='/')) return 1;	//�R�����g.
	return 0;
}
/** *********************************************************************************
 *	�A�Z���u���F�P�s�̕����񂩂�A���x��: �j���j�b�N �I�y�����h�ɕ�������.
 ************************************************************************************
 */
int	asm_pre(char *line,int pass)
{
	int c,l;
//	printf("%d:%s\n",lnum,line);
	getsym(symbuf);
	c = *symbuf;
	if(is_comment(symbuf)) {return 0;}
	(void)c;

	l=strlen(symbuf);
	if((symbuf[l-1] == ':')||(is_spc(*line)==0)) {
		strcpy(label,symbuf);
		if(symbuf[l-1] == ':') {
			label[l-1]=0;			// ':'  �����.
		}
		getsym(symbuf);
	}
	
	if(is_comment(symbuf)) {return 1;}
	strcpy(mnemonic,symbuf);	//�j���j�b�N�擾.
	
	getsym(symbuf);
	if(is_comment(symbuf)) {return 2;}
	strcpy(operandbuf,symbuf);		//�I�y�����h�擾.

	return 3;
}

/** *********************************************************************************
 *	
 ************************************************************************************
 */
int	set_label(char *label,int val)
{
	hash_insert(sym,label,reg.pc,1);
	return 0;
}
void add_spc(char *buf,int spc)
{
	int len = strlen(buf);
	int i = spc - len;
	if( i <= 0) return;

	buf += len;
	while(i>=0) {
		*buf++ = ' ';
		i--;
	}
	*buf = 0;
}
/** *********************************************************************************
 *	
 ************************************************************************************
 */
int	asm_list(int pass)
{
	char hexbuf[256]="";
	char label1[256];
	char *t = hexbuf;
	WORD *d = &memory[reg.pc_bak];
	int  m = reg.pc - reg.pc_bak;
	int	i;

	if(pass == 0) return 0;		//�P�p�X�ƂQ�p�X�ŃA�h���X�������Ƃ��́A���̍s��
								//�R�����g�A�E�g�̏�ԂłQ�̃��X�g���o���Ă݂�.
	if(m>=12) m=12;
	for(i=0;i<m;i++) {
		sprintf(t,"%04x ",*d++);
		t+=5;
	}
	if(s_lopt) { add_spc(hexbuf,32);}

	strcpy(label1,label);
	if(label1[0]) {
		strcat(label1,":");
	}
	if((label1[0]==0)&&(mnemonic[0]==0)) {
		fprintf(lfp,"%5s %-10s%s\n","",hexbuf,line);
	}else{
		fprintf(lfp,"%04x:%-10s%-10s %-4s %s %s\n"
			,reg.pc_bak,hexbuf,label1,mnemonic,operandbuf,comment);
	}
	return 0;
}

int	op_exec(char *inst,char *oper)
{
	OPCODE *p = search_code(inst);
	if(p==NULL) {
		Error("Mnemonic error(%s)",inst);
	}else{
		operand = oper;
		p->emufunc(p->data,p);	//�j���j�b�N�̊e����.
	}
	return 0;
}
/** *********************************************************************************
 *	�A�Z���u���F�P�s�̏���.
 ************************************************************************************
 */
int	asm_line(char *line,int pass)
{
	int pre = asm_pre(line,pass);

	reg.pc_bak = reg.pc;

	strcpy(comment,lptr);
	if(pre<1) {
		return pre;				//�R�����g�����̍s.
	}
	
	set_label(label,reg.pc);	//���x��:
	if(mnemonic[0]==0) {
		return 0;				//���x��:  �����̍s.
	}

	op_exec(mnemonic,operandbuf);

	return 0;
}
/** *********************************************************************************
 *	
 ************************************************************************************
 */
void init_line(void)
{
	lptr = line;
		symbuf[0]  =0;
		label[0]   =0;
		mnemonic[0]=0;
		operandbuf[0] =0;
}
/** *********************************************************************************
 *	�A�Z���u���F�P�p�X�̏���.
 ************************************************************************************
 */
int	asm_pass(char *infile,int pass)
{
	int rc;
	lnum = 1;

	s_pass = pass;

	reg.pc=0;

	Ropen(infile);
	while(1) {
		rc = getln(line,ifp);
		if(rc == EOF) break;
		init_line();
		asm_line(line,pass);
		asm_list(pass);

		lnum++;
	}
	Rclose();
	return 0;
}

/** *********************************************************************************
 *	�A�Z���u���F�V���{���\��.
 ************************************************************************************
 */
int printsym_func(char *p,int v)
{
	char *is_reg="";
	if(v & IS_REG) is_reg="R";
	v &= ~IS_REG;
	fprintf(lfp,"%-16s = %s%d\t(0x%x)\n",p,is_reg,v,v);
	return 0;
}
void asm_printsym(void)
{
	fprintf(lfp,"\n\nSYMBOL LIST:\n");
	hash_iterate(sym,printsym_func);
}

/** *********************************************************************************
 *	�A�Z���u���F���C�����[�`��
 ************************************************************************************
 */
int	r16_asm(char *infile,char *outfile,char *listfile,int lopt)
{
	int i;
	sym = hash_create(0x1000);
	opcode_init();
	s_infile=infile;
	s_lopt = lopt;
	save_ptr=0;

	Lopen(listfile);
	Wopen(outfile);
	for(i=0;i<3;i++) {		//���̂Ƃ���K��.
		asm_pass(infile,0);
	}
	asm_pass(infile,1);
	Wsave();
	Wclose();

	asm_printsym();

	Lclose();
	return 0;
}
/** *********************************************************************************
 *
 ************************************************************************************
 */
