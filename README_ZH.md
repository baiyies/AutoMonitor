# AutoMonitor
[中文][url-doczh]

AutoMonitor是一个用来监控windows主机的工具，当检测到用户在操作电脑时会自动发送截屏到服务器。 

# 特性
<img src="https://github.com/baiyies/asset/assets/87919016/e904ed6f-a876-4760-907d-59b6507d5683"/>

* 支持x86和x64。
* 生成的shellcode和exe非常小。
* 支持多屏幕。
* 支持DPI缩放启用时完整获取截屏。
* 动态调用windows api, 无导入表。

# 使用方法

## 1.上传 AutoMonitorServer
上传所有AutoMonitorServer目录下的php文件到服务器.

## 2.编译 AutoMonitorAgent

安装 vs2019, 再打开AutoMonitorAgent项目，修改config.h中的url，再选中Release-Exe或Release-Shellcode进行编译。

Release-Exe 用来生成exe格式, 可直接运行。

Release-Shellcode 用来生成exe, 运行后会提取出shellcode, 再运行shellcode即可。

# TODO
* 在传输到server时增加AES加密.
* 添加后台管理功能.

# 免责声明
本项目只限用于教育和研究目的，不得用于商业或任何违法目的。

在使用本工具进行检测时，您应确保该行为符合当地的法律法规，并且已经取得了足够的授权。

如您在使用本工具的过程中存在任何非法行为，您需自行承担相应后果，我们将不承担任何法律及连带责任。

除非您已充分阅读、完全理解并接受本协议所有条款，否则，请您不要使用本工具。您的使用行为或者您以其他任何明示或者默示方式表示接受本协议的，即视为您已阅读并同意本协议的约束。

[url-doczh]: README_ZH.md