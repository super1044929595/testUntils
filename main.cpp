
//
//  main.cpp
//  nodetree
//
//  Created by 华锦文 on 2022/8/28.
//



#include <iostream>
#include <deque>
#include <string>
#include <vector>

//-既然标题是c++，那么就写成满满的c++风格吧
using Color = bool;//-颜色因为只有红或者黑，选择bool类型
using KEY_TYPE = int;//-为了更好理解红黑树，就不写成模板类了，所以首选万年int（笑~）
using VALUE_TYPE = int;//-同理

//-全局静态红黑变量
static const Color red = false;
static const Color black  = true;

//-红黑树的节点特点，有color，有parent

class RBtree_node{
public:
    Color color;
    RBtree_node * parent;
    RBtree_node * left;
    RBtree_node * right;

    KEY_TYPE key;//-后期如果想解耦合，可以将key和value抽离出去
    VALUE_TYPE value;
    RBtree_node(Color color_):color(color_),parent(nullptr),left(nullptr),right(nullptr),key(-99999){}
    RBtree_node(Color color_, KEY_TYPE key_,RBtree_node * nil):
    color(color_),parent(nil),left(nil),right(nil),key(key_){}
};


class RBtree{
private:
    //-红黑树数据成员：其中nil的意义在于，因为红黑树的所有叶子节点都是黑色的，所以可以将所有临近末尾的节点，
    //-都连接到这一个叶子结点nil上，同理，root的parent也可以连接到nil上，形成一个dummy空节点
    RBtree_node * root;
    RBtree_node * nil;
public :
    //-以下实现了红黑树常用接口：
    //-构造函数
    RBtree(){
    nil = new RBtree_node(black);//-为所有叶子节点nil初始化，颜色为黑色
    root = nil;//-红黑树为空的时候，让nil作为root
}
//-左旋
void leftRotate(RBtree_node *left_node);

//- 右旋
void rightRotate(RBtree_node * right_node);

//-插入key
void insertNode(KEY_TYPE key);

//-修复插入
void fixInsert(RBtree_node * node);

//-查找某个key的节点
RBtree_node* searchNode(KEY_TYPE key);

//-查找某个节点的中序后继
RBtree_node* successor(RBtree_node * node);

//-删除key
void deleteNode(KEY_TYPE key);

//-修复删除
void fixDelete(RBtree_node * node);

//-层序遍历打印红黑树
void print();

//-打印中序遍历
void printMiddle(RBtree_node * node);

};

//-左旋
void RBtree::leftRotate(RBtree_node *left_node){
    RBtree_node * right_node = left_node->right;
    //-右节点的左枝条接在左节点的右枝条上
    left_node->right = right_node->left;
    if(right_node->left!=nil){
        left_node->right->parent = left_node;
    }
    //-右节点接在左节点的父亲上
    right_node->parent = left_node->parent;
    if(left_node == root){
        //-nil不会指向任何节点，但是root->parent是nil
        root = right_node;
    }
    else if(left_node == left_node->parent->left){
        left_node->parent->left = right_node;
    }else{
        left_node->parent->right = right_node;
    }
    //-左节点接在右节点的左枝上
    left_node->parent = right_node;
    right_node->left = left_node;
}

//- 右旋：写完左旋后，把所有left和right对调即可
void RBtree::rightRotate(RBtree_node * right_node){
    RBtree_node * left_node = right_node->left;
    right_node->left = left_node->right;
    if(left_node->right!=nil){
        right_node->left->parent = right_node;
    }
    left_node->parent = right_node->parent;
    if(right_node == root){
        root = left_node;
    }
    else if(right_node == right_node->parent->right){
        right_node->parent->right = left_node;
    }else{
        right_node->parent->left = left_node;
    }
    right_node->parent = left_node;
    left_node->right = right_node;
}

