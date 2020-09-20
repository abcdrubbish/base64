#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char base64_table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
char base64_decode_table[256] = {0};

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
		tmp[0] = base64_decode_table[base64[i]];
		tmp[1] = base64_decode_table[base64[i + 1]];
		tmp[2] = base64_decode_table[base64[i + 2]];
		tmp[3] = base64_decode_table[base64[i + 3]];
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
	unsigned char *bin, *base64;
	FILE *fp_src, *fp_dst;
	int bin_len, base64_len, opt;
	int i, j;

	printf("================\n");
	printf("1. encode\n");
	printf("2. decode\n");
	printf("================\n");
	scanf("%d", &opt);
	printf("enter src file name: ");
	scanf("%s", src);
	printf("enter dst file name: ");
	scanf("%s", dst);
	base64_len = strlen(base64_table);
	memset(base64_decode_table, 0xff, sizeof(base64_decode_table));
	for (i = 0; i < base64_len; i++) {
		base64_decode_table[base64_table[i]] = i;
	}
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
	bin = (unsigned char *)malloc(sizeof(unsigned char) * 32 * 1024 * 1024 * 3);
	if (!bin) {
		fclose(fp_src);
		fclose(fp_dst);
		printf("[%s] malloc failed\n", __FUNCTION__);
	}
	base64 = (unsigned char *)malloc(sizeof(unsigned char) * 32 * 1024 * 1024 * 4);
	if (!base64) {
		fclose(fp_src);
		fclose(fp_dst);
		free(bin);
		printf("[%s] malloc failed\n", __FUNCTION__);
	}
	if (opt == 1) { 
		do {
			bin_len = fread(bin, sizeof(char), 32 * 1024 * 1024 * 3, fp_src);
			base64_len = base64_encode(bin, bin_len, base64);
			fwrite(base64, sizeof(char), base64_len, fp_dst);
		} while (bin_len == 32 * 1024 * 1024 * 3);
	} else if (opt == 2) {
		do {
			base64_len = fread(base64, sizeof(char), 32 * 1024 * 1024 * 4, fp_src);
			bin_len = base64_decode(base64, base64_len, bin);
			fwrite(bin, sizeof(char), bin_len, fp_dst);
		} while (base64_len == 32 * 1024 * 1024 * 4);
	}
	fclose(fp_src);
	fclose(fp_dst);
	return 0;
}