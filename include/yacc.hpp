#pragma once 
#include <iostream>
#include <string>
#include <fstream>
#include <set>
#include <queue>
#include <stack>

using namespace std;

struct Lr1Item {
	string leftp;//产生式左部
	string rightp[10];//产生式右部
	set<string> predict;//LR1项的预测符
	int numofrights;//右部符号个数
	int dot; //右部中圆点在第几个符号前面
	bool operator <(const Lr1Item& item) const//重载<，用于比较两个LR1项
	{
		if (strcmp(leftp.c_str(), item.leftp.c_str()) == 0)
		{
			int i = 0;
			while (i < 10)
			{
				if (strcmp(rightp[i].c_str(), item.rightp[i].c_str()) != 0)
					return rightp[i] < item.rightp[i];//产生式右部不同
				i++;

			}
			if (predict == item.predict)
				return dot < item.dot;//预测符相同但圆点位置不同           
			else
				return predict < item.predict;//预测符不同
		}
		else
			return leftp < item.leftp;//左部不同
	}
};

//LR1自动机的状态的结构
struct pdaState {
	int name;//状态名，如I0
	int numofitems;//该状态拥有的LR1项数目
	set<Lr1Item> items;//用于存放状态中的等价LR1项
	bool operator < (const pdaState& state) const //重载<，用于比较状态中的LR1项
	{
		return items < state.items;
	}
};

//LR1自动机从一个状态下推到另一个状态的边的结构
struct pdaEdge {
	string symbol;//遇到的符号
	int startState;//原状态名
	int endState;//后来的状态名
	bool operator < (const pdaEdge& edge) const//重载<，用于比较边
	{
		if (startState == edge.startState)
		{
			if (endState == edge.endState)
				return symbol < edge.symbol;
			else
				return endState < edge.endState;
		}
		else
			return startState < edge.startState;

	}
};

//整个LR1自动机的结构
struct Lr1Pda {
	int numofstates;//整个自动机的状态数
	int numofedges;//整个自动机的边的数量
	set<pdaState> states;//存放自动机的所有状态
	set<pdaEdge> edges;//存放自动机所有边
};

//LR1分析表的结构
struct table {
	vector<string> symbol;//一个状态的边上符号的集合
	vector<int> nextState;//边上所指后继状态的集合
};

struct op {//存储操作符
	string dest;//左操作符，还是右操作符
	int level;//level越大，操作符优先级越高
	string name;//操作符名称
};

//存放终结符
extern set<string> terminal;
//存放非终结符
extern set<string> nonterminal;

extern vector<Lr1Item> Item;//全局变量，保存最初的LR1项
extern pdaState I0;
extern Lr1Pda pda;//整个自动机