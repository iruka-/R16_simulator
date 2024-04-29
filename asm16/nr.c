/*	R16: ナイトライダー */

	org 0;
	do {
		r80h = 7;
		r81h = 16;
		do {
			ld	r80h;
			OUT	0;
//			do {
//				r82h++;
//			} while(nf);		//	} while(r82h != 0xFFFF) ;
			r80h <<= 1 ;
			r81h--;
		} while(nf);			//	if (r81h != (-1)) goto A0002;
		halt;
	} while(1);

