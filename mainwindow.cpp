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
#include <QTimer>
#include <QPainter>
#include <QPen>
#include "weatherData.h"
#include "ui_mainwindow.h"
#include "weatherTool.h"

#define INCREMENT 3     //温度每升高或降低1度，y轴坐标的增量
#define POINT_RADIUS 3  //曲线秒点的大小
#define TEXT_OFFSET_X 12    //温度文本相对于点的偏移
#define TEXT_OFFSET_Y 12

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    //设置窗口属性
    setWindowFlag(Qt::FramelessWindowHint);  // 无边框
    setFixedSize(width(), height());         // 固定窗口大小

    // 右键菜单：退出程序
    mExitMenu = new QMenu(this);
    mExitAct = new QAction();
    mExitAct->setText(tr("退出"));
    mExitAct->setIcon(QIcon(":/res/close.ico"));
    mExitMenu->addAction(mExitAct);

    connect(mExitAct, &QAction::triggered, this, [=]() { qApp->exit(0); });

    //将控件添加到控件数组
    mWeekList << ui->lblWeek0 << ui->lblWeek1 << ui->lblWeek2 << ui->lblWeek3 << ui->lblWeek4 << ui->lblWeek5;
    mDateList << ui->lblDate0 << ui->lblDate1 << ui->lblDate2 << ui->lblDate3 << ui->lblDate4 << ui->lblDate5;

    mTypeList << ui->lblType0 << ui->lblType1 << ui->lblType2 << ui->lblType3 << ui->lblType4 << ui->lblType5;
    mTypeIconList << ui->lblTypeIcon0 << ui->lblTypeIcon1 << ui->lblTypeIcon2 << ui->lblTypeIcon3 << ui->lblTypeIcon4 << ui->lblTypeIcon5;

    mAqiList << ui->lblQuality0 << ui->lblQuality1 << ui->lblQuality2 << ui->lblQuality3 << ui->lblQuality4 << ui->lblQuality5;

    mFxList << ui->lblFx0 << ui->lblFx1 << ui->lblFx2 << ui->lblFx3 << ui->lblFx4 << ui->lblFx5;
    mFlList << ui->lblFl0 << ui->lblFl1 << ui->lblFl2 << ui->lblFl3 << ui->lblFl4 << ui->lblFl5;

    mTypeMap.insert("暴雪",":/res/type/BaoXue.png");
    mTypeMap.insert("小雪",":/res/type/XiaoXue.png");
    mTypeMap.insert("霾",":/res/type/Mai.png");
    mTypeMap.insert("暴雨",":/res/type/BaoYu.png");
    mTypeMap.insert("多雨",":/res/type/DuoYu.png");
    mTypeMap.insert("多云",":/res/type/DuoYun.png");
    mTypeMap.insert("晴",":/res/type/Qing.png");
    mTypeMap.insert("阴",":/res/type/Yin.png");
    mTypeMap.insert("雨",":/res/type/Yu.png");
    mTypeMap.insert("阵雨",":/res/type/ZhenYu.png");
    mTypeMap.insert("中雨",":/res/type/ZhongYu.png");
    mTypeMap.insert("小雨",":/res/type/XiaoYu.png");


    //网络请求
    mNetAccessManager = new QNetworkAccessManager(this);
    connect(mNetAccessManager,&QNetworkAccessManager::finished,this,&MainWindow::onReplied);

    //直接在构造中请求天气数据
    //深圳的城市编码
    getWeatherInfo("杭州");

    //给标签添加事件过滤器
    ui->lblHighCurve->installEventFilter(this);
    ui->lblLowCurve->installEventFilter(this);

    //给搜索绑定回车键（注意大小都需要绑定） --comment by ssz 230308
    ui->btnSearch->setShortcut(Qt::Key_Enter);
    ui->btnSearch->setShortcut(Qt::Key_Return);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::contextMenuEvent(QContextMenuEvent* event) {
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
        QMessageBox::warning(this,"提示","请检查城市名是否输错",QMessageBox::Ok);
        return;
    }
    QUrl url("http://t.weather.itboy.net/api/weather/city/" + cityCode);
    mNetAccessManager->get(QNetworkRequest(url));
}

void MainWindow::parseJson(QByteArray &byteArray)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray,&err);
    if(err.error != QJsonParseError::NoError){
        return;
    }
    QJsonObject rootObj = doc.object();
    qDebug() << rootObj.value("message").toString();

    //解析日期和城市
    mToday.date = rootObj.value("date").toString();
    mToday.city = rootObj.value("cityInfo").toObject().value("city").toString();

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
    mDay[0].aqi = objYesterday.value("aqi").toDouble();

    //解析预报中的5天数据
    QJsonArray forecatArr =  objData.value("forecast").toArray();
    for(int i = 0;i < 5;++i){
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
        mDay[i + 1].aqi = objForecast.value("aqi").toDouble();

    }

    //解析今天的数据
    mToday.ganmao = objData.value("ganmao").toString();

    mToday.wendu = objData.value("wendu").toString().toInt();
    mToday.shidu = objData.value("shidu").toString();
    mToday.pm25 = objData.value("pm25").toDouble();
    mToday.quality = objData.value("quality").toString();
    //forecast 中的第一个数组元素，即今天的数据
    mToday.type = mDay[1].type;

    mToday.fx = mDay[1].fx;
    mToday.fl = mDay[1].fl;

    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;

    //更新UI
    updateUI();

    //绘制温度曲线
    ui->lblHighCurve->update();
    ui->lblLowCurve->update();
}

