/*************************************************************************
	> File Name: queHtml.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年08月01日 星期二 20时00分03秒
 ************************************************************************/
#include"htmlQue.h"
#include<iostream>
#include<map>
#include<queue>
#include<set>
#include<string>
using namespace std;

queHtml::queHtml(){}

queHtml::~queHtml()
{
}

void queHtml::addUrls(set<string>& urls)
{
    for(auto & it : urls){
        size_t hid = std::hash<string>()(it);
        if(Urls.find(hid) == Urls.end())
        {
            pair<size_t, string> p = make_pair(hid, it);   
            Urls.insert(p);
            m_que.insert(it);
        
        }
    }
}

set<string>& queHtml::getUrls()
{
    return m_que;
}

void queHtml::clean()
{
    m_que.clear();
}







