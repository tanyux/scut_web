# scut_web
华工校园网断网自动重连

根据电脑环境自行修改main.cpp中宏定义部分。
编译指令：g++ main.cpp -o scut_internet_minitor -lwsock32 -liphlpapi
管理员权限运行scut_internet_minitor.exe，挂后台即可。
