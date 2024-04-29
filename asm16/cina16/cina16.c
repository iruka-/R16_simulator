/*
 *	Cina86 Copyright (C) Dolphinsoft 1989 Release 0.01
 *	makefile:	A>tcc -1 -Z -O cina
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#ifndef	_LINUX_
#include <dos.h>
#include <process.h>
#else
#define	P_WAIT	0
#define	spawnvp(x,y,z)	(0)
#endif


#define MAXLEV  16
#define MAXFOR  4096
#define MAXLINE 256
#define _IF_	1
#define _ELSE_	2
#define _CASE_  3
#define _WHILE_ 4
#define _DO_	5
#define _FOR_	6
#define _SWITCH_ 7

#define EOL  	0
#define SEMI    ';'
#define COLON   ':'
#define LABELC	'::'
#define PUBLAB	':|'
#define CALLER  '()'
#define ID      'a'
#define IND     'i'
#define NUM     '0'
#define LITERAL 'l'
#define PLUS    '+'
#define MINUS   '-'
#define DIV	'/'
#define MUL	'*'
#define ASSIGN  '='
#define AMPER   '&'
#define OR      '|'
#define XOR	'^'
#define NOT	'!'
#define EQUAL   '=='
#define NEQUAL  '!='
#define GREAT   '>'
#define GEQUAL  '>='
#define LEQUAL  '<='
#define LESS    '<'
#define SHR	'>>'
#define SHL	'<<'
#define ASPLUS  '+='
#define ASMINUS '-='
#define ASCPLUS '+<'
#define ASCMINUS '-<'
#define ASDIV   '/='
#define ASMUL   '*='
#define ASSHR	'>|'
#define ASSHL	'<|'
#define IPLUS   '++'
#define IMINUS  '--'
#define ASAMPER '&='
#define ASOR    '|='
#define ASXOR	'^='
#define REMLIN	'//'
#define REMSTART '/*'
#define CONDFLAGS 36

#define MAXOBJ  32
#define MAXNAME 256
#define NON   	0
#define EXE   	1
#define COM   	2
#define BASE  	0
#define MASM  	1
#define LINK  	2
#define EXE2BIN 3
#define delfile(fname) if ( midfile == 0 ) unlink(fname)
#define OP0(inst) {noop (inst) 	   ; 			return(1);}
#define OP1(inst) {oneop(inst,dst) ;			return(1);}
#define OP2(inst) {c=getstoken(src);twoop(inst,dst,src);return(1);}
#define ret(i)    {id=i;goto gret;}
#define	spskip(p) {while((*p==' ')||(*p=='\t')) p++;}

static	char *srcname;
static	char *asmname;
static  char *brklab[]= {"??","??","??","ca","ew","od","of","sw"};

static int getln();
#define	ZZ	printf("%s:%d: ZZ\n",__FILE__,__LINE__);

enum jnam {
	JO,JNO,	JB,JNB,
	JZ,JNZ,	JBE,JA,
	JS,JNS,	JF,JNF,
	JL,JGE,	JLE,JG,
	JMP,BRN,JC,JNC,
	JNAE,JAE,JE,JNE,
	JNL ,JNGE,JNBE,JNA,
	JNLE,JNG,JC_ ,JNC_,
	JP  ,JNP,JCXZ,LOOP
};

static char *jcontab[CONDFLAGS]= {
	"jo" ,"jno","jc" ,"jnc",
	"jf" ,"jnf","jbe","ja" ,
	"js" ,"jns","jf","jnf",
	"jl" ,"jge","jle","jg" ,
	"jmp","???","jc" ,"jnc",
	"jc","jnc","je" ,"jne",
	"jnl" ,"jnge","jnbe","jna",
	"jnle","jng","jc"  ,"jnc",
	"jp"  ,"jnp","dec cx\r\n\tjz","loop"
};

static char *flagtab[CONDFLAGS]= {
	"o" ,"no","b" ,"nb",
	"z" ,"nz","be","a" ,
	"s" ,"ns","f","nf",
	"l" ,"ge","le","g" ,
	"1"  ,"0","cy" ,"nc",
	"nae","ae","e"  ,"ne",
	"nl" ,"nge","nbe","na",
	"nle","ng","c"  ,"nc",
	"p"  ,"np","xloop","loop"
};
static 	char lnbuf[MAXLINE];
static  FILE *ifp,*ofp;
static  int  tokenid;
static  char token[MAXLINE];
static 	char *lp;
static 	int ugflg;
static 	int eoflg;
static  int lpos;
static 	int synerr;
static char obuf2[1024]="";
static char obuf[1024]="";
static char *obufp;
static int  ifstackp   =0;
static int  ifstack  [MAXLEV];
static int  ifstackid[MAXLEV];
static int  ifstackfor[MAXLEV];
static int  ifcount    =0;
static int  toplabel;
static int  topmark;

//static char iflabel[]="__if000";
static char ellabel[]="__el000";
static char filabel[]="__fi000";
static char whlabel[]="__wh000";
static char hwlabel[]="__ew000";
static char dolabel[]="__do000";
static char odlabel[]="__od000";
static char swlabel[]="__sw000";
static char calabel[]="__ca000";
static char folabel[]="__fo000";
static char oflabel[]="__of000";
static char  llabel[]="__??000";

static char switchvar[64]="";
static char formem[MAXFOR];
static char *formema[32];
static int  formemap;
static char *formemp;
static int  formemf;
static int  linenum;
static char nambuf[64];
static char numbuf[32];

static int  dflag = 0 ;
static int  rflag = 0 ;
static int  lflag = 0 ;
static int  tflag = 1 ;

static	char pname[MAXNAME];
static  char aname[MAXNAME];
static	char oname[MAXOBJ][MAXNAME];
static  char ename[MAXNAME];
static  char cname[MAXNAME];
static	int	objcnt = 0;
static	int	errcnt = 0;


/** *********************************************************************************
 *
 ************************************************************************************
 */