//-插入key
void RBtree::insertNode(KEY_TYPE key){
    RBtree_node * prev = nil;
    RBtree_node * cur = root;
    while(cur!=nil){
        prev = cur;
        if(key>cur->key){
            cur = cur->right;
        }else if(key<cur->key){
            cur = cur->left;
        }else{//-该key已经存在
            return;
        }
    }
    //-创建新节点
    RBtree_node * new_node = new RBtree_node(red,key,nil);
    //-如果节点没有元素
    new_node->parent = prev;
    if(prev == nil){
        root = new_node;
    }
    else if(key<prev -> key){
        prev ->left = new_node;
    }else{
        prev ->right = new_node;
    }
    fixInsert(new_node);
    print();
    printf("new node %d",new_node);
}

//-修复插入
void RBtree::fixInsert(RBtree_node * new_node){
    while(new_node -> parent->color == red){//-终止条件要注意
        //-如果父亲是左枝
        if(new_node->parent == new_node -> parent->parent->left){
            //-获得其伯父节点
            RBtree_node * uncle = new_node->parent->parent->right;
            if(uncle->color == red){//-如果伯父是红色，那么将父亲和伯父同时变黑，不会破坏左右平衡
                uncle->color = black;
                new_node->parent->color = black;
                new_node->parent ->parent->color = red;//-将祖父变红，才能实现下一轮回溯修复
                new_node = new_node->parenjdjht->parent;
            }else{//-如果伯父是黑色
                //-判断new_node是不是右孩子，如果是右孩子转换成左孩子
                if(new_node == new_node -> parent->right){
                    new_node = new_node->parent;
                    leftRotate(new_node);
                }
                //-此时红色节点是左孩子
                //-如果结构本是平衡状态，右边本该比左边多一个黑，但是我们将父亲（左)变黑会破坏平衡，
                //-所以需要右旋祖父，把父亲上浮，相当于在左枝多一个黑的时候给右枝也多了黑，这样左右就能平衡
                new_node->parent->color = black;
                new_node->parent ->parent->color = red;
                rightRotate(new_node->parent->parent);
            }
        }
        //-如果父亲是右枝（将上边代码的left和right全部对调即可，不用记规则）
        else {
            RBtree_node * uncle = new_node->parent->parent->left;
            if(uncle->color == red){//-如果伯父是红色
                uncle->color = black;
                new_node->parent->color = black;
                new_node->parent ->parent->color = red;
                new_node = new_node->parent->parent;
            }else{//-如果伯父是黑色
                if(new_node == new_node -> parent->left){
                    new_node = new_node->parent;
                    rightRotate(new_node);
                }
                new_node->parent->color = black;
                new_node->parent ->parent->color = red;
                leftRotate(new_node->parent->parent);
            }
        }
    }
    //-如果new_node回溯到root，此时root->parent==nil（black）打破了循环，而此时root被改变成了黑色，违反了规则1，
    //-所以最后需要强行把root fix成黑色
    root->color = black;
}

//-查找某个key的节点
RBtree_node* RBtree::searchNode(KEY_TYPE key){
    RBtree_node * cur = root;
    while(cur!=nil){
        if(key>cur -> key){
            cur = cur->right;
        }else if(key < cur -> key){
            cur = cur->left;
        }else{
            return cur;
        }
    }
    return cur;
}

//-查找某个节点的中序后继
RBtree_node* RBtree::successor(RBtree_node * node){
    //-如果节点有右孩子
    if(node->right!=nil){
        RBtree_node * res = node -> right;
        while(res->left!=nil){
            res = res->left;
        }
        return res;
    }else{
        while(node!=root&&node!=node->parent->left){
            node = node->parent;
        }
        return node->parent;
    }
}

