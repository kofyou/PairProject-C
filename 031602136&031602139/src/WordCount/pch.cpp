// pch.cpp: 与预编译标头对应的源文件；编译成功所必需的

#include "pch.h"

// 一般情况下，忽略此文件，但如果你使用的是预编译标头，请保留它。
void counter::initParameter(int argc, char *argv[])
{
	if (argc < 7)
	{
		printf("Missing parameters.\n");
		exit(0);
	}
	for (int i = 1; i < argc; i++)
	{
		string tmp(argv[i]);
		i++;
		if (tmp == "-i")
		{
			inFilename = (char*)malloc((strlen(argv[i]) + 2) * sizeof(char));
			if (inFilename != NULL)
				strcpy_s(inFilename, (strlen(argv[i]) + 2), argv[i]);
		}
		if (tmp == "-o")
		{
			outFilename = (char*)malloc((strlen(argv[i]) + 2) * sizeof(char));
			if (outFilename != NULL)
				strcpy_s(outFilename, (strlen(argv[i]) + 2), argv[i]);
		}
		if (tmp == "-w")
		{
			weight = atoi(argv[i]);
		}
		if (tmp == "-m")
		{
			phraseSize = atoi(argv[i]);
		}
		if (tmp == "-n")
		{
			topNum = atoi(argv[i]);
		}
	}
}

/*
void counter::initInFilename(char tempName[])
{
	inFilename = (char*)malloc( (strlen(tempName)+2) * sizeof(char));
	strcpy(inFilename, tempName);
}*/

void counter::countPerLine(string line)
{
	unsigned int i = 0;
	// empty line
	while (i < line.length() && isspace( (unsigned char) line[i]))
	{
		char_num++;
		i++;
	}
	if (i != line.length())
	{
		line_num++;
		while (i < line.length())
		{
			char_num++;
			i++;
		}
	}
}

pair<int,int> counter::countCharLine()
{
	ifstream inFile(inFilename, ios::in);
	if (inFile.fail())cout << "fail to open input file\n";
	while (inFile)
	{
		string line;
		//处理编号
		getline(inFile, line);
		if (inFile.fail() && line == "")
			break;
		//处理Title
		getline(inFile, line);
		if (inFile.fail() && line == "")
			break;
		if (!inFile.eof())
			line += '\n';
		line = line.substr(7);
		countPerLine(line);
		//处理Abstract
		getline(inFile, line);
		if (inFile.fail() && line == "")
			break;
		if (!inFile.eof())
			line += '\n';
		line = line.substr(10);
		countPerLine(line);
		//处理两个空行
		if (!inFile.eof())
		{
			getline(inFile, line);
			getline(inFile, line);
		}
	}
	inFile.close();
	return pair<int, int>(char_num, line_num);
}

void counter::splitPerLine(string line, int lineType)
{
	int value;
	if (lineType == lineTitle)
	{
		if (weight == 1)
			value = 10;
		else
			value = 1;
	}
	else
	{
		value = 1;
	}
	queue<int> wordList;
	unsigned int i = 0;
	while (i < line.length())
	{
		// handle characters before a word
		while (i < line.length() && !isalpha((unsigned char)line[i]))
		{
			// handle "123file"
			if (isdigit((unsigned char)line[i]))
			{
				if (!wordList.empty())
				{
					queue<int> empty;
					swap(empty, wordList);
				}
				while (isalnum((unsigned char)line[i]) && i < line.length())
				{
					i++;
				}
			}
			i++;
		}
		// handle a word
		int tempWord=0;
		bool firstAlpha = true;
		while (i < line.length())
		{
			if (isalpha((unsigned char)line[i]))
			{
				if (firstAlpha)
				{
					wordList.push(i);
					firstAlpha = false;
				}
				line[i] = tolower(line[i]);
				tempWord ++;
			}
			if (isdigit((unsigned char)line[i]))
			{
				if (tempWord < 4)
				{
					i++;
					if (!wordList.empty())
					{
						queue<int> empty;
						swap(empty, wordList);
					}
					break;
				}
				else
				{
					tempWord ++;
				}
			}
			if (!isalnum((unsigned char)line[i]) || i == (int)line.length() - 1)
			{
				if (tempWord >= 4)
				{
					word_num++;
					if (wordList.size() == phraseSize)
					{
						int phraseLen = i - wordList.front();
						if (i == (int)line.length() - 1 && isalnum((unsigned char)line[i]))
							phraseLen++;
						string phrase = line.substr(wordList.front(), phraseLen);
						map<string, int>::iterator iter;
						iter = dic.find(phrase);
						if (iter != dic.end())
							iter->second += value;
						else
							dic.insert(pair<string, int>(phrase, value));
						wordList.pop();
					}
				}
				else
				{
					if (!wordList.empty())
					{
						queue<int> empty;
						swap(empty, wordList);
					}
				}
				i++;
				break;
			}
			i++;
		}
	}
}