int		str_cmpi(char *t,char *s);
void	usage();
int		main( int argc,char *argv[]);
int		as();
int		cc();
int		as();
int		lk();
int		child( char *path );
void	renext(char *s,char *ext);
void	delext(char *s);
enum	TYPEXT ckext(char *s);
void	prologue();
void	epilogue();
void	mx();
int		cina( char *infile, char *outfile );
void	exprb();
void	expr1();
void	asmout(char *t,char *s);
int		expr();
int		isreg(char *s);
int		iszero(char *s);
int		conditional(int f);
void	twoop(char *op,char *dst,char *src);
void	oneop(char *op,char *dst);
int		is_jmp_full(char *op);
void	oneop_jnf(char *op,char *dst);
void	noop(char *op);
void	outlab(char *s);
void	underflow();
void	makelabel(char *s);
void	makellabel(char *s);
void	genlabel(char *s,int id);
void	stackdrop();
void	errsyntax(char *s1,char *s2);
void	crx();
void	cr();
void	tab();
void	pop_for_phrase();
void	push_for_phrase(char *s);
void	outs(char *s);
void	outc(int c);
void	outrem(char *s);
int		getstoken(char *s);
int		getstoken2(char *s);
int		getptoken(char *s);
int		getnptoken(char *s);
int		getbtoken(char *s);
int		peektoken(char *s);
int		gettoken(char *s);
int		remskips(int f);
void	outasm();
int		getid(char *s);
static	int getln();
int		getlnx();
void	seq(char *s);
int		nump(int *n);
int		isnum(int c);
int		isal(int c);
int		isan(int c);

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

enum TYPEXT {
	fNULL	= 0,
	fBASE	= 1,
	fASM	= 2,
	fOBJ	= 3,
	fEXE	= 4,
	fCOM	= 5,
	fELSE	= 6,
};
static char *extnam[6] = {
	"",
	".m",
	".asm",
	".obj",
	".exe",
	".com",
};
static int  pro_model= 0;
static int  pro_exec = MASM;
static int  midfile=0;
static const char *spargv[MAXOBJ+8];
void usage()
{
	printf(
	    "Syntax is: CINA [ options ] file[s]\n"
	    "	-S	generate assembly\n"
	    "	-e	generate executable\n"
	    "	-c	generate .com file \n"
	    "	-r	Insert remarks\n"
	    "	-v	View output\n"
	    "	-t	noTab\n"
	    "	-l	Linenumber\n"
	);
	exit( 1 );
}

//static enum TYPEXT ckext( char * );