//-删除key
void RBtree::deleteNode(KEY_TYPE key){
    //-查找key所在节点
    RBtree_node * key_node = searchNode(key);
    //-实际删除的节点
    RBtree_node* delete_node;
    //-delete_node的孩子
    RBtree_node* delete_son;
    //-如果同时有左枝或者右枝条
    if(key_node->left != nil&&key_node->right != nil){
        delete_node = successor(key_node);
        delete_son = delete_node->right;
    }//-如果仅有左枝或者右枝条或者左右都没有
    else{
        delete_node = key_node;
        if(key_node->left != nil){
            delete_son = key_node->left;
        }else{
            delete_son = key_node->right;
        }
    }

    //-删除deletenode
    delete_son->parent = delete_node->parent;
    //-先判断deletenode是不是根节点
    if(delete_node == root){
        root = delete_son;
    }
    else if(delete_node == delete_node->parent->left){
        delete_node->parent->left = delete_son;
    }else{
        delete_node -> parent -> right = delete_son;
    }
    //-覆盖key_node原有数据
    key_node->key = delete_node -> key;
    key_node ->value = delete_node -> value;

    //-如果删除节点是黑色的，需要修复delete_son,注意是孩子
    if(delete_node->color == black){
        fixDelete(delete_son);
    }
    //-释放空间
    delete delete_node;
    //-打印
    print();
}

//-修复删除
void RBtree::fixDelete(RBtree_node * delete_son){
    //-修复的原因是因为delete_son所在的枝条的黑节点比另一个枝条少一个，所以不平衡，所以需要填上左边缺失的黑，或者减掉右边多余的黑
    //-当delete_son是黑色的一直循环
    while(delete_son!=root&&delete_son->color == black){
        //-判断delete_son所在枝条,如果是左枝
        if(delete_son == delete_son->parent->left){
            //-如果兄弟是红色的
            RBtree_node * bro = delete_son->parent->right;
            if(bro->color == red){
                bro->color = black;//-兄弟变黑
                delete_son->parent->color = red;//-父亲变红
                leftRotate(delete_son->parent);//-左旋父亲，兄弟上浮，相当于左右都加了一个黑，不改变平衡状态
                bro = delete_son->parent->right;//-新的bro是原来bro的左枝，因为原bro是红的，其左右枝都是黑色的，这样保证新的兄弟是黑色的
            }
            //-此时兄弟是黑色的,判断兄弟的孩子
            //-左黑右黑（兄弟的孩子平衡了）
            if(bro->left->color == black&&bro->right -> color == black){
                bro->color = red;//-相当于右边减去多的一个黑，达到平衡
                delete_son = delete_son->parent;
            }else{
                //-如果是左红右黑,变成左黑右红
                if (bro->right->color == black){
                    bro -> color = red;
                    bro->left->color = black;
                    rightRotate(bro);//-左节点上浮，相当于左右都加了一个黑，不改变平衡
                }
                bro->color = bro->parent -> color;
                bro->parent -> color = black;
                bro->right->color = black;//-给右边加了一个黑
                leftRotate(delete_son->parent);//-父亲下沉，兄弟上浮，左边加一个黑，右边减一个黑，总体上左边填上了缺少的黑也达到了平衡
                delete_son = root;
            }
        }
        //-如果是右枝（不用记规则，把上面的代码left和right对调即可）
        else {
            RBtree_node * bro = delete_son->parent->left;
            if(bro->color == red){
                bro->color = black;
                delete_son->parent->color = red;
                rightRotate(delete_son->parent);
                bro = delete_son->parent->left;
            }
            if(bro->right->color == black&&bro->left -> color == black){
                bro->color = red;
                delete_son = delete_son->parent;
            }else{
                if (bro->left->color == black){
                    bro -> color = red;
                    bro->right->color = black;
                    leftRotate(bro);
                }
                bro->color = bro->parent -> color;
                bro->parent -> color = black;
                bro->left->color = black;
                rightRotate(delete_son->parent);
                delete_son = root;
            }
        }
    }
    delete_son->color = black;
}

