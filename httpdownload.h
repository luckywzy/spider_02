/*************************************************************************
	> File Name: httpdownload.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年07月26日 星期三 22时34分09秒
 ************************************************************************/

#include<iostream>
#include<string>
#include<curl/curl.h>
#include<utility>
using namespace std;

class httpdownload{
public:
    httpdownload(string& url);
    bool run();
    int size()
    {
        return content.size();
    }
    string& getcontent() //得到页面内容的句柄
    {
        return content;    
    }
    string& gethandle(){
        return content;
    }
    void setPageLen(int len){
        pageLen += len;
    }
    int getPageLen(){
        return pageLen;
    }
    static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream);
    
    void clearCurl();
private:
    void curl_init();
    CURLcode perform();
    string url; //传入的url
    CURL* curl;
    string content; //下载的页面内容
    int pageLen; //
};




