//math.h
#ifndef _MATH_H
#define MATH_H

typedef struct {
	int high;
	int low;
} Int64;

typedef struct {
	Int64 high;
	Int64 low;
} Int128;

Int64 add_64(Int64 first, Int64 second) {
	Int64 result;
	int low_sum = first.low + second.low;
	result.high = first.high + second.high + (low_sum < first.low);
	result.low = low_sum;
	if (low_sum < first.low) {
		result.low = result.low * -1;
	}
	return result;
}

Int64 sub_64(Int64 first, Int64 second) {
	Int64 result;
	int low_diff = first.low - second.low;
	result.high = first.high - second.high - (low_diff > first.low);
	result.low = low_diff;
        if (low_diff > first.low) {
                result.low = result.low * -1;
        }
	return result;
}

Int64 mul_64(Int64 a, Int64 b) {
	Int64 result;
	int low_mul = a.low * b.low;
	int high_mul = a.high * b.high;
	int cross_mul1 = a.low * b.high;
	int cross_mul2 = a.high * b.low;
	result.low = low_mul;
	result.high = high_mul + (cross_mul1 + cross_mul2) + (low_mul < a.low);
        if (low_mul < a.low) {
                result.low = result.low * -1;
        }

	return result;
}

Int64 div_64(Int64 a, Int64 b) { //do this thing is hard
	Int64 result;
	result.high = 0;
	result.low = 0;
	if (b.low == 0) {
		result.low = 0;
	} else {
		result.low = a.low / b.low;
		if (b.high == 0) {
			result.high = a.high / b.low;
		} else {
			result.high = a.high / b.high;
		}
	}
	if (b.high == 0) {
		result.high = 0;
	} else {
		result.high = a.high / b.high;
	}
	return result;
}

Int64 mod_64(Int64 first, Int64 second) {
    Int64 result = sub_64(first, mul_64(div_64(first, second), second)); // first - first / second * second
    return result;
}

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
	int p = 61343;
	int g = 48799;
	int m = 36277;
	int out = ((g * seed*m)/(g+seed-m)) % p;
	return out;
}

void heapify(int heap[], int n, int i) { // i hate this, but it's efficient
	int largest=i;
	int r_son=i*2+2;
	int l_son=i*2+1;
	if (l_son < n && heap[l_son]> heap[largest]){
		largest=l_son;
	}
	if (r_son < n && heap[r_son]> heap[largest]) {
		largest=r_son;
	}
	if (largest != i) {
		int temp = heap[i];
		heap[i] = heap[largest];
		heap[largest]=temp;
		heapify(heap, n, largest);
	}
}

void build_heap(int heap[], int n) { // wtf??
	for (int i=(n-1)/2; i>=0; i--) {
		heapify(heap, n, i);
	}
}

void swap(int *a, int *b) { // Oh a simple swap function for do little things
	int temp = *a;
	*a = *b;
	*b = temp;
}

long long bin_pow(long long base, long long exp) {
	long long ret=1;
	long long quad_base=base;
	while (exp > 0) {
		if (exp % 2 == 1) {
			ret *= quad_base;
		}
		quad_base *= quad_base;
		exp /= 2;
	}
	return ret;
}

#endif
