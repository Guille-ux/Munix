#ifndef _KERROR_H
#define _KERROR_H

void raise_zero_error() {
	clear(0x4F);
	print("Zero Division Error", 20, 0);
}

#endif