int	main( int argc,char *argv[])
{
	int  i;
	printf("Cina80 Copyright (C) Dolphinsoft 1989 Release 0.01\n");
	printf("All rights reserved.\n");
	ename[0] = cname[0] = '\0';
	while (1) {
		if ( argc < 2 ) break;
		if ( argv[1][0] != '-' ) break;
		switch ( toupper( argv[1][1] ) ) {
		case 'C' :
			pro_model = COM;
			pro_exec = EXE2BIN;
			if ( argv[1][2] != '\0' ) {
				strcpy( cname, &argv[1][2] );
			}
			break;
		case 'E' :
			pro_model = EXE;
			pro_exec = LINK;
			if ( argv[1][2] != '\0' ) {
				strcpy( ename, &argv[1][2] );
			}
			break;
		case 'S' :
			pro_model = NON;
			pro_exec = BASE;
			break;
		case 'V' :
			dflag^=1;
			break;
		case 'R' :
			rflag^=1;
			lflag=0;
			break;
		case 'L' :
			lflag^=1;
			break;
		case 'T' :
			tflag^=1;
			break;
		default:
			usage();
		}
		argc--;
		argv++;
	}
	if ( argc < 2 ) usage();
	if ( ename[0] == '\0' ) strcpy( ename, argv[1] );
	if ( cname[0] == '\0' ) strcpy( cname, argv[1] );
	renext( ename, extnam[fEXE] );
	renext( cname, extnam[fCOM] );
	for ( i = 1; i < argc; i++ ) {
		strcpy( pname, argv[i] );
		switch ( ckext( pname ) ) {
		case fASM:
		case fOBJ:
			if ( pro_exec >= MASM ) {
				strcpy( aname, pname );
				errcnt += as();
			}
			break;
		case fEXE:
		case fCOM:
			printf( "Bad extention '%s'\n", pname );
			errcnt++;
			break;
		case fNULL:
			strcat( pname, extnam[fBASE] );
		default:
			if ( cc() != 0 ) {
				errcnt++;
				break;
			}
			if ( pro_exec >= MASM ) {
				if ( as() == 0 ) {
					delfile( aname );
				} else {
					errcnt++;
				}
				break;
			}
		}
	}
//	if ( ( errcnt == 0 ) && ( pro_exec >= LINK ) ) errcnt += lk();
	exit( errcnt );
}
int as()
{
	int errlvl = 0;
	static int objover = 0;
	if (objcnt >= MAXOBJ ) {
		objover++;
		printf( "Too many .obj files.(%d)\n", objcnt + objover );
		return( 1 );
	}
	if ( ckext( aname ) != fOBJ ) {
		spargv[1] = "-l";
		spargv[2] = aname;
		spargv[3] = NULL;
		errlvl = ( child("asm16") != 0 );
		if(errlvl) {
			printf("SPAWN ERROR(%d):asm16.exe\n",errlvl);
		}
	}
	strcpy( oname[objcnt], aname );
	renext( oname[objcnt], ".obj" );
	objcnt++;
	return( errlvl );
}


int cc()
{
	int errlvl = 0;
	printf( "%s\n", pname );
	if ( dflag == 0 ) {
		strcpy( aname, pname );
		renext( aname, extnam[fASM] );
	} else {
		strcpy( aname, "con" );
	}
	if( strcmp( pname, aname ) == 0 ) {
		printf( "Both file are same.\n" );
		errlvl = 1;
	} else {
		errlvl = ( cina( pname, aname ) != 0 );
	}
	return( errlvl );
}
#if	0
int as()
{
	int errlvl = 0;
	static int objover = 0;
	if ( objcnt >= MAXOBJ ) {
		objover++;
		printf( "Too many .obj files.(%d)\n", objcnt + objover );
		return( 1 );
	}
	if ( ckext( aname ) != fOBJ ) {
		spargv[1] = "/ml";
		spargv[2] = aname;
		spargv[3] = ";";
		spargv[4] = NULL;
		errlvl = ( child("optasm") != 0 );
	}
	strcpy( oname[objcnt], aname );
	renext( oname[objcnt], ".obj" );
	objcnt++;
	return( errlvl );
}
int lk()
{
	int errlvl = 0;
	int i;
	for ( i = 0; i < objcnt; i++ ) {
		spargv[i+1] = oname[i];
	}
	spargv[i+1] = ",";
	spargv[i+2] = ename;
	spargv[i+3] = ",NUL;";
	spargv[i+4] = NULL;
	errlvl = ( child("tlink") != 0 );
	return( errlvl );
}
#endif


int child( char *path )
{
	int i;
	spargv[0] = path;
	if ( dflag == 0 ) {
		return( spawnvp( P_WAIT, path, spargv ) );
	} else {
		i = 0;
		while ( spargv[i] != NULL ) {
			printf( "%s ", spargv[i++] );
		}
		printf( "\n" );
		return( 0 );
	}
}

