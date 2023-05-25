#include "mainwindow.h"

#include <QContextMenuEvent>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMenu>
#include <QMessageBox>
#include <QNetworkReply>
#include <QPainter>
#include <QPainter>
#include <QPen>
#include <QDir>


#include "weatherData.h"
#include "ui_mainwindow.h"
#include "weatherTool.h"

#define INCREMENT 2     //温度每升高或降低1度，y轴坐标的增量
#define POINT_RADIUS 3  //曲线秒点的大小
#define TEXT_OFFSET_X 9    //温度文本相对于x点的偏移
#define TEXT_OFFSET_Y 7    //温度文本相对于y点的偏移

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    //设置窗口属性
    setWindowFlag(Qt::FramelessWindowHint);  // 设置无边框
    setFixedSize(width(), height());         // 设置固定窗口大小

    //设置应用图标
    setWindowIcon(QIcon(":/res/WeaFor.png"));

    //构建右键菜单
    mExitMenu = new QMenu(this);
    mExitAct = new QAction();
    mExitAct->setText(tr("退出"));
    mExitAct->setIcon(QIcon(":/res/close.ico"));
    mExitMenu->addAction(mExitAct);
    //用lamba函数连接退出按钮，触发就退出该应用
    connect(mExitAct, &QAction::triggered, this, [=]() { qApp->exit(0); });

    //天气图标录入
    weaType();

    //网络请求
    mNetAccessManager = new QNetworkAccessManager(this);
    connect(mNetAccessManager,&QNetworkAccessManager::finished,this,&MainWindow::onReplied);

    //直接在构造中请求天气数据
    //目前默认为深圳，可更改
    getWeatherInfo("深圳");

    //给标签添加事件过滤器
    ui->lblHighCurve->installEventFilter(this);
    ui->lblLowCurve->installEventFilter(this);

    //选择大洲窗口
    connect(ui->Jump,&QPushButton::clicked,this,[=](){
        List->setGeometry(this->geometry());
        List->show();
        this->hide();   //隐藏当前窗口
    });

    //链接到全球天气窗口
    connect(ui->World,&QPushButton::clicked,this,[=](){
        World->setGeometry(this->geometry());
        World->show();
        this->hide();
    });

    //监听返回
    connect(List,&WorldListWea::BackButton,this,[=]{
        this->setGeometry(List->geometry());
        this->show();
        List->hide();
    });

    connect(World,&AsiaWea::MainBtn,this,[=](){
        this->setGeometry(World->geometry());
        this->show();
        World->hide();
    });

    //连接传递城市参数信号和槽
    connect(List,SIGNAL(setData(QString)),this,SLOT(receiveData(QString)));
}

MainWindow::~MainWindow() { delete ui; }

//重写父类虚函数
//父类中的默认实现是忽略右键菜单时间，重写后就可以
void MainWindow::contextMenuEvent(QContextMenuEvent* event) {
    //弹出右键菜单
    mExitMenu->exec(QCursor::pos());
    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mOffset = event->globalPos() - this->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}

void MainWindow::getWeatherInfo(QString cityName)
{
    QString cityCode = WeatherTool::getCityCode(cityName);
    if(cityCode.isEmpty()){
        QMessageBox::warning(this,"提示","请检查城市名是否输错，该页面只支持国内。",QMessageBox::Ok);
        return;
    }
    QUrl url("http://t.weather.itboy.net/api/weather/city/" + cityCode);
    mNetAccessManager->get(QNetworkRequest(url));
}

void MainWindow::parseJson(QByteArray &byteArray)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray,&err);  // 检测json格式
    if(err.error != QJsonParseError::NoError){    // Json格式错误
        return;
    }

    QJsonObject rootObj = doc.object();
