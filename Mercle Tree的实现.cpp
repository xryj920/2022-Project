// 程序实现
// 利用 Merkle Tree, 来实现对一段话的存储。
// 如 Hello, This Is Cielle.
// 一共分`Hello` + `,` + `This` + `Is` + `Cielle` + `.` 为这六个字串。     

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int uint;

// Merkle Tree 结构体定义 
typedef struct MerkleTreeNode {
	struct MerkleTreeNode* left;		// 左子节点 
	struct MerkleTreeNode* right;		// 右子节点 
	struct MerkleTreeNode* parent;		// 父节点 
	uint level;							// 当前树深，底层为0 
	uint data;							// 当前节点的值，最底层为原始数据，其上均为计算的哈希值 
	char* str;
}MerkleTree;

// 定义宏函数，创建一个 Merkle Tree节点
#define New_Merkle_Node(mt, tree_depth) {	\
	mt = (MerkleTree *)malloc(sizeof(MerkleTree)); \
	mt->left = NULL; \
	mt->right = NULL; \
	mt->parent = NULL; \
	mt->level = (uint)tree_depth; \
	mt->data = 0;	\
	mt->str = NULL; \
	}

// 打印 Merkle tree，中续遍历方式 
void Print_Merkle_Tree(MerkleTree* mt, int high)
{
	MerkleTree* p = mt;
	int i;

	if (p == NULL)
		return;

	if (p->left == NULL && p->right == NULL) {
		for (i = 0; i < high - p->level; i++)
			printf("           ");

		printf("------>%s\n", p->str);
	}
	else {
		Print_Merkle_Tree(mt->left, high);
		printf("\n");
		for (i = 0; i < high - p->level; i++)
			printf("           ");

		printf("------>%-6d\n", p->data);

		Print_Merkle_Tree(mt->right, high);
	}
}

// 计算一个字符串的hash值 
uint hash_string(char* key1, char* key2) {
	uint cal = 5, hash = 0;

	if (key1 != NULL)
		while (*key1 != '\0' && *key1 != 0) {
			hash = hash * cal + *key1;
			key1++;
		}
	if (key2 != NULL)
		while (*key2 != '\0' && *key2 != 0) {
			hash = hash * cal + *key2;
			key2++;
		}

	return hash & 0x7FFFFFFF;
}

// 计算两个整数的hash 值 
uint hash_two_node(uint num1, uint num2) {
	uint cal = 3, hash = 0;
	hash = num1 + num2;
	hash *= cal;
	return hash & 0x7FFFFFFF;
}

// 遍历二叉树，找到最后一个叶子节点，遍历方式为前续遍历方式 
MerkleTree* Find_Last_Node(MerkleTree* mt) {
	MerkleTree* p = mt, * tmp;

	// 如果左右子节点都是NULL，肯定是叶子节点，直接返回 
	if (p->left == NULL && p->right == NULL)
		return p;
	// 如果 右子节点为空，左子节点不为空，继续遍历左子节点，直到叶子节点 
	else if (p->right == NULL && p->left != NULL)
		return Find_Last_Node(p->left);
	// 如果 右子节点不为空，遍历右子节点 
	else if (p->right != NULL)
		return Find_Last_Node(p->right);
}

// 根据最后一个节点，找到插入的位置 
MerkleTree* Find_Empty_Node(MerkleTree* mt) {
	MerkleTree* p = mt->parent;

	// 向上找到第一个存在空缺的节点，直到顶节点 
	while (p->left != NULL && p->right != NULL && p->parent != NULL)  p = p->parent;

	// 如果 p 指向顶节点，说明当前是满二叉树 ，返回 NULL 
	if (p->parent == NULL && p->left != NULL && p->right != NULL)
		return NULL;
	// 否则当前节点就是要插入节点的位置 
	else
		return p;
}

