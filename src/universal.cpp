#include "universal.h"

bitstring BitString::IntToBitstring(uint32 i, uint16 l, bool b) {
  bitstring r;
  if (b) {
    do {
      r = ((i & 1) ? "1" : "0") + r;
      i = i >> 1;
    } while (i);
  } else {
    for (int32 j = 0; j < l; j++) {
      r = ((i & 1) ? "1" : "0") + r;
      i = i >> 1;
    }
  }
  return r;
}

void BitString::BitstringShiftLeft(bitstring& b, uint16 l) {
  while (l--) {
    b += "0";
  }
  return;
}

bitstring BitString::BitstringAdd(const bitstring& a, const bitstring& b) {
  int32 la = a.length(), lb = b.length(), c = 0, s = 0;
  bitstring r;
  while (la > 0 || lb > 0) {
    s = c;
    s += ((la > 0) ? (a[la - 1] - '0') : 0);
    s += ((lb > 0) ? (b[lb - 1] - '0') : 0);
    c = s / 2;
    r = (s % 2) ? ("1" + r) : ("0" + r);
    la--;
    lb--;
  }
  if (c > 0) r = "1" + r;
  return r;
}
