//
// Created by no3core on 2023/8/24.
//



#include <QJsonObject>
#include <string>

int main(){
    auto testjson = QJsonObject{
            {"id",      QString::fromStdString(std::to_string(123))},
//            {"id",      QJsonValue::Null },
            {"type",    "text"   },
            {"content", "simpletest for test"},
            {"time",    1288391},
            {"uid",     1231},
            {"mid",     QString::fromStdString(std::to_string(5648))},
            {"gid",     12813}
    };

    QTextStream(stdout) << testjson.contains("id") << "\n";
    QTextStream(stdout) << testjson.value("id").isNull() << "\n";

}