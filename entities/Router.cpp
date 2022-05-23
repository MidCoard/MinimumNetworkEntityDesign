//
// Created by 周蜀杰 on 2022/5/21.
//

#include "Router.h"


void buildRoterTable() {

}

typedef struct yingshebiao {
	char x1;
	char ysip[16];
};

//邻居表封装
typedef struct yglj {
	unsigned char bhlj;
	int ilj;
	char iplj[32];
};

typedef struct ljb {
	int ljnum;
	yglj f[6];//上限与物理层对应
};
ljb lj;//定义邻居表


//总路由表封装
typedef struct zluyoubiao {
	char x2;
	int rport;
	int xnum;
	int tiao;
};


//路由表的封装
typedef struct lyb {
	int nummm;
	zluyoubiao d[26];//一个路由网的路由数量上限为26
};
lyb lll;//全局变量-----路由表


//链路状态图
int e[26][26];//全局变量-----链路状态表

//获取邻居信息数据包构建
int getljsjb() {
	int n = 0;
	n = ipbhtobit(EOC);
	n = jadd(EOC, n);
	n = zadd(EOC, n);
	return n;
}

//将邻居信息变成bit流
int ljtobit() {
	memset(FOC, 0, sizeof(FOC));
	int n = 0;
	body *l;
	char s1[8];
	char s3[8];
	chartobit((unsigned char) (lj.ljnum), s1);
	l = init();
	l = buftoline(FOC, 1);
	for (int i = 1; i < 6; i++) {
		if (lj.f[i].bhlj != 0) {
			for (int j = 31; j >= 0; j--) {
				insert(l, 1, lj.f[i].iplj[j]);
				n = l->value;
			}
			chartobit(lj.f[i].bhlj, s3);
			for (int j = 7; j >= 0; j--) {
				insert(l, 1, s3[j]);
				n = l->value;
			}
		}
	}
	for (int j = 31; j >= 0; j--) {
		insert(l, 1, bip[j]);
		n = l->value;
	}
	for (int j = 7; j >= 0; j--) {
		insert(l, 1, bh[j]);
		n = l->value;
	}
	for (int j = 7; j >= 0; j--) {
		insert(l, 1, s1[j]);
		n = l->value;
	}
	//链表转数组
	linetobuf(FOC, l);
	destroy(l);
	n = n - 1;;
	//其它封装
	n = jadd(FOC, n);
	n = zadd(FOC, n);
	return n;
}