void renext(char *s,char *ext)
{
	delext( s );
	strcat( s, ext );
}
void delext(char *s)
{
	char *p;
	p = NULL;
	while( *s != '\0' ) {
		if ( *s == '.' ) p = s;
		s++;
	}
	if( ( p != NULL ) && ( ( s - p ) < 5 ) ) {
		*p = '\0';
	}
}
enum TYPEXT ckext(char *s)
{
	char *p;
	enum TYPEXT i;
	p = NULL;
	while( *s != '\0' ) {
		if ( *s == '.' ) p = s;
		s++;
	}
	if ( p == NULL ) return( fNULL );
	for ( i = fNULL; i < fELSE; i++ ) {
//		if ( stricmp( p, extnam[i] ) == 0 ) break;
		if ( str_cmpi( p, extnam[i] ) == 0 ) break;
	}
	return( i );
}
void prologue()
{
	obufp=obuf;
	outs("\r\n");
}
void epilogue()
{
	outs("\r\n");
}
void mx()
{
	while(!eoflg) expr();
}
int cina( char *infile, char *outfile )
{
	srcname = infile;
	asmname = outfile;
	ugflg = eoflg = synerr = 0;
	tokenid = -1;
	token[0] = '\0';
	formemp = formem;
	linenum = formemf = formemap= 0;
	ifp = fopen( srcname, "rb" );
	if ( ifp == NULL ) {
		printf( "cannot open '%s'.\n", srcname );
		return( 2 );
	}
	ofp = fopen( asmname,"wb");
	if( ofp == NULL ) {
		printf( "cannot open '%s'.\n", asmname );
		return( 2 );
	}
	strcpy( nambuf, ";~" );
	strcat( nambuf, srcname );
	strcat( nambuf, "(" );
	prologue();
	mx();
	epilogue();
	fclose( ifp );
	fclose( ofp );
	if ( synerr ) {
		printf( "\n %d Error(s)\n" , synerr );
		return( 1 );
	}
	return( 0 );
}
void exprb()
{
	static char buf[MAXLINE];
	if(peektoken(buf)=='{') {
		gettoken(buf);
		while(!eoflg) {
			if(peektoken(buf)=='}') {
				gettoken(buf);
				return;
			}
			expr();
		}
	} else expr1();
}
void expr1()
{
	static char buf[MAXLINE];
	while(!eoflg) {
		if(expr()) {
			if(eoflg)return;
			if(peektoken(buf)==';') expr();
			return;
		}
	}
}
void asmout(char *t,char *s)
{
	outs(t);
	outs(s);
	crx();
	while(*lp)lp++;
}
int	expr()
{
	int c;
	int jcond;
	char dst[MAXLINE],src[MAXLINE];
	if(lp==NULL) {
		if(getln()==0) return(0);
	}
	spskip(lp);
	if(*lp==0) {
		if(getln()==0) return(0);
		spskip(lp);
	}
	c=peektoken(dst) ;
	c=getstoken(dst) ;
	if (c==';') return(1);
	if(c==ID) {
		if(strcmp("goto",dst)==0) {
			c=getstoken(src);
			oneop("jmp",src);
			return(1);
		}
		if(strcmp("return",dst)==0) {
			strcpy(dst,"ret");
		}
		if(strcmp("if",dst)==0) {
			seq("(");
			jcond=conditional(0);
			seq(")");
			c=peektoken(src);
			if(strcmp("break",src)==0) {
				c=gettoken(src);
				makellabel(llabel);
				jcond ^=1;
				oneop_jnf(jcontab[jcond],llabel);
				return(1);
			}
			if(strcmp("goto",src)==0) {
				c=gettoken(src);
				c=getstoken(src);
				jcond ^=1;
				oneop(jcontab[jcond],src);
				return(1);
			}
			genlabel(ellabel,_IF_);
			oneop(jcontab[jcond],ellabel);
			exprb();
			if(peektoken(dst)==ID) {
				if(strcmp(dst,"else")==0) {
					gettoken(dst) ;
					makelabel(filabel);
					oneop("jmp",filabel);
					makelabel(ellabel);
					outlab(ellabel);
					ifstackid[ifstackp]=_ELSE_;
					exprb();
					makelabel(filabel);
					outlab(filabel);
					stackdrop();
					return(1);
				}
			}
			makelabel(ellabel);
			outlab(ellabel);
			stackdrop();
			return(1);
		}
		if(strcmp("while",dst)==0) {
			genlabel(whlabel,_WHILE_);
			outlab(whlabel);
			seq("(");
			jcond=conditional(0);
			seq(")");
			makelabel(hwlabel);
			if(jcond!=BRN) oneop_jnf(jcontab[jcond],hwlabel);
			exprb();
			makelabel(whlabel);
			oneop("jmp",whlabel);
			makelabel(hwlabel);
			outlab(hwlabel);
			stackdrop();
			return(1);
		}
		if(strcmp("for",dst)==0) {
			seq("(");
			exprb();
			genlabel(folabel,_FOR_);
			outlab(folabel);
			jcond=conditional(0);
			seq(";");
			makelabel(oflabel);
			if(jcond!=BRN) oneop_jnf(jcontab[jcond],oflabel);
			formemf=1;
			exprb();
			formemf=0;
			seq(")");
			exprb();
			pop_for_phrase();
			makelabel(folabel);
			oneop("jmp",folabel);
			makelabel(oflabel);
			outlab(oflabel);
			stackdrop();
			return(1);
		}
		if(strcmp("do",dst)==0) {
			genlabel(dolabel,_DO_);
			outlab(dolabel);
			exprb();
			seq("while");
			seq("(");
			jcond=conditional(1);
			seq(")");
			makelabel(dolabel);
			jcond^=1;
			if(jcond!=BRN) oneop_jnf(jcontab[jcond],dolabel);
			makelabel(odlabel);
			outlab(odlabel);
			stackdrop();
			return(1);
		}
		if(strcmp("switch",dst)==0) {
			genlabel(calabel,_SWITCH_);
			genlabel(calabel,_CASE_);
			seq("(");
			getstoken(switchvar);
			seq(")");
			exprb();
			if(ifstackid[ifstackp]==_CASE_) {
				makelabel(calabel);
				outlab(calabel);
				stackdrop();
			}
			makelabel(swlabel);
			outlab(swlabel);
			stackdrop();
			return(1);
		}
		if(strcmp("case",dst)==0) {
			makelabel(calabel);
			outlab(calabel);
			stackdrop();
			genlabel(calabel,_CASE_);
			c=getptoken(src);
			seq(":");
			twoop("cp",switchvar,src);
//			oneop("jnz",calabel);
			oneop_jnf("jnf",calabel);

			return(1);
		}
		if(strcmp("default",dst)==0) {
			makelabel(calabel);
			outlab(calabel);
			stackdrop();
			seq(":");
			return(1);
		}
		if(strcmp("break",dst)==0) {
			makellabel(llabel);
			oneop("jmp",llabel);
			return(1);
		}
		spskip(lp);
		if(*lp==0)OP0( dst )
			c=getstoken2(src);
		if(c==ASSIGN) {
			c=getstoken(src);
			if(strcmp(src,dst)!=0) {
//					twoop("ld",dst,src);
				twoop("mov",dst,src);
				return(1);
			}
			c=gettoken(src);
			switch(c) {
			case PLUS:
				c=ASPLUS ;
				break;
			case MINUS:
				c=ASMINUS;
				break;
			case DIV:
				c=ASDIV  ;
				break;
			case MUL:
				c=ASMUL  ;
				break;
			case OR:
				c=ASOR   ;
				break;
			case AMPER:
				c=ASAMPER;
				break;
			case XOR:
				c=ASXOR  ;
				break;
			default:
				errsyntax("Both Operand are the same",src);
				return(0);
			}
		}
		if((c==ID)||(c==NUM)||(c==LITERAL)||(c=='<')) {
			if ( toplabel==0) tab();
			else toplabel=0;
			outs(dst);
			tab();
			outs(src);
			while(1) {
				if(*lp==' ') outs(" ");
				if(*lp=='\t') outs("\t");
				spskip(lp);
				if(*lp==0) {
					cr();
					return(1);
				}
				c=gettoken(src);
				if((c==0)||(c==EOL)||(c==SEMI)) {
					cr();
					return(1);
				}
				outs(src);
			}
		}
		switch(c) {
		case PUBLAB:
			oneop("public",dst);
		case LABELC:
			outlab(dst);
			toplabel=0;
			return(1);
		case SEMI:
			lp--;
			OP0( dst )
		case ASPLUS:
			OP2("add")
		case ASMINUS:
			OP2("sub")
		case ASCPLUS:
			OP2("adc")
		case ASCMINUS:
			OP2("sbc")
		case ASAMPER:
			OP2("and")
		case ASOR:
			OP2("or" )
		case ASXOR:
			OP2("xor")
		case IPLUS:
			OP1("inc")
		case IMINUS:
			OP1("dec")
		case CALLER:	/*OP1("call")*/
			if(toplabel) {
				//oneop("public",dst);
				if(gettoken(src)!='{') {
					errsyntax("Missing func","{");
				}
				outlab(dst);
#if	1
				noop("enter");
#endif

				while(!eoflg) {
					c=peektoken(src);
					if(c=='}') break;
					expr();
				}
				gettoken(src);
#if	1
				noop("leave");
#else
				noop("ret");
#endif
				return(1);
			} else {
				oneop("call",dst) ;
				return(1);
			}
		case SHR:
			seq("=");
			OP2("sfr")
		case SHL:
			seq("=");
			OP2("sfl")
		case ASMUL:
			c=getstoken(src);
			oneop("mul",src);
			return(1);
		case ASDIV:
			c=getstoken(src);
			oneop("div",src);
			return(1);
		default:
			errsyntax("Illegal Operator",src);
			return(0);
		}
	} else {
		switch(c) {
		case 0:
			return(0);
		case '#': {
			asmout("\t",lp);
			return(0);
		}
		case '.': {
			asmout("\t.",lp);
			return(0);
		}
		default:
			errsyntax("Missing Identifier",dst);
			return(0);
		}
	}
}
int	isreg(char *s)
{
	if(str_cmpi(s,"acc")==0) return 1;
	return 0;
#if	0
	register int c1,c2;
	if ( s[2] != 0) return(0);
	c1= (s[0] & 0xdf);
	c2= (s[1] & 0xdf);
	if( (c2 == 'X')||(c2 == 'H')||(c2 == 'L')) {
		if ( (c1 == 'A')||(c1 =='B')||(c1 =='C')||(c1=='D') )
			return(1);
	}
	if( c2 =='I') {
		if( (c1=='S')||(c1=='D') )
			return(1);
	}
	return(0);
#endif
}
int	iszero(char *s)
{
	if( (s[0]=='0')&&(s[1]==0) ) return(1);
	return(0);
}
int	conditional(int f)
{
	int c1,c2;
	int jc,i,revcon;
	char op1[MAXLINE],op2[MAXLINE],op3[MAXLINE];
	revcon=0;
	c1=peektoken(op1);
	if(c1=='{') exprb();
	else if(c1==NOT) {
		c1=gettoken(op1);
		revcon ^=1;
	};
	c1=peektoken(op1);
	if(c1==IMINUS) {
		gettoken(op1);
		getstoken(op1);
		if(f) {
			if( (strcmp("cx",op1)==0) || (strcmp("CX",op1)==0) ) {
				return(JCXZ ^revcon);
			}
		}
		oneop("dec",op1);
		c2=peektoken(op2);
		if( (c2== ')')||(c2== ';') ) {
//			oneop("sub","1");			//
			return(JZ ^revcon);			// JF!
		} else goto _label2;
	}
	getstoken(op1);
	c2=peektoken(op2);
	if( (c2 == ')') || (c2 == ';') ) {
		for(i=0; op1[i]; i++) op1[i]=tolower(op1[i]);
		for(i=0; i<CONDFLAGS; i++) {
			if(strcmp(op1,flagtab[i])==0) {
				return(i ^ 1 ^revcon);
			}
		}
		errsyntax("Illegal condition",op1);
		return(BRN^revcon);
	}
	if(c2 == '.') {
		c2=gettoken(op2);
		c2=gettoken(op2);
		c2=gettoken(op3);
		for(i=0; op1[i]; i++) op2[i]=tolower(op2[i]);
		for(i=0; i<CONDFLAGS; i++) {
			if(strcmp(op2,flagtab[i])==0) {
				getstoken(op3);
				twoop("cp",op1,op3);
				return(i ^ 1^revcon);
			}
		}
		errsyntax("Illegal condition",op1);
		return(BRN^revcon);
	}
_label2:
	c2=gettoken(op2);
	getstoken(op3);
	switch(c2) {
	case EQUAL :
		jc=JNZ;
		goto _or;
	case NEQUAL:
		jc=JZ ;
		goto _or;
	case GEQUAL:
		jc=JB ;
		goto _cmp;
	case LEQUAL:
		jc=JA ;
		goto _cmp;
	case GREAT :
		jc=JBE;
		goto _cmp;
	case LESS  :
		jc=JAE;
		goto _cmp;
	case AMPER :
		jc=JZ ;
		goto _test;
	default:
		errsyntax("Illegal compare operator",op2);
		return(BRN^revcon);
	}
_or:
	if( isreg(op1) && iszero(op3) ) {
		twoop("or",op1,op1);
		return(jc^revcon);
	}
_cmp:
	twoop("cp",op1,op3);
	return(jc^revcon);
_test:
	twoop("test",op1,op3);
	return(jc^revcon);
}
void twoop(char *op,char *dst,char *src)
{
	tab();
	outs(op);
	tab();
	outs(dst),outs(",");
	outs(src);
	cr();
}
void oneop(char *op,char *dst)
{
	tab();
	outs(op);
	tab();
	outs(dst);
	cr();
}