void MainWindow::updateUI()
{
    //更新日期和城市
    ui->lblDate->setText(QDateTime::fromString(mToday.date,"yyyyMMdd").toString("yyyy/MM/dd") + " " + mDay[1].week);
    ui->lblCity->setText(mToday.city);

    //更新今天
    ui->lblTypeIcon->setPixmap(mTypeMap[mToday.type]);
    ui->lblTemp->setText(QString::number(mToday.wendu) +"°");
    //ui->lblTemp->setText(mToday.type);
    ui->lblLowHigh->setText(QString::number(mToday.low) + "~" + QString::number(mToday.high) + "°C");

    ui->lblGanMao->setText("感冒指数：" + mToday.ganmao);
    ui->lblWindFx->setText(mToday.fx);
    ui->lblWindFl->setText(mToday.fl);

    ui->lblPM25->setText(QString::number(mToday.pm25));

    ui->lblShiDu->setText(mToday.shidu);
    ui->lblQuality->setText(mToday.quality);

    //更新六天的数据
    for(int i = 0;i < 6;++i){
        //更新日期和时间
        mWeekList[i]->setText("周" + mDay[i].week.right(1));
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
            mAqiList[i]->setStyleSheet("background-color: rgb(121,184,0);");
        }else if(mDay[i].aqi > 50 && mDay[i].aqi <= 100){
            mAqiList[i]->setText("良");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,187,23);");
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

        //更新风力、风向
        mFxList[i]->setText(mDay[i].fx);
        mFlList[i]->setText(mDay[i].fl);

    }



}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lblHighCurve && event->type() == QEvent::Paint){
        paintHighCurve();
    }
    if(watched == ui->lblLowCurve && event->type() == QEvent::Paint){
        paintLowCurve();
    }
    return QWidget::eventFilter(watched,event);
}

void MainWindow::paintHighCurve()
{
    QPainter painter(ui->lblHighCurve);

    // 抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);

    //获取x坐标
    int pointX[6] = {0};
    for(int i = 0;i < 6;++i){
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width() / 2;
    }
    //获取y坐标
    int tmpSum = 0;
    int tmpAvg = 0;
    for(int i = 0;i < 6;++i){
        tmpSum += mDay[i].high;
    }
    tmpAvg = tmpSum / 6;

    int pointY[6] = {0};
    int yCenter = ui->lblHighCurve->height() / 2;
    for(int i = 0;i < 6;++i){
        pointY[i] = yCenter - ((mDay[i].high - tmpAvg) * INCREMENT);
    }
    //绘制
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(255,170,0));

    painter.setPen(pen);
    painter.setBrush(QColor(255,170,0));    //设置画刷内部填充的颜色
    //画点、写文本
    for(int i = 0;i < 6;++i){
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),POINT_RADIUS,POINT_RADIUS);

        //显示温度文本
        painter.drawText(pointX[i] - TEXT_OFFSET_X,pointY[i] - TEXT_OFFSET_Y,QString::number(mDay[i].high) + "°");
    }
    for(int i = 0;i < 5;++i){
        if(i == 0){
            pen.setStyle(Qt::DashLine);
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i],pointY[i],pointX[i + 1],pointY[i + 1]);
    }
    painter.restore();
}

void MainWindow::paintLowCurve()
{
    QPainter painter(ui->lblLowCurve);

    // 抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);

    //获取x坐标
    int pointX[6] = {0};
    for(int i = 0;i < 6;++i){
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width() / 2;
    }
    //获取y坐标
    int tmpSum = 0;
    int tmpAvg = 0;
    for(int i = 0;i < 6;++i){
        tmpSum += mDay[i].low;
    }
    tmpAvg = tmpSum / 6;

    int pointY[6] = {0};
    int yCenter = ui->lblLowCurve->height() / 2;
    for(int i = 0;i < 6;++i){
        pointY[i] = yCenter - ((mDay[i].low - tmpAvg) * INCREMENT);
    }
    //绘制
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(0,255,255));

    painter.setPen(pen);
    painter.setBrush(QColor(0,255,255));    //设置画刷内部填充的颜色
    //画点、写文本
    for(int i = 0;i < 6;++i){
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),POINT_RADIUS,POINT_RADIUS);

        //显示温度文本
        painter.drawText(pointX[i] - TEXT_OFFSET_X,pointY[i] - TEXT_OFFSET_Y,QString::number(mDay[i].low) + "°");
    }
    for(int i = 0;i < 5;++i){
        if(i == 0){
            pen.setStyle(Qt::DashLine);
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i],pointY[i],pointX[i + 1],pointY[i + 1]);
    }

    painter.restore();
}

void MainWindow::callKeyBoard()
{
//     KeyBoard *keyBoard = new KeyBoard(0, ui->cityLineEdit);
//     keyBoard->show();
}

void MainWindow::onReplied(QNetworkReply *reply)
{
    qDebug() << "请求成功";

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

//    qDebug() << "方法:" << reply->operation();
//    qDebug() << "code:" << statusCode;
//    qDebug() << "url:" << reply->url();
//    qDebug() << "请求头:" << reply->rawHeaderList();

    if(reply->error() != QNetworkReply::NoError || statusCode != 200){
        qDebug() << reply->errorString().toLatin1().data();
        QMessageBox::warning(this,"天气","请求数据失败",QMessageBox::Ok);
    }else{
        QByteArray  byteArray = reply->readAll();
        qDebug() << "读所有：" << byteArray.data();
        parseJson(byteArray);
    }
    reply->deleteLater();
}

void MainWindow::on_btnSearch_clicked()
{
    QString cityName = ui->leCity->text();
    getWeatherInfo(cityName);
}
