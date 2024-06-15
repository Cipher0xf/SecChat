### 功能需求
聊天开始时双向认证  
聊天内容混合加密  
公钥证书由证书机构颁发（CA公钥是已知随机数）  
公钥加密RSA：密钥64bit、明文/密文64bit（AES密钥）  
对称加密AES：密钥64bit（公私钥）、明文/密文分组128bit  
数字签名DSA：密钥64bit（公私钥）、消息（id+RSA公钥）、签名  
哈希函数MD5：消息、摘要128bit  

### 项目结构
SecChat/  
--server/ 服务器云端文件    
----server.cpp 服务端  
----server_config.text [DSA公私钥]  
----database/  
------user_info.csv [序号,用户id,用户名,口令哈希值]  
------sign_cert.csv  [序号,签名时间,用户id,RSA公钥,随机数,RSA公钥证书]  
------chat_record/ 用户会话记录  
--------A_B.csv  [序号,发送时间,发送方id/名称,接收方id/名称,消息密文]  
--------A_C.csv  
--------...  
--client/ 用户本地文件  
----Alice/  
------client.cpp 客户端  
------client_config.txt [登录状态,用户id,用户名,RSA公私钥,RSA公钥证书]  
------chat.cpp 会话窗口  
------session_key.csv [序号,会话建立时间,会话地址,用户id,用户名,AES密钥]  
----Bob/   
----Catherine/  
----...  

### 运行流程
进程p0 服务器  
--线程t1 和A通信  
--线程t2 和B通信  
--...  
进程p1 用户A（输入）  
进程p2 用户B（输入）  
...  
进程p3 聊天窗口（输出）  
  
(1)登录请求 [login]  
用户A/B输入[id+口令]，客户端A/B生成[RSA公私钥]，发送[id+口令MD5哈希值+RSA公钥]  
服务端根据数据库验证，成功用[DSA私钥]则对[id+RSA公钥+随机数]签名，返回登录许可[True]和[DSA证书]，否则报错[False]  

(2)对话请求(A-->B) [contact]  
A主动开启对话，发送[src_id(A)+dst_id(B)+RSA公钥(A)+DSA证书(A)]，服务端线程t1存入缓冲区  
B查询对话请求，服务端线程t2查找缓冲区，发送[src_id(A)+RSA公钥(A)+随机数(A)+DSA证书(A)+DSA公钥]给B验签  
B对A进行身份验证，成功则返回对话许可[True]和[src_id(B)+RSA公钥(B)+DSA证书(B)]，否则报错[False]  
服务端收到B对话许可，发送[src_id(B)+RSA公钥(B)+随机数(B)+DSA证书(B)+DSA公钥]给A验签  
A对B进行身份验证，成功则用[RSA公钥(B)]加密[AES密钥]，返回对话许可[True]和[RSA密文(AES密钥)]，否则报错[False]  
服务端收到A对话许可，说明A和B已完成双向身份认证，发送[RSA密文]给B，同时创建会话记录[A_B.csv]，将文件地址发送给双方  
B用[RSA私钥(B)]解密[RSA密文]，得到[AES密钥]，双方成功获取临时会话密钥  

(3)对话 [chat]  
A/B运行chat.cpp，输入[A_B.csv]地址打开会话窗口  
A/B可同时在client.cpp的通信窗口输入消息，用[AES密钥]加密后发给服务端  
服务端将[序号,发送时间,发送方id/名称,接收方id/名称,消息密文]写入文件[A_B.csv]  
会话窗口会读取A/B本地的[AES密钥]，实时刷新显示明文  

### 编译  
g++ server.cpp -lwsock32 -lstdc++ -o server  
g++ client.cpp -lwsock32 -lstdc++ -o client  






