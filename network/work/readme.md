# 操作方法
主要实现一个简单了客户端之间聊天功能
## 编译
```bash
gcc epoll_loop.c server.c wrap.c -o server
gcc client.c wrap.c -o client
```
## 运行

因为是本地运行，因此ip都默认绑定了127.0.0.1，端口为可选参数，默认8090

```bash
./server <port>
./client <port>
```

客户端运行后，需要输入登录的用户名

客户端发送给另一个客户端格式为“1 目的客户端名称 消息”， 例如 “1 tian hello\n"表示发给tian一个hello的信息。

客户端输入2则表示退出
