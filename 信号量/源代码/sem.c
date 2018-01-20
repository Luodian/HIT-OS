#include <linux/sem.h>
#include <linux/sched.h>
#include <unistd.h>
#include <asm/segment.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <linux/fdreg.h>
#include <asm/system.h>
#include <asm/io.h>
#include <string.h>

sem_t semtable[SEMTABLE_LEN];
int cnt = 0;

/*创建或打开一个进程，name是进程的逻辑地址 （信号量的名字），内核中使用get_fs_byte()函数，可以获得用户空间中该地址的一个字节*/
sem_t *sys_sem_open(const char *name, unsigned int value) {
	char sem_name[100];
    int i = 0;
    int name_cnt = 0;
/*    获取一个字节，'\0'表示已经到达了末尾，记录信号量名字的长度 */
    while (get_fs_byte(name + name_cnt) != '\0')
	    ++name_cnt;
/*名字太长，创建不了 */
    if (name_cnt > SEM_NAME_LEN)
        return NULL;
/*    把信号量名字存储进sem_name */
    for (i = 0; i < name_cnt; ++i)
        sem_name[i] = get_fs_byte(name + i);
/*    如果已经建立了同名信号量，就使用它 */
    for (i = 0; i < cnt; ++i)
        if (!strcmp(sem_name, semtable[i].name))
            return (sem_t*)(&semtable[i]);
/*    无同名信号量，就建立，并加入信号量数组，数组长度增1 */
	for (i = 0; i < strlen(sem_name); ++i)
        semtable[cnt].name[i] = sem_name[i];
	semtable[cnt].value = value;
	return (sem_t*)(&semtable[cnt ++]);
}

/*P操作 */
int sys_sem_wait(sem_t *sem) {
/*    关中断 */
    cli();
/*    插入队列 */
    while (sem->value <= 0)
        sleep_on(&(sem->queue));
    sem->value--;
/*    开中断 */
    sti();
    return 0;
}

/* V操作 */
int sys_sem_post(sem_t *sem) {
    cli();
    sem->value++;
    if ((sem->value) <= 1)
        wake_up(&(sem->queue));
    sti();
    return 0;
}

/* 删除信号量 */
int sys_sem_unlink(const char *name) {
    char sem_name[100];
    int i, name_cnt = 0, tmp;
    while (get_fs_byte(name+name_cnt) != '\0')
        ++ name_cnt;
    if (name_cnt > SEM_NAME_LEN)
        return -1;
    for (i = 0; i < name_cnt; ++i)
        sem_name[i] = get_fs_byte(name+i);
	for (i = 0; i < cnt; ++i)
        if (!strcmp(sem_name, semtable[i].name)) {
		    for (tmp = i; tmp < cnt-1; ++tmp)
	                semtable[tmp] = semtable[tmp + 1];
		cnt --;
            return 0;
	}
    return -1;
}
