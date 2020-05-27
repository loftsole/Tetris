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

#define BUF_SIZE 512
#define MAX_CLNT 1024

void * handle_clnt(void * arg);
void send_msg(int clnt_sock,string str_msg);//向指定客户端发送消息
void send_msg_all(char * msg, int len);//广播
void send_user_list(int clnt_sock);//发送在线玩家列表
void error_handling(char * msg);
int clnt_cnt = 0;//客户端数量
int clnt_socks[MAX_CLNT];//客户端套接字列表
pthread_mutex_t mutx;//互斥量

struct user_sock_default {//为客户端套接字设定默认值
  int sock = -1;
  bool game=false;
};
map<string,user_sock_default> online_user;//记录用户名和对应套接字

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t t_id;

    //创建互斥量
    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    //设定套接字
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(16555);
    //绑定套接字
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
    int op_clnt_sock=-1;
    int str_len = 0, i;
    char msg[BUF_SIZE];
    string user_name,op_user_name;
    bool op_game_status;
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
            ori_data=ori_data.substr(1);//去除分隔符
            int dis_pos=ori_data.find('/');//下一个分隔符位置
            if (dis_pos!=string::npos)//有下一个分隔符
              {
                data=ori_data.substr(0,dis_pos);
                ori_data=ori_data.substr(dis_pos);
              }
            else//没有下一个分隔符
              data=ori_data;

            //打印消息
            cout<<user_name<<":"<<data<<endl;

            //分隔符处理完成
            if (data[0]=='n' && get_user_name==false)//客户端发送用户名
              {
                user_name=data.substr(1);//记录用户名
                cout<<user_name<<" 已上线"<<endl;
                get_user_name=true;

                pthread_mutex_lock(&mutx);
                online_user[user_name].sock=clnt_sock;//使其上线
                pthread_mutex_unlock(&mutx);
              }
            else if (data[0]=='r' && game_start==false)//客户端请求刷新用户列表
              {
                send_user_list(clnt_sock);
                //cout<<"refresh"<<endl;
              }
            else if (data[0]=='>' && game_start==false)//发送连接请求
              {
                op_user_name=data.substr(1);//记录对方用户名
                pthread_mutex_lock(&mutx);
                op_clnt_sock=online_user[op_user_name].sock;
                op_game_status=online_user[op_user_name].game;
                pthread_mutex_unlock(&mutx);
                if (op_clnt_sock==-1 || op_game_status==true)
                  {
                    send_msg(clnt_sock,"e1");//返回对方不在线或已经开始游戏
                  }
                else
                  {
                    string msg="<"+user_name;
                    send_msg(op_clnt_sock,msg);//向对方客户端发送询问信息
                  }
              }
            else if (data[0]=='<' && game_start==false)//接受连接请求
              {
                op_user_name=data.substr(1);//记录对方用户名
                pthread_mutex_lock(&mutx);
                op_clnt_sock=online_user[op_user_name].sock;
                op_game_status=online_user[op_user_name].game;
                pthread_mutex_unlock(&mutx);
                if (op_clnt_sock==-1 || op_game_status==true)
                  {
                    send_msg(clnt_sock,"e1");//返回对方不在线信息
                  }
                else
                  {
                    //连接成功 游戏开始
                    game_start=true;
                    send_msg(op_clnt_sock,"go");
                    send_msg(clnt_sock,"go");
                    pthread_mutex_lock(&mutx);
                    online_user[op_user_name].game=true;
                    online_user[user_name].game=true;
                    pthread_mutex_unlock(&mutx);
                  }
              }
            else if (data[0]=='!' && game_start==false)//拒绝连接请求
              {
                op_user_name=data.substr(1);
                pthread_mutex_lock(&mutx);
                op_clnt_sock=online_user[op_user_name].sock;
                pthread_mutex_unlock(&mutx);
                send_msg(op_clnt_sock,"decline");
              }
            else if (data[0]=='g' && game_start==false)//发送方确认游戏开始
              {
                game_start=true;
              }
            else if (game_start==true)//游戏已开始
              {
                send_msg(op_clnt_sock,data);//进行数据转发
                if (data[0]=='o')//游戏结束
                  {
                    pthread_mutex_lock(&mutx);
                    online_user[op_user_name].game=false;
                    online_user[user_name].game=false;
                    pthread_mutex_unlock(&mutx);
                    game_start=false;
                    cout<<"game over"<<endl;
                  }
              }
          }
      }
    //通知对方已下线
    if (op_clnt_sock!=-1)
      send_msg(op_clnt_sock,"e2");
    //从数组中移除当前客服端
    pthread_mutex_lock(&mutx);
    online_user[user_name].game=false;
    online_user[op_user_name].game=false;
    online_user[user_name].sock=-1;//user下线
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
  for (map<string,user_sock_default>::iterator it=online_user.begin();it!=online_user.end();it++)//遍历玩家列表
    {
      if (it->second.sock!=-1 && it->second.game==false)
        {
          user_list.push(it->first);
        }
    }
  pthread_mutex_unlock(&mutx);
  while (!user_list.empty())//发送
    {
      string msg="-"+user_list.front();
      send_msg(clnt_sock,msg);
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

