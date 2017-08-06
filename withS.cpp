/*************************************************************************
    > File Name: t_withselect.c
    > Author: 
    > Mail: 
    >
 Created Time: 2017年08月05日 星期六 14时54分57秒
 ************************************************************************/
#include"../spider/httpdownload.h"
#include"htmlQue.h"
#include"../spider/downloadImage.h"
#include"../spider/parsepage.h"
#include<vector>
#include<string>
#include<regex>
#include<set>
#include<stdio.h>
#include<curl/curl.h>
#include<string.h>
#include<sys/time.h>
#include<stdlib.h>
#include<unistd.h>
using namespace std;

#define num_handlers 1000

static void *curl_hnd[num_handlers]; 
static int num_transfers;

struct MemoryStruct {
       char *memory;
       size_t size;

     MemoryStruct(){
         memory = static_cast<char*>(malloc(1));
        size = 0;
     }
 };
  
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    
    mem->memory = (char*)realloc((char*)mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        /* out of memory! */ 
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
            
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}


static void setup(CURL* hnd,MemoryStruct& chunk, int num, string urls)
{
   // chunk.memory = static_cast<char*>(malloc(1));
   // chunk.size = 0;
    
    //设置 urls
    curl_easy_setopt(hnd, CURLOPT_URL, urls.c_str());

    
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void*)& chunk);

    //HTTP/2 please
    curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
    
    curl_hnd[num] = hnd;
}


void add_url(CURL** hnd, CURLM* multi_hanlder, vector<MemoryStruct>& chunk, set<string>& url)
{
    int i=0;
    for(auto& it: url)
    {
        hnd[i] = curl_easy_init();
        setup(hnd[i], chunk[i], i, it);
        curl_multi_add_handle(multi_hanlder, hnd[i]);
        i++;
    }
}

int main(int argc, char **argv)
{
    CURL* easy[num_handlers];//定义最多可以有多少个handler
    CURLM* multi_hanlder; //多路复用的handler
    CURLMsg* msg;
    int Q;
    //struct MemoryStruct chunk[num_handlers]; 
    vector<MemoryStruct> chunk(1000);
    int i;
    int still_running_handler; //保持的链接
    
    string t=  "www.meizitu.com";

    string pa = "http(s)?://([\\w-]+\\.)+[\\w-]+(/[\\w-./?%&=]*)?";
    pa = "[[:alpha:]]*" + pa + "[[:alpha:]]*";
    regex reg(pa);
    
    //初始化multi stack
    multi_hanlder = curl_multi_init();
    //添加urls
    /*{
         httpdownload h(t);
        if(!h.run()){
            std::cout<<"error : http failed\n";
        }
        parsepage re(reg, h.getcontent());
        re.match();
        set<string> hurls = re.getHtmlUrl();

        add_url(easy, multi_hanlder,chunk, hurls);
    } */
    set<string> hurls;
    hurls.insert(t);
    add_url(easy, multi_hanlder, chunk, hurls);
    std::cout<<"log: start multi_hanlder setopt\n";
    curl_multi_setopt(multi_hanlder, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);

    //start action

    curl_multi_perform(multi_hanlder, &still_running_handler);
    int cnt = 0;
    int circie = 0;

    queHtml que;
    FILE* out = fopen("urls.txt", "wb");
    
    do{
        std::cout<<"log:: start select "<<circie ++ <<"\n";
        struct timeval timeout;
        int rc;  //select() return code
        CURLMcode mc; //curl_multi_fdset() return code

        fd_set fdread;
        fd_set fdwrite;
        fd_set fdexcep;

        int maxfd = -1;

        long curl_timeo = -1;

        FD_ZERO(&fdread);
        FD_ZERO(&fdwrite);
        FD_ZERO(&fdexcep);

        //设置超时
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        curl_multi_timeout(multi_hanlder, &curl_timeo);
        if(curl_timeo >= 0 ){
            timeout.tv_sec = curl_timeo / 1000;
            if(timeout.tv_usec > 1)
                timeout.tv_usec = 1;
            else
                timeout.tv_usec = (curl_timeo % 1000) * 1000;

        }

        //得到文件描述符 
        mc = curl_multi_fdset(multi_hanlder, &fdread, &fdwrite, &fdexcep, &maxfd);
        if(mc != CURLM_OK){
            fprintf(stderr, "curl_multi_fdset() failed , code %d\n", mc);
            break;
        }

        //success
        if(maxfd == -1){

            //如果没有得到文件描述符 就sleep 100 ms
            struct timeval wait = {0, 100*1000}; //100ms
            rc = select(0, NULL, NULL, NULL, &wait);
        }
        else{
            
            rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
            if(rc == 0){
                static int getZero = 0;
                getZero ++ ;
                if(getZero == 100)
                {
                    cout<<"log : too more time for wait\n";
                    break;
                }
            }
            std::cout<<"log: get select return :"<<rc<<"\n";
        }

        switch(rc){
            case -1:
                //select error
                std::cout<<"log: select error\n";
            case 0:
            default:
                std::cout<<"log: start multi perform\n";
                curl_multi_perform(multi_hanlder, &still_running_handler);
               // cout<<"log:  content is:"<<chunk[0].memory<<"\n";
                //解析下载的网页
                for(int i=0 ;i<rc; i++)
                {
                    while((msg = curl_multi_info_read(multi_hanlder, &Q))){
                        if(msg->msg == CURLMSG_DONE){
                            CURL* e = msg->easy_handle;

                            curl_multi_remove_handle(multi_hanlder, e);
                            curl_easy_cleanup(e);
                        }
                    }

                    std::cout<<"log: start parse page with :"<< rc<<"\n";
                    struct MemoryStruct& tp = chunk[i];
                    string tmp(tp.memory);

                    std::cout<<"log: page length :"<<tmp.size()<<"\n";
                    parsepage re(reg, tmp);
                    re.match();
                    set<string> urls = re.getHtmlUrl();
                    
                    //判断其唯一性
                    que.addUrls(urls);
                    set<string>& getU = que.getUrls();
                    //并将其加入 multi_hanlder 中                     
                    add_url(easy,multi_hanlder,chunk, getU);
                    for(auto& it: getU){
                        
                        fwrite(it.c_str(), it.size(), 1, out);    
                        fwrite("\n", strlen("\n"), 1, out);    
                        
                        std::cout<<"log : url:"<<it<<"\n";
                        cnt ++;
                    }
                    que.clean();
                    cout<<" urls  count is :"<<cnt<<"\n";
                    curl_multi_setopt(multi_hanlder, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);
                } 
                break;    
            }
        }while(still_running_handler);

    curl_multi_cleanup(multi_hanlder);

    for(i=0; i<num_transfers; i++)
        curl_easy_cleanup(easy[i]);
    
    return 0;
}

