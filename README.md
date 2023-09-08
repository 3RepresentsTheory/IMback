# IMback

这是[FluentChat](https://github.com/flwfdd/FluentChat)
的后端项目IMback，目前是一个 sqlite + websocket + qt的简易后端服务器，绝大部分的网络组件均采用Qt，非常的“小学期”。

由于最后时间不够所以只开发了sqlite版本，最终以`sqlite_ver`分支为主，如果有部署或者运行的问题麻烦提个issue!

## 部署方法
克隆仓库并切换分支:
```shell
git clone https://github.com/3RepresentsTheory/IMback.git
cd IMback
git checkout sqlite_ver
```
采用`Qt6.5.2` 的`MingW`，使用`QtCreator`打开项目中的`CMakeLists.txt`即可，启动后会自动在当前项目根目录建立后端数据库文件
`IMBACK.db`。对于上传的文件会存放到`uploads`文件夹中

由伍卓珩与秦崇开发🎈
