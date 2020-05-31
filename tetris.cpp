#include "tetris.h"

Tetris::Tetris()
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    //init();
}
void Tetris::init()
{
    block_count=0;
    for (int i=0;i<20;i++)
        for (int j=0;j<20;j++)
        {
            stable_block[i][j]=0;
            op_block[i][j]=0;
        }
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
        {
            next_block[i][j]=0;
            op_next_block[i][j]=0;
            cur_block[i][j]=0;
        }
    for (int i=0;i<3;i++)
        item[i]=0;
    item_count=0;
    ink_count=0;
    reverse_count=0;
    game_over=false;
    is_reverse=false;
    is_ink=false;
}

void Tetris::gameStart()
{
    init();
    createBlock(next_block);
    setBlock();
    emit print();
}
int Tetris::getBox(int x,int y)
{
    //下落方块
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
        {
            if (curx+i==x && cury+j==y && cur_block[i][j]>0)
                return cur_block[i][j];
        }
    return stable_block[x][y];
}
int Tetris::getOpBox(int i, int j)
{
    return op_block[i][j];
}
void Tetris::sendMsg()
{
    QString msg;
    //预告方块
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
        {
            msg+=QString::number(next_block[i][j]);
        }
    for (int i=0;i<ROW;i++)
        for (int j=0;j<COL;j++)
        {
            msg+=QString::number(stable_block[i][j]);
        }
    emit send(msg);
}
void Tetris::dealOpMsg(QString op_msg)
{
    if (op_msg.startsWith("i"))//对方使用道具
    {
        op_msg=op_msg.mid(1);
        int num=op_msg.toInt();
        emit opUseItem(num);
        if (num==1)
            copyBlock(next_block,item8);
        if (num==2)
        {
            is_reverse=true;
            reverse_count=block_count+7;
        }
        if (num==3)
        {
            is_ink=true;
            ink_count=block_count+10;
        }
    }
    int num=0;
    QString c;
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
        {
            c=op_msg[num];
            op_next_block[i][j]=c.toInt();
            num++;
        }
    for (int i=0;i<ROW;i++)
        for (int j=0;j<COL;j++)
        {
            c=op_msg[num];
            op_block[i][j]=c.toInt();
            num++;
        }

    emit print();
}
void Tetris::useItem(int item_pos)//使用第pos个道具
{
    if (item[item_pos]>0)//道具存在
    {
        int item_num=item[item_pos];
        QString msg="i"+QString::number(item_num);
        emit send(msg);
        emit useItemSuccess(item_num);
        item_count--;
        for (int i=item_pos;i<2;i++)
            item[i]=item[i+1];
        item[2]=0;
        emit print();
    }

}
void Tetris::moveLeft()
{
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
        {
            if (cur_block[i][j]>0 && cury+j==0)
                return;
            if (cur_block[i][j]>0 && stable_block[i+curx][j+cury-1]>0)
                return;
        }
    cury--;
    emit print();
}
void Tetris::moveRight()
{
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
        {
            if (cur_block[i][j]>0 && cury+j==COL-1)
                return;
            if (cur_block[i][j]>0 && stable_block[i+curx][j+cury+1]>0)
                return;
        }
    cury++;
    emit print();
}
void Tetris::rotate()
{
    int temp_block[4][4],tempx=curx,tempy=cury;
    copyBlock(temp_block,cur_block);
    rotateBlock(temp_block);
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
        {
            if (temp_block[i][j]>0 && j+tempy<0)
                tempy-=j+tempy;
            if (temp_block[i][j]>0 && j+tempy>COL-1)
                tempy-=j+tempy-COL+1;
        }

    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
        {
            if (temp_block[i][j]>0 && curx+i>ROW-1)
                return;
            if (temp_block[i][j]>0 && stable_block[i+tempx][j+tempy]>0)
                return;
        }
    cury=tempy;
    rotateBlock(cur_block);
    emit print();
}
void Tetris::moveDown()
{
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
        {
            if (cur_block[i][j]>0 && curx+i==ROW-1)
            {
                toStable();
                check();
                setBlock();
                return;
            }
            if (cur_block[i][j]>0 && stable_block[i+curx+1][j+cury]>0)
            {
                toStable();
                check();
                setBlock();
                return;
            }
        }
    curx++;
    emit print();
}
void Tetris::check()
{
    int count=0;
    for (int i=ROW-1;i>=cury;i--)
    {
        bool is_full=true;//检测
        for (int j=0;j<COL;j++)
            if (stable_block[i][j]==0)
                is_full=false;
        if (is_full)//消除
        {
            count++;
            for (int k=i;k>0;k--)
                for (int j=0;j<COL;j++)
                {
                    stable_block[k][j]=stable_block[k-1][j];
                }
            for (int j=0;j<COL;j++)
                stable_block[0][j]=0;
            i++;
        }
    }
    if (count>0)//获得道具
    {
        while (count--)
        {
            int randnum=qrand()%10+1;
            if (randnum==1 && item_count<3)
            {
                item[item_count]=1;
                item_count++;
            }
            else if (randnum<=3 && item_count<3)
            {
                item[item_count]=2;
                item_count++;
            }
            else if (randnum<=5 && item_count<3)
            {
                item[item_count]=3;
                item_count++;
            }
        }
    }
    emit print();
}
void Tetris::toStable()
{
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
        {
            if (cur_block[i][j]>0)
                stable_block[i+curx][j+cury]=cur_block[i][j];
        }
    emit print();
}
void Tetris::rotateBlock(int (*block)[4])
{
    int temp_block[4][4];
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            temp_block[3-j][i]=block[i][j];
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            block[i][j]=temp_block[i][j];
}
void Tetris::setBlock()
{
    if (game_over)
        return;
    copyBlock(cur_block,next_block);//预告方块变为当前方块
    createBlock(next_block);//产生预告方块
    curx=0;
    cury=4;//当前方块坐标设为中间
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
            if (cur_block[i][j]>0 && stable_block[curx+i][cury+j]>0)
            {
                toStable();
                sendMsg();
                end();
            }
    sendMsg();
    block_count++;
    if (block_count%10==0)
        emit changeTimer(block_count);
    if (block_count==ink_count)
    {
        is_ink=false;
        emit relieveItem(3);
    }
    if (block_count==reverse_count)
    {
        is_reverse=false;
        emit relieveItem(2);
    }
}
void Tetris::end()
{
    if (game_over)
        return;
    emit gameOver();
    game_over=true;
}
void Tetris::createBlock(int block[4][4])
{
    int num=qrand()%7+1;
    if (num==1)
        copyBlock(block,item1);
    if (num==2)
        copyBlock(block,item2);
    if (num==3)
        copyBlock(block,item3);
    if (num==4)
        copyBlock(block,item4);
    if (num==5)
        copyBlock(block,item5);
    if (num==6)
        copyBlock(block,item6);
    if (num==7)
        copyBlock(block,item7);
}

void Tetris::copyBlock(int (*sblock)[4], int (*tblock)[4])
{
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            sblock[i][j]=tblock[i][j];
}
