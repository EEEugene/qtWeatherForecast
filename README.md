# qtWeatherForecast
## 个人qt项目，天气预报

## 目标：
制作一个方便快捷可用的天气预报，市面上目前的天气预报都太过臃肿，自己制作一个自己查看，更加灵活小巧可用。

# 技术点：
该项目从国家气象局提供的 API  接口中获取 JSON  数据，使用 QT  提供的类来解析 JSON  数据并显示在项目上；发送 HTTP  请求，获得天气数据；自行绘制温度曲线和查看当前天气，所有天气图表都有显示；可以搜索中国所有城市天气预报；查看感冒指数、空气质量、未来6天空气、风力风向和最高最低温度；无边框窗口；重写了鼠标移动事件，窗口可跟随鼠标移动；右键菜单退出功能；实现选择城市列表功能；全球天气API窗口；选择城市后同步更新列表天气；编写自定义QListWidget列表，可以显示天气温度城市；传递参数到具体天气界面中；

# Notice：
如果需要使用的话，需要自己修改waetherTool.h中的第20行位置filePath，方可对城市搜索进行正常使用；

AsiaWea中的api也需要进行修改，这边是接入全球天气的api接口，需要在https://tianqiapi.com/user/register 这里进行注册，然后填写id和秘钥即可使用

230502更新：目前已支持全球主要城市的天气，还有一个列表可以进行选择，使用了QListWideget进行操作，然后可以进行五大洲的切换，具体再双击进行查看详细天气

目前已在 Github  开源

***可直接下载Setup.exe安装使用***

**界面展示效果:**

<img src="https://sszblog.oss-cn-shenzhen.aliyuncs.com/img/weather.png">

<img src="https://sszblog.oss-cn-shenzhen.aliyuncs.com/img/微信截图_20230603180123.png">

<img src="https://sszblog.oss-cn-shenzhen.aliyuncs.com/img/WorldWeaDetail.png">

# 总结：
该天气预报总体制作点不难，难点在于从接口中获取数据，然后得到JSON数据并切割出自己想要的数据并显现在界面上，这是一个小难点。总体来说制作成功，并且已经打包发布。
