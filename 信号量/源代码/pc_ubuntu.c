#define   __LIBRARY__
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>

/*数字数目*/
#define MAX_NUM 500
/*消费者数目*/
#define PRONUM 10
/*缓冲区容量*/
#define BUFFERSIZE 10

/*生产函数*/
int produce(FILE* fp, int pos_to_put,int num){
    int wr_code;
    fseek( fp, pos_to_put*sizeof(int) , SEEK_SET );
    wr_code= (int) fwrite(&num, 1, sizeof(num), fp);
    fflush(fp);
    /*写入出错，就返回*/
    if(wr_code==-1) {
        printf("生产时写入文件出错");
        exit(-1);
    }
    return (pos_to_put+1)%BUFFERSIZE;
}

/*消费函数*/
int consume(FILE* fp, int pos_to_get,int* num){
    int read_code;
    fseek( fp, pos_to_get*sizeof(int) , SEEK_SET );
    read_code = (int) fread(num, sizeof(int), 1, fp);
    if(read_code==-1){
        printf("消费时读取文件出错");
        exit(-1);
    }
    return (pos_to_get+1)%BUFFERSIZE;
}
int main()
{
    int consume_num[PRONUM] = {100,100,100,200,100,100,50,50,100,100};
    int  i,j,k;
    /**/
    int  cost_num;

    /*消费者取数字的位置*/
    int  pos_to_get = 0;
    /*生产者存放数字的位置*/
    int  pos_to_put = 0;
    sem_t   *empty, *full, *mutex;
    FILE *fp = NULL;
    empty =(sem_t *)sem_open("empty",O_CREAT,0064,10);
    full  = (sem_t *)sem_open("full",O_CREAT,0064,0);
    mutex = (sem_t *)sem_open("mutex",O_CREAT,0064,1);
    fp=fopen("buffer.txt", "wb+");
    /*子进程，当生产者*/
    if(!fork())
    {
        for( i = 0 ; i < MAX_NUM; i++)
        {
            sem_wait(empty);
            sem_wait(mutex);
            /*生产数字*/
            pos_to_put = produce(fp,pos_to_put,i);
            sem_post(mutex);
            sem_post(full);
        }
        exit(0);
    }
    /*消费者*/
    for( k = 0; k < PRONUM ; k++ )
    {
        int pid = fork();
        if(!pid)
        {
            for( j = 0; j < consume_num[k]; j++ )
            {
                sem_wait(full);
                sem_wait(mutex);
                /*获得pos_to_get*/
                consume(fp,10,&pos_to_get);
                /*消费*/
                pos_to_get = consume(fp,pos_to_get,&cost_num);
                printf("%d:\t%d\n",getpid(),cost_num);
                fflush(stdout);
                /*把pos_to_get写到文件*/
                produce(fp,10,pos_to_get);
                sem_post(mutex);
                sem_post(empty);
            }
            exit(0);
        }
    }
    while(wait(NULL)!=-1);

    sem_unlink("empty");
    sem_unlink("full");
    sem_unlink("mutex");

    fclose(fp);
    return 0;
}