//	jf,jnfなら1
int	is_jmp_full(char *op)
{
	if(strcmp(op,"jnf")==0) return 1;
	if(strcmp(op,"jf")==0) return 1;
	return 0;
}

//	zeroフラグがないので、fullで代用する.
void oneop_jnf(char *op,char *dst)
{
	if(is_jmp_full(op)) {
		oneop("sub","1");
	}
	oneop(op,dst);
}

void noop(char *op)
{
	tab();
	outs(op);
	cr();
}

void outlab(char *s)
{
	outs(s);
	outs(":");
	cr();
}

void underflow()
{
	errsyntax(" } stack underflow.","");
}
void makelabel(char *s)
{
	sprintf(s+4,"%03d",ifstack[ifstackp]);
}
void makellabel(char *s)
{
	int i;
	i=ifstackp;
	while(i>0) {
		if(ifstackid[i]>=_WHILE_) {
			strcpy(s+2,brklab[ifstackid[i]]);
			sprintf(s+4,"%03d",ifstack[i]);
			return;
		}
		i--;
	}
	errsyntax("Misplaced" ,"break;");
}
void genlabel(char *s,int id)
{
	if(ifstackp >=MAXLEV) {
		errsyntax(" FATAL> ","NESTING LEVEL OVERFLOW(16)");
		exit(9);
	}
	ifstackp++;
	ifstackid[ifstackp]=id;
	ifstack  [ifstackp]=ifcount;
	ifstackfor[ifstackp]=formemap;
//	makelabel(s,ifcount);
	makelabel(s);
	ifcount++;
}
void stackdrop()
{
	ifstackp--;
	if(ifstack<0) {
		errsyntax(" FATAL> ","NESTING LEVEL UNDERFLOW");
		exit(9);
	}
}
void errsyntax(char *s1,char *s2)
{
	printf("%s(%d):*** Error %s (%s)***\n",srcname,linenum,s1,s2);
	while(*lp) lp++;
	synerr++;
}
void crx()
{
	outs("\r\n");
}
void cr()
{
	if(!lflag) {
		outs("\r\n");
		return;
	}
	if(tflag) {
		while(lpos<32) outs("\t");
	}
	outs(nambuf);
	sprintf(numbuf,"%d)\r\n",linenum);
	outs(numbuf);
}
void tab()
{
	if(tflag) outs("\t");
	else outs(" ");
}
void pop_for_phrase()
{
	int i;
	for(i=ifstackfor[ifstackp]; i<formemap; i++) {
		outs(formema[i]);
	}
	formemap=ifstackfor[ifstackp];
	formemp =formema[formemap];
}
void push_for_phrase(char *s)
{
//	int i;
	char *p;
	p=formemp;
	formema[formemap++]=formemp;
	formemp += (strlen(s)+1);
	if(formemp< &formem[MAXFOR] )
		strcpy(p,s);
	else {
		errsyntax(" FATAL> "," 'FOR' STACK OVERFLOW");
		exit(9);
	}
}
void outs(char *s)
{
	while(*s) outc(*s++);
}
void outc(int c)
{
	if(c!='\n') {
		*obufp++ = c;
		if(c=='\t') {
			lpos &= 0xff8;
			lpos +=     8;
		} else lpos++;
		return;
	}
	*obufp++ =c;
	*obufp   =0;
	lpos    =0;
	obufp	 =obuf;
	if(formemf) {
		push_for_phrase(obuf);
		return;
	}
	if(obuf2[0]) fprintf(ofp,"%s",obuf2);
	strcpy(obuf2,obuf);
}
void outrem(char *s)
{
	if(obuf2[0]) fprintf(ofp,"%s",obuf2);
	strcpy(obuf2,s);
}

