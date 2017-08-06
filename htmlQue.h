/*************************************************************************
	> File Name: queHtml.h
	> Author: 
	> Mail: 
	> Created Time: 2017年08月01日 星期二 19时52分56秒
 ************************************************************************/

#ifndef _HTMLQUE_H
#define _HTMLQUE_H
#include<map>
#include<queue>
#include<string>
#include<set>
#include<iostream>
using namespace std;


class queHtml{

private:
    map<size_t, string> Urls;
    set<string> m_que;
    
public:
    queHtml();
    ~queHtml();
    void addUrls(set<string>& urls);
    set<string>& getUrls();
    
    void clean(); //clean set
    
};

#endif