//-层序遍历打印红黑树
void RBtree::print(){
    std::deque<RBtree_node*> dqueue;//-使用deque实现队列
    dqueue.push_back(root);
    while(!dqueue.empty()){
        int size = (int)dqueue.size();
        for (int i = 0; i < size; ++i) {
            RBtree_node* temp = dqueue.front();
            dqueue.pop_front();
            if(temp->left!=nullptr){
                dqueue.push_back(temp -> left);
            }
            if(temp -> right != nullptr){
                dqueue.push_back(temp -> right);
            }
            std::string color = temp->color?"b: ":"r: ";
            std::string keystr = temp==nil?"nil":std::to_string(temp->key);
            std::cout<<color<<keystr<<"  ";
        }
        std::cout<<std::endl;
    }
}

//-打印中序遍历
void RBtree::printMiddle(RBtree_node * node){
    if(node == nil){
        return;
    }
    printMiddle(node->left);
    std::string color = node->color?"b:":"r:";
    std::cout<<color<<std::to_string(node->key)<<"  ";
    printMiddle(node->right);
}

int main(){

    std::vector<int> i={1,4,66,6,2,323,3};
    for(int v:i){
        std::cout<<v<<' ';
    }
#if 0
    RBtree rb;
    std::string select;
    KEY_TYPE key;
    RBtree_node *serch_node=NULL;
    while(true){
        std::cout<<"\r\n-----------------------------------------------------"<<std::endl;
        std::cout<<"\n \r 输入操作：i:插入key  ,d:删除key ,c:查找key ,q:退出\r\n"<<std::endl;
        std::cin>>select;
        if(select == "i"){
            std::cout<<"输入key"<<std::endl;
            std::cin>>key;
            rb.insertNode(key);
        }else if(select == "d"){
            std::cout<<"输入key"<<std::endl;
            std::cin>>key;
            rb.deleteNode(key);
        }else if(select == "q"){
            break;
        }else if(select =="c"){
            std::cout<<"查找key"<<std::endl;
            std::cin>>key;
            serch_node=rb.searchNode(key);
            printf("\r\nserch_node %d",serch_node);
            
        }
        else{
            std::cout<<"输入不合法，重新输入"<<std::endl;
        }
        printf("\r\nprintf 红黑树");
        rb.print();
        //std::cout<<"\r\n-----------------------------------------------------"<<std::endl;
        printf("\033[1;30;40m这是一行黑色测试字体\033[0m");
     
        printf("\033[1;32;40m这是一行绿色测试字体\033[0m");
    }
#endif
    return 0;
}


//
//  main.c
//  tetct
//
//  Created by 华锦文 on 2022/8/7.
//
#if 0
#include <stdio.h>
#include <stdlib.h>
#include "sys/rbtree.h"
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <unistd.h>

typedef unsigned int uint32_t;

typedef struct{
    uint32_t *m_file;
    uint32_t m_mutex;

}jw_Socket_s;



//----------------------------------------------------------

#define XFIND_SETUP_EVNET_NUM   8UL
uint32_t pos_lossevnt[ XFIND_SETUP_EVNET_NUM ];
uint32_t xRecord_Event_Process=0;
uint32_t xRecord_Event_Warning=0;

void xFind_LosstepParse(uint32_t pos)
{
    uint32_t i=0;
    uint32_t pos_temp=pos;

    for(pos_temp=pos;pos_temp!=0;pos_temp=pos_temp>>1){
        if(pos_temp&0x01){
            pos_lossevnt[i]     &=~0x01;
        }else{
            pos_lossevnt[i]     |=0x01;
            //printf("\r\n find the loss step :------->[%d]",i);
            xRecord_Event_Warning|=pos_lossevnt[i];
        }
        //update
        i++;
    }
    
    for(int x=0;x<XFIND_SETUP_EVNET_NUM;x++){
        if(pos_lossevnt[x]!=0){
            xRecord_Event_Warning |=0x01<<x;
        }
    }
    printf("\r\n \r\n");

}


