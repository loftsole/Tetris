#ifndef TETRIS_H
#define TETRIS_H

#include <QObject>
#include <QTime>
#include <QDebug>

class Tetris:public QObject
{
    Q_OBJECT
public:
    Tetris();

    void init();
    void gameStart();
    void dealOpMsg(QString op_msg);//处理对方消息

    void rotate();
    void moveLeft();
    void moveRight();
    void moveDown();
    void useItem(int item_pos);

    int getBox(int i,int j);
    int getNextBlock();
    int getOpBox(int i,int j);

    int next_block[4][4];
    int op_next_block[4][4];
    int item[3];
    bool is_ink;
    bool is_reverse;
signals:
    void gameOver();
    void print();
    void send(QString msg);
    void changeTimer(int block_count);
    void opUseItem(int item_num);
    void useItemSuccess(int item_num);
    void relieveItem(int item_num);
private:
    void createBlock(int block[4][4]);
    void rotateBlock(int block[4][4]);
    void copyBlock(int sblock[4][4],int tblock[4][4]);
    void check();//检测消行
    void setBlock();
    void toStable();
    void end();
    void sendMsg();//发送我方数据

    static const int ROW=20;
    static const int COL=12;
    int stable_block[ROW][COL];
    int op_block[ROW][COL];
    int cur_block[4][4];
    int curx,cury;
    int block_count;
    int ink_count;
    int reverse_count;
    int item_count;
    bool game_over;

    //定义图案代码和边界
    //田字
    int item1[4][4]=
    {
        {0,1,1,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    };
    //右L
    int item2[4][4]=
    {
        {0,2,0,0},
        {0,2,0,0},
        {0,2,2,0},
        {0,0,0,0}
    };
    //左L
    int item3[4][4]=
    {
        {0,0,3,0},
        {0,0,3,0},
        {0,3,3,0},
        {0,0,0,0}
    };
    //右S
    int item4[4][4]=
    {
        {0,4,0,0},
        {0,4,4,0},
        {0,0,4,0},
        {0,0,0,0}
    };
    //左S
    int item5[4][4]=
    {
        {0,0,5,0},
        {0,5,5,0},
        {0,5,0,0},
        {0,0,0,0}
    };
    //山形
    int item6[4][4]=
    {
        {0,0,6,0},
        {0,6,6,6},
        {0,0,0,0},
        {0,0,0,0}
    };
    //长条
    int item7[4][4]=
    {
        {0,0,7,0},
        {0,0,7,0},
        {0,0,7,0},
        {0,0,7,0}
    };
    //道具
    int item8[4][4]=
    {
        {8,8,8,0},
        {8,8,8,0},
        {8,8,8,0},
        {0,0,0,0}
    };
};

#endif // TETRIS_H
