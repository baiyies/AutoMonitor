# AutoMonitor
[中文][url-doczh]

AutoMonitor is designed to monitor Windows screen, whenever detect user input, will screenshot automatically. 

# Feature
<img src="https://github.com/baiyies/asset/assets/87919016/e904ed6f-a876-4760-907d-59b6507d5683"/>

* supports both x86 and x64.
* the output Exe and shellcodes are pretty small.
* works fine when there are multiple screens.
* works fine when enable DPI Scaling.
* all the windows api calling is dynamic, so no import table.

# Usage

## 1.upload AutoMonitorServer
upload all the php files in AutoMonitorServer to your web server.

## 2.build AutoMonitorAgent

Install vs2019, open the AutoMonitorAgent project, change the server url in config.h, then build AutoMonitorAgent.

Release-Exe mode builds the Exe file, you can directly run it.

Release-Shellcode mode builds the Exe file that generates the shellcodes, just run it once, then you will get the shellcodes.

# TODO
* add AES encryption when post data to server.
* add server admin control panel.

# Disclaimer
This tool is only intended for legally authorized enterprise security activities. When using this tool for detection, you should ensure that such activities comply with local laws and regulations and that you have obtained sufficient authorization.

If you engage in any illegal activities while using this tool, you will be solely responsible for the consequences, and we will not assume any legal or joint liability.

Unless you have read, fully understood, and accepted all the terms of this agreement, please do not use this tool. Your use of this tool or any other express or implied acceptance of this agreement will be deemed as your acknowledgment and agreement to be bound by this agreement.

[url-doczh]: README_ZH.md