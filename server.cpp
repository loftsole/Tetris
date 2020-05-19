//临界区是：clnt_cnt和clnt_socks访问处

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include <map>
#include <iostream>
#include <string>
#include <queue>
using namespace std;

#define BUF_SIZE 128
#define MAX_CLNT 256

void * handle_clnt(void * arg);
void send_msg(int clnt_sock,string str_msg);
void send_msg_all(char * msg, int len);
void send_user_list(int clnt_sock);
void error_handling(char * msg);
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;
map<string,bool> online_user;//记录用户名是否在线

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t t_id;

    //创建互斥量
    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(16555);

    bind(serv_sock, (struct sockaddr *) &serv_adr, sizeof(serv_adr));
    listen(serv_sock, 5);

    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz); //阻断，监听客服端连接请求

        //临界区
        pthread_mutex_lock(&mutx); //加锁
        clnt_socks[clnt_cnt++] = clnt_sock; //新连接的客服端保存到clnt_socks数组里
        pthread_mutex_unlock(&mutx); //释放锁

        //创建线程
        pthread_create(&t_id, NULL, handle_clnt, (void*) &clnt_sock);
        pthread_detach(t_id); //销毁线程，线程return后自动调用销毁，不阻断

        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
    }

    close(serv_sock);
    return 0;
}

//线程执行
void * handle_clnt(void * arg)
{
    int clnt_sock = *((int *)arg);
    int str_len = 0, i;
    char msg[BUF_SIZE];
    string user_name;
    bool get_user_name=false;//用户名是否已输入
    bool game_start =false;//游戏是否开始

    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
      {
        string ori_data;
        for (int i=0;i<str_len;i++)//转换为string方便处理
          ori_data+=msg[i];

        while (ori_data[0]=='/')
          {
            string data;
            ori_data=ori_data.substr(1);
            int dis_pos=ori_data.find('/');
            if (dis_pos!=string::npos)
              {
                data=ori_data.substr(0,dis_pos);
                ori_data=ori_data.substr(dis_pos);
              }
            else
              data=ori_data;
            //cout<<"ori:"<<ori_data<<endl;
            //cout<<data<<endl;
            if (data[0]=='n' && get_user_name==false)//客户端发送用户名
              {
                user_name=data.substr(1);//记录用户名
                cout<<user_name<<endl;
                get_user_name=true;
                pthread_mutex_lock(&mutx);
                online_user[user_name]=true;//使其上线
                pthread_mutex_unlock(&mutx);
              }
            else if (data[0]=='r' && game_start==false)//客户端请求刷新用户列表
              {
                send_user_list(clnt_sock);
                //cout<<"refresh"<<endl;
              }
            else//已发送用户名
              {
                string user_msg=user_name+":"+data;//群发消息测试
                for (int i=0;i<user_msg.length();i++)
                  msg[i]=user_msg[i];
                msg[user_msg.length()]='\0';
                send_msg_all(msg,user_msg.length());
              }
          }
      }
    //从数组中移除当前客服端
    pthread_mutex_lock(&mutx);
    online_user[user_name]=false;//user下线
    for (i = 0; i < clnt_cnt; i++)
    {
        if (clnt_sock == clnt_socks[i])
        {
            while (i++ < clnt_cnt - 1)
                clnt_socks[i] = clnt_socks[i + 1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}

//向指定客户端发送消息
void send_msg(int clnt_sock,string str_msg)
{
  char msg[BUF_SIZE];
  msg[0]='/';
  for (int i=0;i<str_msg.length();i++)
    msg[i+1]=str_msg[i];
  msg[str_msg.length()+1]='\0';
  write(clnt_sock, msg,str_msg.length()+1);
}

void send_msg_all(char * msg, int len)
{
  int i;
  pthread_mutex_lock(&mutx);
  for (i = 0; i < clnt_cnt; i++)
    //write(clnt_socks[i], msg, len);
    send_msg(clnt_socks[i],msg);
  pthread_mutex_unlock(&mutx);
}
//向指定客户端发送在线玩家列表
void send_user_list(int clnt_sock)
{
  queue<string> user_list;//获取在线玩家列表
  while (!user_list.empty()) user_list.pop();
  pthread_mutex_lock(&mutx);
  //online_user[user_name]=true;
  for (map<string,bool>::iterator it=online_user.begin();it!=online_user.end();it++)//遍历玩家列表
    {
      if (it->second)
        {
          user_list.push(it->first);
        }
    }
  pthread_mutex_unlock(&mutx);
  while (!user_list.empty())//发送
    {
      send_msg(clnt_sock,user_list.front());
      user_list.pop();
    }
  send_msg(clnt_sock,"finish");
}


void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