// Merkle tree 初始化 (递归实现)
//MerkleTree * Creat_Merkle_Tree(MerkleTree *mt, int *arr, int nums)
MerkleTree* Creat_Merkle_Tree(MerkleTree* mt, char** arr, int nums)
{
	MerkleTree* node, * tmp, * p;
	int i;

	// 1. nums 等于0时，数据添加完毕，此时返回merkle tree头结点 
	if (nums == 0) {

		printf("Merkle Tree 创建完毕！！！\n");
		return mt;
	}
	else {

		// 2. 创建一个叶节点 
		New_Merkle_Node(node, 0);
		//node->data = *arr;
		node->str = (char*)malloc(strlen(*arr) + 1);
		memset(node->str, '\0', strlen(*arr) + 1);
		for (i = 0; i < strlen(*arr); i++)
			node->str[i] = (*arr)[i];
		printf("新建叶节点 [%d] tree_depth=%d, level=%d, data=%-6d, nums=%d, str=%-5s, arr=%-5s (%d)\n", __LINE__, mt == NULL ? 0 : mt->level, node->level, node->data, nums, node->str, *arr, strlen(*arr));

		// 3. 如果 mt 为空，说明当前没有树，需要新建一个头结点 
		if (mt == NULL) {

			// 3.1 创建头结点
			New_Merkle_Node(mt, 1);
			mt->left = node;
			node->parent = mt;
			printf("新建头节点 [%d] tree_depth=%d, level=%d, data=%-6d \n", __LINE__, mt->level, mt->level, mt->data);

			// 3.2 更新头结点哈希值 
			//mt->data = hash_two_node(mt->left->data, mt->right==NULL ? 0 : p->right->data);
			mt->data = hash_string(mt->left->str, mt->right == NULL ? NULL : mt->right->str);

			// 3.3 递归添加下一个值 
			mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1);
		}
		// 4.如果 mt 不为空，说明当前已经有树了 
		else
		{
			// 5. 遍历当前树，找到一个空的叶子节点，满二叉树时返回NULL 
			p = Find_Empty_Node(Find_Last_Node(mt));

			// 6. 如果返回值不为 NULL, 说明已经找到需要插入的节点 
			if (p != NULL) {
				// 6.1 如果最底下就是叶子节点，就直接赋值 right
				if (p->left->left == NULL && p->right == NULL)
				{
					p->right = node;
					node->parent = p;
					// 更新哈希值 
					//p->data = hash_two_node(p->left->data, p->right==NULL ? 0 : p->right->data);
					p->data = hash_string(p->left->str, p->right == NULL ? NULL : p->right->str);

				}
				// 6.2 如果返回的节点是中间节点 
				else
				{
					// 6.2.1 给该中间节点，新建一个 right 子节点         
					i = p->level - 1;
					New_Merkle_Node(tmp, i);
					p->right = tmp;
					tmp->parent = p;
					printf("新中间节点 [%d] tree_depth=%d, level=%d, data=%-6d \n", __LINE__, mt->level, tmp->level, tmp->data);

					// 6.2.2 更新指针p，指向当前新建的 right 子节点，从此开始，只需要循环添加 left子节点就可以了 
					p = p->right;
					i = p->level - 1;		// 更新level - 1 

					while (i > 0) {
						// 创建中间节点 
						New_Merkle_Node(tmp, i);
						p->left = tmp;
						tmp->parent = p;
						printf("新中间节点 [%d] tree_depth=%d, level=%d, data=%-6d \n", __LINE__, mt->level, tmp->level, tmp->data);

						p = p->left;
						i--;
					}

					// 6.2.3 当前 p 节点指向的是 leven=1，此时需添加叶子节点了 
					p->left = node;
					node->parent = p;
					p->data = hash_string(p->left->str, p->right == NULL ? NULL : p->right->str);
				}
			}
			// 7. 如果没有空的节点，说明当前是满二叉树，需要新增头节点了，level也要加1 
			else
			{
				tmp = mt;				// 保存当前头结点

				// 7.1 创建一个新的头节点， 树高度 +1 
				New_Merkle_Node(mt, tmp->level + 1);
				mt->left = tmp; 		// 头节点赋值 
				tmp->parent = mt;
				printf("新建头节点 [%d] tree_depth=%d, level=%d, data=%-6d \n", __LINE__, mt->level, mt->level, mt->data);

				// 7.2 创建头节点下的第一个 right 子节点 
				New_Merkle_Node(tmp, mt->level - 1);
				mt->right = tmp;
				tmp->parent = mt;
				printf("新中间节点 [%d] tree_depth=%d, level=%d, data=%-6d \n", __LINE__, mt->level, tmp->level, tmp->data);

				p = mt->right;
				i = p->level - 1;

				// 根据树的深度创建同样深度的左树 
				while (i > 0) {
					// 创建结点
					New_Merkle_Node(tmp, i);
					p->left = tmp;
					tmp->parent = p;
					printf("新中间节点 [%d] tree_depth=%d, level=%d, data=%-6d \n", __LINE__, mt->level, tmp->level, tmp->data);

					p = p->left;
					i--;
				}
				// 叶子节点赋值 
				p->left = node;
				node->parent = p;
				p->data = hash_string(p->left->str, p->right == NULL ? NULL : p->right->str);

			}

			// 8. 经过前三种判断，已经成功新增叶子节点，由于p 指向当前新增叶子节点的parent，所以从 p 开始自下而上 更新哈希值  
			if (p != mt) {
				p = p->parent;

				while (p != mt) {
					p->data = hash_two_node(p->left->data, p->right == NULL ? 0 : p->right->data);
					p = p->parent;
				}
				// 8.1 更新父节点哈希值  
				p->data = hash_two_node(p->left->data, p->right == NULL ? 0 : p->right->data);

			}
			// 9 节点插入成功，递归添加下一个值  
			mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1);
		}
	}
}

