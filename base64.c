#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char base64_table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_encode(unsigned char *bin, int len, unsigned char *base64)
{
	int i, j, k;

	if (!bin || !base64 || len < 1) {
		return 0;
	}
	for (i = 0, j = 0; i < len; i += 3) {
		k = (bin[i] >> 2);
		base64[j++] = base64_table[k];

		k = ((bin[i] & 0x3) << 4);
		if (i + 1 >= len) {
			base64[j++] = base64_table[k];
			base64[j++] = '=';
			base64[j++] = '=';
			break;
		}
		k |= (bin[i + 1] >> 4);
		base64[j++] = base64_table[k];

		k = ((bin[i + 1] & 0xf) << 2);
		if (i + 2 >= len) {
			base64[j++] = base64_table[k];
			base64[j++] = '=';
			break;
		}
		k |= (bin[i + 2] >> 6);
		base64[j++] = base64_table[k];

		k = (bin[i + 2] & 0x3f);
		base64[j++] = base64_table[k];
	}
	return j;
}

int base64_decode(unsigned char *base64, int len, unsigned char *bin)
{
	int i, j, k;
	int tmp[4] = {0};

	if (!base64 || !bin || len < 1) {
		return 0;
	}
	for (i = 0, k = 0; i < len; i += 4) {
		for (j = 0; j < sizeof(base64_table); j++) {
			if (base64_table[j] == base64[i]) {
				tmp[0] = j;
				break;
			}
		}
		for (j = 0; j < sizeof(base64_table); j++) {
			if (base64_table[j] == base64[i + 1]) {
				tmp[1] = j;
				break;
			}
		}
		for (j = 0; j < sizeof(base64_table); j++) {
			if (base64_table[j] == base64[i + 2]) {
				tmp[2] = j;
				break;
			}
		}
		for (j = 0; j < sizeof(base64_table); j++) {
			if (base64_table[j] == base64[i + 3]) {
				tmp[3] = j;
				break;
			}
		}
		bin[k++] = (tmp[0] << 2) | ((tmp[1] & 0x30) >> 4);
		if (base64[i + 2] == '=') {
			break;
		}

		bin[k++] = ((tmp[1] & 0xf) << 4) | ((tmp[2] >> 2) & 0xf);
		if (base64[i + 3] == '=') {
			break;
		}

		bin[k++] = (((tmp[2] & 0x3) << 6) | tmp[3]);
	}
	return k;
}

int main(void)
{
	char src[256], dst[256];
	unsigned char bin[1200], base64[1600];
	FILE *fp_src, *fp_dst;
	int bin_len, base64_len, opt;

	printf("================\n");
	printf("1. encode\n");
	printf("2. decode\n");
	printf("================\n");
	scanf("%d", &opt);
	printf("enter src file name: ");
	scanf("%s", src);
	printf("enter dst file name: ");
	scanf("%s", dst);
	fp_src = fopen(src, "rb");
	if (!fp_src) {
		printf("[%s] open src file error\n", __FUNCTION__);
		return -1;
	}
	fp_dst = fopen(dst, "wb");
	if (!fp_dst) {
		fclose(fp_src);
		printf("[%s] open dst file error\n", __FUNCTION__);
		return -1;
	}
	if (opt == 1) { 
		do {
			bin_len = fread(bin, sizeof(char), 1200, fp_src);
			base64_len = base64_encode(bin, bin_len, base64);
			fwrite(base64, sizeof(char), base64_len, fp_dst);
		} while (bin_len == 1200);
	} else if (opt == 2) {
		do {
			base64_len = fread(base64, sizeof(char), 1600, fp_src);
			bin_len = base64_decode(base64, base64_len, bin);
			fwrite(bin, sizeof(char), bin_len, fp_dst);
		} while (base64_len == 1600);
	}
	fclose(fp_src);
	fclose(fp_dst);
	return 0;
}