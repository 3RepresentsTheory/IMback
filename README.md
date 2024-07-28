# IMback

这是[FluentChat](https://github.com/flwfdd/FluentChat)
的后端项目IMback，目前是一个 sqlite + websocket + qt的简易后端服务器，绝大部分的网络组件均采用Qt，非常的“小学期”。

由于最后时间不够所以只开发了sqlite版本，如果有部署或者运行的问题麻烦提个issue!

## 部署

```shell
git clone https://github.com/3RepresentsTheory/IMback.git
cd IMback
```
采用`Qt6.5.2` 的`MingW`，使用`QtCreator`打开项目中的`CMakeLists.txt`即可，启动后会自动在当前项目根目录建立后端数据库文件
`IMBACK.db`。对于上传的文件会存放到`uploads`文件夹中

## 配置
硬编码一般事务端口```TXPORT```为```1235```，广播端口```BCPORT```为```1234```，可以在```dispatcher.cpp```中
修改

## 项目基本结构
事务与消息推送分离。主线程中处理一般事务，如加入群组，注册登录等；

消息推送机制事项如下，首先每个客户端登录后必须监听指定广播端口，并在限定时间内向广播端口发送身份验证信息。
广播线程根据验证信息，建立用户身份与```websocket```连接的关联。这样则可以实现在线用户的查询，与消息的分发推送。

事务服务器线程与广播线程之间采用信号进行通讯。

## etc
直接使用```HTTP```访问事务服务器根页```/```可以得到一个简单的```html```前端页面

利用这个页面可以对服务器行为进行测试/调试。

需要注意的是这里的前端页面中没有对用户密码进行```md5 hash```

由伍卓珩与秦崇开发🎈