//セグメント付きトークン.
int	getstoken(char *s)
{
	int c,c1,c2;
//	char buf[MAXLINE];
	c=getnptoken(s);
	c1=*lp;
	c2=lp[1];
	if( c==IND ) return(ID);
	if( (c==ID) && (c1==COLON) ) {
		if( (c2>' ')&&(c2!=COLON) ) {
			while(*s)s++;
			getbtoken(s);
			while(*s)s++;
			getptoken(s);
			c=ID;
		}
	}
	c1=*lp;
	if(( (c==ID) || (c==NUM) ) && (c1=='[') ) {
		while(*s)s++;
		getptoken(s);
		return(ID);
	}
	return(c);
}
//セグメント付きトークン.
int	getstoken2(char *s)
{
	int c,c1;
//	char buf[MAXLINE];
	c=getptoken(s);

//	printf("getstoken2:%s\n",s);

	c1=*lp;
	if( c==IND ) return(ID);
	c1=*lp;
	if(( (c==ID) || (c==NUM) ) && (c1=='[') ) {
		while(*s)s++;
		getptoken(s);
		return(ID);
	}
	return(c);
}

// ( expr ) を得る  (結果はexprになる)
int	getptoken(char *s)
{
	int c;
	c=getbtoken(s);
	if(c=='(') {
		while(*lp) {
			c=getnptoken(s);
			if(c==')') {
				*s=0;
				return(IND);
			}
			while(*s)s++;
			if((*lp==' ')||(*lp=='\t')) {
				*s++=*lp++;
				*s=0;
			}
		}
		errsyntax(" Missing ",")");
		return(0);
	}
	return(c);
}

