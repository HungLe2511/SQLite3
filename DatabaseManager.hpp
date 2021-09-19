/*
* DatabaseManager.hpp
* Created by DuNTK on 09/06/2021.
* Copyright © 2021 BKAV. All rights reserved.
*/


#ifndef DatabaseManager_hpp
#define DatabaseManager_hpp

#include <iostream>
#include <sqlite3.h> 
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "json.h"
#include <sstream>
#include <vector>
#include <typeinfo>
class DatabaseManager {
private:
    std::string namedatabase;
    std::string databasePath;
    sqlite3 *db;
    sqlite3 *db1;
    sqlite3 *db2;
    sqlite3 *db3;
    sqlite3 *db4;
    
public:

    ~DatabaseManager();

    DatabaseManager();

    bool openDatabase(const char * , sqlite3 *);

    void createTable(std::string);
 
    bool deleteTable(std::string);

    bool deleteField(std::string, std::string);

    void addField(std::string, std::string, std::string);

    void renameTable(std::string, std::string);

    void renameField(std::string, std::string, std::string);

    bool getMonitorData(std::string, json&);

    std::string getMonitorData(std::string, std::string);

    bool getMonitorDataWithout(std::string, std::string, json&);

    void selectDatabase(int);

    bool isExist(const char *);

    std::vector<std::string> getTablesName();

    bool createTable(std::string, json);

    bool setTable(std::string, json);

    void sqlCloseDatabase();

    int isJsonBoolean(std::string);

    bool isJsonString( std::string);

    std::string boolToString(bool);

    std::string getFromTable(std::string, std::string);

    bool setTable(std::string, std::string, std::string);

    bool setTable(std::string, std::string, int);

    void connectDatabase();

    std::string boolToJsonString( bool);

};

#endif
