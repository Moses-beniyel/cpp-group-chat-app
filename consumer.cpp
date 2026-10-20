#include <winsock2.h>
#include <windows.h>

#include <sql.h>
#include <sqlext.h>

#include <iostream>
#include <string>
#include <rdkafkacpp.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

bool insertUser(SQLHDBC dbc,
                const std::string& userId,
                const std::string& passwordHash)
{
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    const char* sql =
        "INSERT INTO tbl_userdetails "
        "(UserId, Password, Timestamp) "
        "VALUES (?, ?, NOW())";

    SQLRETURN rc = SQLPrepareA(
        stmt, (SQLCHAR*)sql, SQL_NTS);

    SQLLEN idLen = SQL_NTS;
    SQLLEN passwordLen = SQL_NTS;

    if (SQL_SUCCEEDED(rc))
        rc = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
            SQL_C_CHAR, SQL_VARCHAR, 50, 0,
            (SQLPOINTER)userId.c_str(), 0, &idLen);

    if (SQL_SUCCEEDED(rc))
        rc = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT,
            SQL_C_CHAR, SQL_VARCHAR, 255, 0,
            (SQLPOINTER)passwordHash.c_str(), 0, &passwordLen);

    if (SQL_SUCCEEDED(rc))
        rc = SQLExecute(stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    if (!SQL_SUCCEEDED(rc))
    {
        std::cout << "User insert failed.\n";
        return false;
    }

    std::cout << "User saved successfully.\n";
    return true;
}

bool insertMessage(SQLHDBC dbc,
                   const std::string& userId,
                   const std::string& chatMessage)
{
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    const char* sql =
        "INSERT INTO tbl_chatdetails "
        "(UserId, Timestamp, ChatMessage) "
        "VALUES (?, NOW(), ?)";

    SQLRETURN rc = SQLPrepareA(
        stmt, (SQLCHAR*)sql, SQL_NTS);

    SQLLEN idLen = SQL_NTS;
    SQLLEN messageLen = SQL_NTS;

    if (SQL_SUCCEEDED(rc))
        rc = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
            SQL_C_CHAR, SQL_VARCHAR, 50, 0,
            (SQLPOINTER)userId.c_str(), 0, &idLen);

    if (SQL_SUCCEEDED(rc))
        rc = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT,
            SQL_C_CHAR, SQL_LONGVARCHAR, 0, 0,
            (SQLPOINTER)chatMessage.c_str(), 0, &messageLen);

    if (SQL_SUCCEEDED(rc))
        rc = SQLExecute(stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    if (!SQL_SUCCEEDED(rc))
    {
        std::cout << "Message insert failed.\n";
        return false;
    }

    std::cout << "Message saved successfully.\n";
    return true;
}

void showOdbcError(SQLSMALLINT handleType, SQLHANDLE handle)
{
    SQLCHAR state[6], text[1024];
    SQLINTEGER nativeError;
    SQLSMALLINT textLength;

    if (SQL_SUCCEEDED(SQLGetDiagRecA(
        handleType, handle, 1, state, &nativeError,
        text, sizeof(text), &textLength)))
    {
        std::cout << "ODBC error [" << state << "]: "
                  << text << '\n';
    }
}

int main()
{
    std::string dsn = "ChatAppDSN";
    std::string dbUser, dbPassword;

    std::cout << "MySQL username: ";
    std::getline(std::cin, dbUser);

    std::cout << "MySQL password: ";
    std::getline(std::cin, dbPassword);

    SQLHENV env;
    SQLHDBC dbc;

    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION,
                  (SQLPOINTER)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);

    SQLRETURN dbResult = SQLConnectA(
        dbc,
        (SQLCHAR*)dsn.c_str(), SQL_NTS,
        (SQLCHAR*)dbUser.c_str(), SQL_NTS,
        (SQLCHAR*)dbPassword.c_str(), SQL_NTS);

    if (!SQL_SUCCEEDED(dbResult))
    {
        std::cout << "MySQL connection failed.\n";
        showOdbcError(SQL_HANDLE_DBC, dbc);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc);
        SQLFreeHandle(SQL_HANDLE_ENV, env);
        return 1;
    }

    std::cout << "MySQL connected!\n";

    std::string brokers = "localhost:9092";
    std::string topic = "test";
    std::string errstr;

    RdKafka::Conf* conf =
        RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

    if (!conf)
    {
        std::cout << "Kafka configuration creation failed.\n";
        SQLDisconnect(dbc);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc);
        SQLFreeHandle(SQL_HANDLE_ENV, env);
        return 1;
    }

    if (conf->set("bootstrap.servers", brokers, errstr)
        != RdKafka::Conf::CONF_OK)
    {
        std::cout << "Kafka configuration failed: "
                  << errstr << '\n';
        delete conf;
        SQLDisconnect(dbc);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc);
        SQLFreeHandle(SQL_HANDLE_ENV, env);
        return 1;
    }

    conf->set("group.id", "my-consumer-group", errstr);

    RdKafka::KafkaConsumer* consumer =
        RdKafka::KafkaConsumer::create(conf, errstr);

    delete conf;

    if (!consumer)
    {
        std::cout << "Kafka consumer creation failed: "
                  << errstr << '\n';
        SQLDisconnect(dbc);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc);
        SQLFreeHandle(SQL_HANDLE_ENV, env);
        return 1;
    }

    if (consumer->subscribe({topic}) != RdKafka::ERR_NO_ERROR)
    {
        std::cout << "Kafka subscription failed.\n";
        delete consumer;
        SQLDisconnect(dbc);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc);
        SQLFreeHandle(SQL_HANDLE_ENV, env);
        return 1;
    }

    std::cout << "Listening to Kafka topic: "
              << topic << '\n';

    while (true)
    {
        RdKafka::Message* msg = consumer->consume(1000);

        if (!msg)
            continue;

        if (msg->err() == RdKafka::ERR_NO_ERROR)
        {
            std::string payload(
                static_cast<const char*>(msg->payload()),
                msg->len());

            try
            {
                json data = json::parse(payload);

                std::string type = data.at("type").get<std::string>();
                std::string userId = data.at("userId").get<std::string>();

                if (userId.empty() || userId.size() > 50)
                {
                    std::cout << "Invalid userId.\n";
                }
                else if (type == "login")
                {
                    std::string passwordHash =
                        data.at("passwordHash").get<std::string>();

                    if (passwordHash.empty() || passwordHash.size() > 255)
                        std::cout << "Invalid password hash.\n";
                    else
                        insertUser(dbc, userId, passwordHash);
                }
                else if (type == "message")
                {
                    std::string chatMessage =
                        data.at("message").get<std::string>();

                    if (chatMessage.empty())
                        std::cout << "Empty message ignored.\n";
                    else
                        insertMessage(dbc, userId, chatMessage);
                }
                else
                {
                    std::cout << "Unknown event type: "
                              << type << '\n';
                }
            }
            catch (const std::exception& e)
            {
                std::cout << "Invalid JSON/message: "
                          << e.what() << '\n';
            }
        }
        else if (msg->err() != RdKafka::ERR__TIMED_OUT)
        {
            std::cout << "Kafka error: "
                      << msg->errstr() << '\n';
        }

        delete msg;
    }

    // Cleanup is reached if you later add a loop-exit condition.
    consumer->close();
    delete consumer;
    SQLDisconnect(dbc);
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, env);
    RdKafka::wait_destroyed(5000);
    return 0;
}
