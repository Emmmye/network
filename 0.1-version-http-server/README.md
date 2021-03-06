# 期考试卷查询网
对之前的代码进行了模块化细分,并引入了epoll水平触发的IO多路复用技术

## 项目简介
本项目为C语言编写的Web服务器,解析了GET和POST请求,可处理静态资源和动态资源,GET请求为用户返回首页,POST请求为用户提供登录,检索试卷以及上传试卷功能

## 使用技术
* 使用Epoll水平触发的IO多路复用技术,使用Reactor模式
* 使用多线程充分利用多核CPU，并使用半同步/半反应堆线程池模型避免线程频繁创建销毁的开销
* 访问量过多时,线程池能够进行动态扩容
* 主线程只负责accept请求，事件就绪后将其插入到就绪队列中,等待在就绪队列上的某一工作线程被唤醒
* 使用CGI机制处理用户提交数据
* 使用C语言连接数据库实现用户信息以及用户上传文件的存储

## 项目测试
* 用户的登陆功能,文件上传和试卷检索功能都得以正常运行
![测试](https://github.com/Emmmye/network/blob/master/0.1-version-http-server/user_upload_picture/test.png)
* 使用webbench工具对服务器基础功能进行压力测试  

## 项目改进
* 引入缓冲队列,在访问量过多时,减轻服务器压力
* 引入定时器处理超时请求
* 使用半同步/半异步进程池模型处理用户提交数据,避免频繁创建子进程的开销