void counter::initDic()
{
	ifstream inFile(inFilename, ios::in);
	if (inFile.fail())cout << "fail to open input file\n";
	while (inFile)
	{
		string line;
		//处理编号
		getline(inFile, line);
		if (inFile.fail() && line == "")
			break;
		//处理Title
		getline(inFile, line);
		if (inFile.fail() && line == "")
			break;
		if (!inFile.eof())
			line += '\n';
		line = line.substr(7);
		splitPerLine(line, lineTitle);
		//处理Abstract
		getline(inFile, line);
		if (inFile.fail() && line == "")
			break;
		if (!inFile.eof())
			line += '\n';
		line = line.substr(10);
		splitPerLine(line, lineAbstract);
		//处理两个空行
		if (!inFile.eof())
		{
			getline(inFile, line);
			getline(inFile, line);
		}
	}
	inFile.close();
}

int counter::countWord()
{
	initDic();
	return word_num;
}

//it is a one step solution
void counter::splitAndcount(string line,int lineType)
{
	int value;
	if (lineType == lineTitle)
	{
		if (weight == 1)
			value = 10;
		else
			value = 1;
	}
	else
	{
		value = 1;
	}
	queue<int> wordList;
	unsigned int i = 0;
	// empty line
	while (i < line.length() && isspace( (unsigned char) line[i]))
	{
		char_num++;
		i++;
	}
	if (i != line.length())
	{
		line_num++;
		while (i < line.length())
		{
			// handle characters before a word
			while (i < line.length() && !isalpha( (unsigned char) line[i]))
			{
				// handle "123file"
				if (isdigit( (unsigned char) line[i]))
				{
					if (!wordList.empty())
					{
						queue<int> empty;
						swap(empty, wordList);
					}
					while (isalnum( (unsigned char) line[i]) && i < line.length())
					{
						char_num++;
						i++;
					}
				}
				if (i != line.length())
				{
					char_num++;
				}
				i++;
			}
			// handle a word
			int tempWord=0;
			bool firstAlpha=true;
			while (i < line.length())
			{
				char_num++;
				if (isalpha( (unsigned char) line[i]))
				{
					if (firstAlpha)
					{
						wordList.push(i);
						firstAlpha = false;
					}
					line[i] = tolower(line[i]);
					tempWord ++;
				}
				if (isdigit( (unsigned char) line[i]))
				{
					if (tempWord < 4)
					{
						i++;
						if (!wordList.empty())
						{
							queue<int> empty;
							swap(empty, wordList);
						}
						break;
					}
					else
					{
						tempWord ++;
					}
				}
				if (!isalnum( (unsigned char) line[i]) || i == (int) line.length() - 1)
				{
					if (tempWord >= 4)
					{
						word_num++;
						if (wordList.size() == phraseSize)
						{
							int phraseLen = i - wordList.front();
							if (i == (int)line.length() - 1 && isalnum((unsigned char)line[i]))
								phraseLen++;
							string phrase = line.substr(wordList.front(),phraseLen);
							map<string, int>::iterator iter;
							iter = dic.find(phrase);
							if (iter != dic.end())
								iter->second += value;
							else
								dic.insert(pair<string, int>(phrase, value));
							wordList.pop();
						}
					}
					else
					{
						if (!wordList.empty())
						{
							queue<int> empty;
							swap(empty, wordList);
						}
					}
					i++;
					break;
				}
				i++;
			}
		}
	}
}

void counter::countAll()
{
	ifstream inFile(inFilename, ios::in);
	if (inFile.fail())cout << "fail to open input file\n";
	while (inFile)
	{
		string line;
		//处理编号
		getline(inFile, line);
		if (inFile.fail() && line == "")
			break;
		//处理Title
		getline(inFile, line);
		if (inFile.fail() && line == "")
			break;
		if (!inFile.eof())
			line += '\n';
		line = line.substr(7);
		splitAndcount(line,lineTitle);
		//处理Abstract
		getline(inFile, line);
		if (inFile.fail() && line == "")
			break;
		if (!inFile.eof())
			line += '\n';
		line = line.substr(10);
		splitAndcount(line,lineAbstract);
		//处理两个空行
		if (!inFile.eof())
		{
			getline(inFile, line);
			getline(inFile, line);
		}
	}
	inFile.close();
}

bool cmp(const pair<string, int>& a, const pair<string, int>& b)
{
	if (a.second != b.second)
		return a.second > b.second;
	else
		return a.first < b.first;
}

void counter::frequency()
{
	sortedDic.assign(dic.begin(), dic.end());
	sort(sortedDic.begin(), sortedDic.end(), cmp);
}

/////////////////////////////////////////////


void counter::print()   
{
	ofstream outFile(outFilename,ios::out);
	if (outFile.fail())cout << "fail to open output file\n";
	outFile<<"characters: "<<char_num<<endl;
	outFile<<"words: "<<word_num<<endl;
	outFile<<"lines: "<<line_num<<endl;

	for(int i=0;i< (int) sortedDic.size() && i<topNum ;i++)
		outFile<<"<"<<sortedDic[i].first<<">"<<": "<<sortedDic[i].second<<endl;

	outFile.close();

	/*
	cout << "characters: " << char_num << endl;
	cout << "words: " << word_num << endl;
	cout << "lines: " << line_num << endl;

	for (int i = 0; i < sortedDic.size(); i++)
		cout << "<"<<sortedDic[i].first << ">"<<": " << sortedDic[i].second << endl;
	*/
}