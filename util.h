//Copyright Mattias Lasersköld: mattias.l.sk@gmail.com
#pragma once

inline bool isUtfTail(char c) {
	return((c & 0xC0) == 0x80);
}

inline double rnd() {
	constexpr double max2 = RAND_MAX * 2.;
	return (double) rand() / max2 - 1.;
}

inline double triangle(double t) {
	return t - (int) t - .5;
}

inline double lowPass(double sample, double history, double cutof) {
	return sample * cutof + history * (1 - cutof);
}