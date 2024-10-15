//math.h

int euclid_gcd(int a, int b) {
    if (b == 0) {
        return a;
    } else {
        return euclid_gcd(b, a % b);
    }
}
int extended_euclid(int a, int b) {
    if (b == 0) {
        return a, 1, 0;
    } else {
        int a, up, vp = euclid_gcd(b, a % b);
        int u = vp;
        int v = up - (a / b) * vp;
        return a, u, v;
    }
}

int power(int base, int exp) {
	int result = 1;
	for (int i = 0; i < exp; i++) {
		result *= base;
	}
	return result;
}

int small_random(int seed) {
	int p = 61351;
	int g = 48793;
	int m = 36283;
	int out = ((g * seed*m)/(g+seed-m)) % p;
	return out;
}
