#include<graphics.h>
#include<cstdio>
#include<iostream>
using namespace std;
int xcord=10,barwidth=110,barheight=40,gap=30,extra=25;
int ycord[50];
int WINDOW=1200;
int main()
{
    char name[50];    cout<<"Enter file name : ";    cin>>name;
    int x=xcord,y=10,choice=0;
    long long int screen_Width=GetSystemMetrics(SM_CXSCREEN), screen_Height=GetSystemMetrics(SM_CYSCREEN);
    initwindow(screen_Width,screen_Height,"CODE-IMAGE",0,0);

    for(int i=0;i<2;i++)
    {
        setbkcolor(WHITE);setcolor(WHITE);setfillstyle(1,WHITE);    setcolor(RED);
        bar(x, y,x + barwidth, y + barheight);    settextstyle(4,HORIZ_DIR,1);  //panel
        switch(i){
            case 0: outtextxy(15 ,y+5 , "Refresh");  break;
            case 1: outtextxy(25 ,y+5 , "Clear"); break;
        }
        ycord[i]=y;
        y += barheight + gap;
    }
    char file[]="image/";
    char file_name[50];
    strcat(file,name);
    strcat(file,".bmp");
    cout<<file<<"   "<<file_name<<" "<<name<<endl;
    readimagefile(file,230,10,screen_Width-50,screen_Height-50);

    POINT cpos;
    while(1)
    {
        choice=0;
        if(GetAsyncKeyState(VK_LBUTTON))
        {
            GetCursorPos(&cpos);
            for(int i=0;i<2;i++)
            {
                if(cpos.x>=10 && cpos.x <= 10+barwidth && cpos.y>=ycord[i]+extra && cpos.y<= ycord[i]+extra+ barheight)
                {                        choice=i+1;               break;       }
            }

            switch(choice)
            {
                case 1:setcolor(BLACK);setfillstyle(1,BLACK);  rectangle(230,0,screen_Width,screen_Height);floodfill(232,55,BLACK);
                    readimagefile(file,230,10,screen_Width-50,screen_Height-50);
                    break;
                case 2:  return 0;
            }delay(200);
        }

    }
    char ch;
    scanf("%c",&ch);
    return 0;
}
