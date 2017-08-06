/*************************************************************************
	> File Name: httpdownload.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年07月26日 星期三 22时41分47秒
 ************************************************************************/
#include"httpdownload.h"
#include<curl/curl.h>
#include<iostream>
#include<iconv.h>
#include<locale.h>
#include<string>
#include<string.h>
#include<stdlib.h>
using namespace std;
//GBK 到 UTF-8
int gb2312toutf8(char *sourcebuf,size_t sourcelen,char *destbuf,size_t* destlen) {   

    iconv_t cd;   
    if( (cd = iconv_open("utf-8","gb2312")) ==0  )     
        return -1;

    memset(destbuf,0,*destlen);   
    char **source = &sourcebuf;   

    char **dest = &destbuf;
    if((size_t)-1 == iconv(cd,source,&sourcelen,dest,destlen))     
        return -1;   

    iconv_close(cd);   
    cout<<"log:\t转码成功\n";
    return 0;   
}

int gbk2_utf8(char* src, char* des, int maxLen)
{
    if(src == NULL)
    {
        cout<<"bad parameter! src is NULL\n";
        return -1;
    }
    //转unicode
    if(NULL == setlocale(LC_ALL, "zh_CN.gb2312"))
    {
        cout<<"bad parameter  gbk\n";
        return -1;
    }
    int unicodeLen = mbstowcs(NULL, src, 0); //计算长度
    if(unicodeLen <= 0)
    {
        cout<<"can not transfer\n";
        return -1;
    }
    wchar_t* unicodesrc =(wchar_t*)calloc(sizeof(wchar_t), unicodeLen+1);
    mbstowcs(unicodesrc, src, strlen(src));
    //转UTF-8
    if(NULL == setlocale(LC_ALL, "zh_CN.utf8"))
    {
        cout<<"bad parameter to utf8\n";
        return -1;
    }
     int utfLen = wcstombs(NULL, unicodesrc, 0); 
    if(utfLen <= 0)
    {
        cout<<"can not transfer\n";
        return -1;
    }
    if(utfLen >= maxLen)
    {
        cout<<"des str memory not enough\n";
        return -1;
    }
     wcstombs(des, unicodesrc, utfLen);
    des[utfLen] = '\0';
    free(unicodesrc);

    return utfLen;
}


httpdownload::httpdownload(string& url):url(url)
{
    content = "";
    pageLen = 0;
}

void httpdownload::curl_init()
{
    curl =  curl_easy_init();
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_ENCODING, "");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &httpdownload::write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    cout<<"log; setopt done\n";
}

CURLcode httpdownload::perform()
{
    cout<<"log: perform is started\n";
    return curl_easy_perform(curl);
}
//写入到大buf中
size_t httpdownload::write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
    size_t res_size = size * nmemb;
    httpdownload* h_ptr = (httpdownload*)stream;
    

    string tmp(static_cast<char*>(ptr));    
    
    //这里不一定是一次写入，所以需要+=
    //std::swap(h_ptr->gethandle(),tmp);
    string& sptr = h_ptr->gethandle();
    sptr += tmp;
    h_ptr->setPageLen(res_size);

    cout<<"log:\tpage length "<<"length: "<<res_size<<"\t buf length:"<<sptr.length()<<"\n";
    return res_size;
}

void httpdownload::clearCurl()
{
    if(curl)
    {
        curl_easy_cleanup(curl);
    }
}

bool httpdownload::run() //得到返回结果
{
    curl_init();
    CURLcode ret;
    if((ret = perform()) != CURLE_OK)
    {
        cout<<"http is failed\n";
        clearCurl();
        return false;
    }
    clearCurl();
    cout<<"log:\t run is succeed\t buf length: "<<content.size()<<"\n";
    
    size_t dstLen = content.size();
    
    char* tmp = new char[content.length() * 2];
    char* ptr = new char[content.length() + 1];
    strcpy(ptr, content.c_str());
    size_t transLen = 0;
    string s;
    //转码的时候每次只能转码一定长度的字符串，所以当你有一个长字符串的时候，需要用循环
    while(transLen < getPageLen()  || tmp[transLen] != NULL){
        gb2312toutf8(ptr + transLen, content.size(), tmp, &dstLen);
        string str(tmp);
        s += str;
        transLen += str.length();
    }
    std::swap(s, content);

    cout<<"log: transform to utf8 length : "<<content.length()<<"\n";
    delete[] tmp;
    delete[] ptr;
    return true;
}

