#include "openssl/evp.h"
#include <iostream>
#include <time.h> 
//使用了openssl库
using namespace std;

int sm3_hash(char* message, size_t len, unsigned char* hash, unsigned int* hash_len)
{
	EVP_MD_CTX* md_ctx;
	const EVP_MD* md;

	md = EVP_sm3();
	md_ctx = EVP_MD_CTX_new();
	EVP_DigestInit_ex(md_ctx, md, NULL);
	EVP_DigestUpdate(md_ctx, message, len);
	EVP_DigestFinal_ex(md_ctx, hash, hash_len);
	EVP_MD_CTX_free(md_ctx);
	return 0;
}
void int_to_hex(int a, char ch[10]) {
	char m_c;
	int temp = 0, i = 0;
	while (a / 16 > 0)
	{
		temp = a % 16;
		if (temp > 9)
			ch[i] = temp + 97;
		else
			ch[i] = temp + 48;
		i++;
		a = a / 16;

		if (a < 16)
		{
			ch[i] = a + 48;
			ch[i + 1] = 0;
			break;
		}
	}
	temp = strlen(ch);
	for (i = 0; i < strlen(ch) / 2; i++)
	{
		m_c = ch[i];
		ch[i] = ch[temp - i - 1];
		ch[temp - i - 1] = m_c;
	}
}
int main(void)
{
	unsigned char hash_value[64];
	unsigned int i, hash_len;
	unsigned char hash_value2[64];
	char s2[] = { 0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64,
				0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64,
				0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64,
				0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64,
				0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64,
				0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64,
				0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64,
				0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64 };
	unsigned int sample2_len = sizeof(s2);
	sm3_hash(s2, sample2_len, hash_value, &hash_len);
	printf("raw data:\n");
	for (i = 0; i < sample2_len; i++)
	{
		printf("0x%x  ", s2[i]);
	}
	printf("\n");
	printf("hash length: %d bytes.\n", hash_len);
	printf("hash value:\n");
	for (i = 0; i < hash_len; i++)
	{
		printf("0x%x  ", hash_value[i]);
	}
	printf("\n");
	char sample[10];
	srand((unsigned)time(NULL));
	int a = rand() * rand();
	for (int i = a; i < a + 0xfffff; i++)
	{
		int_to_hex(i, sample);
		unsigned int sample_len = strlen((char*)sample);
		sm3_hash(sample, sample_len, hash_value2, &hash_len);
		//摘要值的前16bit相同就算是一次碰撞
		if (hash_value2[0] == hash_value[0] && hash_value2[1] == hash_value[1])
		{
			printf("找到一个16bit前缀碰撞,共穷搜%d次\n", i - a);
			printf("raw data: %x\n", i);
			printf("hash length: %d bytes.\n", hash_len);
			printf("hash value:\n");
			for (int j = 0; j < hash_len; j++)
			{
				printf("0x%x  ", hash_value2[j]);
			}
			printf("\n\n");
			break;
		}
	}
	return 0;
}