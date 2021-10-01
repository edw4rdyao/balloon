#include "universal.h"

BS IntToBS(uint32 i, uint16 l, bool b){
	string r;
	if (b) {
		do {
			r = ((i & 1) ? "1" : "0") + r;
			i = i >> 1;
		} while (i);
	}
	else {
		for (int32 j = 0; j < l; j++) {
			r = ((i & 1) ? "1" : "0") + r;
			i = i >> 1;
		}
	}
	return r;
}

void BSShiftLeft(BS& b, uint16 l){
	while (l--) {
		b += "0";
	}
	return ;
}

BS BSAdd(const BS& a, const BS& b){
	int32 la = a.length(), lb = b.length(), c = 0, s = 0;
	BS r;
	while (la > 0 || lb > 0) {
		s = c;
		s += ((la > 0) ? (a[la - 1] - '0') : 0);
		s += ((lb > 0) ? (b[lb - 1] - '0') : 0);
		c = s / 2;
		if (s % 2) {
			r = "1" + r;
		}
		else {
			r = "0" + r;
		}
		la--;
		lb--;
	}
	if(c > 0) r = "1" + r;
	return r;
}
