/*************************************************************************
	> File Name: parsepage.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年07月27日 星期四 19时44分58秒
 ************************************************************************/
#include"parsepage.h"
#include<set>
#include<regex>
#include<iostream>

using namespace std;

parsepage::parsepage(regex pat, string& in):
    url_pattern(pat),
    page(in)
{
}


bool parsepage::match()
{
    static string jpg(".jpg");
    static string html(".html");
    for(std::sregex_iterator it(page.begin(), page.end(), url_pattern), end;
        it != end;  ++it)
    {
        const string& u = it->str();
        string t3(u.end() - 4, u.end());
        string t4(u.end() - 5, u.end());
        if(t3 == jpg)
        {
            picUrls.insert(it->str());
        }
        else if(t4 == html)
        {
            htmlUrls.insert(it->str());
        }
        //urls.insert(it->str());

    }
    cout<<"log:\t 匹配的pic url数量为: "<<picUrls.size()<<"\n";
    cout<<"log:\t 匹配的html url数量为: "<<htmlUrls.size()<<"\n";
    return true;
}

set<string>& parsepage::getJpgUrl()
{
    return picUrls;
}

set<string>& parsepage::getHtmlUrl()
{
    return htmlUrls;
}