void set_eventProcess( uint32_t event )
{
    uint32_t i=0,temp=0;
    xRecord_Event_Process=event ;
    xFind_LosstepParse(xRecord_Event_Process);
    
    
    if(xRecord_Event_Process)
    {
       for(temp=xRecord_Event_Process;temp!=0;temp=temp>>1){
       
           if(temp&0x01){

          switch(i){

                /*--------------------------------------------------------------------------
                AW_PT_NOISE_ACKENBALE_MASK       0x01
                AW_PT_RAW0_ACKENBALE_MASK         0x02
                AW_PT_RAW1_ACKENBALE_MASK            0x04
                AW_PT_VERIFY0_ACKENBALE_MASK      0x08
                AW_PT_VERIFY1_ACKENBALE_MASK      0x10
                AW_PT_PEEK_NOISE_ACKENBALE_MASK      0x20
                AW_PT_MASS_SIGNE_ACKENBALE_MASK      0x40
                AW_PT_OFFERSET_ACKENBALE_MASK        0x80
                ---------------------------------------------------------------------------*/

                case 0:   printf(0,"jw---OK event noise ");     break;

                case 1:   printf(0,"jw---OK event raw0 ");      break;

                case 2:   printf(0,"jw---OK event raw1 ");      break;

                case 3:   printf(0,"jw---OK event verify0 ");   break;

                case 4:   printf(0,"jw---OK event verify1 ");   break;

                default : break;

         }
          
        }else{

              switch(i){

                /*--------------------------------------------------------------------------
                 AW_PT_NOISE_ACKENBALE_MASK       0x01
                 AW_PT_RAW0_ACKENBALE_MASK         0x02
                 AW_PT_RAW1_ACKENBALE_MASK            0x04
                 AW_PT_VERIFY0_ACKENBALE_MASK      0x08
                 AW_PT_VERIFY1_ACKENBALE_MASK      0x10
                 AW_PT_PEEK_NOISE_ACKENBALE_MASK      0x20
                 AW_PT_MASS_SIGNE_ACKENBALE_MASK      0x40
                 AW_PT_OFFERSET_ACKENBALE_MASK        0x80
              ---------------------------------------------------------------------------*/
              
              case 0:   printf(0,"jw---loss event noise ");     break;

              case 1:   printf(0,"jw---loss event raw0 ");      break;

              case 2:   printf(0,"jw---loss event raw1 ");      break;
              
              case 3:   printf(0,"jw---loss event verify0 ");   break;

              case 4:   printf(0,"jw---loss event verify1 ");   break;
              
              default : break;
              
              }
          
        }
        i++;
       }
    }
    
}

void clear_eventprocess(void)
{
    xRecord_Event_Process=0;
    xRecord_Event_Warning=0;
}

void printfnum(int num)
{
    for(int i=0;i<num;i++){
        printf("-");
    }
}

void showDice(int* dice,size_t num) {
    printf("\t-----------\n");
    printf("       /          /|\n");
    printf("      /    %d     / |\n",dice[0]);
    printf("     /          /  |\n");
    printf("     -----------   |\n");
    printf("     |         | %d |\n",7-dice[2]);
    printf("  %d  |    %d    |   /\n", dice[2],dice[1]);
    printf("     |         |  /\n");
    printf("     |         | /\n");
    printf("     -----------/\n");
    printf("      %d\n", 7-dice[0]);
    printf("\n");
    return;
}





#define pi                3.14159265358979323846
#define c                3
#define screen_width    50
#define screen_height    25
float cube[6][4][3] = {
    {
        {-0.5, -0.5,  0.5}, { 0.5, -0.5,  0.5}, {-0.5,  0.5,  0.5}, { 0.0,  0.0,  1.0}
     },
    {
        {-0.5, -0.5,  0.5}, {-0.5, -0.5, -0.5}, {-0.5,  0.5,  0.5}, {-1.0,  0.0,  0.0}
    },
    {
        {-0.5, -0.5,  0.5}, {-0.5, -0.5, -0.5}, { 0.5, -0.5,  0.5}, { 0.0, -1.0,  0.0}
    },
    {
        {-0.5,  0.5,  0.5}, { 0.5,  0.5,  0.5}, {-0.5,  0.5, -0.5}, { 0.0,  1.0,  0.0}
    },

    {
        { 0.5, -0.5,  0.5}, { 0.5, -0.5, -0.5}, { 0.5,  0.5,  0.5}, { 1.0,  0.0,  0.0}
    },
    {
        {-0.5, -0.5, -0.5}, { 0.5, -0.5, -0.5}, {-0.5,  0.5, -0.5}, { 0.0,  0.0, -1.0}
     }
};

