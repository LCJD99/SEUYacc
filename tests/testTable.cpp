#include <yacc.hpp>

table* analysisOfTable()//构建LR1分析表
{
	ofstream output("../Debug/table.txt");

	table* Lr1table = new table[pda.numofstates];//假设当成每个状态含一张表
	set<pdaEdge>::iterator edge = pda.edges.begin();
	for (edge; edge != pda.edges.end(); edge++)
	{
		Lr1table[(*edge).startState].symbol.push_back((*edge).symbol);
		Lr1table[(*edge).startState].nextState.push_back((*edge).endState);
	}

	//开始输出分析表,间隔一个制表符
	cout << "State" << "        ";
	output << "State" << "      ";

	set<string>::iterator it;//先画好action部分的属性
	for (it = terminal.begin(); it != terminal.end(); it++)
	{
		cout << (*it) << "      ";
		output << (*it) << "        ";
	}
	cout << "#" << "        ";
	output << "#" << "         ";

	for (it = nonterminal.begin(); it != nonterminal.end(); it++)//再画Goto部分的属性
	{
		cout << (*it) << "      ";
		output << (*it) << "        ";
	}
	cout << endl;
	output << endl;

	//开始输出每个状态的分析表，一行一个状态
	for (int i = 0; i < pda.numofstates; i++)
	{
		cout << i << "		";
		output << i << "	  ";

		set<pdaState>::iterator state;
		for (state = pda.states.begin(); state != pda.states.end(); state++)
		{
			if ((*state).name == i)
			{
				set<Lr1Item>::iterator it;
				for (it = (*state).items.begin(); it != (*state).items.end(); it++)
				{
					if ((*it).dot == (*it).numofrights)//如果是需要规约的LR1项，找出其为第几个产生式
					{
						int count = 1;
						vector<Lr1Item>::iterator it_item;
						for (it_item = Item.begin(); it_item != Item.end(); it_item++)//set容器能否保证读LR1项的顺序为自然向下？
						{
							bool flag = true;
							if ((*it).leftp == (*it_item).leftp)//若左部相同
							{
								for (int j = 0; j < 10; j++)
								{
									if ((*it).rightp[j] != (*it_item).rightp[j])//若右部不同，
									{
										flag = false;
									}
								}
							}
							else//左部不同
							{
								flag = false;
							}

							if (flag)//如果左右部相同，即找到其产生式
							{
								if ((*it).leftp == "S'")
								{
									count = 0;//第0个产生式
								}
								break;//跳出for
							}
							count++;
						}

						set<string>::iterator it_pre;
						for (it_pre = (*it).predict.begin(); it_pre != (*it).predict.end(); it_pre++)//解决移入规约冲突和规约规约冲突
						{
							bool conflict = false;
							int k;
							for (k = 0; k < Lr1table[i].nextState.size(); k++)
							{
								if (Lr1table[i].symbol[k] == (*it_pre))//如果边上的符号中找到了该LR1项展望符中的符号，又该项已经是待规约项，冲突
								{
									conflict = true;
									break;
								}
							}

							int reducenum = 0 - count;//负数表示规约

							if (conflict)//有冲突，处理，选择规约？
							{
								Lr1table[i].nextState[k] = reducenum;//改写值
							}
							else//不冲突，规约，规约项是无边的，故使用push_back()放末尾
							{
								Lr1table[i].nextState.push_back(reducenum);
								Lr1table[i].symbol.push_back((*it_pre));//给待规约项加个符号，相当于增加一条边
							}

						}
					}
				}
			}
		}
		for (int m = 0; m < Lr1table[i].nextState.size(); m++)
		{
			if (Lr1table[i].nextState[m] == 0)//accept
			{
				cout << "accept" <<"  "<<Lr1table[i].symbol[m]<< "		   ";
				output << "accept" <<"  "<<Lr1table[i].symbol[m]<< "		     ";
			}
			else if (Lr1table[i].nextState[m] > 0)//移进项
			{
				cout << "s" << Lr1table[i].nextState[m] <<"  "<<Lr1table[i].symbol[m]<< "		  ";
				output << "s" << Lr1table[i].nextState[m] <<"  "<<Lr1table[i].symbol[m]<< "		    ";
			}
			else//负数代表规约项
			{
				cout << "r" << Lr1table[i].nextState[m] <<"  "<<Lr1table[i].symbol[m]<< "		    ";
				output << "r" << Lr1table[i].nextState[m] <<"  "<<Lr1table[i].symbol[m]<< "		      ";
			}

		}

		cout << endl;
		output << endl;
	}
	return Lr1table;
}