//    qDebug() << rootObj.value("message").toString();

    //解析日期和城市
    mToday.date = rootObj.value("date").toString();
    mToday.city = rootObj.value("cityInfo").toObject().value("city").toString();
    int index = mToday.city.indexOf("市");
    QString result = mToday.city.left(index); // 取出 "市" 前面的子串
    mToday.city = result;

    //解析昨天
    QJsonObject objData = rootObj.value("data").toObject();

    QJsonObject objYesterday = objData.value("yesterday").toObject();
    mDay[0].week = objYesterday.value("week").toString();
    mDay[0].date = objYesterday.value("ymd").toString();

    mDay[0].type = objYesterday.value("type").toString();

    QString s;
    s = objYesterday.value("high").toString().split(" ").at(1);
    s = s.left(s.length() - 1);
    mDay[0].high = s.toInt();

    s = objYesterday.value("low").toString().split(" ").at(1);
    s = s.left(s.length() - 1);
    mDay[0].low = s.toInt();

    //风向风力
    mDay[0].fx = objYesterday.value("fx").toString();
    mDay[0].fl = objYesterday.value("fl").toString();
    //空气质量指数
    mDay[0].aqi = objYesterday.value("aqi").toInt();

    //解析预报中的5天数据
    QJsonArray forecatArr = objData.value("forecast").toArray();
    for(int i = 0;i < 5;i++){
        QJsonObject objForecast = forecatArr[i].toObject();
        mDay[i + 1].week = objForecast.value("week").toString();
        mDay[i + 1].date = objForecast.value("ymd").toString();
        //天气类型
        mDay[i + 1].type = objForecast.value("type").toString();

        QString s;
        s = objForecast.value("high").toString().split(" ").at(1);
        s = s.left(s.length() - 1);
        mDay[i + 1].high = s.toInt();

        s = objForecast.value("low").toString().split(" ").at(1);
        s = s.left(s.length() - 1);
        mDay[i + 1].low = s.toInt();

        //风向风力
        mDay[i + 1].fx = objForecast.value("fx").toString();
        mDay[i + 1].fl = objForecast.value("fl").toString();
        //空气质量指数
        mDay[i + 1].aqi = objForecast.value("aqi").toInt();
    }

    //解析今天的数据
    mToday.ganmao = objData.value("ganmao").toString();
    mToday.wendu = objData.value("wendu").toString().toInt();
    mToday.shidu = objData.value("shidu").toString();
    mToday.pm25 = objData.value("pm25").toInt();
    mToday.quality = objData.value("quality").toString();
    //forecast 中的第一个数组元素，即今天的数据
    mToday.type = mDay[1].type;

    mToday.fx = mDay[1].fx;
    mToday.fl = mDay[1].fl;

    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;
    //更新UI
    updateUI();

    //绘制最高最低温度曲线
    ui->lblHighCurve->update();
    ui->lblLowCurve->update();
}

void MainWindow::updateUI()
{
    //更新日期和城市
    ui->lblDate->setText(QDateTime::fromString(mToday.date,"yyyyMMdd").toString("yyyy/MM/dd")
                         + " " + mDay[1].week);
    ui->lblCity->setText(mToday.city);

    //更新今天
    ui->lblTypeIcon->setPixmap(mTypeMap[mToday.type]);
    ui->lblTemp->setText(QString::number(mToday.wendu) + "°");
    ui->lblType->setText(mToday.type);
    ui->lblLowHigh->setText(QString::number(mToday.low) + "~"
                            + QString::number(mToday.high) + "°C");

    ui->lblGanMao->setText("感冒指数：" + mToday.ganmao);
    ui->lblWindFx->setText(mToday.fx);
    ui->lblWindFl->setText(mToday.fl);

    ui->lblPM25->setText(QString::number(mToday.pm25));

    ui->lblShiDu->setText(mToday.shidu);
    ui->lblQuality->setText(mToday.quality);

    //更新六天的数据
    for(int i = 0;i < 6;i++){
        //更新日期和时间
        mWeekList[i]->setText("星期" + mDay[i].week.right(1));
        ui->lblWeek0->setText("昨天");
        ui->lblWeek1->setText("今天");
        ui->lblWeek2->setText("明天");
        QStringList ymdList = mDay[i].date.split("-");
        mDateList[i]->setText(ymdList[1] + "/" + ymdList[2]);

        //更新天气类型
        mTypeList[i]->setText(mDay[i].type);
        mTypeIconList[i]->setPixmap(mTypeMap[mDay[i].type]);

        //更新空气质量
        if(mDay[i].aqi >0 && mDay[i].aqi <= 50){
            mAqiList[i]->setText("优");
            mAqiList[i]->setStyleSheet("background-color: rgb(139,195,74);");
        }else if(mDay[i].aqi > 50 && mDay[i].aqi <= 100){
            mAqiList[i]->setText("良");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,170,0);");
        }else if(mDay[i].aqi > 100 && mDay[i].aqi <= 150){
            mAqiList[i]->setText("轻度");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,87,97);");
        }else if(mDay[i].aqi > 150 && mDay[i].aqi <= 200){
            mAqiList[i]->setText("中度");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,17,27);");
        }else if(mDay[i].aqi > 150 && mDay[i].aqi <= 200){
            mAqiList[i]->setText("重度");
            mAqiList[i]->setStyleSheet("background-color: rgb(170,0,0);");
        }else{
            mAqiList[i]->setText("严重");
            mAqiList[i]->setStyleSheet("background-color: rgb(110,0,0);");
        }

        //修正了100判断成10的情况
        int sdIndex = mToday.shidu.indexOf("%");
        QString shiduRes = mToday.shidu.left(sdIndex); // 取出 "市" 前面的子串
