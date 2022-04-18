#include "LinkLayer.h"

#include <utility>

const int kFrameHeader = 0x7E;
const int kFrameSize = 64;
const char flag='x';
const char esc='y';
char *getFrame(const char *a,int len)//返回帧内容 字符填充法 
{
	bool pesc=0;bool ok=0;bool err=0;
	
	int start=-1;int cnt=0;
	
	char *b;
	

	for (int i=0;i<len;i++)
	{
		if(a[i]==flag)// 遇到标志字符 
		{
			if(pesc==0) // 之前没有转义字符 
			{
				if(start==-1)start=i;
				else break;
				ok=0;
			}
			else ok=1;
			pesc=0;
		}
		else if(a[i]==esc)// 遇到转义字符 
		{
			if(pesc==1)// 之前有转义字符 
			{
				ok=1;
				pesc=0;
			}
			else 
			{
				ok=0;
				pesc=1;
			}
		}
		else if(pesc==1&&start!=-1) 
		//遇到单独的转义字符，说明数据出现错误 
		{
			err=1;break;
		}
		else if(start!=-1)
		// 记录有效数据 
		{
			ok=1;pesc=0; 
		}		
		if(start!=-1&&ok==1) b[cnt++]=a[i];
	}
	if(err==1)
	{
        char *c ;
		return c ;
	}
	else return b ;
} 

char* generateFrame(char *a,int len)
{
	char *b;
    b[0]=flag;
    int cnt=1;
	// 增加标志字符 
	for(int i=0;i<len;i++)
	{
		if(a[i]==flag||a[i]==esc)b[cnt++]=esc;
		// 增加转义字符 
		b[cnt++]=a[i];
	}
	b+=flag;
	return b;
}


LinkLayer::LinkLayer(INetAddress address) :address(std::move(address)),socket(address.createSocket()) {}

void LinkLayer::receive(const char *data, int len) const {
    char *Frame=getFrame(data,len);

    if()
    {
        //data is error
    }

	//todo decode the data
}

void LinkLayer::send(char *data, int len) const {
    data=generateFrame(data,len);
	//todo encode the data
	this->socket.send(data, len);
}
