#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include<QString>

class Today{

public:
    Today()
    {
        date = "2023-03-07";
        city = "深圳";

        ganmao = "感冒指数";
        wendu = 0;
        shidu = "0%";
        pm25 = 0;
        quality = "无数据";

        type = "多云";
        fl = "2级";
        fx = "南风";
        high = 20;
        low = 18;
    }
    QString date;
    QString city;

    QString ganmao;
    QString shidu;

    int wendu;
    int pm25;

    QString quality;
    QString type;

    QString fl;
    QString fx;

    int high;
    int low;
};

class Day{
public:
    Day(){
        date = "2023-03-07";
        week = "周二";

        type = "多云";

        high = 0;
        low = 0;

        fl = "2级";
        fx = "南风";

        aqi = 0;
    }
    QString date;
    QString week;

    QString type;

    int high;
    int low;

    QString fl;
    QString fx;

    int aqi;
};

#endif // WEATHERDATA_H
