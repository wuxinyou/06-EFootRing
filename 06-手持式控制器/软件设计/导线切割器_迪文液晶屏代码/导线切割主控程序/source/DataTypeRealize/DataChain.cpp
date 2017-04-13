/**
  ******************************************************************************
	*文件：DataChain.c
	*作者：叶依顺
	*版本：2.0
	*日期：2013-06-17
	*概要：链表结构类的实现
	*备注：
	*
  ******************************************************************************  
	*/ 

#include "DataChain.h"

//构造函数
RingChain::RingChain(int len)
{
	if(len<1)
	{
		current=NULL;
		return;
	}
	length=len;
	current=new ChainNode(0,NULL,NULL);
	current->m_next=current;
	current->m_previous=current;
	for(int i=0;i<len-1;i++)
	{
		current->m_next=new ChainNode(0,current,current->m_next);
		current->m_next->m_next->m_previous=current->m_next;
	}
}

//析构函数
RingChain::~RingChain()
{
// 	delete [] Chains;
	ChainNode *q,*p=current->m_next;	
	while(p!=current)
	{
		q=p->m_next;
		delete (p);
		p=q;
	}
	delete (current);
// 	current=NULL;
}


/**
  * 功  能：获取当前节点的数据
  * 参  数：无
  * 返回值：当前节点的数据值
  * 
  */
DATATYPE RingChain::getData()
{
	return current->data;
}

/**
  * 功  能：将当前节点指针指向下个节点
  * 参  数：无
  * 返回值：无
  * 
  */
void RingChain::next()
{
	current=current->m_next;
}

/**
  * 功  能：将当前节点指针指向上个节点
  * 参  数：无
  * 返回值：无
  * 
  */
void RingChain::previous()
{
	current=current->m_previous;
}

/**
  * 功  能：获取链表的长度
  * 参  数：无
  * 返回值：链表的长度
  * 
  */
int RingChain::getLen()
{
	return length;
}

/**
  * 功  能：设置当前节点的数据
  * 参  数：待设定的值
  * 返回值：无
  * 
  */
void RingChain::setData(DATATYPE value)
{
	current->data=value;
}