// (hl) を得る.
int	getnptoken(char *s)
{
	int c;
	c=getbtoken(s);
	if(c=='(') {
		while(*lp) {
			while(*s)s++;
			c=getnptoken(s);
			while(*s)s++;
			if(c==')') {
				*s=0;
				return(IND);
			}
			while(*s)s++;
			if((*lp==' ')||(*lp=='\t')) {
				*s++=*lp++;
				*s=0;
			}
		}
		errsyntax(" Missing ",")");
		return(0);
	}
	return(c);
}
int	getbtoken(char *s)
{
	int c;
	c=gettoken(s);
	if(c=='[') {
		while(*lp) {
			while(*s)s++;
			c=gettoken(s);
			while(*s)s++;
			if((*lp==' ')||(*lp=='\t')) {
				*s++=*lp++;
				*s=0;
			}
			if(c==']') return(IND);
		}
		errsyntax(" Missing ","]");
		return(0);
	}
	return(c);
}
int	peektoken(char *s)
{
	int id;
	if(tokenid!=-1) {
		strcpy(s,token);
		id=tokenid;
		return(id);
	}
	tokenid=gettoken(token);
	strcpy(s,token);
	return(tokenid);
}
int	gettoken(char *s)
{
	int c,t,id;
	if(tokenid!=-1) {
		strcpy(s,token);
		id=tokenid;
		tokenid=-1;
		return(id);
	}
GT0:
	c=getid(s);
//GT1:
	while(c==0) {
		if (getln()==0) return(0);
		c=getid(s);
	}
	t=*lp;
	switch (c) {
	case PLUS:
		if(t == '+') ret(IPLUS);
		if(t == '=') ret(ASPLUS);
		if(t == '<') ret(ASCPLUS);
		break;
	case MINUS:
		if(t == '-') ret(IMINUS);
		if(t == '=') ret(ASMINUS);
		if(t == '<') ret(ASCMINUS);
		break;
	case ASSIGN:
		if(t == '=') ret(EQUAL);
		break;
	case AMPER:
		if(t == '=') ret(ASAMPER);
		break;
	case OR:
		if(t == '=') ret(ASOR);
		break;
	case XOR:
		if(t == '=') ret(ASXOR);
		break;
	case GREAT:
		if(t == '=') ret(GEQUAL);
		if(t == '>') ret(SHR);
		break;
	case LESS:
		if(t == '=') ret(LEQUAL);
		if(t == '<') ret(SHL);
		break;
	case NOT:
		if(t == '=') ret(NEQUAL);
		break;
	case '(':
		if( t ==')') ret(CALLER);
	case DIV:
		if(t == '/') {
			remskips(REMLIN)  ;
			goto GT0;
		}
		if(t == '*') {
			remskips(REMSTART);
			goto GT0;
		}
		break;
	case COLON:
		if( (t == ' ') || (t == '\t') ||(t == 0) ) {
			return(LABELC);
		}
		if(  t == ':'  ) ret(PUBLAB);
		break;
	case '{':
		if( t =='*') {
			outasm();
			return(0);
		}
	default:
		break;
	}
	return(c);
gret:
	s++;
	*s++=*lp++;
	*s=0;
	return(id);
}
int remskips(int f)
{
	lp++;
	{
		while(1) {
			if(*lp==0) {
				if (getln()==0) return(0);
				if (f==REMLIN)  return(0);
			}
			if( (*lp=='*') && (lp[1]=='/') ) {
				lp+=2;
				break;
			}
			lp++;
		}
		if(*lp==0) getln();
	}
	return 0;
}
void outasm()
{
	while(1) {
		if(getln()==0) return;
		if( (*lp=='*')&&(lp[1]=='}') ) {
			lp+=2;
			return;
		}
		outs(lp);
		crx();
	}
}
int getid(char *s)
{
	char *p,c;
	p=s;
	*p=0;
	spskip(lp);
	c= *lp;
	if(c==0) return(0);
	if((c=='\'')||(c=='\"')) {
		while( *lp ) {
			*p++ = *lp++;
			if(*lp==c) {
				lp++;
				break;
			}
		}
		*p++=c;
		*p=0;
		return(LITERAL);
	}
	if(c=='$') {
		lp++;
		*p++='0';
		while( *lp ) {
			if(!isan(*lp)) break;
			*p++ = *lp++;
		}
		*p++='h';
		*p=0;
		return(NUM);
	}
	if(!isan(c)) {
		*p++ = *lp++;
		*p=0 ;
		return(c);
	}
	*p++ = *lp++;
	if(isnum(c) && ( ( (*lp) | 0x20)=='x' ) ) {
		lp++;
		while( *lp ) {
			if(!isan(*lp)) break;
			*p++ = *lp++;
		}
		*p++='h';
		*p=0;
		return(NUM);
	}
	while( *lp ) {
		if(!isan(*lp)) break;
		*p++ = *lp++;
	}
	*p=0;
	if(isal(c))	return(ID);
	else		return(NUM);
}

