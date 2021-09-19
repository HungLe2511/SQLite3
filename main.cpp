#include "DatabaseManager.hpp"


DatabaseManager* dbMng = NULL;

sqlite3 *db[4];


int main(int argc, char* argv[]) {;

    json json1, json_test;
    std::vector<std::string> vectors;
    std::string string1;
    bool bool_test;
    
    string1 =  R"({
  "endpoint_id": 345,
  "monitor_type": "FaceRecognition",
  "enable": false,
  "sleep": 20000423,
  "monitor_id": 1,
  "regions": [],
  "frame_size": {
    "width": 190022,
    "height": 1080
  },
  "server_data": {
    "use": true,
    "url": "http://urlserver.com/eventdata",
    "access_token": "token",
    "identify": true
  },
  "server_media": {
    "use": true,
    "url": "http://urlserver.com/eventmedia",
    "access_token": "token",
    "event_type": "123"
  }
})";

    json_test = json::parse(string1);

    DatabaseManager test;
    test.selectDatabase(2);
    //vectors = test.getTablesName();
    //test.deleteTable("ABC11");
    //test.createTable("ABC11", json_test);
    //test.getMonitorData("ABC10", json1);
    //test.getMonitorDataWithout("ABC10", "sleep", json1);
    //std::cout << json1.dump(4) << '\n';
    test.setTable("ABC", json_test);
    //std::cout << json_test.dump(4) << '\n';
    //bool_test = test.isExist("ABC2");
    //test.setTable("ABC10", "endpoint_id", "47");
    //std::cout << test.getFromTable("ABC10", "enable") << std::endl;
    //test.dropField("ABC", "name");
    //std::cout << bool_test << std::endl;
    //std::cout <<"json : "<<json << std::endl;

    return 0;
}