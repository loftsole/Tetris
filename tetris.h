//游戏主体
#ifndef TETRIS_H
#define TETRIS_H

#include <QObject>
#include "tcpclient.h"

class Tetris:public QObject
{
    Q_OBJECT
public:
    Tetris(QObject *parent=nullptr);

    void init();//初始化

    /*UI交互*/
    //游戏部分
    //输入
    void move_left();//左移
    void move_right();//右移
    void move_rotate();//旋转
    void move_down();//下落
    void use_item();//使用道具
    //输出
    int* get_my_ui();//我方游戏界面
    int* get_opponent_ui();//对方游戏界面
    int get_next_block();//下一个方块
    int get_item();//道具信息

    //联机部分
    void input_user_name(QString name);//输入用户名

    /*测试用*/
    void send_test_msg(QString msg);
signals:
    /*需要显示对应内容时发出信号*/
    void update_my_ui();//我方界面变化
    void update_opponent_ui();//对方界面变化
    void update_next_block();//下一个方块变化
    void opponent_use_item();//对方使用道具
    void game_over();//游戏结束
    /*在ui中设置对应的槽*/
private:
    void test_client();

    tcpClient *client;
    QString user_name;

    int game_matrix[25][15];//模拟矩阵
    int next_block_number;//下一个方块的编号
    int item[5];//道具信息
    QString opponent_game_matrix;//对方数据

};

#endif // TETRIS_H