int face[6][3][3] = {
    {
        {0, 0, 0}, {0, 1, 0}, {0, 0, 0}
    },
    {
        {0, 0, 1}, {0, 0, 0}, {1, 0, 0}
    },
    {
        {0, 0, 1}, {0, 1, 0}, {1, 0, 0}
    },
    {
        {1, 0, 1}, {0, 0, 0}, {1, 0, 1}
    },
    {
        {1, 0, 1}, {0, 1, 0}, {1, 0, 1}
    },
    {
        {1, 0, 1}, {1, 0, 1}, {1, 0, 1}
    }

};
int judgeFace(int ID, float x, float y) {
    return face[ID][(int)(3.f * y)][(int)(3.f * x)];
}
void ini()
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float x = cube[i][j][0];
            float y = cube[i][j][1];
            float z = cube[i][j][2];

            cube[i][j][0] = (sqrt(3) / 6.f + 0.5) * x - sqrt(3) / 3.f * y + (-0.5 + sqrt(3) / 6.f) * z;
            cube[i][j][1] = (sqrt(3) / 3.f) * x + (sqrt(3) / 3.f) * y + (sqrt(3) / 3.f) * z;
            cube[i][j][2] = (-0.5 + sqrt(3) / 6.f) * x + (-sqrt(3) / 3.f) * y + (sqrt(3) / 6.f + 0.5) * z;
        }
    }
}
void renderFrame()
{
    ini();
    double time = 0;
    while (1) {
        time = time + 0.01;
        float z_buffer[screen_height + 1][screen_width + 1];
        for (int i = 0; i <= screen_height; i++)
            for (int j = 0; j <= screen_width; j++)
                z_buffer[i][j] = -100;
        char output[screen_height + 1][screen_width + 1];
        memset(output, ' ', sizeof(output));

        for (int i = 0; i < 6; i++)
        {
            for (float u = 0.f; u < 1.f; u = u + 0.01)
                for (float v = 0.f; v < 1.f; v = v + 0.01)
                {
                    float m_x = (cube[i][1][0] - cube[i][0][0]);
                    float m_y = (cube[i][1][1] - cube[i][0][1]);
                    float m_z = (cube[i][1][2] - cube[i][0][2]);

                    float n_x = (cube[i][2][0] - cube[i][0][0]);
                    float n_y = (cube[i][2][1] - cube[i][0][1]);
                    float n_z = (cube[i][2][2] - cube[i][0][2]);


                    float x = m_x * u + n_x * v + cube[i][0][0];
                    float y = m_y * u + n_y * v + cube[i][0][1];
                    float z = m_z * u + n_z * v + cube[i][0][2];

                    float rotation_x = cos(time) * x - sin(time) * z;
                    float rotation_y = y;
                    float rotation_z = sin(time) * x + cos(time) * z;

                    float normal_x = (cube[i][3][0]) * cos(time) - sin(time) * (cube[i][3][2]);
                    float normal_y = cube[i][3][1];
                    float normal_z = (cube[i][3][0]) * sin(time) + cos(time) * (cube[i][3][2]);

                    int screen_x = (rotation_x / (1 - rotation_z / c) + 1.f) / 2 * screen_width;
                    int screen_y = (rotation_y / (1 - rotation_z / c) + 1.f) / 2 * screen_height;
                    float screen_z = rotation_z / (1 - rotation_z / c);

                    float L = normal_z;
                    if (L > 0) {
                        if (z_buffer[screen_y][screen_x] < screen_z)
                        {
                            z_buffer[screen_y][screen_x] = screen_z;
                            if (judgeFace(i, u, v) == 1)
                            {
                                float tempu = u - (float)((int)(u * 3.f)) * 1.f / 3.f;
                                float tempv = v - (float)((int)(v * 3.f)) * 1.f / 3.f;
                                if ((tempu - 1.f / 6.f) * (tempu - 1.f / 6.f) + (tempv - 1.f / 6.f) * (tempv - 1.f / 6.f) <= 1.f / 36.f)
                                {
                                    L = 0;
                                }
                                else
                                    L = (L + 0.1) * sqrt(2);
                            }
                            else
                                L = (L + 0.1) * sqrt(2);
                            int luminance_index = L * 8;
                            if (luminance_index > 11)
                                luminance_index = 11;
                            output[screen_y][screen_x] = ".,-~:;=!*#-@"[luminance_index];
                        }
                    }
                    else
                        if (z_buffer[screen_y][screen_x] < screen_z)
                            z_buffer[screen_y][screen_x] = screen_z;
                }
        }
        for (int j = screen_height; j >= 0; j--) {
            for (int i = 0; i <= screen_width; i++) {
                putchar(output[j][i]);
            }
            putchar('\n');
        }
        usleep(5000);
        printf("\x1b[26A");
    }
}

