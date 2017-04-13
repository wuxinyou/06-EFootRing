
/**
  ******************************************************************************
	*文件：sqStack.cpp
	*作者：吴新有
	*版本：1.0
	*日期：2016-12-15
	*概要：顺序栈类实现
	*备注：可实现FILO的数据处理，同时可以随意的遍历栈内数据
	*
  ******************************************************************************  
	*/ 
	
#include "sqStack.h"

SqStack::SqStack()
{
	st.top=-1;
	
};

//void SqStack::InitStack()

int SqStack::Push(ElemType x)
{
	if(st.top==STACKSIZE-1)  //栈满
		return 0;
	else
	{
		st.top++;
		st.data[st.top]=x;
		return 1;
	}
	
}

int SqStack::Pop(ElemType &x)
{
	if(st.top==-1)        //栈空
		return 0;
	else
	{
		x=st.data[st.top];
		st.top--;
		return 1;			
	}	
}

ElemType SqStack::Pop()
{
	if(st.top==-1)        //栈空
		return 0;
	else
	{
		return 	st.data[st.top--];	//注意一定是要先返回，后自减		
	}	
}

int SqStack::GetTopElem(ElemType &x)
{
	if(st.top==-1)        //栈空
		return 0;
	else
	{
		x=st.data[st.top];
		return 1;
	}
}

ElemType SqStack::GetTopElem()
{
	if(st.top==-1)        //栈空
		return 0;
	else
	{
		return st.data[st.top];		
	}
}

int SqStack::GetTop()
{
	return st.top;
}

bool SqStack::IsEmpty()
{
	if(st.top==-1)
		return true;
	else
		return false;
}

bool SqStack::IsFull()
{
	if(st.top==STACKSIZE-1)  //栈满
		return true;
	else
	{
		return false;
	}
}

void SqStack::ClearStack()
{
	//清空栈
	st.top=-1;	
		
}


