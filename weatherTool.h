#ifndef WEATHERTOOL_H
#define WEATHERTOOL_H

#include <QMap>
#include <QFile>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonValue>


class WeatherTool{

private:
    static QMap<QString,QString> mCityMaps;

    static void initCityMap(){
        QString filePath = "D:/citycode-2019-08-23.json";
        QFile file(filePath);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray json = file.readAll();
        file.close();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(json,&err);
        if(err.error != QJsonParseError::NoError){
            return;
        }
        if(!doc.isArray()){
            return;
        }
        QJsonArray cities = doc.array();
        for(int i = 0;i <cities.size();i++){
            QString city = cities[i].toObject().value("city_name").toString();
            QString code = cities[i].toObject().value("city_code").toString();
            if(code.size() > 0){
                mCityMaps.insert(city,code);
            }
        }
    }

public:
    static QString getCityCode(QString cityName){
        if(mCityMaps.isEmpty()){
            initCityMap();
        }
        QMap<QString,QString>::iterator it = mCityMaps.find(cityName);
        //
        if(it == mCityMaps.end()){
            it = mCityMaps.find(cityName + "市");
        }
        if(it != mCityMaps.end()){
            return it.value();
        }
        return "";
    }
};

QMap<QString,QString> WeatherTool::mCityMaps={};

#endif // WEATHERTOOL_H
