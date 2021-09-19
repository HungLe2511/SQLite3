/*
* DatabaseManager.cpp
* Created by DuNTK on 09/06/2021.
* Copyright © 2021 BKAV. All rights reserved.
*/
#include "DatabaseManager.hpp"

DatabaseManager::DatabaseManager()
{
    #if defined(__ANDROID__)
    {
            this->databasePath = "/data/user/0/com.bkav.aiviewconfig/database/";

    }
    #elseif defined(__LINUX) 
    {
            this->databasePath = "../";
    }
    #endif
    this->connectDatabase();
}

DatabaseManager::~DatabaseManager() {
    sqlite3_close(this->db1);
    sqlite3_close(this->db2);
    sqlite3_close(this->db3);
    sqlite3_close(this->db4);
}

void DatabaseManager::createTable(std::string tableName) {
    std::string str = "CREATE TABLE " + tableName + "(empty TEXT);";
    char *zErrMsg = 0;

    int rc = sqlite3_exec(this->db, str.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

bool DatabaseManager::openDatabase(const char *dataBasename, sqlite3 *db) {
    int rc = sqlite3_open(dataBasename, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return false;
    }
    return true;
}

bool DatabaseManager::deleteTable(std::string tableName) {
    char *zErrMsg = 0;
    std::string str = "DROP TABLE " + tableName + ";";

    int rc = sqlite3_exec(this->db, str.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
    return true;
}

void DatabaseManager::addField(std::string tableName, std::string fieldName, std::string fieldType) {
    char *zErrMsg = 0;
    std::string str = "ALTER TABLE " + tableName + "\n   ADD " + fieldName + " " + fieldType + ";";

    int rc = sqlite3_exec(this->db, str.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

void DatabaseManager::renameTable(std::string tableName, std::string newName) {
    char *zErrMsg = 0;
    std::string str = "ALTER TABLE " + tableName + " RENAME TO " + newName + ";";

    int rc = sqlite3_exec(this->db, str.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

void DatabaseManager::renameField(std::string tableName, std::string fieldName, std::string newName) {
    char *zErrMsg = 0;
    std::string str = "ALTER TABLE " + tableName + " RENAME COLUMN " + fieldName + " TO " + newName + ";";

    int rc = sqlite3_exec(this->db, str.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

bool DatabaseManager::getMonitorData(std::string tableName, json &jsonOut) {
    sqlite3_stmt *stmt;
    const unsigned char * sqlite3ColumnText;
    std::string str = "SELECT * FROM " + tableName + ";";

    int rc = sqlite3_prepare_v2(this->db, str.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        std::cout <<"error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    int maxCol = sqlite3_column_count(stmt);
    // 
    const char *keyName[maxCol];
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        for (int j = 0; j < maxCol; j++){
        keyName[j] = sqlite3_column_name(stmt, j);
        sqlite3ColumnText = sqlite3_column_text(stmt, j);
        if (strcmp(sqlite3_column_decltype(stmt, j), "INTEGER") == 0) {
            jsonOut[keyName[j]] = sqlite3_column_int(stmt, j);
        }
        else {
            if (!isJsonString(std::string((const char *)sqlite3ColumnText))) {
                jsonOut[keyName[j]] = json::parse(std::string((const char *)sqlite3ColumnText));
                }
            else {
                if(isJsonBoolean(std::string((const char *)sqlite3ColumnText)) == -1)
                    jsonOut[keyName[j]] = (const char *)sqlite3ColumnText;
                else 
                    jsonOut[keyName[j]] = json::parse(std::string((const char *)sqlite3ColumnText));
                }
            }
        }
    }
    if (rc != SQLITE_DONE) {
        std::cout <<"error: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    if(!jsonOut.empty()) return true;
    else return false;
}

std::string DatabaseManager::getMonitorData(std::string tableName, std::string fieldName) {
    sqlite3_stmt *stmt;
    char *zErrMsg = 0;
    std::string str;
    std::string sql = "SELECT * FROM " + tableName + ";";
    
    int rc = sqlite3_prepare_v2(this->db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) std::cout << "error: " << sqlite3_errmsg(db) << std::endl;
    int maxCol = sqlite3_column_count(stmt);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        for (int j = 0; j < maxCol; j++) {
            if (std::string((const char *)sqlite3_column_name(stmt, j)) == fieldName) {
                if (strcmp(sqlite3_column_decltype(stmt, j), "INTEGER") == 0) 
                    str = (std::string)(const char *)sqlite3_column_int(stmt, j);
                else 
                    str = (std::string)(const char *)sqlite3_column_text(stmt, j);
            }
        }
    }
    sqlite3_finalize(stmt);

    return str;
}

bool DatabaseManager::getMonitorDataWithout(std::string tableName, std::string fieldName, json &jsonOut) {
    sqlite3_stmt *stmt;
    std::string str = "SELECT * FROM " + tableName + ";";
    int rc = sqlite3_prepare_v2(this->db, str.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        std::cout << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return false;
        }
    int maxCol = sqlite3_column_count(stmt);
    const char *keyName[maxCol];
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        for (int j = 0; j < maxCol; j++) {
            keyName[j] = sqlite3_column_name(stmt, j);
            if(fieldName == std::string(keyName[j])) continue;
            if (strcmp(sqlite3_column_decltype(stmt, j), "INTEGER") == 0) {
                jsonOut[keyName[j]] = sqlite3_column_int(stmt, j);
            } else {
                std::string colValue = std::string((const char *)sqlite3_column_text(stmt, j));
                if (!isJsonString(colValue)) {
                    jsonOut[keyName[j]] = json::parse(colValue);
                } else {
                    if (isJsonBoolean(colValue) == -1) {
                        jsonOut[keyName[j]] = (const char *)sqlite3_column_text(stmt, j);
                    } else {
                        jsonOut[keyName[j]] = json::parse(colValue);
                    }
                }
            }
        }
    }
    if (rc != SQLITE_DONE) {
        std::cout << "error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_finalize(stmt);
    if (!jsonOut.empty()) return true;
    else return false;
}

void DatabaseManager::selectDatabase(const int cameraId) {
    switch (cameraId) {
        case 1: {
            std::cout << "ENDPOINT " << cameraId << std::endl;
            //LOGD("ENDPOINT %d", cameraId);
            this->db = db1;
            break;
        }
        case 2: {
            std::cout << "ENDPOINT " << cameraId << std::endl;
            //LOGD("ENDPOINT %d", cameraId);
            this->db = db2;
            break;
        }
        case 3: {
            std::cout << "ENDPOINT " << cameraId << std::endl;
            //LOGD("ENDPOINT %d", cameraId);
            this->db = db3;
            break;
        }
        case 4: { 
            std::cout << "ENDPOINT " << cameraId << std::endl;
            //LOGD("ENDPOINT %d", cameraId);
            this->db = db4; 
            break;
        }
        default: {
            std::cout << "This database does not exist!\n";
            break; 
        }
    }
}

void DatabaseManager::connectDatabase() {
    std::string path1 = this->databasePath + "aicam1database.db";
    if (sqlite3_open(path1.c_str(), &db1)) {
        std::cout << "Can't open database 1, please check your database" << std::endl;
        //LOGD("Can't open database 1, please check your database");
    } else {
        std::cout << "Opened database 1 successfully" << std::endl;
        //LOGD("Opened database 1 successfully");
    }

    std::string path2 = this->databasePath + "aicam2database.db";
    if (sqlite3_open(path2.c_str(), &db2)) {
        std::cout << "Can't open database 2, please check your database" << std::endl;
        //LOGD("Can't open database 2, please check your database");
    } else {
        std::cout << "Opened database 2 successfully" << std::endl;
        //LOGD("Opened database 2 successfully");
    }

    std::string path3 = this->databasePath + "aicam3database.db";
    if (sqlite3_open(path3.c_str(), &db3)) {
        std::cout << "Can't open database 3, please check your database" << std::endl;
        //LOGD("Can't open database 3, please check your database");
    } else {
        std::cout << "Opened database 3 successfully" << std::endl;
        //LOGD("Opened database 3 successfully");
    }

    std::string path4 = this->databasePath + "aicam4database.db";
    if (sqlite3_open(path4.c_str(), &db4)) {
        std::cout << "Can't open database 4, please check your database" << std::endl;
        //LOGD("Can't open database 4, please check your database");
    } else {
        std::cout << "Opened database 4 successfully" << std::endl;
        //LOGD("Opened database 4 successfully");
    }
}

std::vector<std::string> DatabaseManager::getTablesName() {
    sqlite3_stmt* stmt;
    std::vector<std::string> vectors;
    char *zErrMsg = 0;
    std::string str = "SELECT name FROM sqlite_master WHERE type = \"table\";";

    int rc = sqlite3_prepare_v2(this->db, str.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        std::cout << "error: " << sqlite3_errmsg(db) << std::endl;
    }
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) vectors.push_back((std::string)(const char*)sqlite3_column_text(stmt, 0));
    if (rc != SQLITE_DONE) 
        std::cout << "error: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);

    return vectors;
}
// para = string
bool DatabaseManager::isExist(const char *tableName) {
    sqlite3_stmt* stmt;
    bool checkFlag;
    std::string str = "SELECT name FROM sqlite_master WHERE type = \"table\";";

    int rc = sqlite3_prepare_v2(this->db, str.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        std::cout << "error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (strcmp((const char *)sqlite3_column_text(stmt, 0), tableName) == 0) {
            checkFlag =  true;
            break;
        }
    }
    if (rc != SQLITE_DONE) std::cout << "error: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);

    if (checkFlag) return true;
    else return false;
}

// them truong ID de dinh danh tung jsontring
bool DatabaseManager::createTable(std::string tableName, json data) {
    std::string strInsertData;
    std::string strCreateTable;
    char *zErrMsg = 0;

    for (auto &el : data.items()) {
        if((std::string)el.value() == "") return false;
        else {
            if(strcmp(el.value().type_name(), "number") == 0)
                strCreateTable += (std::string)el.value() + " INTEGER,";
            else 
                strCreateTable += (std::string)el.value() + " TEXT,";
        }
        
    }
    strCreateTable.pop_back();
    strCreateTable = "CREATE TABLE " + tableName + "(" + strCreateTable + ");";
    int rc = sqlite3_exec(this->db, strCreateTable.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    strInsertData = "INSERT INTO " + tableName + " VALUES ( ";
    for (auto &el : data.items()) {
        if (el.value().type_name() == "boolean") {
            strInsertData +=  "'" + boolToJsonString(el.value()) + "',";
            continue;
        }
        else if (el.value().type_name() == "number") {
            strInsertData +=  to_string(el.value()) + ",";
            continue;
        }
        else if (el.value().type_name() == "object") {
            strInsertData += "'" + el.value().dump() + "',";
            continue;
        }
        else if (el.value().type_name() == "string") {
            strInsertData +="'" + (std::string)el.value() + "',";
            continue;
        }
        else if (el.value().type_name() == "array") {
            strInsertData += "'" + el.value().dump() + "',";
            continue;
        }
    }
    strInsertData.pop_back();
    strInsertData += ");";
    rc = sqlite3_exec(db, strInsertData.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
    else return true;
}

// overload cho nhieu dong them para la ID
bool DatabaseManager::setTable(std::string tableName, json data) {
    char *zErrMsg = 0;
    std::string str_sql;
    
    for (auto &el : data.items()) {
        if (el.value().type_name() == "boolean") {
            str_sql = "UPDATE " + tableName + " SET " + el.key() + " = '" + boolToJsonString(el.value()) + "';";
        } else if (el.value().type_name() == "number") {
            str_sql = "UPDATE " + tableName + " SET " + el.key() + " = " + nlohmann::to_string(el.value()) + ";";
        } else if (el.value().type_name() == "object") {
            str_sql = "UPDATE " + tableName + " SET " + el.key() + " = '" + el.value().dump() + "';";
        } else if (el.value().type_name() == "string") {
            str_sql = "UPDATE " + tableName + " SET " + el.key() + " = '" + std::string(el.value()) + "';";
        } else if (el.value().type_name() == "array") {
            str_sql = "UPDATE " + tableName + " SET " + el.key() + " = '" + el.value().dump() + "';";
        }
        int rc = sqlite3_exec(this->db, str_sql.c_str(), NULL, 0, &zErrMsg);
          if (rc != SQLITE_OK)
          {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            return false;
            break;
          }
    }
    return true;
}

// return true false
bool DatabaseManager::deleteField(std::string tableName, std::string fieldName) {
    sqlite3_stmt *stmt;
    std::string listFieldName, listFieldName1;
    std::string str1 = "SELECT * FROM " + tableName + ";";
    std::string str2;
    char *zErrMsg = 0;
    int rc = sqlite3_prepare_v2(this->db, str1.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
    int i = sqlite3_column_count(stmt);
    if (rc != SQLITE_OK) std::cout << "error: " << sqlite3_errmsg(db) << std::endl;
    for ( int j = 0; j < i; j++) {
        if(((std::string)sqlite3_column_name(stmt, j) != fieldName)) {
            listFieldName += (std::string)sqlite3_column_name(stmt, j)+" " + (std::string)sqlite3_column_decltype(stmt, j) +",";
            listFieldName1 += (std::string)sqlite3_column_name(stmt, j) + ",";
        }
    }
    listFieldName1.pop_back();
    listFieldName.pop_back();
    str2 = "begin TRANSACTION; CREATE TABLE t1_backup( "
         + listFieldName + "); " + "INSERT INTO t1_backup SELECT " 
         + listFieldName1 + " FROM " + tableName +";"
         + "DROP TABLE " + tableName + ";" + "ALTER TABLE t1_backup RENAME TO " + tableName + ";  COMMIT;";
    rc = sqlite3_exec(this->db, str2.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

int DatabaseManager::isJsonBoolean(std::string str) {
    if(str == "true") return 1;
    else if (str == "false") return 0;
    else return -1;
}

bool DatabaseManager::isJsonString(std::string str) {
    return  ! ((str.find('[') != std::string::npos && str.find(']') != std::string::npos) 
            || (str.find('{') != std::string::npos && str.find('}') != std::string::npos));
}

std::string DatabaseManager::boolToJsonString(bool value) {
    return value ? "true" : "false";
}

std::string DatabaseManager::getFromTable(std::string tableName, std::string fieldName) {
    sqlite3_stmt* stmt;
    char *zErrMsg = 0;
    std::string str;
    std::string sql = "SELECT * FROM " + tableName + ";";
    std::stringstream ss;

    int rc =sqlite3_prepare_v2(this->db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        std::cout << "error: "<< sqlite3_errmsg(db) << std::endl;
    }
    int i = sqlite3_column_count(stmt);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        for (int j = 0; j < i; j++) {
            if ((std::string)(const char *)sqlite3_column_name(stmt, j) == fieldName) {
                if ((std::string)sqlite3_column_decltype(stmt, j) == "TEXT") str = (std::string)(const char *)sqlite3_column_text(stmt, j);
                else {
                    ss << sqlite3_column_text(stmt, j);
                    str = ss.str();
                }  
            }
        }
    }
    sqlite3_finalize(stmt);

    return str;
}

bool DatabaseManager::setTable(std::string tableName, std::string FieldName, std::string value) {

    char *zErrMsg = 0;
    std::string str = "UPDATE " + tableName + " SET " + FieldName + " = '" + value + "';";
    int rc = sqlite3_exec(this->db, str.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        return false;
    }

    return true;
}

bool DatabaseManager::setTable(std::string tableName, std::string FieldName, int value) {
    std::string strValue;
    std::stringstream ss;
    ss << value;
    strValue = ss.str();
    char *zErrMsg = 0;
    std::string str = "UPDATE " + tableName + " SET " + FieldName + " = " + strValue + ";";
    int rc = sqlite3_exec(this->db, str.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        return false;
    }
    return true;
}