//        qDebug() << shiduRes;

        if(shiduRes.toInt() > 70){
            QPixmap high("://res/humidity.png");
            ui->lblShiDuIcon->setPixmap(high);
        }
        else {
            QPixmap low("://res/lowHumidty.png");
            ui->lblShiDuIcon->setPixmap(low);
        }

        //更新风力、风向
        mFxList[i]->setText(mDay[i].fx);
        mFlList[i]->setText(mDay[i].fl);
    }
    QString city = ui->lblCity->text();
    QString temp = ui->lblLowHigh->text();
    QString type = mTypeMap[mDay[1].type];
    emit sendTemp(city,temp,type);
}

//消息过滤，主要用于重绘子控件，过滤Paint事件
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    //绘制最高温度曲线
    if(watched == ui->lblHighCurve && event->type() == QEvent::Paint){
        paintHighCurve();
    }
    //绘制最低温度曲线
    if(watched == ui->lblLowCurve && event->type() == QEvent::Paint){
        paintLowCurve();
    }
    return QWidget::eventFilter(watched,event);
}

//绘制温度曲线函数
void MainWindow::paintHighCurve()
{
    QPainter painter(ui->lblHighCurve);

    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);

    //获取x坐标
    int pointX[6] = {0};
    for(int i = 0;i < 6;i++){
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width() / 2;
    }

    int tmpSum = 0;
    int tmpAvg = 0;
    for(int i = 0;i < 6;i++){
        tmpSum += mDay[i].high;
    }
    tmpAvg = tmpSum / 6;

    //获取y坐标
    int pointY[6] = {0};
    int yCenter = ui->lblHighCurve->height() / 2;
    for(int i = 0;i < 6;i++){
        pointY[i] = yCenter - ((mDay[i].high - tmpAvg) * INCREMENT);
    }

    //绘制
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(255,170,0));
    painter.setPen(pen);
    painter.setBrush(QColor(255,170,0));    //设置画刷内部填充的颜色

    //画点、写文本
    for(int i = 0;i < 6;i++){
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),POINT_RADIUS,POINT_RADIUS);
        //显示温度文本
        painter.drawText(pointX[i] - TEXT_OFFSET_X,pointY[i] - TEXT_OFFSET_Y,
                         QString::number(mDay[i].high) + "°");
    }
    for(int i = 0;i < 5;i++){
        if(i == 0){
            pen.setStyle(Qt::DashLine);
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i],pointY[i],pointX[i + 1],pointY[i + 1]);
    }
}

void MainWindow::paintLowCurve()
{
    QPainter painter(ui->lblLowCurve);

    // 抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);

    //获取x坐标
    int pointX[6] = {0};
    for(int i = 0;i < 6;i++){
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width() / 2;
    }

    //获取y坐标
    int pointY[6] = {0};
    int yCenter = ui->lblLowCurve->height() / 2;

    //求平均气温
    int tmpSum = 0;
    int tmpAvg = 0;
    for(int i = 0;i < 6;i++){
        tmpSum += mDay[i].low;
    }
    tmpAvg = tmpSum / 6;

    for(int i = 0;i < 6;i++){
        pointY[i] = yCenter - ((mDay[i].low - tmpAvg) * INCREMENT);
    }

    //绘制
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(0,255,255));

    painter.setPen(pen);
    painter.setBrush(QColor(0,255,255));    //设置画刷内部填充的颜色

    //画点、写文本
    for(int i = 0;i < 6;i++){
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),POINT_RADIUS,POINT_RADIUS);
        //显示温度文本
        painter.drawText(pointX[i] - TEXT_OFFSET_X,pointY[i] - TEXT_OFFSET_Y,QString::number(mDay[i].low) + "°");
    }
    for(int i = 0;i < 5;i++){
        if(i == 0){
            pen.setStyle(Qt::DashLine);
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i],pointY[i],pointX[i + 1],pointY[i + 1]);
    }
}

