
/**
  ******************************************************************************
	*文件：sqStack.cpp
	*作者：吴新有
	*版本：1.0
	*日期：2016-12-15
	*概要：顺序栈类头文件
	*备注：
	*
  ******************************************************************************  
	*/ 


#ifndef SQSTACK_H
#define SQSTACK_H

#define STACKSIZE  5
typedef unsigned char ElemType;

//顺序队列
typedef struct 
{
	ElemType data[STACKSIZE];  //保存栈元素
	int top;   //栈顶
}SqStackType;


//循环队列
class SqStack
{
public:
	SqStack();
//	void InitStack(SqStackType &st);  //初始化一个空栈
	int Push( ElemType x);  //入栈
	int Pop( ElemType &x);   //出栈
	ElemType Pop();          //出栈
	int GetTopElem( ElemType &x);   //获取栈顶元素
	ElemType GetTopElem();          //返回栈顶元素
	int GetTop();   //返回栈顶
	void ClearStack();

	bool IsEmpty();    //判断为空
	bool IsFull();  //判断队满


public:
	SqStackType st;

};


#endif  //SQSTACK_H