// 清空 Merkle tree, 回收所有分配的内存 
void Delete_Merkle_Tree(MerkleTree* mt)
{
	// 如果是叶子节点，直接回收 
	if (mt->level == 0) {
		printf("回收叶子节点，level=%d, data=%-6d, str=%s\n", mt->level, mt->data, mt->str);
		free(mt->str);
		free(mt);
	}
	else {
		if (mt->left != NULL) {
			Delete_Merkle_Tree(mt->left);
		}
		if (mt->right != NULL) {
			Delete_Merkle_Tree(mt->right);
		}
		// 释放当前节点
		printf("回收中间节点，level=%d, data=%-6d \n", mt->level, mt->data);
		free(mt);
	}
}

char** divide_str(char* str, int* number) {
	char* p = str, * tmp = str, ** result, ** res;
	int num = 0, i;

	// 统计该句子中有多少个词组，及最长词组的长度 
	while (*p != '\0') {
		if (*p == ',' || *p == '.' || *p == '!' || *p == '?' || *p == ';') {
			num += 2;
			tmp = p + 1;
			//printf("[1] num=%d p=%s \n", num, p);
		}
		else if (*p == ' ') {
			// 跳过单独的空格 
			if (p - tmp == 0) {
				tmp = p + 1;
			}
			else {
				num += 1;
				tmp = p + 1;
				//printf("[2] num=%d p=%s \n", num, p);	
			}
		}
		p++;
	}
	if (p - tmp > 0) {
		num += 1;
		//printf("[3] num=%d p=%s \n", num, p);
	}

	printf("原词组 %s\n解析到 %d个词组\n\n", str, num);

	// 动态分配内存
	result = (char**)malloc(sizeof(char*) * num);
	res = result;

	tmp = str;
	p = str;

	// 统计该句子中有多少个词组，及最长词组的长度 
	while (*p != '\0') {
		// 遇到标点，说明有两个字符串 
		if (*p == ',' || *p == '.' || *p == '!' || *p == '?' || *p == ';') {
			// 拷贝第一个字符串 
			*res = (char*)malloc(sizeof(char) * (p - tmp + 1));
			memset(*res, '\0', p - tmp + 1);
			for (i = 0; i < p - tmp; i++)
				(*res)[i] = *(tmp + i);

			printf(" [%s] ", *res); 		// 打印当前词组 
			res++;

			// 拷贝标点符号 
			*res = (char*)malloc(sizeof(char) * 2);
			memset(*res, '\0', 2);
			(*res)[0] = *p;
			printf(" [%s] ", *res);  		// 打印当前词组 
			res++;

			p++;
			tmp = p;
		}
		// 遇到空格说明 只有一个字符串 
		else if (*p == ' ') {
			if (p - tmp == 0) {
				// 说明只有一个空格，去除
				p++;
				tmp = p;
				//printf("跳过当前空格 p=%s \n", p);
			}
			else {
				// 拷贝第一个字符串 
				*res = (char*)malloc(sizeof(char) * (p - tmp + 1));
				memset(*res, '\0', p - tmp + 1);
				for (i = 0; i < p - tmp; i++)
					(*res)[i] = *(tmp + i);
				printf(" [%s] ", *res); 		// 打印当前词组 
				res++;

				// 跳过空格 
				p++;
				tmp = p;
			}
		}
		else
			p++;
	}
	if (p - tmp > 0) {
		// 拷贝第一个字符串 
		*res = (char*)malloc(sizeof(char) * (p - tmp + 1));
		memset(*res, '\0', p - tmp + 1);
		for (i = 0; i < p - tmp; i++)
			(*res)[i] = *(tmp + i);
		printf(" [%s] ", *res); 			// 打印当前词组 
	}

	*number = num;
	printf("\n\n");
	return result;
}

void Delete_arr(char** arr, int num) {
	int i;

	for (i = 0; i < num; i++) {
		printf("释放数组内存，str = %s \n", *(arr + i));
		free(*(arr + i));
	}
	free(arr);
	printf("\n");
}


int main(void)
{
	//int array[]={11, 22, 33 ,44 ,55 ,66, 77, 88, 99, 1010, 1111, 1212};
	//char* str_arr[]={"Hello", ",",  "This", "Is", "Ciell", "!", "Ha", "Ha"};

	char** str_arr;
	int num;
	MerkleTree* mt = NULL;

	// 分割字符串，以 , ! . 空格，这几个区分 
	char str[1000] = "SDU!2022!Summer Vaction!";
	str_arr = divide_str(str, &num);


	// 根据数组动态创建 Merkle Tree 
	//mt = Creat_Merkle_Tree(mt, array, sizeof(array)/sizeof(int));
	//mt = Creat_Merkle_Tree(mt, str_arr, sizeof(str_arr)/sizeof(char *));

	mt = Creat_Merkle_Tree(mt, str_arr, num);

	// 打印当前merkle Tree
	if (mt != NULL) {
		printf("\n开始打印当前 Merkle 树:\n");
		Print_Merkle_Tree(mt, mt->level);
		printf("\n\n");
	}

	//清空二维数组内存
	// 回收内存
	Delete_arr(str_arr, num);
	Delete_Merkle_Tree(mt);

	return 0;
}
