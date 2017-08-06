/*************************************************************************
    > File Name: parsepage.h
    > Author: 
    > Mail: 
    > Createn Time: 2017年07月27日 星期四 19时32分04秒
 ************************************************************************/

#ifndef _PARSEPAGE_H
#define _PARSEPAGE_H
#include<regex>
#include<string>
#include<set>
using namespace std;

class parsepage{
public:
    parsepage(std::regex , string&);
    bool match();

    set<string>& getJpgUrl();
    set<string>& getHtmlUrl();
private:
    std::regex url_pattern;//模式串
    string& page;
     //匹配的urls
    std::set<string> picUrls;
    std::set<string> htmlUrls;
};



#endif