/*static void ungetln()
{
	ugflg=1;
}*/

static int getln()
{
	int rc;
	char buf[MAXLINE];
	rc=getlnx();
	if(rc==0) return(rc);

	topmark= *lnbuf;
	toplabel=0;
	if(*lnbuf>='@') toplabel=1;/* 行頭の識別子をラベルとみなす */

	if(rflag) {
		strcpy(buf,";");
		if(!tflag) {
			sprintf(buf+1,"%d",linenum);
			strcat(buf,":\t\t\t\t");
		}
		strcat(buf,lnbuf);
		strcat(buf,"\r\n");
		outrem(buf);
	}
	return(rc);
}
int getlnx()
{
	int i,c;
	lp=lnbuf;
	if(ugflg) {
		ugflg=0;
		return(1);
	}
	for(i=0; i<MAXLINE; i++)lnbuf[i]=0;
	if(eoflg) return(0);
	i=0;
	while((c=getc(ifp))!=EOF) {
		if(c=='\r') c=getc(ifp);
		if(c== EOF) break;
		if(c==0x1a) break;
		if(c=='\n') {
			linenum ++;
			return(1);
		}
		lnbuf[i++]=c;
	}
	eoflg=1;
	return(0);
}
void seq(char *s)
{
	char buf[MAXLINE];
	gettoken(buf);
	if(strcmp(s,buf)!=0) {
		errsyntax("Missing ",s);
	}
}
int nump(int *n)
{
	spskip(lp);
	if(isnum(*lp)==0)return(0);
	*n = 0;
	while(isnum(*lp)) {
		*n *= 10;
		*n += (*lp - '0');
		lp++;
	}
	return(1);
}
int isnum(int c)
{
	return((c>='0') && (c<='9'));
}
int isal(int c)
{
	if( (c>='A') && (c<='Z') )return (1);
	if( (c>='a') && (c<='z') )return (1);
	if( (c == '_' ) || (c == '@' ) ) return (1);
	return(0);
}
int isan(int c)
{
	if(isnum(c))return (1);
	return(isal(c));
}