ofstream outc("../Debug/seuyacc.cpp");

void gc(string content, int num)
{
	for (int i = 0; i < num; i++)
		outc << "\t";

	outc << content << endl;
}



void gencode(table* mtable)
{
	gc("#define _CRT_SECURE_NO_WARNINGS", 0);
	gc("#include<iostream>", 0);
	gc("#include <stack>", 0);
	gc("#include<queue>", 0);
	gc("#include<fstream>", 0);
	gc("#include<vector>", 0);
	gc("#include<map>", 0);
	gc("#include<string>", 0);
	gc("using namespace std;", 0);
	gc("#define BUFF 512", 0);

	gc("bool isused = true;", 0);
	gc("bool iseplsion = false;", 0);
	gc("struct production", 0);
	gc("{", 0);
	gc("string leftp;", 1);
	gc("string rightp[10];", 1);
	gc("int numofrights;", 1);
	gc("};", 0);
	gc("vector<production> productions(50);", 0);
	gc("struct token{", 0);
	gc("string name;", 1);
	gc("string type;", 1);
	gc("};", 0);
	gc("queue<token> tokenqueue;", 0);
	gc("struct table2 {", 0);
	gc("vector<string> tg;", 1);
	gc("vector<int> next;", 1);
	gc("};", 0);
	//gc("", 0);
	gc("void printbuffer(vector<string> a)", 0);
	gc("{", 0);
	gc("vector<string>::iterator i;", 1);
	gc("for (i = a.begin();i != a.end();i++)", 1);
	gc("{", 1);
	gc("cout << (*i) << \" \";", 2);
	gc("}", 1);
	gc("}", 0);

	gc("void tableAnalyse(stack<token>& mtokenstack, stack<int>& mnstack, vector<string>& moutbuffer, token t, table2*& GOTO2)", 0);
	gc("{", 0);
	gc("for (int i = 0;i < GOTO2[mnstack.top()].tg.size();i++)", 1);
	gc("{", 1);
	gc("if (t.type == GOTO2[mnstack.top()].tg[i])", 2);
	gc("{", 2);
	gc("if (GOTO2[mnstack.top()].next[i] > 0)", 3);
	gc("{", 3);
	gc("cout << \"\t转移到状态:\" << GOTO2[mnstack.top()].next[i] << endl;", 4);
	gc("mnstack.push(GOTO2[mnstack.top()].next[i]);", 4);
	gc("if (t.type != \"eplsion\")", 4);
	gc("{", 4);
	gc("mtokenstack.push(t);", 5);
	gc("moutbuffer.push_back(t.name);", 5);
	gc("}", 4);
	gc("break;", 4);
	gc("}", 3);
	gc("else if (GOTO2[mnstack.top()].next[i] < 0)", 3);
	gc("{", 3);
	gc("int num = 0 - GOTO2[mnstack.top()].next[i];", 4);
	gc("cout << \"\t使用产生式规约:\" << productions[num].leftp << \"--->\";", 4);
	gc("for (int i = 0;i < productions[num].numofrights;i++)", 4);
	gc("{", 4);
	gc("cout << productions[num].rightp[i];", 5);
	gc("if (productions[num].rightp[0] != \"eplsion\")", 5);
	gc("{", 5);
	gc("mtokenstack.pop();", 6);
	gc("moutbuffer.pop_back();", 6);
	gc("}", 5);
	gc("mnstack.pop();", 5);
	gc("isused = false;", 5);
	gc("}", 4);
	gc("token a{ productions[num].leftp ,productions[num].leftp };", 4);
	gc("tableAnalyse(mtokenstack, mnstack, moutbuffer, a, GOTO2);", 4);
	gc("break;", 4);
	gc("}", 3);
	gc("else", 3);
	gc("{", 3);
	gc("cout << \"编译完成\" << endl;system(\"pause\");", 4);
	gc("exit(1);", 4);
	gc("}", 3);
	gc("}", 2);
	gc("if (GOTO2[mnstack.top()].tg[i] == \"eplsion\")", 2);
	gc("iseplsion = true;", 3);
	gc("if (i == GOTO2[mnstack.top()].tg.size() - 1)", 2);
	gc("{", 2);
	gc("if (iseplsion == true)", 3);
	gc("{", 3);
	gc("token b{ \"eplsion\",\"eplsion\" };", 4);
	gc("iseplsion = false;", 4);
	gc("tableAnalyse(mtokenstack, mnstack, moutbuffer, b, GOTO2);", 4);
	gc("isused = false;", 4);
	gc("break;", 4);
	gc("}", 3);
	gc("cout << endl;", 3);
	gc("cout << \"fail in compiling.\" << endl;", 3);
	gc("exit(1);", 3);
	gc("}", 2);
	gc("}", 1);
	gc("iseplsion = false;", 1);
	gc("}", 0);

	gc("int main()", 0);
	gc("{", 0);
	outc << "map<int, string> I2S;\n I2S[1]=\"ASSIGN\";I2S[2]=\"COMMA\";I2S[3]=\"DIVIDE\";I2S[4]=\"DOT\";I2S[5]=\"ELSE\";I2S[6]=\"EQUAL\";I2S[7]=\"FLOAT\";I2S[8]=\"IF\";I2S[9]=\"INT\";I2S[10]=\"LBRACE\";I2S[11]=\"LBRACK\";I2S[12]=\"LPAR\";I2S[13]=\"MINUS\";I2S[14]=\"NAME\";I2S[15]=\"NUMBER\";I2S[16]=\"PLUS\";I2S[17]=\"RBRACE\";I2S[18]=\"RBARCK\";I2S[19]=\"RETURN\";I2S[20]=\"RPAR\";I2S[21]=\"SEMICOLON\";I2S[22]=\"STRUCT\";I2S[23]=\"TIMES\";" << endl;
	gc("fstream input;", 1);
	gc("char content[BUFF];", 1);
	gc("input.open(\"../Debug/token.txt\", ios::in);", 1);
	gc("token nowtoken;", 2);
	gc("while (!input.eof()) {", 2);
	gc("input.getline(content, BUFF);", 3);
	gc("char* p;", 3);
	gc("p = strtok(content, \", \");", 3);
	gc("nowtoken.type = I2S.at(atoi(p));", 3);
	gc("p = strtok(NULL, \", \");", 3);
	gc("nowtoken.name = p;", 3);
	gc("tokenqueue.push(nowtoken);", 3);
	gc("}", 2);
	gc("token endtoken;", 2);
	gc("endtoken.name = \"#\";", 2);
	gc("endtoken.type = \"#\";", 2);
	gc("tokenqueue.push(endtoken);", 2);
	outc << "\t\tint num =" << to_string(pda.numofstates);  outc << ";" << endl;
	gc("table2* GOTO2 = new table2[num];", 2);

	for (int i = 0; i < pda.numofstates; i++)
	{
		vector<string>::iterator iter = mtable[i].symbol.begin();
		for (iter; iter != mtable[i].symbol.end(); iter++)
			outc << "\t\tGOTO2[" << i << "].tg.push_back(\"" << (*iter) << "\");" << endl;
		vector<int>::iterator iter2 = mtable[i].nextState.begin();
		for (iter2; iter2 != mtable[i].nextState.end(); iter2++)
			outc << "\t\tGOTO2[" << i << "].next.push_back(" << (*iter2) << ");" << endl;
	}
	vector<Lr1Item>::iterator pit;
	int i = 1;
	for (pit = Item.begin(); pit != Item.end(); pit++)
	{
		outc << "\t\tproductions[" << i << "].leftp=" << "\"" << (*pit).leftp << "\"" << ";" << endl;
		outc << "\t\tproductions[" << i << "].numofrights=" << (*pit).numofrights << ";" << endl;
		for (int j = 0; j < (*pit).numofrights; j++)
		{
			outc << "\t\tproductions[" << i << "].rightp[" << j << "]=" << "\"" << (*pit).rightp[j] << "\"" << ";" << endl;
		}
		i++;
	}
	gc("stack<token> tokenstack;//token栈", 2);
	gc("stack<int> nstack;//state栈", 2);
	gc("nstack.push(0);", 2);
	gc("token ini;", 2);
	gc("ini.name = \"#\";", 2);
	gc("ini.type = \"#\";", 2);
	gc("tokenstack.push(ini);", 2);
	gc("vector<string> outbuffer;", 2);
	gc("outbuffer.push_back(\"#\");", 2);
	gc("token now=tokenqueue.front();", 2);
	gc("cout << \"编译开始！\" << endl;", 2);
	gc("cout << \"词法单元栈\" << \"\t\" << \"移进规约具体操作\"<<endl;", 2);
	gc("while (!(tokenqueue.empty()))", 2);
	gc("{", 2);
	gc("printbuffer(outbuffer);", 3);
	gc("tableAnalyse(tokenstack, nstack, outbuffer, now, GOTO2);", 3);
	gc("if (isused)", 3);
	gc("{", 3);
	gc("tokenqueue.pop();", 4);
	gc("now = tokenqueue.front();", 4);
	gc("}", 3);
	gc("isused = true;", 3);
	gc("}", 2);
	gc("}", 0);
}