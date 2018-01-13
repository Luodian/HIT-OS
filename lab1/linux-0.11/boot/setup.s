INITSEG = 0x9000

entry _start
_start:
! 在显示字符串之前必须先获取当前光标的位置，这样就可以把字符串显示到当前光标处了
    mov    ah,#0x03
    xor    bh,bh
    int    0x10
    
! 利用10号中断的13号功能打印字符串"Now we are in SETUP."
    mov    cx,#26
    mov    bx,#0x000b
    mov    bp,#msg1
    mov     ax,cs
    mov    es,ax
    mov    ax,#0x1301
    int    0x10

! 下面开始读取一些硬件参数

    ! 读入光标位置信息，保存到0x90000处
    mov    ax,#INITSEG
    mov    ds,ax
    mov    ah,#0x03
    xor    bh,bh
    int    0x10
    mov    [0],ds

    ! 读入内存大小位置信息，保存到0x90002处
    mov    ah,#0x88
    int    0x15
    mov    [2],ax

    ! 从0x41处拷贝16个字节（磁盘参数表）到0x90004处
    mov    ax,#0x0000
    mov    ds,ax
    lds    si,[4*0x41]
    mov    ax,#INITSEG
    mov    es,ax
    mov    di,#0x0004
    mov    cx,#0x10
    rep       ! 重复16次
    movsb


! 先打印光标位置
    ! 打印字符串之前要先读取光标位置，将字符串打印到当前光标处
    mov    ah,#0x03
    xor    bh,bh
    int    0x10

    ! 打印字符串 "Cursor POS:"
    mov    cx,#11
    mov    bx,#0x000c
    mov    ax,cs
    mov    es,ax
    mov    bp,#msg2
    mov    ax,#0x1301
    int    0x10    
    
    ! 调用打印函数，打印光标位置
    mov    ax,#0x9000
    mov    ds,ax
    mov    dx,0x0
    call    print_hex
    call    print_nl

! 打印内存大小
    ! 打印字符串"Memory SIZE:"
    mov    ah,#0x03
    xor    bh,bh
    int    0x10    ! 读取光标位置

    mov    cx,#12
    mov    bx,#0x000c
    mov    ax,cs
    mov    es,ax
    mov    bp,#msg3
    mov    ax,#0x1301
    int    0x10    


    ! 调用打印函数，打印内存大小信息
    mov    ax,#0x9000    
    mov    ds,ax
    mov    dx,0x2
    call     print_hex


    ! 打印字符串"KB"
    mov    ah,#0x03
    xor    bh,bh
    int    0x10    ! 读取光标位置    
    
    mov    cx,#2
    mov    bx,#0x000c
    mov    ax,cs
    mov    es,ax
    mov    bp,#msg4
    mov    ax,#0x1301
    int    0x10    
    call    print_nl    

!打印柱面数
    ! 打印字符串"Cyls"
    mov    ah,#0x03
    xor    bh,bh
    int    0x10    ! 读取光标位置

    mov    cx,#5
    mov    bx,#0x000c
    mov    ax,cs
    mov    es,ax
    mov    bp,#msg5
    mov    ax,#0x1301
    int    0x10    
    

    ! 调用打印函数打印磁盘柱面数
    mov    ax,#0x9000    
    mov    ds,ax
    mov    dx,0x4
    call    print_hex
    call    print_nl

! 打印磁头数
    ! 打印字符串"Heads:"
    mov    ah,#0x03
    xor    bh,bh
    int    0x10    ! 读取光标位置

    mov    cx,#6
    mov    bx,#0x000c
    mov    ax,cs
    mov    es,ax
    mov    bp,#msg6
    mov    ax,#0x1301
    int    0x10    

    
    ! 调用打印函数打印磁盘磁头数
    mov    ax,#0x9000    
    mov    ds,ax
    mov    dx,0x6
    call    print_hex
    call    print_nl


! 打印每磁道扇区数
    ! 打印字符串"sectors"
    mov    ah,#0x03
    xor    bh,bh
    int    0x10    ! 读取光标位置

    mov    cx,#8
    mov    bx,#0x000c
    mov    ax,cs
    mov    es,ax
    mov    bp,#msg7
    mov    ax,#0x1301
    int    0x10    

    ! 调用打印函数打印扇区数
    mov    ax,#0x9000    
    mov    ds,ax
    mov    dx,0x12
    call    print_hex
    call    print_nl

Inf_loop:
    jmp Inf_loop    ! 无限循环

! print_hex函数：将一个数字转换为ascii码字符，并打印到屏幕上
! 参数值：dx
! 返回值：无
print_hex:
    mov    cx,#4            ! 要打印4个十六进制数字,故循环4次
print_digit:
    rol    dx,#4            ! 循环以使低4比特用上 !! 取dx的高4比特移到低4比特处
    mov    ax,#0xe0f        ! ah = 请求的功能值，al = 半字节(4个比特)掩码
    and    al,dl            ! 取dl的低4比特值
    add    al,#0x30        ! 给al数字加上十六进制0x30
    cmp    al,#0x3a    
    jl    outp            ! 如果是一个不大于十的数字
    add    al,#0x07    ! 如果是a～f，要多加7
outp:
    int    0x10
    loop    print_digit    ! 用loop重复4次
    ret

! 打印回车换行
print_nl:
    mov    ax,#0xe0d
    int    0x10
    mov    al,#0xa
    int    0x10
    ret

msg1:
    .byte 13,10
    .ascii "Now we are in SETUP."
    .byte 13,10,13,10

msg2:
    .ascii "Cursor POS:"

msg3:    
    .ascii "Memory SIZE:"

msg4:
    .ascii "KB"

msg5:    
    .ascii "Cyls:"

msg6:
    .ascii "Heads:"

msg7:
    .ascii "Sectors:"


.org 510
boot_flag:
    .word 0xAA55