void MainWindow::weaType()
{
    //给搜索绑定回车键
    ui->btnSearch->setShortcut(Qt::Key_Enter);
    ui->btnSearch->setShortcut(Qt::Key_Return);
    //将控件添加到控件数组
    mWeekList << ui->lblWeek0 << ui->lblWeek1 << ui->lblWeek2 << ui->lblWeek3 << ui->lblWeek4 << ui->lblWeek5;
    mDateList << ui->lblDate0 << ui->lblDate1 << ui->lblDate2 << ui->lblDate3 << ui->lblDate4 << ui->lblDate5;

    mTypeList << ui->lblType0 << ui->lblType1 << ui->lblType2 << ui->lblType3 << ui->lblType4 << ui->lblType5;
    mTypeIconList << ui->lblTypeIcon0 << ui->lblTypeIcon1 << ui->lblTypeIcon2 << ui->lblTypeIcon3 << ui->lblTypeIcon4 << ui->lblTypeIcon5;

    mAqiList << ui->lblQuality0 << ui->lblQuality1 << ui->lblQuality2 << ui->lblQuality3 << ui->lblQuality4 << ui->lblQuality5;

    mFxList << ui->lblFx0 << ui->lblFx1 << ui->lblFx2 << ui->lblFx3 << ui->lblFx4 << ui->lblFx5;
    mFlList << ui->lblFl0 << ui->lblFl1 << ui->lblFl2 << ui->lblFl3 << ui->lblFl4 << ui->lblFl5;
    //天气对应图标
    mTypeMap.insert("暴雪",":/res/type/BaoXue.png");
    mTypeMap.insert("暴雨",":/res/type/BaoYu.png");
    mTypeMap.insert("暴雨到暴雪",":/res/type/BaoYuDaoDaBaoYu.png");
    mTypeMap.insert("大暴雨",":/res/type/DaBaoYu.png");
    mTypeMap.insert("大暴雨到大暴雪",":/res/type/DaBaoYuDaoTeDaBaoYu.png");
    mTypeMap.insert("大到暴雪",":/res/type/DaDaoBaoXue.png");
    mTypeMap.insert("大到暴雨",":/res/type/DaDaoBaoYu.png");
    mTypeMap.insert("大雪",":/res/type/DaXue.png");
    mTypeMap.insert("大雨",":/res/type/DaYu.png");
    mTypeMap.insert("冻雨",":/res/type/DongYu.png");
    mTypeMap.insert("多云",":/res/type/DuoYun.png");
    mTypeMap.insert("浮尘",":/res/type/FuChen.png");
    mTypeMap.insert("雷阵雨",":/res/type/LeiZhenYu.png");
    mTypeMap.insert("雷阵雨伴有冰雹",":/res/type/LeiZhenYuBanYouBingBao.png");
    mTypeMap.insert("霾",":/res/type/Mai.png");
    mTypeMap.insert("强沙尘暴",":/res/type/QiangShaChenBao.png");
    mTypeMap.insert("晴",":/res/type/Qing.png");
    mTypeMap.insert("沙尘暴",":/res/type/ShaChenBao.png");
    mTypeMap.insert("特大暴雨",":/res/type/TeDaBaoYu.png");
    mTypeMap.insert("雾",":/res/type/Wu.png");
    mTypeMap.insert("小到中雨",":/res/type/XiaoDaoZhongYu.png");
    mTypeMap.insert("小到中雪",":/res/type/XiaoDaoZhongXue.png");
    mTypeMap.insert("小雪",":/res/type/XiaoXue.png");
    mTypeMap.insert("小雨",":/res/type/XiaoYu.png");
    mTypeMap.insert("雪",":/res/type/Xue.png");
    mTypeMap.insert("扬沙",":/res/type/YangSha.png");
    mTypeMap.insert("阴",":/res/type/Yin.png");
    mTypeMap.insert("雨",":/res/type/Yu.png");
    mTypeMap.insert("雨夹雪",":/res/type/YuJiaXue.png");
    mTypeMap.insert("阵雨",":/res/type/ZhenYu.png");
    mTypeMap.insert("阵雪",":/res/type/ZhenXue.png");
    mTypeMap.insert("中雨",":/res/type/ZhongYu.png");
    mTypeMap.insert("中雪",":/res/type/ZhongXue.png");
}

void MainWindow::onReplied(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(reply->error() != QNetworkReply::NoError || statusCode != 200){
        qDebug() << reply->errorString().toLatin1().data();
        QMessageBox::warning(this,"天气","请求数据失败",QMessageBox::Ok);
    }else{
        QByteArray  byteArray = reply->readAll();
//        qDebug() << "读所有：" << byteArray.data();
        parseJson(byteArray);
    }
    reply->deleteLater();
}

void MainWindow::on_btnSearch_clicked()
{
    QString cityName = ui->leCity->text();
    if(cityName == ""){
        return;
    }
    getWeatherInfo(cityName);
}

//获取数据并且判断是否在国内
void MainWindow::receiveData(QString cityName)
{
    QString cityCode = WeatherTool::getCityCode(cityName);
    if(cityCode.isEmpty()){
        QMessageBox::information(List,"提示","请检查城市名是否输错，该页面只支持国内。",QMessageBox::Ok);
    }
    else{
        QUrl url("http://t.weather.itboy.net/api/weather/city/" + cityCode);
        mNetAccessManager->get(QNetworkRequest(url));
        connect(this,SIGNAL(sendTemp(QString,QString,QString)),List,SLOT(getTemp(QString,QString,QString)));
        List->hide();
        this->setGeometry(List->geometry());       //跳转定位问题
        this->show();
    }
}
