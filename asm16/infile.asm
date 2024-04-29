
	org	0
	jmp	_start
	org	0100h
_start:
	mov	r15,0100h
	mov	r4,0
	mov	r5,0
__do000:
__do001:
	ld	r5
	stv *r4
	inc	r4
	and	01fh
	jne	__el002
	inc	r5
	out	r4
__el002:
	cp	r4,65527
	jne	__do001
__od001:
	mov	r4,0
	jmp	__do000
__od000:
main:
	enter
	mov	r8,1
	call	sub
	cp	r15,4
	jc	__el003
	inc	r3
__el003:
	leave
sub:
	enter
	mov	r4,r5
	add	r4,r6
	sub	r4,3
	sub	r4,267
	sub	300
	and	ffffh
	or	r4,01fffh
	and	r4,055aah
	leave
