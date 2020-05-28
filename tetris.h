#ifndef TETRIS_H
#define TETRIS_H

//为了获得随机数
#include <cstdlib>
#include <ctime>

#define MAXX 10     //显示窗口的横向格数
#define MAXY 20     //显示窗口的竖向格数
#define NEXTMAXX 6  //“下一个”显示窗口的横向格数
#define NEXTMAXY 6  //“下一个”显示窗口的竖向格数
#define WIDTH 30    //单格的宽度
#define HEIGHT 30   //单格的高度
#define INTERVAL 4  //单格之间的间隔
#define COUNT 4     //每个方块的格数

//Block结构体：一个方块
struct Block
{
    int x[COUNT];   //方块单格的x坐标
    int y[COUNT];   //方块单格的y坐标
    int centerX;    //方块的中心x坐标
    int centerY;    //方块的中心y坐标
    int ID;         //方块的ID
};


class Tetris
{
public:
    Tetris();
    void gameStart();
    void createBlock();             //创建当前方块
    Block getNextBlock();           //获得下一个方块
    Block getBlock();               //获得当前方块
    int getScore();                 //获得分数
    int getBox(int x, int y);       //获得相应坐标的状态
    bool rotate();                  //旋转
    bool moveToLeft();              //向左移动
    bool moveToRight();             //向右移动
    bool moveToBottom();            //向下移动
    bool isEnd();                   //判断是否结束
    void killLines();               //消去整行
    void clear();                   //重新初始化

    static int getWidth();          //获得窗口的宽度
    static int getHeight();         //获得窗口的高度
    static int getNextWidth();      //获得“下一个”窗口的宽度
    static int getNextHeight();     //获得“下一个”窗口的高度



private:
    void createNextBlock();         //创建下一个方块
    bool move(int dx, int dy);      //是否可以移动
    void blockToBox();              //将block中的数据转移到box中
    bool isRotatable();             //是否可以旋转
    int getFirstFullLine();         //获得第一个整行

private:
    int score;          //分数
    Block block;        //当前方块
    Block nextBlock;    //下一个方块
    int box[MAXX][MAXY];//方格的坐标系 1表示右方格，0表示没有方格


};

#endif // TETRIS_H
