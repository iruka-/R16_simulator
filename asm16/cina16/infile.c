/*
 ===========================================
 *	RETROF-16 Sample Program
 ===========================================
 */

	org(0);
	jmp(_start);

//==========================================
	org(0x100);
_start:
	r15=0x100;	// Set Stack.
//==========================================
	r4=0;
	r5=0;
	r82++;

	do {
	do {
		ld(r5);
		#stv *r4

		r4++;
		and(0x1f);
		if(z) {
			r5++;
			out(r4);
		}
	}while(r4!=65527);
	r4=0;
	}while(1);
/*
//	do {
	do {
		ld(r5);stv(*r4);
		r4++;
		if((r4 & 0x1f)==0) {
			r5++;
			out(r4);
		}
	}while(r4!=65527);
//	r4=0;
//	}while(1);
//==========================================
main()
{
	r8=1;
	sub();
	if(r15 >= 4) {
		r3++;
	}
}

sub()
{
	r4=r5;
	r4+=r6;
	r4-=3;
	r4-=267;
	sub(300);
	and(ffffh);
	r4|=0x1fff;
	r4&=0x55aa;
}
*/