//add by hjw

typedef struct{
    uint32_t data_mutex;
    uint32_t read_queuemutex;
    
    struct Epoll_Epitem *repollthree;
    
    //struct epoll_file fds[1024];
}Epoll_ManageInfo;

typedef struct list_head
{
    
    
}list_head;

typedef struct  _Epoll_Epitem{
//挂在红黑树节点
    struct rb_node rbn;
 //挂载就绪列 4
    struct list_head rdllink;
    
    struct _Epoll_Epitem *next;
    
    uint32_t keyalue;
    
}Epoll_Epitem;


Epoll_Epitem item[10];
int key=0;
int Epoll_Item_add(Epoll_Epitem *epitem)
{
    Epoll_Epitem *root=(Epoll_Epitem*)item;
#if 1
    for(root=root;root->next!=NULL;root=(Epoll_Epitem*)root->next){
       
    }
#endif
    root->next=epitem;
    epitem->next=NULL;
    epitem->keyalue=key++;
    printf("\r\n epitem %d \r\n ",root->keyalue);
    return 0;
    
}

//红黑树宏定义

void serverrun(void)
{
    int connect_num;
    for(;;){
        //int connect_num=epoll();
        if(connect_num <0 ){
            printf("\r\n jw---> epoll error !");
        }else if( connect_num ==0 ){
            continue;
        }
        
        if( 1 ){
            //accept();
        }else{
            //recv();
        }
    }
}



int main(int argc, const char * argv[]) {
    // insert code here...
    
    
  /*
    int data[6][4] = {
            {2, 3, 5, 4},
            {6, 3, 1, 4},
            {1, 2, 6, 5},
            {1, 5, 6, 2},
            {1, 3, 6, 4},
            {2, 4, 5, 3} };
        int dice[3];
        while (1)
        {
            printf("\n\n==================================\n");
            unsigned int value = 0;
          
            value = 4;

            printf("掷到的点数为:%d\n", value + 1);            //因为取值范围是0~5，所以需要加一个一
            dice[0] = value + 1;                            //top side

            unsigned int value_2 = 0;
            value_2 = 3;                            //找环的起点，取值范围是0~3，四个数中随机取一个

            dice[1] = data[value][value_2];                    //front side
            dice[2] = data[value][(value_2 + 1) ];        //left side
            showDice(dice, 3);
            printf("再扔一次？\n");
            getchar();
        }
    */
    //renderFrame();

    std::vector<int> i={1,4,66,6,2,323,3};
    fo r(int v:i){
        std::cout<<v<<'';
    }
    return 0;
}
#endif
