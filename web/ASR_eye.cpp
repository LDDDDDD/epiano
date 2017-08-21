
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <mysql.h>	// #include <mysql/mysql.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <pthread.h>
#include <iconv.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include "ASRStatApi.h"

#include "ASR_eye.h"

#ifdef WIN32
	#define DIRDASH '\\'
#else
	#define DIRDASH '/'
#endif

using namespace std;

unsigned long CUdpPk::get_time_ms()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

unsigned long get_time_ms()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}


char * datetime(char delemeter)
{
    time_t now;
    struct tm *tm_now;
	static char buf[30];

    time(&now);
    tm_now = localtime(&now); 

    sprintf(buf, "%d%c%d%c%d %d:%d:%d", 1900 + tm_now->tm_year, delemeter, tm_now->tm_mon+1, delemeter, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec); 

    return buf;
}

int DateCmp(long date0, long date1)
{
    time_t day0 = (time_t)date0;
	time_t day1 = (time_t)date1;
	struct tm TM0;
	struct tm *tm0 = &TM0;
	struct tm *tm1;
	//static char buf[30];
	//static char buf1[30];

	*tm0 = *localtime(&day0); 	// 注: localtime的返回值存于栈中, 必须要立即使用，否则可能被下一次调用冲掉
	tm1 = localtime(&day1);

	// test
	//printf("\n------------------------%d %d %d    %d %d %d", 
	//	tm0->tm_year, tm0->tm_mon,tm0->tm_mday,
	//	tm1->tm_year, tm1->tm_mon,tm1->tm_mday);

	if (tm0->tm_year > tm1->tm_year)
	{
		return 1;
	}
	else if (tm0->tm_year < tm1->tm_year)
	{
		return -1;
	}
	else
	{
		if (tm0->tm_mon > tm1->tm_mon)
		{
			return 1;
		}
		else if (tm0->tm_mon < tm1->tm_mon)
		{
			return -1;
		}
		else
		{
			if (tm0->tm_mday > tm1->tm_mday)
			{
				return 1;
			}
			else if (tm0->tm_mday < tm1->tm_mday)
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}
	}

	return 0;
}

// 获取今天串
char * GetCurDateString(char delemeter)
{
    time_t now;
    struct tm *tm_now;
	static char buf[30];

    time(&now);
    tm_now = localtime(&now); 

    sprintf(buf, "%d%c%d%c%d", 1900 + tm_now->tm_year, delemeter, tm_now->tm_mon+1, delemeter, tm_now->tm_mday); 

    return buf;
}

// 获取某天串
char * GetDateString(int date, char delemeter)
{
    time_t now = (time_t)date;
	static char buf[30];
	struct tm *tm_now;
	
	tm_now = localtime(&now); 

    sprintf(buf, "%d%c%d%c%d", 1900 + tm_now->tm_year, delemeter, tm_now->tm_mon+1, delemeter, tm_now->tm_mday); 

    return buf;
}


MYSQL mysql;
static ASRAnalyser * pASRAnalyser = NULL;
int gudpsock = 0; 
int gStatThreadRuning = 1;
unsigned int gStatThreadRuningBeats = 0;
unsigned int gStatThreadRepProcessRuningBeats = 0;
void * UDPThread(void * arg);
void DB_demo();
int ASREventRecordStatCfg_Internel(ASR_REC_CFG * pASRRecCfg);

main()
{
	// 严格的讲，应该应用方发送cfg消息, 触发cfg操作
	// 为暂时防止本程序中间退出，而应用层程序未重启导致的cfg遗漏问题, 在此执行cfg操作
	ASR_REC_CFG ASRRecCfg;
	strcpy(ASRRecCfg.DBName, "epiano"); // ASR_DB
	strcpy(ASRRecCfg.HtmlSubDir, ""); // /var/www/html/");
	ASREventRecordStatCfg_Internel(&ASRRecCfg);

	{
		pthread_t th;
		int arg = 0; //(int)this;
		//int *thread_ret;  
		printf( "\nCreating UDPThread ... !"); 
		int ret = pthread_create( &th, NULL, UDPThread, &arg );  
	    if( ret != 0 ){  
	        printf( "\nCreate UDPThread error!");  
	        return -1;  
	    }
	    printf("\nCreating UDPThread done." );  
	    //pthread_join( th, NULL); //(void**)&thread_ret );  		
	}

//#define SELF_TEST
#ifdef SELF_TEST
	sleep(1);
	DB_demo();
#endif

	while(1)
	{
		sleep(1);
	}	
	getchar();

	gStatThreadRuning = 0;
	close(gudpsock);

	sleep(1);
}


#if 0
main()
{
	DB_demo();
}
#endif
#if 1
void DB_demo()
{
	int i = 0;
	ASR_EVENT Event = {EVENT_TYPE_NULL};
	ASR_EVENT * pEvent = &Event;
	char buf[1000];
	char SessionId[1000];

	// test
	if (0)
	{
		FILE * pf;
		FILE * pfOut;

		// load template
		pf = fopen("index_temp.html", "r+b");
		if (!pf)
		{
			printf("\nOpen input file error.");
			return;
		}

		pfOut = fopen("index_out.html", "w+b");
		if (!pfOut)
		{
			printf("\nOpen output file error.");

			fclose(pf);
			
			return;
		}

		char indexhtml[100000];
		int infilesize = fread(indexhtml, 1, 100000, pf);
		puts(indexhtml);


		fclose(pf);
		fclose(pfOut);

		return;
	}
	//LoadFile("index_temp.html");

	puts("\n1---------------");
	{
#define ASR_DB_NAME_TEST "ASR_DB_TEST"
		
		ASR_REC_CFG ASRRecCfg;
		strcpy(ASRRecCfg.DBName, ASR_DB_NAME_TEST);
		strcpy(ASRRecCfg.HtmlSubDir, "test");
		ASREventRecordStatCfg(&ASRRecCfg);

		// 模仿EVENT_TYPE_ASR_INSERT, EVENT_TYPE_ASR_UPDATE
		for(i = 0; i < 2; i++)
		{
			//printf("\n2.%d---------------", i);

			
			pEvent->eType = EVENT_TYPE_ASR_INSERT;
			strcpy(pEvent->u_data.AsrInsert.Version, "000001");
			strcpy(pEvent->u_data.AsrInsert.StartTime, datetime('-'));
			sprintf(pEvent->u_data.AsrInsert.SessionId, "%d", random()%100000);
			strcpy(SessionId, pEvent->u_data.AsrInsert.SessionId);
			sprintf(pEvent->u_data.AsrInsert.UserName, "w00%d", random()%10000);
			sprintf(pEvent->u_data.AsrInsert.ASRResultStr, "一二三%d", random()%1000);
			sprintf(pEvent->u_data.AsrInsert.AudioFilePathName, "d:\audiofile\%d", random()%2000);
			pEvent->u_data.AsrInsert.ASRE2EDurationInMs = 0;
			pEvent->u_data.AsrInsert.ASRRecongitioanDurationInMs = 100 + (random()%100);
			ASREventRecordStatRep(ASR_DB_NAME_TEST, pEvent);
			
			pEvent->eType = EVENT_TYPE_ASR_UPDATE;
			strcpy(pEvent->u_data.AsrUpdate.SessionId, SessionId);
			pEvent->u_data.AsrUpdate.ASRE2EDurationInMs = 200 + (random()%100);
			ASREventRecordStatRep(ASR_DB_NAME_TEST, pEvent);
			/**/
		}

		// 模仿EVENT_TYPE_ASR_INSERT, EVENT_TYPE_ASR_UPDATE
		/**/
		for(i = 0; i < 2; i++)
		{
			//printf("\n3.%d---------------", i);
			pEvent->eType = EVENT_TYPE_USR_HABIT;
			sprintf(pEvent->u_data.Habit.UserName, "w00%d", random()%10000);
			strcpy(pEvent->u_data.Habit.StartTime, datetime('-'));
			sprintf(pEvent->u_data.Habit.ActionDesc, "行为描述_%d", random()%1000);
			ASREventRecordStatRep(ASR_DB_NAME_TEST, pEvent);
		}
		
	}

	printf("\n\n\nPress any key to quit:");

	char c = getchar();	

	if (pASRAnalyser)
	{
		delete pASRAnalyser;
	}

#if 0	
	//int i;
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;
	mysql_init(&mysql);
	MYSQL * myp = mysql_real_connect(&mysql, "localhost", "root", "ddshlmstzczx", "test", 3306, NULL, 0);
	if (!myp)
	{
		printf("\nmysql_real_connect fail, quit.");
		return;
	}

	/* 设置数据库默认字符集 */ 
    if ( mysql_set_character_set( &mysql, "utf8" ) ) { 
        fprintf ( stderr , "错误, %s/n" , mysql_error( & mysql) ) ; 
    } 

	// 中文字符, 注，此三行在mysql中手工执行后可正常显示中文?
	{
		string sql = "SET character_set_client='gbk'";
		mysql_query(&mysql, sql.c_str());
		sql = "SET character_set_connection='gbk'";
		mysql_query(&mysql, sql.c_str());
		sql = "SET character_set_results='gbk'";
		mysql_query(&mysql, sql.c_str());
	}


	/*
	drop table RegUserTbl;
	drop table AllUserASRActTbl;
	drop table StatofOneDayTbl;
	*/
	{
		string sql = "drop database test";
		mysql_query(&mysql, sql.c_str());
		printf("\ndrop database test.");
		mysql_free_result(result);
		
		sql = "drop table RegUserTbl";
		mysql_query(&mysql, sql.c_str());
		printf("\ndrop DB RegUserTbl.");
		mysql_free_result(result);

		sql = "drop table AllUserASRActTbl";
		mysql_query(&mysql, sql.c_str());
		printf("\ndrop DB AllUserASRActTbl.");
		mysql_free_result(result);

		sql = "drop table StatInOneDayTbl_2017_3_15";
		mysql_query(&mysql, sql.c_str());
		printf("\ndrop DB StatofOneDayTbl.");
		mysql_free_result(result);
	}

	// create db
	{
		string sql = "CREATE DATABASE test CHARACTER SET 'utf8' COLLATE 'utf8_general_ci'"; 
		//string sql = "CREATE DATABASE test"; 
		mysql_query(&mysql, sql.c_str());
		printf("\nCreate DB.");
		mysql_free_result(result);

		sql = "use test"; 
		mysql_query(&mysql, sql.c_str());
		printf("\nuse test.");
		mysql_free_result(result);
	}

	// create user tbl
	{
		string sql = "create table t1 (id int, name varchar(30))";
		mysql_query(&mysql, sql.c_str());
		printf("\nCreate DB t1.");
		mysql_free_result(result);
		
		// 注册用户表: 用户唯一标识(w001xx); 注册日期时间
		sql = "create table RegUserTbl (UserName varchar(30) NOT NULL PRIMARY KEY, RegDatetime datetime NOT NULL)";
		mysql_query(&mysql, sql.c_str());
		printf("\nCreate DB RegUserTbl.");
		mysql_free_result(result);

		// 所有用户的识别记录: 用户唯一标识, ASR操作时间, 原始录音文件, 降噪录音文件, 识别结果, 识别耗时(ms)，是否人工确认
		// CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
		// ALTER TABLE Persons
		// ADD PRIMARY KEY (Id_P)
		// ALTER TABLE Persons
		// ADD CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
		sql = "create table AllUserASRActTbl (ASRDatetime datetime NOT NULL, uuid bigint,UserName varchar(10) NOT NULL, AduioFileOrg varchar(20), AudioFileNS varchar(20), ASRResult varchar(30), ASRDuration int, ASRE2EDuration int, Verified int)";
		mysql_query(&mysql, sql.c_str());
		printf("\nCreate DB AllUserASRActTbl.");
		mysql_free_result(result);

		// 天内统计, 五分钟一个点, 288点, idx:0~287
		sql = "create table StatInOneDayTbl_20170310 (Idx int, StartTime time, RegUserNum int, ActiveUserNum int, ASRRate float, TraditionalActNum int, ASRNum int, ASRFailNum int, C0 int,CF0 int,	C1 int,CF1 int,	C2 int,CF2 int,	C3 int,CF3 int, C4 int,CF4 int, C5 int,CF5 int, C6 int,CF6 int, C7 int,CF7 int, C8 int,CF8 int, C9 int,CF9 int, C10 int,CF10 int, C11 int,CF11 int, C12 int,CF12 int, C13 int,CF13 int, C14 int,CF14 int, C15 int,CF15 int, C16 int,CF16 int, C17 int,CF17 int, C18 int,CF18 int, C19 int,CF19 int, C20 int,CF20 int, C21 int,CF21 int, C22 int,CF22 int, C23 int,CF23 int, C24 int,CF24 int, C25 int,CF25 int, C26 int,CF26 int, C27 int,CF27 int, C28 int,CF28 int, C29 int,CF29 int, C30 int,CF30 int, C31 int,CF31 int, C32 int,CF32 int, C33 int,CF33 int, C34 int,CF34 int, C35 int,CF35 int, C36 int,CF36 int, C37 int,CF37 int, C38 int,CF38 int, C39 int,CF39 int, C40 int,CF40 int, C41 int,CF41 int, C42 int,CF42 int, C43 int,CF43 int, C44 int,CF44 int, C45 int,CF45 int, C46 int,CF46 int, C47 int,CF47 int, C48 int,CF48 int, C49 int,CF49 int)";
		mysql_query(&mysql, sql.c_str());
		printf("\nCreate DB StatofOneDayTbl.");
		mysql_free_result(result);

		// 跨天统计, 一天一个点
		// 也可根据天内统计
		//sql = "create table StatofDaysTbl (idx int, Day GetCurDateString, RegUserNum int, ActiveUserNum int, ASRRate float, TraditionalActNum int, ASRNum int, ASRFailNum int, C0 int,CF0 int,	C1 int,CF1 int,	C2 int,CF2 int,	C3 int,CF3 int, C4 int,CF4 int, C5 int,CF5 int, C6 int,CF6 int, C7 int,CF7 int, C8 int,CF8 int, C9 int,CF9 int, C10 int,CF10 int, C11 int,CF11 int, C12 int,CF12 int, C13 int,CF13 int, C14 int,CF14 int, C15 int,CF15 int, C16 int,CF16 int, C17 int,CF17 int, C18 int,CF18 int, C19 int,CF19 int, C20 int,CF20 int, C21 int,CF21 int, C22 int,CF22 int, C23 int,CF23 int, C24 int,CF24 int, C25 int,CF25 int, C26 int,CF26 int, C27 int,CF27 int, C28 int,CF28 int, C29 int,CF29 int, C30 int,CF30 int, C31 int,CF31 int, C32 int,CF32 int, C33 int,CF33 int, C34 int,CF34 int, C35 int,CF35 int, C36 int,CF36 int, C37 int,CF37 int, C38 int,CF38 int, C39 int,CF39 int, C40 int,CF40 int, C41 int,CF41 int, C42 int,CF42 int, C43 int,CF43 int, C44 int,CF44 int, C45 int,CF45 int, C46 int,CF46 int, C47 int,CF47 int, C48 int,CF48 int, C49 int,CF49 int)";
		//mysql_query(&mysql, sql.c_str());
		//printf("\nCreate DB StatofOneDayTbl.");
		//mysql_free_result(result);
	}

	// insert
	if (1)
	{
		string sql = "insert into t1 (id, name) values (1, 'java1');";
		mysql_query(&mysql, sql.c_str());
		printf("\ninsert into t1;");
		mysql_free_result(result);
		sql = "insert into t1 (id, name) values (2, 'java2');";
		mysql_query(&mysql, sql.c_str());
		mysql_free_result(result);
	}

	// update
	if (1)
	{
		string sql = "update t1 set name = 'java33' where id = 2;";
	    mysql_query(&mysql, sql.c_str());
		mysql_free_result(result);
	}

	// call proc
	//string sql = "call p01();";
    //mysql_query(&mysql, sql.c_str());

	// query
	printf("\n0-------------------------");
	//if (1)
	//{
		string sql = "select id,name from t1;";
	    mysql_query(&mysql, sql.c_str());
	    result = mysql_store_result(&mysql);
	    int rowcount = mysql_num_rows(result);
	    cout << rowcount << endl;
		//mysql_free_result(result);
		int fieldcount = mysql_num_fields(result);
	    cout << fieldcount << endl;
		//mysql_free_result(result);
		for(int i = 0; i < fieldcount; i++)
		{
			printf("\n0.%d-------------------------",i);
			field = mysql_fetch_field_direct(result,i);
			cout << field->name << "\t\t";
		}

		//mysql_free_result(result);
		MYSQL_ROW row = NULL;
		int k = 0;
		
		while(row = mysql_fetch_row(result))
		{			
			printf("\n0.1 %d-------------------------",k);
			k++;
			
			for(int j=1; j<fieldcount; j++)
			{
				cout << row[j] << "\t\t";
			}
			cout << endl;
			//row = mysql_fetch_row(result);
		}
		/**/
		mysql_free_result(result);
	//}

	printf("\n1-------------------------");

	ASRAnalyser * pASRAna = new ASRAnalyser();

	printf("\n1.1-------------------------");


	pASRAna->CreateDir("eSpace/asr/");
#if 1
	// test	
	printf("\n2-------------------------");
	for(int i = 0; i < 100; i++)
	{
		ASR_EVENT Event;
		ASR_EVENT * pEvent = &Event;
		pEvent->eType = EVENT_TYPE_STAT_REP; //EVENT_TYPE_ASR_INSERT;
		printf("\n2 1-------------------------");
		strcpy(pEvent->u_data.statrep.StartTime, datetime('-')); 		//StartTime time, 	now();sysdate();'2000:01:31 23:59:59'
		printf("\n2 2-------------------------");
		pEvent->u_data.statrep.RegUserNum = random() % 200; 		//RegUserNum int, 
		pEvent->u_data.statrep.ActiveUserNum = random() % 100; 	//ActiveUserNum int, 
		pEvent->u_data.statrep.ASRRate = random() % 50; 		//ASRRate float, 
		pEvent->u_data.statrep.TraditionalActNum = random() % 100; //TraditionalActNum int,
		pEvent->u_data.statrep.ASRNum = random() % 100;			//ASRNum int, 
		pEvent->u_data.statrep.ASRFailNum = random() % 100;		//ASRFailNum int, 			
		for(int j = 0; j < MAX_AUDIO_CMD_NUM; j++)
		{
			printf("\n2 3-------------------------");
			pEvent->u_data.statrep.CmdStat[j].CmdSuccessCount = random() % 300;
			pEvent->u_data.statrep.CmdStat[j].CmdFailureCount = random() % 300;
		}

		printf("\n2 4-------------------------");
		

printf("\n2.1 %d-------------------------", i);

		pASRAna->ASREventRecord(pEvent);
	}
#if 1
	// test	
	printf("\n3-------------------------");
	for(int i = 0; i < 200; i++)
	{
		ASR_EVENT Event;
		ASR_EVENT * pEvent = &Event;
		pEvent->u_data.act.uuid = random();
		pEvent->u_data.act.uuid *= pEvent->u_data.act.uuid;
		strcpy(pEvent->u_data.act.StartTime, datetime('-')); 		//StartTime time, 	now();sysdate();'2000:01:31 23:59:59'
		sprintf(pEvent->u_data.act.UserName, "wg_%d", random()% 100);		// UserName varchar(10) NOT NULL, 
		sprintf(pEvent->u_data.act.AudioFilePathName, "123.wav"); 	// ASRDatetime datetime NOT NULL, 
		sprintf(pEvent->u_data.act.ASRResultStr, "拉王耕"); //la wang geng"); 		// ASRResult varchar(30), 
		pEvent->u_data.act.ASRE2EDurationInMs = random()%100, // ASRDuration int, 
		pEvent->u_data.act.ASRE2EDurationInMs = random()%100, // ASRE2EDuration int, 
		pEvent->u_data.act.pAudioData = (char*)pEvent;
		pEvent->u_data.act.AudioDataLen = sizeof(Event);
		
		pEvent->eType = EVENT_TYPE_ASR_INSERT;
		pASRAna->ASREventRecord(pEvent);
	}
#endif
	printf("\ne-------------------------");


	delete pASRAna;

#endif
#endif


	mysql_close(&mysql);

	printf("\n");
	
}
#endif

/////////////////////////// 事件队列管理 //////////////////////////
//
pthread_mutex_t mutex;

// 用缓冲队列管理事件, 避免在时延上影响应用层
#define MAX_EVENT_COUNT 1000
typedef struct S_ASR_EVENT_QUEUE
{
	int MsgMaxCount;
	int MsgActCount;
	int Hdr;
	char DBName[300];
		
	ASR_EVENT Event[MAX_EVENT_COUNT];
	time_t EventT[MAX_EVENT_COUNT];
	
} ASR_EVENT_QUEUE;
ASR_EVENT_QUEUE gASREventQue = {0};

// 处理每一个事件
int ASREventRecordStatRepPorccess(char *DBName, ASR_EVENT * pEvent, time_t now);
// 事件进队
int ASR_EVENT_QUEUE_In(ASR_EVENT_QUEUE *pASREventQue, char *DBName, ASR_EVENT * pEvent, time_t tnow);
// 事件出队
ASR_EVENT * ASR_EVENT_QUEUE_Out(ASR_EVENT_QUEUE *pASREventQue, time_t *tnow);


// 进队
int ASR_EVENT_QUEUE_In(ASR_EVENT_QUEUE *pASREventQue, char *DBName, ASR_EVENT * pEvent, time_t tnow)
{
	if (pASREventQue->MsgActCount >= MAX_EVENT_COUNT)
	{
		printf("\nASR_EVENT_QUEUE_In(), Fatal error, ASR record Event queue full. Event lost.");

		//pthread_mutex_unlock(&mutex);

		return -1;
	}

	if (DBName[0] && pASREventQue->DBName[0] == 0)
	{
		strcpy(pASREventQue->DBName, DBName);
	}

	if (tnow == 0)
	{
		printf("\nASR_EVENT_QUEUE_In(), error, now is 0, Event:%d.", pEvent->eType);
		
		//exit(0); // test
		
		return 0;
	}
	
	int tail = (pASREventQue->Hdr + pASREventQue->MsgActCount) % MAX_EVENT_COUNT;
	pASREventQue->Event[tail] = pEvent[0];
	pASREventQue->EventT[tail] = tnow;
	pASREventQue->MsgActCount++;

	return 1;
}

// 出队, 输出 ASR_EVENT 和 tnow
ASR_EVENT * ASR_EVENT_QUEUE_Out(ASR_EVENT_QUEUE *pASREventQue, time_t *tnow)
{
	if (pASREventQue->MsgActCount <= 0)
	{
		return NULL;
	}

	ASR_EVENT * pOut = pASREventQue->Event + pASREventQue->Hdr;
	tnow[0] = pASREventQue->EventT[pASREventQue->Hdr];
	pASREventQue->Hdr = (pASREventQue->Hdr + 1) % MAX_EVENT_COUNT;
	pASREventQue->MsgActCount--;

	return pOut;
}


int ASREventRecordStatRepPorccess(char *DBName, ASR_EVENT * pEvent, time_t now)
{
	int r = 0;
    //time_t now;
    struct tm *tm_now;

	// tbd
	// 优化点: 为了不影响应用层的速度, 可以在收到消息后先记录并立即返回, 再"慢慢"处理。

#if 1
	// 数据库初始化
	if (!pASRAnalyser)
	{
		printf("\n----------------------> pASRAnalyser Not Start. <-------------------.");

		return -1;
		/*
		pthread_mutex_init(&mutex,NULL);
		
		pASRAnalyser = new ASRAnalyser(DBName);
		if (!pASRAnalyser)
		{
			printf("\nCreate ASRAnalyser fail.");

			return -1;
		}

		pASRAnalyser->InitDB();
		*/
	}
#endif

	if (pEvent->eType != EVENT_TYPE_KEEP_ALIVE)
	{
		//printf("\nASREventRecordStatRep(), new event:%d.", pEvent->eType);
	}
	// 取当前时间
	//strcpy(pASRAnalyser->mCurDate, datetime('-'));
    //time(&now);
    if (now == 0 || now == -1)
    {
		printf("\nASREventRecordStatRep(), time error, now = %d", now);

		//exit(0);
		//getchar(); // test
		
		return -1;
    }
    
	//pthread_mutex_lock(&mutex);
	

	pASRAnalyser->mCurDateTM = now;
    tm_now = localtime(&now); 
	pASRAnalyser->mCurDate = tm_now->tm_mday;
	pASRAnalyser->mCurHour = tm_now->tm_hour;
	pASRAnalyser->mCurMin = tm_now->tm_min;
	pASRAnalyser->mCurSec = tm_now->tm_sec;

	// 时间分片统计处理
	pASRAnalyser->OnDayChanged();
	pASRAnalyser->OnTimeSegChanged();

	pASRAnalyser->mLastDateTM = pASRAnalyser->mCurDateTM;
	pASRAnalyser->mLastDate = tm_now->tm_mday;
	pASRAnalyser->mLastHour = tm_now->tm_hour;
	pASRAnalyser->mLastMin = tm_now->tm_min;
	pASRAnalyser->mLastSec = tm_now->tm_sec;
	
	// 事件处理
	switch(pEvent->eType)
	{
	case EVENT_TYPE_ASR_INSERT:
		r = pASRAnalyser->AsrInsert(&pEvent->u_data.AsrInsert);
		break;
	case EVENT_TYPE_ASR_UPDATE:
		r = pASRAnalyser->AsrUpdate(&pEvent->u_data.AsrUpdate);
		break;
	case EVENT_TYPE_USR_HABIT:
		r = pASRAnalyser->StatHabit(&pEvent->u_data.Habit);
		break;
	case EVENT_TYPE_KEEP_ALIVE:
		//r = pASRAnalyser->StatHabit(&pEvent->u_data.Habit);
		break;
	default:
		printf("\nASREventRecordStatRep no act, eType = %d.", pEvent->eType);
		break;
	}

	//pthread_mutex_unlock(&mutex);

	if (pEvent->eType != EVENT_TYPE_KEEP_ALIVE)
	{
		//printf("\nASREventRecordStatRep(), new event:%d. processed.", pEvent->eType);
	}
	
	return r;
}
//
/////////////////////////// 事件队列管理 //////////////////////////

// 事件队列统计
int EventQueueStat()
{
	char buf[1000];
	char buf1[1000];
	static long tick = 0;
	static unsigned int LastEventInCount = 0;	// 接收事件总数
	static unsigned int LastEventProcessCount = 0;	// 处理事件总数

	if (tick == 0)
	{
		tick = get_time_ms();
		LastEventInCount = pASRAnalyser->mEventInCount;
		LastEventProcessCount = pASRAnalyser->mEventProcessCount;
	}

	int curtick = get_time_ms();

	long tickdiff = curtick - tick;

	if (tickdiff == 0)
	{
		pASRAnalyser->mEventInSpeed = 0;
		pASRAnalyser->mEventPrcessSpeed = 0;
	}
	else
	{
		pASRAnalyser->mEventInSpeed = (pASRAnalyser->mEventInCount - LastEventInCount) * 1000 / tickdiff;
		pASRAnalyser->mEventPrcessSpeed = (pASRAnalyser->mEventProcessCount - LastEventProcessCount) * 1000 / tickdiff;
	}
	
	// 打印接收的各类事件数量
	{
		sprintf(buf, "\nEvent rcved(In Speed:%.1f, Prs Speed:%.1f, Qued:%d): ", 
			pASRAnalyser->mEventInSpeed,
			pASRAnalyser->mEventPrcessSpeed,
			gASREventQue.MsgActCount);
		
		for(int i = 0; i < EVENT_TYPE_BUTT; i++)
		{
			sprintf(buf1, "E_%d:%d, ", i, pASRAnalyser->mASREventRcvStat[i]);
			strcat(buf, buf1);
		}
		printf(buf);
	}

	tick = curtick;
	LastEventInCount = pASRAnalyser->mEventInCount;
	LastEventProcessCount = pASRAnalyser->mEventProcessCount;
	
	return 1;
}

#if 0
// 仿真ASR操作
int SimulateASR()
{
	time_t now;
    time(&now);

	char SessionId[100];

	struct tm *tm_beginOfThisDay;
	tm_beginOfThisDay = localtime(&now); 
	//tm_beginOfThisDay->tm_mday;
	tm_beginOfThisDay->tm_hour = 0;
	tm_beginOfThisDay->tm_min = 0;
	tm_beginOfThisDay->tm_sec = 0;
	time_t StartTMThisDay = mktime(tm_beginOfThisDay); 

	struct tm *tm_endOfThisDay;
	tm_endOfThisDay = localtime(&now); 
	//tm_beginOfThisDay->tm_mday;
	tm_endOfThisDay->tm_hour = 23;
	tm_endOfThisDay->tm_min = 59;
	tm_endOfThisDay->tm_sec = 59;
	time_t EndTMThisDay = mktime(tm_endOfThisDay); 

	// tk当前时刻在一天中的位置(0~1)
	float tk = ((float)now - StartTMThisDay) / (EndTMThisDay - StartTMThisDay);

	// 单位时间ASR发起次数, 二次曲线, 在一天的中午最高发起10次
	int calltimes = 10 - 40 * (tk - 0.5) * (tk - 0.5);

	calltimes *= ((float)(random() % 10) / 100) + 1;

	printf("\nSimulateASR() ------------------------- tk:%f, %d ASR added.", tk, calltimes);

	ASR_EVENT Event;
	ASR_EVENT *pEvent = &Event;
	Event.eType = EVENT_TYPE_ASR_INSERT;
	for(int i = 0; i < calltimes; i++)
	{
		pEvent->eType = EVENT_TYPE_ASR_INSERT;
		strcpy(pEvent->u_data.AsrInsert.Version, "000001");
		strcpy(pEvent->u_data.AsrInsert.StartTime, datetime('-'));
		sprintf(pEvent->u_data.AsrInsert.SessionId, "%d", random()%100000);
		strcpy(SessionId, pEvent->u_data.AsrInsert.SessionId);
		sprintf(pEvent->u_data.AsrInsert.UserName, "w00%d", random()%100);
		sprintf(pEvent->u_data.AsrInsert.ASRResultStr, "一二三%d", random()%1000);
		sprintf(pEvent->u_data.AsrInsert.AudioFilePathName, "d:\audiofile\%d", random()%200000);
		pEvent->u_data.AsrInsert.ASRE2EDurationInMs = 0;
		pEvent->u_data.AsrInsert.ASRRecongitioanDurationInMs = 100 + (random()%100);
		ASREventRecordStatRep(ASR_DB_NAME_TEST, pEvent);
		
		pEvent->eType = EVENT_TYPE_ASR_UPDATE;
		strcpy(pEvent->u_data.AsrUpdate.SessionId, SessionId);
		pEvent->u_data.AsrUpdate.ASRE2EDurationInMs = 200 + (random()%100);
		ASREventRecordStatRep(ASR_DB_NAME_TEST, pEvent);

		pEvent->eType = EVENT_TYPE_USR_HABIT;
		sprintf(pEvent->u_data.Habit.UserName, "w00%d", random()%100);
		strcpy(pEvent->u_data.Habit.StartTime, datetime('-'));
		strcpy(pEvent->u_data.Habit.ActionDesc, pASRAnalyser->mASRCmdSet[random() % pASRAnalyser->mASRCmdSetSize].CmdStr);
		ASREventRecordStatRep(ASR_DB_NAME_TEST, pEvent);

	}
	
	return 1;
}
#endif


int UDPSend(char * data, int datalen)
{
	int s;                                     /*套接字文件描述符*/
	struct sockaddr_in to;                     /*接收方的地址信息*/
	int n;                                     /*发送到的数据长度*/
	//char buff[100] = {0};                             /*发送数据缓冲区*/
	s = socket(AF_INET, SOCK_DGRAM, 0); /*初始化一个IPv4族的数据报套接字*/
	if (s == -1) {                             /*检查是否正常初始化socket*/
		printf("\ncreate udp socket failed");
		//WriteLog(DSN_LOG_ERROR,"create udp socket failed \r\n");
		//exit(0);
	}

	to.sin_family = AF_INET;                   /*协议族*/
	to.sin_port = htons(ASR_EYE_BIND_PORT);                 /*本地端口*/
	//to.sin_addr.s_addr = inet_addr("192.168.1.1");
	to.sin_addr.s_addr = inet_addr("127.0.0.1");     //(INADDR_ANY);


	//memcpy(buff,ClientName,10);							   
	n = sendto(s, data, datalen, 0, (struct sockaddr*)&to, sizeof (to));
	/*将数据buff发送到主机to上*/
	if(n == -1){                       /*发送数据出错*/
		perror("sendto");
		//WriteLog(DSN_LOG_ERROR,"udp sendto self fail");
		//exit(EXIT_FAILURE);
	}else if(n >= datalen)
	{
		//printf("\nthe data send ok");
		//WriteLog(DSN_LOG_ERROR,"the data send ok \r\n");
	}
	close(s);

	return 1;
}


void * StatThread(void * arg);
void * StatThread(void * arg)  // 不实质操作数据库
{  
    //printf( "\nThis is a thread and arg = %d.\n", *(int*)arg);  
    //ASRAnalyser pASR = (ASRAnalyser*)arg = 0;
    printf("\nStatThread is runing.");
	//char buf[1000];
	//char buf1[1000];

	unsigned int counter = 0;

	while(gStatThreadRuning)
	{
		//printf("\nStatThread is runing, %u", gStatThreadRuningBeats++);

		sleep(1);	// sec
		//usleep(1);	// usec

		//if (counter % 1 == 0)
		{
			// test
			// 仿真ASR操作
//#ifdef THIS_IS_TEST_VER
//			SimulateASR();
//#endif
			if (counter % 5 == 0)
			{
				ASR_EVENT Event;
				Event.eType = EVENT_TYPE_KEEP_ALIVE;
				//ASREventRecordStatRep("DB_NAME_XXX", &Event);	// tbd
				
				// 生成html
				//pASRAnalyser->ASREventGenHtml();

				// 打印接收的各类事件数量
				EventQueueStat();
			}
		}

		counter++;
	}

	printf("\nStatThread is over.");
	
    return 0;  
}

void printfmem(unsigned char * b, int len)
{
	if (!b)
	{
		return;
	}

	//puts("\n");
	for(int i = 0; i < len; i++)
	{
		if (i % 16 == 0) puts("\n");
			
		printf("0x%d%d ", b[i] / 16, b[i] % 16);		
	}
}

int ASREventRecordStatRep_Internel(char *DBName, ASR_EVENT * pEvent);

CUdpPk * pUdpPk = NULL; // new CUdpPk();

int CUdpPk::GetTableRowCount(char * TableName, char *where_condition)
{
	//char TableName[100];
	char buf[1000];
	char buf1[1000];
	
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;

	int RowCount = 0;
	
	//sprintf(TableName, "DateTbl");	
	if (where_condition)
	{
		sprintf(buf, "select count(*) from %s %s", TableName, where_condition);
	}
	else
	{
		sprintf(buf, "select count(*) from %s", TableName);
	}
	
	//puts(buf); // test
	string sql2(buf);
	//cout << sql2 << endl;
	mysql_query(&mysql, sql2.c_str());

	//string sql = "select id,name from t1;";
    //mysql_query(&mysql, sql.c_str());
    result = mysql_store_result(&mysql);
    int rowcount = mysql_num_rows(result);
    //cout << rowcount << endl;
		//mysql_free_result(result);
	int fieldcount = mysql_num_fields(result);
    //cout << fieldcount << endl;
		//mysql_free_result(result);

#if 0	
	for(int i = 0; i < fieldcount; i++)
	{
		printf("\n0.%d-------------------------",i);
		field = mysql_fetch_field_direct(result,i);
		cout << field->name << "\t\t";
	}
#endif

	//mysql_free_result(result);
	MYSQL_ROW row = NULL;
	int k = 0;	

	//printf("\nffffffffffffffff: fieldcount:%d.", fieldcount);

	while(row = mysql_fetch_row(result))
	{
		//printf("\n0.1 %d-------------------------",k);
		k++;
		
		for(int j=0; j<fieldcount; j++)
		{
			//printf("\nffffffffffffffff");
			//cout << row[j] << "\t\t";
			//printf("\neeeeeeeeeeeeeeee");

			RowCount = atoi(row[j]);
			
			break;	///////////////
		}
		//cout << endl;
		//row = mysql_fetch_row(result);

		break;	/////////////////
	}
	
	/**/
	mysql_free_result(result);

	return RowCount;

}

int CUdpPk::GetUserIdFromTbl(char * UserName)
{
	//char TableName[100];
	char buf[1000];
	char buf1[1000];

	int userid = -1;
	
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;

	int RowCount = 0;
	
	sprintf(buf, "select UserId from %s where UserName = '%s'", "EpRegUsrTbl", UserName);	
	
	//puts(buf); // test
	string sql2(buf);
	//cout << sql2 << endl;
	mysql_query(&mysql, sql2.c_str());

	//string sql = "select id,name from t1;";
    //mysql_query(&mysql, sql.c_str());
    result = mysql_store_result(&mysql);
    int rowcount = mysql_num_rows(result);
    //cout << rowcount << endl;
		//mysql_free_result(result);
	int fieldcount = mysql_num_fields(result);
    //cout << fieldcount << endl;
		//mysql_free_result(result);

#if 0	
	for(int i = 0; i < fieldcount; i++)
	{
		printf("\n0.%d-------------------------",i);
		field = mysql_fetch_field_direct(result,i);
		cout << field->name << "\t\t";
	}
#endif

	//mysql_free_result(result);
	MYSQL_ROW row = NULL;
	int k = 0;	

	//printf("\nffffffffffffffff: fieldcount:%d.", fieldcount);

	while(row = mysql_fetch_row(result))
	{
		//printf("\n0.1 %d-------------------------",k);
		k++;
		
		for(int j=0; j<fieldcount; j++)
		{
			//printf("\nffffffffffffffff");
			//cout << row[j] << "\t\t";
			//printf("\neeeeeeeeeeeeeeee");

			userid = atoi(row[j]);
			
			break;	///////////////
		}
		//cout << endl;
		//row = mysql_fetch_row(result);

		break;	/////////////////
	}
	
	/**/
	mysql_free_result(result);

	return userid;

}

char * CUdpPk::GetUserNameFromTbl(int userid)
{
	//char TableName[100];
	char buf[1000];
	char buf1[1000];

	char username[100] = "";
	
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;

	int RowCount = 0;
	
	sprintf(buf, "select UserName from %s where UserId = %d", "EpRegUsrTbl", userid);	
	
	//puts(buf); // test
	string sql2(buf);
	//cout << sql2 << endl;
	mysql_query(&mysql, sql2.c_str());

	//string sql = "select id,name from t1;";
    //mysql_query(&mysql, sql.c_str());
    result = mysql_store_result(&mysql);
    int rowcount = mysql_num_rows(result);
    //cout << rowcount << endl;
		//mysql_free_result(result);
	int fieldcount = mysql_num_fields(result);
    //cout << fieldcount << endl;
		//mysql_free_result(result);

#if 0	
	for(int i = 0; i < fieldcount; i++)
	{
		printf("\n0.%d-------------------------",i);
		field = mysql_fetch_field_direct(result,i);
		cout << field->name << "\t\t";
	}
#endif

	//mysql_free_result(result);
	MYSQL_ROW row = NULL;
	int k = 0;	

	//printf("\nffffffffffffffff: fieldcount:%d.", fieldcount);

	while(row = mysql_fetch_row(result))
	{
		//printf("\n0.1 %d-------------------------",k);
		k++;
		
		for(int j=0; j<fieldcount; j++)
		{
			//printf("\nffffffffffffffff");
			//cout << row[j] << "\t\t";
			//printf("\neeeeeeeeeeeeeeee");

			strcpy(username, row[j]);
			
			break;	///////////////
		}
		//cout << endl;
		//row = mysql_fetch_row(result);

		break;	/////////////////
	}
	
	/**/
	mysql_free_result(result);

	return username;

}


int CUdpPk::RegMsgHandler(byte * uuid, int i, byte * Udpdata, int udplen, char * PeerPubIp, int PeerPubPort, struct sockaddr_in * from)
{
	int dataleft = udplen - i;
	int Tag = 0;
	int L = 0; // 含 T L V总长
	
	printf("\nRegMsgHandler() called.");
	
	while(dataleft > 4)
	{
		Tag = getushort(Udpdata, i);
		i += 2;
		
		L = getushort(Udpdata, i);
		i += 2;
		if (L <= 0 || L > dataleft)
		{
			//Log.i(TAG, "rcv udp pks, bad L: " + L + ", Tag: " + Tag);
			printf("\nrcv udp pks, bad L: %d, Tag:%d.", L, Tag);
			break;
		}
		
		// V

		if (Tag == 0)
		{
			//GetItemBA(Udpdata, i, L, tick);
		}
		else
		{
			printf("\nrcv udp pks, bad T: %d.", Tag);
			break;
		}

		/*
		if (Tag == (int)UDP_ITEM.UDP_ITEM_BA.ordinal())
		{
			GetItemBA(Udpdata, i, L, tick);
		}
		else
		{
			//Log.i(TAG, "rcv udp pks, bad T: " + Tag);
			break;
		}
		*/

		{
			//int USRIDLEN = 20;
			//int USRNAMELEN = 20;
			//int PWDLEN = 8;
			//int IPLEN = 16;
			//int PHONEMODLEN = 10;
			//int MACLEN = 17;			// xx:xx:xx:xx:xx:xx

			int SessionId = 0;
			char * usrid;
			char * usrname;
			char * pwd;
			char * localIP;
			int localPort = 0;
			char * PublicIP;
			int PublicPort = 0;
			char * PhoneModel;
			char * MAC;
			int NatType = 0;
			int keepalive = 0;
			int RegAcked = 0;
			REG_USER_INSERT AsrInsert;
			

			char * d = (char *)Udpdata;
			
			SessionId = getint(Udpdata, i);
			i += 4;

			usrid = d + i;
			i += USRIDLEN;

			usrname = d + i;
			i += USRNAMELEN;

			pwd = d + i;
			i += PWDLEN;

			localIP = d + i;
			i += IPLEN;

			localPort = getushort(Udpdata, i);
			i += 2;
			
			PhoneModel = d + i;
			i += PHONEMODLEN;

			MAC = d + i;
			i += MACLEN;

			keepalive = Udpdata[i];
			i++;

			NatType = getushort(Udpdata, i);

			printf("\nRegMsgHandler(), SessionId:%d, usrid:%s, usrname:%s, pwd:%s,lIP:%s,lport:%d, PhoneModel:%s, NatType:%d.",
				SessionId,
				usrid,
				usrname,
				pwd,				
				localIP,
				localPort,
				PhoneModel,
				NatType
				);

			//sprintf(TableName, "EpRegUsrTbl");
			//sprintf(buf, "create table %s (UserName varchar(20) NOT NULL, Pwd varchar(20) NOT NULL, LocalIp varchar(16), LocalPort int, PhoneModel varchar(20), Mac varchar(30), DateTime datetime NOT NULL, ActionNum int, NatType int, PRIMARY KEY (UserName))", TableName);

			AsrInsert.SessionId = SessionId;
			strcpy(AsrInsert.usrname, usrname); 
			strcpy(AsrInsert.pwd, pwd); 
			strcpy(AsrInsert.localIP, localIP); 
			AsrInsert.localPort = localPort;
			strcpy(AsrInsert.PubIP, PeerPubIp); 
			AsrInsert.PubPort = PeerPubPort;
			strcpy(AsrInsert.PhoneModel, PhoneModel); 
			strcpy(AsrInsert.MAC, MAC); 
			AsrInsert.NatType = NatType;
				
			RegUsrInsert(uuid, &AsrInsert, from);

		}
		
		
		i += L - 4;	// T L V

		dataleft -= L;	// T L V
	}		

	return 1;
}
	
int CUdpPk::GetFriendMsgHandler(byte * uuid, int i, byte * Udpdata, int udplen, char * PeerPubIp, int PeerPubPort, struct sockaddr_in * from)
{
	int dataleft = udplen - i;
	int Tag = 0;
	int L = 0; // 含 T L V总长
	
	printf("\nGetFriendMsgHandler() called, uuid:%s, dataleft:%d.", uuid, dataleft);
	
	while(dataleft > 4)
	{
		Tag = getushort(Udpdata, i);
		i += 2;
		
		L = getushort(Udpdata, i);
		i += 2;
		if (L <= 0 || L > dataleft)
		{
			//Log.i(TAG, "rcv udp pks, bad L: " + L + ", Tag: " + Tag);
			printf("\nrcv udp pks, bad L: %d, Tag:%d.", L, Tag);
			break;
		}
		
		// V

		if (Tag == 0)
		{
			//GetItemBA(Udpdata, i, L, tick);
		}
		else
		{
			printf("\nrcv udp pks, bad T: %d.", Tag);
			break;
		}

		{
			//int USRIDLEN = 20;
			//int USRNAMELEN = 20;
			//int PWDLEN = 8;
			//int IPLEN = 16;
			//int PHONEMODLEN = 10;
			//int MACLEN = 17;			// xx:xx:xx:xx:xx:xx

			int SessionId = 0;
			int usrid;
			char * usrname;
			int RegAcked = 0;
			GET_USER_FRIEND AsrInsert;

			char * d = (char *)Udpdata;
			
			SessionId = getint(Udpdata, i);
			i += 4;

			//usrid = d + i;
			//i += USRIDLEN;
			usrid = getint(Udpdata, i);
			i += 4;

			usrname = d + i;
			i += USRNAMELEN;

			printf("\nGetFriendMsgHandler(), SessionId:%d, usrid:%d, usrname:%s.",
				SessionId,
				usrid,
				usrname
				);

			//sprintf(TableName, "EpRegUsrTbl");
			//sprintf(buf, "create table %s (UserName varchar(20) NOT NULL, Pwd varchar(20) NOT NULL, LocalIp varchar(16), LocalPort int, PhoneModel varchar(20), Mac varchar(30), DateTime datetime NOT NULL, ActionNum int, NatType int, PRIMARY KEY (UserName))", TableName);

			AsrInsert.SessionId = SessionId;

			printf("\nGetFriendMsgHandler(), 1");
				
			AsrInsert.usrid = (long)usrid;

			printf("\nGetFriendMsgHandler(), 2");
			
			strcpy(AsrInsert.usrname, usrname); 
			//strcpy(AsrInsert.PubIP, PeerPubIp); 
			//AsrInsert.PubPort = PeerPubPort;

			printf("\nGetFriendMsgHandler(), 3");
			
			GetUserFriends(uuid, &AsrInsert, from);

			printf("\nGetFriendMsgHandler(), 4");
		}
		
		
		i += L - 4;	// T L V

		dataleft -= L;	// T L V
	}		

	return 1;
}

int CUdpPk::Send(int s, void *to, char * data, int datalen)
{
	struct sockaddr * too = (struct sockaddr * )to;

	unsigned long Tick = get_time_ms();

	// 打时间戳
	SetUdpTick(Tick);// & 0xFFFF

	// 计算包校验
	CRCUdpPkt();

	// test
	//printfmem(udppk, udppkLen);
	
	int n = sendto(s, udppk, udppkLen, 0, too, sizeof(too[0]));	// data, datalen
	/*将数据buff发送到主机to上*/
	if(n == -1){                       /*发送数据出错*/
		perror("sendto");
		//WriteLog(DSN_LOG_ERROR,"udp sendto self fail");
		//exit(EXIT_FAILURE);
	}else if(n >= datalen)
	{
		struct sockaddr_in * t = (struct sockaddr_in *)too;
		char * PeerPubIp = inet_ntoa(t->sin_addr);
		int PeerPubPort = ntohs(t->sin_port);

		printf("\n------the data send ok, PeerPubIp:%s, PeerPubPort:%d.", PeerPubIp, PeerPubPort);
		//WriteLog(DSN_LOG_ERROR,"the data send ok \r\n");
	}

	return n;
}

int CUdpPk::RegUsrInsert(byte * uuid, REG_USER_INSERT *AsrInsert, struct sockaddr_in *from)
{
	// 记录流水
	// save audio file
	// update day rec
	// update week rec
	// update month rec

	//char FileName[300];
	char buf[1000];
	char buf1[1000];
	//FILE * pf;
	char TableName[100];

	//MYSQL mysql;
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;

/*
	if (AsrInsert->Version[0] == 0)
	{
		printf("\nAsrInsert fail, Version is null.");
		
		return -1;
	}

	if (AsrInsert->SessionId[0] == 0)
	{
		printf("\nAsrInsert fail, SessionId is null.");
		
		return -1;
	}	

	if (AsrInsert->UserName[0] == 0)
	{
		printf("\nAsrInsert fail, UserName is null.");
		
		return -1;
	}
*/
	// check passwd
	//GetTableName("AllUserHabit", GetCurDateString('_'), TableName);	
	char wherex[200];
	// 用户是否注册检测
	{
		sprintf(wherex, "where UserName = '%s'", 
			AsrInsert->usrname);
		int c = GetTableRowCount("EpRegUsrTbl", wherex);
		if (c == 0)
		{
			// 发送未注册反馈

			FromUdpPkt(MSG_TYPE_REG_ACK, uuid);

			// 0: success, 1: unreged user, 2: bad pwd
			int ret = 1;
			AppendRegMsgAck2UdpPkt(ret, AsrInsert->SessionId, AsrInsert->usrid);

			Send(mSock, from, (char *)udppk, udppkLen);

			printf("\nUnreged usr, usr:%s", AsrInsert->usrname);

			return 0;
		}
	}
	// 用户和密码检测
	{
		sprintf(wherex, "where UserName = '%s' and Pwd = '%s'", 
			AsrInsert->usrname,
			AsrInsert->pwd);
		int c = GetTableRowCount("EpRegUsrTbl", wherex);
		if (c == 0)
		{
			// 发送密码错反馈

			FromUdpPkt(MSG_TYPE_REG_ACK, uuid);

			// 0: success, 1: unreged user, 2: bad pwd
			int ret = 2;
			AppendRegMsgAck2UdpPkt(ret, AsrInsert->SessionId, AsrInsert->usrid);

			Send(mSock, from, (char *)udppk, udppkLen);

			printf("\nReg pwd fail, usr:%s, pwd:%s", AsrInsert->usrname, AsrInsert->pwd);

			return 0;
		}
	}


	// 返回成功
	{
		// 发送注册反馈

		FromUdpPkt(MSG_TYPE_REG_ACK, uuid);

		AsrInsert->usrid = GetUserIdFromTbl(AsrInsert->usrname);

		// 0: success, 1: unreged user, 2: bad pwd
		int ret = 0;
		AppendRegMsgAck2UdpPkt(ret, AsrInsert->SessionId, AsrInsert->usrid);

		printf("\nReged usr, usr:%s, id:%d.", AsrInsert->usrname, AsrInsert->usrid);

		Send(mSock, from, (char *)udppk, udppkLen);		
	}


	// tablename
	//GetTableName("EpRegUsrTbl", GetCurDateString('_'), TableName);

	// 记录流水
	//printf("\nAsrInsert 1-------------------");
	{
#if 0
		// 转移到OnDayChanged()处理
		{
			// 所有用户的识别记录: 用户唯一标识, ASR操作时间, 原始录音文件, 降噪录音文件, 识别结果, 识别耗时(ms)，是否人工确认
			// CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
			// ALTER TABLE Persons
			// ADD PRIMARY KEY (Id_P)
			// ALTER TABLE Persons
			// ADD CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
			sprintf(buf, "create table %s (ASRDatetime datetime NOT NULL, SessionId varchar(33) NOT NULL, UserName varchar(10) NOT NULL, ASRResultStr varchar(30), AudioFilePathName varchar(100), ASRE2EDurationInMs int, ASRRecongitioanDurationInMs int, Verified int)", 
				TableName);
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
			puts(buf);
			//printf("\nCreate DB AllUserASRActTbl.");
			//mysql_free_result(result);
		}
#endif


		//char utfbuf[300];
		//char gbbuf_ASRResultStr[300];
		//CodeConvert("utf-8", "gb2312", AsrInsert->ASRResultStr, gbbuf_ASRResultStr);


//create table EpRegUsrTbl (UserName varchar(20) NOT NULL, Pwd varchar(20) NOT NULL, LocalIp varchar(16), LocalPort int, PhoneModel varchar(20), Mac varchar(30), DateTime datetime NOT NULL, ActionNum int, NatType int, PRIMARY KEY (UserName));

		// insert into t1 (id, name) values (2, 'java2');
		sprintf(buf1, " values ('%s', '%s', '%s', %d, '%s', %d, '%s', '%s', '%s', %d, %d);",
			AsrInsert->usrname,
			AsrInsert->pwd,
			AsrInsert->localIP,
			AsrInsert->localPort,
			AsrInsert->PubIP,
			AsrInsert->PubPort,
			AsrInsert->PhoneModel,
			AsrInsert->MAC,
			datetime('-'),// ASRDatetime datetime NOT NULL, 
			0,
			AsrInsert->NatType
			);

#if 0
		// 写入按天的表
		{
			sprintf(buf, "insert into %s %s", TableName, buf1);
			puts("\n");
			puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());
		}
#endif

		// 写入全局表
		{
			//long tick0 = get_time_ms();
				
			sprintf(buf, "insert into %s %s", "EpRegUsrTbl", buf1);
			//puts("\n");
			//puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());

			//printf("\nttt------------------------ inert into AllUserASRActTbl, ticks = %d.", get_time_ms() - tick0);
		}
		// 更新全局表
		{
			//long tick0 = get_time_ms();

//create table %s (UserName varchar(20) NOT NULL, Pwd varchar(20) NOT NULL, LocalIp varchar(16), LocalPort int, PubIp varchar(16), PubPort int, PhoneModel varchar(20), Mac varchar(30), DateTime datetime NOT NULL, ActionNum int, NatType int, PRIMARY KEY (UserName))";
			sprintf(buf1, " set LocalIp='%s', LocalPort=%d, PubIp='%s', PubPort=%d,PhoneModel='%s',Mac='%s', DateTime='%s', ActionNum=%d,NatType=%d where UserName='%s' and Pwd='%s';",
				AsrInsert->localIP,
				AsrInsert->localPort,
				AsrInsert->PubIP,
				AsrInsert->PubPort,
				AsrInsert->PhoneModel,
				AsrInsert->MAC,
				datetime('-'),// ASRDatetime datetime NOT NULL, 
				0,
				AsrInsert->NatType,
				AsrInsert->usrname,
				AsrInsert->pwd
				);
			sprintf(buf, "update %s %s", "EpRegUsrTbl", buf1);

			//puts("\n");
			//puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());

			//printf("\nttt------------------------ inert into AllUserASRActTbl, ticks = %d.", get_time_ms() - tick0);
		}

	}

#if 0
	// save audio file
	/*
	printf("AsrInsert 2-------------------");
	if (mASR_REC_CFG.RecRootDir[0])
	{

		sprintf(FileName, "%s%cAudio%c%s_%s_%s.pcm", mASR_REC_CFG.RecRootDir, DIRDASH, DIRDASH,
			pEvent->u_data.act.AudioFilePathName, 
			pEvent->u_data.act.UserName, 
			pEvent->u_data.act.ASRResultStr 
			);
	}
	else
	{
		sprintf(FileName, "Audio%c%s_%s_%s.pcm", DIRDASH,
			pEvent->u_data.act.AudioFilePathName, 
			pEvent->u_data.act.UserName, 
			pEvent->u_data.act.ASRResultStr 
			);
	}
	pf = fopen(FileName, "w+b");
	if (!pf)
	{
		return 0;
	}
	fwrite(pEvent->u_data.act.pAudioData, pEvent->u_data.act.AudioDataLen, 1, pf);
	fclose(pf);
	*/

	// update day rec
	
	// update week rec
	
	// update month rec

	// upade 
	// 活跃用户表: 天级
	{
		//sprintf(TableName, "ActiveUsrTbl");
		GetTableName("ActiveUsrTbl", GetCurDateString('_'), TableName);
		sprintf(buf1, "values ('%s', '%s', 0);",
			AsrInsert->UserName,
			datetime('-')
			);
		sprintf(buf, "insert into %s %s", TableName, buf1);
		//puts(buf); // test
		string sql2(buf);
		//cout << sql2 << endl;
		mysql_query(&mysql, sql2.c_str());
	}

	// upade 
	// 注册用户表:
	{
		{
			sprintf(TableName, "RegUsrTbl");
			sprintf(buf1, "values ('%s', '%s', '%s', 0);",
				AsrInsert->Version,
				AsrInsert->UserName,
				datetime('-')
				);
			sprintf(buf, "insert into %s %s", TableName, buf1);
			//puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());
		}

		// 更新版本号
		{
			sprintf(buf1, " set Version = '%s' where UserName = '%s';",
				AsrInsert->Version,
				AsrInsert->UserName
				);
			sprintf(buf, "update %s %s", TableName, buf1);
			//puts("\n");
			//puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());
		}
	}
#endif

	//printf("\nAsrInsert end-------------------");
	
	return 1;
}

int CUdpPk::GetUserFriends(byte * uuid, GET_USER_FRIEND *AsrInsert, struct sockaddr_in *from)
{
	// 记录流水
	// save audio file
	// update day rec
	// update week rec
	// update month rec
	
	printf("\nGetUserFriends()");

	//char FileName[300];
	char buf[1000];
	char buf1[1000];
	//FILE * pf;
	char TableName[100] = "TSTbl";

	char username[100] = "";

	//MYSQL mysql;
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;


	FromUdpPkt(MSG_TYPE_GET_FRIEND_ACK, uuid);

#if 1
	// check passwd
	//GetTableName("AllUserHabit", GetCurDateString('_'), TableName);	
	char wherex[200];
	{
		//sprintf(wherex, "where UserName = '%s'", 
		//	AsrInsert->usrname);

		{
			//char TableName[100];
			char buf[1000];
			char buf1[1000];
			char * where_condition = wherex;
			
			MYSQL_RES *result = NULL;
		    MYSQL_FIELD *field = NULL;

			int RowCount = 0;
			
			//sprintf(TableName, "DateTbl");	
/*			if (where_condition)
			{
				sprintf(buf, "select * from %s %s", TableName, where_condition);
			}
			else
			{
				sprintf(buf, "select * from %s", TableName);
			}
*/			
			//sprintf(buf, "select * from TSTbl, EpRegUsrTbl where TSTbl.UserId = EpRegUsrTbl.UserId and EpRegUsrTbl.UserName = %s", AsrInsert->usrname);
			//sprintf(buf, "select * from TSTbl, EpRegUsrTbl where TSTbl.TeacherUserId = EpRegUsrTbl.UserId and EpRegUsrTbl.UserId = %d", AsrInsert->usrid);
			sprintf(buf, "select TSTbl.StudentUserId from TSTbl, EpRegUsrTbl where TSTbl.TeacherUserId = EpRegUsrTbl.UserId and EpRegUsrTbl.UserId = %d", AsrInsert->usrid);
			
			//puts(buf); // test
			
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());

			//string sql = "select id,name from t1;";
		    //mysql_query(&mysql, sql.c_str());
		    result = mysql_store_result(&mysql);
		    int rowcount = mysql_num_rows(result);
		    //cout << rowcount << endl;
				//mysql_free_result(result);
			int fieldcount = mysql_num_fields(result);
		    //cout << fieldcount << endl;
				//mysql_free_result(result);

#if 0	
			for(int i = 0; i < fieldcount; i++)
			{
				printf("\n0.%d-------------------------",i);
				field = mysql_fetch_field_direct(result,i);
				cout << field->name << "\t\t";
			}
#endif

			//mysql_free_result(result);
			MYSQL_ROW row = NULL;
			int k = 0;	

			//printf("\nffffffffffffffff: fieldcount:%d.", fieldcount);

printf("\n----------------");
			while(row = mysql_fetch_row(result))
			{
				int j = 0;
				//printf("\n0.1 %d-------------------------",k);
				k++;
				
				for(j=0; j<fieldcount; j++)
				{
					//printf("\nffffffffffffffff");
					//cout << row[j] << "\t\t";
					//printf("\neeeeeeeeeeeeeeee");

					//RowCount = atoi(row[j]);
#if 1

					int userid = (int)atoi(row[j]);
					strcpy(username, GetUserNameFromTbl(userid));
					if (username[0])
					{
						AppendGetFriendsMsgAck2UdpPkt(userid, (char *)username);
					}
#endif
					//
					printf("\nfriend[%d]:%s", j, username);
				}				
				//cout << endl;
				//row = mysql_fetch_row(result);

				//break;	/////////////////
			}

			/**/
			mysql_free_result(result);

			//return RowCount;			
		}

		Send(mSock, from, (char *)udppk, udppkLen);

		//printf("\nUnreged usr, usr:%s", AsrInsert->usrname);
	}
#endif

	return 1;
}

void * UDPThread(void * arg)
{  
    //printf( "\nThis is a thread and arg = %d.\n", *(int*)arg);  
    //ASRAnalyser pASR = (ASRAnalyser*)arg = 0;
    printf("\nUDPThread is runing.");
	//char buf[1000];
	//char buf1[1000];

	unsigned int counter = 0;

	struct sockaddr_in from;                    /*发送方的地址信息*/
	struct sockaddr_in local;                   /*本地的地址信息*/
	int from_len = sizeof(from);                /*地址结构的长度*/
	char smsg[1000] = {0};                              /*接收数据缓冲区*/
	gudpsock = socket(AF_INET, SOCK_DGRAM, 0); /*初始化一个IPv4族的数据报套接字*/
	if (gudpsock == -1) {                              /*检查是否正常初始化socket*/
		perror("create the udp receive socket fail ,exit the model update thread");
		//WriteLog(DSN_LOG_ERROR,"create the udp receive socket fail ,exit the model update thread\n");
	//	exit(EXIT_FAILURE);
		return 0;
	}

	local.sin_family = AF_INET;                     /*协议族*/
	local.sin_port = htons(ASR_EYE_BIND_PORT);                   /*本地端口*/
	local.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1");   //(INADDR_ANY);      /*任意本地地址*/
	if(bind(gudpsock, (struct sockaddr*)&local,sizeof(local))<0 )
	{
		close(gudpsock);
		gudpsock = 0;
		printf("\nudp bind fail,exit the update model thread\n");
		//WriteLog(DSN_LOG_ERROR,"udp bind fail,exit the update model thread\n");
//		exit(1);
		return 0;
	} 

	printf("bind sucess\n");
	//WriteLog(DSN_LOG_ERROR,"udp bind sucess\n");

	pUdpPk = new CUdpPk(gudpsock);
	
	while(gStatThreadRuning)
	{
		//WriteLog(DSN_LOG_ERROR,"enter the model update thread \r\n");
		smsg[0] = 0;
		int ret = recvfrom(gudpsock, smsg, 1000, 0, (struct sockaddr*)&from,(socklen_t*)&from_len);
		if(ret == -1)
		{  
			//cout<<"udp socket read error....exit the update model thread"<<udpsock<<endl;  
			//WriteLog(DSN_LOG_ERROR,"udp socket read error %d....,exit the update model thread \n",udpsock);
			printf("\nudp socket read error %d....,exit the update model thread ", gudpsock);
			
			break;
		}
		else
		{

			char * PeerPubIp = inet_ntoa(from.sin_addr);
			int PeerPubPort = ntohs(from.sin_port);
#if 1
			pUdpPk->ProccessUdpPkt((byte *)smsg, ret, PeerPubIp, PeerPubPort, &from);
#else
			ASR_EVENT * pE = (ASR_EVENT *)smsg;

			if (pE->eType != EVENT_TYPE_KEEP_ALIVE)
			{
				printf("\nudp data got: %d, pE->eType:%d.", ret, pE->eType);
			}
			
			//if (pASRAnalyser)
			{
				switch(pE->eType)
				{
				case EVENT_TYPE_CFG:
					ASREventRecordStatCfg_Internel(&pE->u_data.Cfg);
					break;
				default:
					if (pASRAnalyser)
					{
						ASREventRecordStatRep_Internel(pASRAnalyser->mASR_REC_CFG.DBName, pE);
					}
					break;
				}
			}
#endif			
		}		

		counter++;
	}

	printf("\nUDPThread is over.");
	
    return 0;  
}


void * StatThread_ProcessEvent(void * arg);
void * StatThread_ProcessEvent(void * arg)  // 数据库操作在其中
{  
    //printf( "\nThis is a thread and arg = %d.\n", *(int*)arg);  
    //ASRAnalyser pASR = (ASRAnalyser*)arg = 0;
    printf("\nStatThread_ProcessEvent is runing.");

	unsigned int counter = 0;
	time_t tnow = 0;

	while(gStatThreadRuning)
	{

		//sleep(1);	// sec
		usleep(1);	// usec

		if (pASRAnalyser == 0) // || mutex == 0)
		{
		    printf("\nStatThread_ProcessEvent is runing, pASRAnalyser NOT inited, waiting.");

			usleep(1);
			continue;
		}

		// 从队列中调取事件并处理
		{
			ASR_EVENT *pEvent = NULL;
			ASR_EVENT Event;
			int idx = 0;
			do	// 如有事件则一直调取
			{
				//time(&tnow);

				// 调取事件
				// 锁仅锁住取消息这一短小的时间, 避免应用层因数据处理被拖住，增加时延
				pthread_mutex_lock(&mutex);
				pEvent = ASR_EVENT_QUEUE_Out(&gASREventQue, &tnow);
				if (pEvent)
				{
				 	Event = pEvent[0];
				}
				pthread_mutex_unlock(&mutex);

				// 处理事件
				if (pEvent)
				{

					if (pEvent->eType != EVENT_TYPE_KEEP_ALIVE)
					{
					 	printf("\nStatThread_ProcessEvent yyyyyyyyyyyyyyyyyyyyyyyyyy tnow:%u, Event:%d.", 
							tnow, Event.eType);
					}
					
				 	ASREventRecordStatRepPorccess(gASREventQue.DBName, &Event, tnow);

					pASRAnalyser->mEventProcessCount++;
				}
			} while(pEvent);
		}

		if ((counter % 10000) == 0) // 10 秒
		{
			//printf("\nStatThread_ProcessEvent is runing, %u", gStatThreadRepProcessRuningBeats++);

			if ((counter % 50000) == 0)
			{
				// 生成html
				pASRAnalyser->ASREventGenHtml();
			}
		}

		counter++;
	}

	printf("\nStatThread_ProcessEvent is over.");
	
    return 0;  
}

ASRAnalyser::ASRAnalyser(char *DBName)
{
	memset(&mASR_REC_CFG, 0, sizeof(mASR_REC_CFG));
	strcpy(mHtmlRootDir, HTML_ROOT_DIR);

	strcpy(mDBName, DBName);
	if (mDBName[0] == 0)
	{
		strcpy(mDBName, ASR_DB_NAME);
	}

	memset(mSingleItemStat, sizeof(int) * SINGLE_ITEM_BUTT, 0);
	
	memset(mASREventRcvStat, sizeof(int) * EVENT_TYPE_BUTT, 0);
	mEventInSpeed = 0;
	mEventPrcessSpeed = 0;
	mEventInCount = 0;
	mEventProcessCount = 0;

	mLastDate = 0;
	mLastTimeInSec = -1;

	mLastDate = -1;
	mLastDateReal = -1;
	mLastHour = -1;
	mLastMin = -1;
	mLastSec = -1;
	
	mCurDate = -1;
	mCurHour = -1;
	mCurMin = -1;
	mCurSec = -1;

	mLastDateTM = -1;			// linux localtime
	mLastDateTMReal = -1;
	mCurDateTM = -1;

	mLastTimeSegId = -1;		// 每天按片(比如5分钟)分段, 第x片
	mCurTimeSegId = -1;		// 每天按片(比如5分钟)分段, 第x片
	mStartTMThisDay = -1;
	mTimeSegLongInSec = 1200; //300; //5; //300;	// 秒

	// 命令词, 可通过配置接口输入
	ASR_CMD ASRCmd[] = 
	{
		{0,"CMD_Dail",					"拨打"},
		{0,"CMD_Open",					"打开"},
		{0,"CMD_Add",					"添加"},
		{0,"CMD_Rmv",					"删除"},
		{0,"CMD_Call",					"呼叫"},
		{0,"CMD_Play",					"播放"},
		{0,"CMD_Quit",					"退出"},
		{0,"CMD_Tune",					"调节"},
		{0,"CMD_Close",					"关闭"},
		{0,"CMD_Inc",					"增加"},
		{0,"CMD_Dec",					"减小"},
		{0,"CMD_Message",				"发消息给"},
		{0,"CMD_CallSO",				"打电话给"},
		{0,"CMD_CreateVoiceConf",		"创建聊天会议"},
		{0,"CMD_CreateDataConf",		"创建数据会议"},
		{0,"CMD_DataConf",				"数据会议"},
		{0,"CMD_VoiceConf",				"聊天会议"},
		{0,"CMD_SharePPT",				"共享胶片"},
		{0,"CMD_SetSpeaker",			"设置主讲人"},
		{0,"CMD_AddContact",			"添加联系人"},
		{0,"CMD_RmvContact",			"删除联系人"},
		{0,"CMD_Mute",					"静音"},
		{0,"CMD_Volume",				"音量"},
		{0,"CMD_PickupCall",			"接听呼叫"},
		{0,"CMD_PickupPhone",			"接听电话"},
		{0,"CMD_Pickup",				"接听"},
		{0,"CMD_MuteAll",				"全场静音"},
		{0,"CMD_MuteMe",				"本地静音"},
		{0,"CMD_CancelMute",			"取消静音"},
		{0,"CMD_ShareScreen",			"共享屏幕"},
		{0,"CMD_ShareDesttop",			"共享桌面"},
		{0,"CMD_Pull",					"拉"},
		{0,"CMD_CallGroup",				"呼叫群组"},
		{0,"CMD_MessageGroup",			"消息群组"},
		{0,"CMD_JoinConf",				"入会"},
	};
	int i = 0;
	for(i = 0; ASRCmd[i].CmdStr[0] != 0; i++)
	{
		mASRCmdSet[i] = ASRCmd[i];
	}
	mASRCmdSetSize = i;
	

};

ASRAnalyser::~ASRAnalyser()
{
	// stop thread
	gStatThreadRuning = 0;
}	

// sPathName: eSpace/asr/
int ASRAnalyser::CreateDir(char *sPathName)  
{  
  char   DirName[256];  
  strcpy(DirName,   sPathName);  
  int   i,len   =   strlen(DirName);  
  if(DirName[len-1]!=DIRDASH)
  {
  	//strcat(DirName,   "/");  
	DirName[strlen(DirName)] = DIRDASH;
  }
   
  len   =   strlen(DirName);  
   
  for(i=1;   i<len;   i++)  
  {  
	  if(DirName[i]==DIRDASH)  
	  {  
		  DirName[i]   =   0;  
		  if(   access(DirName,NULL)!=0 )  
		  {  
		      if(mkdir(DirName,   0755)==-1)  
		      {   
                  perror("mkdir   error");   
                  return   0;   
		      }  
		  }  
		  DirName[i]   =   DIRDASH;  
	  }  
  }  
   
  return 1;  
} 

#if 0
int ASRAnalyser::ASREventRecordASRAct(ASR_EVENT * pEvent)
{
	// 记录流水
	// save audio file
	// update day rec
	// update week rec
	// update month rec

	char FileName[300];
	char buf[2000];
	FILE * pf;

	//MYSQL mysql;
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;
	

	// 记录流水
	printf("ASREventRecordASRAct 1-------------------");
	{
		if (mASR_REC_CFG.RecRootDir[0])
		{

			sprintf(FileName, "%s%cASRrec.rec", mASR_REC_CFG.RecRootDir, DIRDASH);
		}
		else
		{
			sprintf(FileName, "ASRrec.rec");
		}
		
		pf = fopen(FileName, "a+b");
		if (!pf)
		{
			return 0;
		}
		fwrite(&pEvent->u_data.act, sizeof(pEvent->u_data.act), 1, pf);
		fclose(pf);

		// 所有用户的识别记录: 用户唯一标识, ASR操作时间, 原始录音文件, 降噪录音文件, 识别结果, 识别耗时(ms)，是否人工确认
		// CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
		// ALTER TABLE Persons
		// ADD PRIMARY KEY (Id_P)
		// ALTER TABLE Persons
		// ADD CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
		string sql = "create table AllUserASRActTbl (UserName varchar(10) NOT NULL, ASRDatetime datetime NOT NULL, AduioFileOrg varchar(20), AudioFileNS varchar(20), ASRResult varchar(30), ASRDuration int, Verified int)";
		mysql_query(&mysql, sql.c_str());
		//printf("\nCreate DB AllUserASRActTbl.");
		mysql_free_result(result);

		// insert into t1 (id, name) values (2, 'java2');
		sprintf(buf, "insert into AllUserASRActTbl values ('%s', %lld, '%s', '%s', '%s', '%s', %d, %d, %d);",
			datetime('-'),//pEvent->u_data.act.AudioFilePathName, 	// ASRDatetime datetime NOT NULL, 
			pEvent->u_data.act.uuid,
			pEvent->u_data.act.UserName, 		// UserName varchar(10) NOT NULL, 
			pEvent->u_data.act.AudioFilePathName, 	// AduioFileOrg varchar(20), 
			pEvent->u_data.act.AudioFilePathName, 	// AudioFileNS varchar(20), 
			pEvent->u_data.act.ASRResultStr, 		// ASRResult varchar(30), 
			pEvent->u_data.act.ASRE2EDurationInMs, // ASRDuration int, 
			pEvent->u_data.act.ASRE2EDurationInMs, // ASRE2EDuration int, 
			0 									// Verified int
			);
		//puts(buf); // test
		string sql2(buf);
		cout << sql2 << endl;
		mysql_query(&mysql, sql2.c_str());
		mysql_free_result(result);

	}

	// save audio file
	printf("ASREventRecordASRAct 2-------------------");
	if (mASR_REC_CFG.RecRootDir[0])
	{

		sprintf(FileName, "%s%cAudio%c%s_%s_%s.pcm", mASR_REC_CFG.RecRootDir, DIRDASH, DIRDASH,
			pEvent->u_data.act.AudioFilePathName, 
			pEvent->u_data.act.UserName, 
			pEvent->u_data.act.ASRResultStr 
			);
	}
	else
	{
		sprintf(FileName, "Audio%c%s_%s_%s.pcm", DIRDASH,
			pEvent->u_data.act.AudioFilePathName, 
			pEvent->u_data.act.UserName, 
			pEvent->u_data.act.ASRResultStr 
			);
	}
	pf = fopen(FileName, "w+b");
	if (!pf)
	{
		return 0;
	}
	fwrite(pEvent->u_data.act.pAudioData, pEvent->u_data.act.AudioDataLen, 1, pf);
	fclose(pf);

	// update day rec
	
	// update week rec
	
	// update month rec

	printf("ASREventRecordASRAct e-------------------");
	
	return 1;
}

int ASRAnalyser::ASREventRecordStatRep(ASR_EVENT * pEvent)
{
	// 记录周期统计

/*
	// 天内统计, 五分钟一个点, 288点, idx:0~287
	sql = "create table StatInOneDayTbl_20170310 (Idx int, StartTime time, RegUserNum int, ActiveUserNum int, ASRRate float, TraditionalActNum int, ASRNum int, ASRFailNum int, C0 int,CF0 int,	C1 int,CF1 int,	C2 int,CF2 int,	C3 int,CF3 int, C4 int,CF4 int, C5 int,CF5 int, C6 int,CF6 int, C7 int,CF7 int, C8 int,CF8 int, C9 int,CF9 int, C10 int,CF10 int, C11 int,CF11 int, C12 int,CF12 int, C13 int,CF13 int, C14 int,CF14 int, C15 int,CF15 int, C16 int,CF16 int, C17 int,CF17 int, C18 int,CF18 int, C19 int,CF19 int, C20 int,CF20 int, C21 int,CF21 int, C22 int,CF22 int, C23 int,CF23 int, C24 int,CF24 int, C25 int,CF25 int, C26 int,CF26 int, C27 int,CF27 int, C28 int,CF28 int, C29 int,CF29 int, C30 int,CF30 int, C31 int,CF31 int, C32 int,CF32 int, C33 int,CF33 int, C34 int,CF34 int, C35 int,CF35 int, C36 int,CF36 int, C37 int,CF37 int, C38 int,CF38 int, C39 int,CF39 int, C40 int,CF40 int, C41 int,CF41 int, C42 int,CF42 int, C43 int,CF43 int, C44 int,CF44 int, C45 int,CF45 int, C46 int,CF46 int, C47 int,CF47 int, C48 int,CF48 int, C49 int,CF49 int)";
	mysql_query(&mysql, sql.c_str());
	printf("\nCreate DB StatofOneDayTbl.");

*/
	//char FileName[300];
	char buft[2000];
	char buf2[2000];
	char tblname[100];
	//FILE * pf;

	//MYSQL mysql;
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;

	printf("\nASREventRecordStatRep() entered.");
	

	{
		// 所有用户的识别记录: 用户唯一标识, ASR操作时间, 原始录音文件, 降噪录音文件, 识别结果, 识别耗时(ms)，是否人工确认
		// CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
		// ALTER TABLE Persons
		// ADD PRIMARY KEY (Id_P)
		// ALTER TABLE Persons
		// ADD CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
		sprintf(tblname, "StatInOneDayTbl_%s", GetCurDateString('_'));
		printf("\n");
		printf(tblname);
		sprintf(buft, "create table %s (Idx int, StartTime time, RegUserNum int, ActiveUserNum int, ASRRate float, TraditionalActNum int, ASRNum int, ASRFailNum int, C0 int,CF0 int,	C1 int,CF1 int,	C2 int,CF2 int,	C3 int,CF3 int, C4 int,CF4 int, C5 int,CF5 int, C6 int,CF6 int, C7 int,CF7 int, C8 int,CF8 int, C9 int,CF9 int, C10 int,CF10 int, C11 int,CF11 int, C12 int,CF12 int, C13 int,CF13 int, C14 int,CF14 int, C15 int,CF15 int, C16 int,CF16 int, C17 int,CF17 int, C18 int,CF18 int, C19 int,CF19 int, C20 int,CF20 int, C21 int,CF21 int, C22 int,CF22 int, C23 int,CF23 int, C24 int,CF24 int, C25 int,CF25 int, C26 int,CF26 int, C27 int,CF27 int, C28 int,CF28 int, C29 int,CF29 int, C30 int,CF30 int, C31 int,CF31 int, C32 int,CF32 int, C33 int,CF33 int, C34 int,CF34 int, C35 int,CF35 int, C36 int,CF36 int, C37 int,CF37 int, C38 int,CF38 int, C39 int,CF39 int, C40 int,CF40 int, C41 int,CF41 int, C42 int,CF42 int, C43 int,CF43 int, C44 int,CF44 int, C45 int,CF45 int, C46 int,CF46 int, C47 int,CF47 int, C48 int,CF48 int, C49 int,CF49 int);",
			tblname);
		//sprintf(buft, "create table StatInOneDayTbl_0 (Idx int, StartTime time, RegUserNum int, ActiveUserNum int, ASRRate float, TraditionalActNum int, ASRNum int, ASRFailNum int, C0 int,CF0 int,	C1 int,CF1 int,	C2 int,CF2 int,	C3 int,CF3 int, C4 int,CF4 int, C5 int,CF5 int, C6 int,CF6 int, C7 int,CF7 int, C8 int,CF8 int, C9 int,CF9 int, C10 int,CF10 int, C11 int,CF11 int, C12 int,CF12 int, C13 int,CF13 int, C14 int,CF14 int, C15 int,CF15 int);");

		printf("\n");
		printf(buft);

		
		string sql(buft);
		mysql_query(&mysql, sql.c_str());
		//printf("\nCreate DB AllUserASRActTbl.");
		//mysql_free_result(result);
		// insert into t1 (id, name) values (2, 'java2');

		puts("\n");
		puts(pEvent->u_data.statrep.StartTime);
			
		static int idx = 0; // tbd
		sprintf(buft, "insert into %s values (%d, '%s', %d, %d, %d, %d, %d, %d",
			tblname,
			idx++, // Idx int, 
			pEvent->u_data.statrep.StartTime, 		//StartTime time, 	now();sysdate();'2000:01:31 23:59:59'
			pEvent->u_data.statrep.RegUserNum, 		//RegUserNum int, 
			pEvent->u_data.statrep.ActiveUserNum, 	//ActiveUserNum int, 
			pEvent->u_data.statrep.ASRRate, 		//ASRRate float, 
			pEvent->u_data.statrep.TraditionalActNum, //TraditionalActNum int,
			pEvent->u_data.statrep.ASRNum,			//ASRNum int, 
			pEvent->u_data.statrep.ASRFailNum		//ASRFailNum int, 			
			);

		puts("\n2----------");
	

		for(int i = 0; i < MAX_AUDIO_CMD_NUM; i++)
		{
			sprintf(buf2, ",%d,%d", 
				pEvent->u_data.statrep.CmdStat[i].CmdSuccessCount,
				pEvent->u_data.statrep.CmdStat[i].CmdFailureCount);
			strcat(buft, buf2);

			//puts("\n");
			//puts(buft);
		}
		strcat(buft, ");");
		puts(buft); // test
#if 1
		string sql2(buft);
		mysql_query(&mysql, sql2.c_str());
		mysql_free_result(result);
#endif	
	
	}

	return 1;
}

// api: 记录事件
int ASRAnalyser::ASREventRecord(ASR_EVENT * pEvent)
{
	printf("\nASREventRecord() entered.");
	if (!pEvent)
	{
		printf("\nASREventRecord(), null pointer if param, error.");
		return 0;
	}

	switch(pEvent->eType)
	{
	case EVENT_TYPE_ASR_INSERT:
		ASREventRecordASRAct(pEvent);
		
		break;
	case EVENT_TYPE_STAT_REP:
		ASREventRecordStatRep(pEvent);
		
		break;
	default:
		return 0;
	}
	
	return 1;
}

// api: 修改某个字段的值: 操作enum FIELD_OP inc, dec, set
int ASRAnalyser::ModifyTableFieldVal(FIELD_DEF * pFieldDef)
{
	// tbl, row, field
	
	
	return 1;
}
#endif

// api: 配置记录器
int ASRAnalyser::ASREventRecordCfg(ASR_REC_CFG * pCfg)
{
	int r = 0;

#if 0	
	mASR_REC_CFG = pCfg[0];

	if (mASR_REC_CFG.RecSubDir[0])
	{
		r = CreateDir(mASR_REC_CFG.RecSubDir);
		if (r <= 0)
		{
			printf("\nASREventRecordCfg(), CreateDir() error, dir:%s.", mASR_REC_CFG.RecSubDir);
			return 0;
		}
	}
#endif

	return 1;
}

// api: 生成/刷新html, 周期调用始可，比如5秒
#define MAX_INPUT_HTMLFILE_SIZE 100000
int ASRAnalyser::ASREventGenHtml()
{
	//printf("\nhhhhhhhhhhhhhhhhhhhhhh ASREventGenHtml(), begin.");
		
	// 生成主页

	long tick0 = get_time_ms();
		
	ASREventGenHtml_IndexHtml();

	//printf("\nttt------------------------ ASREventGenHtml, ticks = %d.", get_time_ms() - tick0);

	//printf("\nhhhhhhhhhhhhhhhhhhhhhh ASREventGenHtml(), end.");
	

	return 1;
}

// 查询VarLable位置
int ASRAnalyser::FindNextVARLabel(char *buf, int StartSearchPos, int BufDataSize, int *VarPos, int *VarEndPos)
{
	int i = 0;
	int j = 0;
	int VARHEADLEN = strlen(VAR_HEAD);
	int VARENDLEN = strlen(VAR_END);
	
	for(i = StartSearchPos; i < BufDataSize - VARHEADLEN; i++)
	{
		/*
		printf("\nxxxx.%d--------------------. :%d. VARHEADLEN:%d.[%c%c%c%c%c%c%c]", i, BufDataSize, VARHEADLEN,
			buf[i + 0],
			buf[i + 1],
			buf[i + 2],
			buf[i + 3],
			buf[i + 4],
			buf[i + 5],
			buf[i + 6]
			);
			*/
		if (strncmp(buf + i, VAR_HEAD, VARHEADLEN) == 0)
		{
			VarPos[0] = i;

		//printf("\nyyyyyyyy.%d--------------------.", i);

			for(j = i + VARHEADLEN; j < BufDataSize - VARENDLEN - 1; j++)
			{
		//printf("\nxxxxxxxx.%d--------------------.", j);
				if (strncmp(buf + j, VAR_END, VARENDLEN) == 0)
				{
					VarEndPos[0] = j + VARENDLEN - 1;

					return 1;
				}
			}
			
			return 0;
		}
	}
	
	return 0;
}

int ASRAnalyser::ASREventGenHtml_CmdStat(char *OutPutStr, char * CmdName)
{
	// 天内各CMD调用次数

	//char buf[2000];
	char buf1[2000];
	//char buf2[2000];
	char TableName[100];

/*
"datasets : [
{
	fillColor : \"rgba(%d,220,220,0.5)\",
	strokeColor : \"rgba(%d,220,220,1)\",
	pointColor : \"rgba(%d,220,220,1)\",
	pointStrokeColor : \"#fff\",
	//data : [1,8,3,4,5,6,7,9,10,11,2,12]
	%s
},";
*/

	// 输出到buf,  如
	char Seg[1000] = 
"\n{\n\
	fillColor : \"rgba(%d,220,220,0.5)\",\n\
	strokeColor : \"rgba(%d,220,220,1)\",\n\
	pointColor : \"rgba(%d,220,220,1)\",\n\
	pointStrokeColor : \"#fff\",\n\
	data : [%s]\n\
},";

	//sprintf(buf, "{");
	buf1[0] = 0;	// 1,2,3,4,5,6,7,8,9,10,11,12,

	//GetTableName("AllUserHabit", GetCurDateString('_'), TableName);
	//char wherex[200];
	//sprintf(wherex, "where ActionDesc = '%s'", CmdName);
	//GetTableRowCount(TableName, wherex);

	GetTableName("StatInOneDay", GetCurDateString('_'), TableName);
	GetTableField(TableName, CmdName, NULL, buf1);

	int len = strlen(CmdName);
	int clr = CmdName[len - 1];
	int clr2 = CmdName[len - 2];
	int clr3 = CmdName[len - 3];

	sprintf(OutPutStr, Seg, 
		//random() % 255, 
		//random() % 255, 
		//random() % 255, 
		clr, clr2, clr3,
		buf1);

	//printf("\nooooooooooooo ASREventGenHtml_CmdStat, output:%s.", OutPutStr);

	return 1;
	
}

// 天内各CMD调用次数
int ASRAnalyser::ASREventGenHtml_CmdsStat(char *OutPutStr)
{
	// 天内各CMD调用次数

	//char buf[2000];
	char buf1[2000];
	//char buf2[2000];
	//char TableName[100];

	/* 生成
	datasets : [
		{
			fillColor : "rgba(220,220,220,0.5)",
			strokeColor : "rgba(220,220,220,1)",
			pointColor : "rgba(220,220,220,1)",
			pointStrokeColor : "#fff",
			data : [1,8,3,4,5,6,7,9,10,11,2,12]
		},
		{
			fillColor : "rgba(151,187,205,0.5)",
			strokeColor : "rgba(151,187,205,1)",
			pointColor : "rgba(151,187,205,1)",
			pointStrokeColor : "#fff",
			//
			data : [1,8,11,2,3,4,5,9,10,6,7,12]
		},
		{
			fillColor : "rgba(51,187,205,0.5)",
			strokeColor : "rgba(51,187,205,1)",
			pointColor : "rgba(51,187,205,1)",
			pointStrokeColor : "#fff",
			//
			data : [1,8,5,6,7,12,9,10,11,2,3,4]
		}
	]
	*/	

	int i;

	sprintf(OutPutStr, "datasets : [\n");

#if 0	
	for(i = 0; i < mASRCmdSetSize; i++)
	{
		ASREventGenHtml_CmdStat(buf1, mASRCmdSet[i].CmdStrName);
		strcat(OutPutStr, buf1);
	}
#else
	ASR_CMD ASRCmdSet[MAX_ASR_CMD_NUM];
	int 	ASRCmdSetSize;

	GetCmdAndCallTimesDesc("ASRCmdTbl", "ASRCmdName", "ASRCmdDesc", "CallTimes", ASRCmdSet, &ASRCmdSetSize);
	const int MAX_SHOW_COUNT = 3;
	for(i = 0; i < ASRCmdSetSize && i < MAX_SHOW_COUNT; i++)
	{
		ASREventGenHtml_CmdStat(buf1, ASRCmdSet[i].CmdStrName);
		strcat(OutPutStr, buf1);
		
		//puts(buf1);
		//exit(0); // test		
	}
#endif

	strcat(OutPutStr, "\n],");

	//printf("\nooooooooooooo ASREventGenHtml_CmdSstat, output:%s, cmd0:%s,ASRCmdSetSize:%d.", OutPutStr, ASRCmdSet[0].CmdStr, ASRCmdSetSize);
	//exit(0); // test
	
	return 1;
}

// 统计各命令的使用总数/比例
int ASRAnalyser::ASREventGenHtml_CmdUsingTimesTblStat(char *OutPutStr)
{
	// 天内各CMD调用次数

	//char buf[2000];
	char buf1[2000];
	//char buf2[2000];
	//char TableName[100];

	/* 生成	
	<tr>
	<td>打开话给</td>
	<td>13.2%</td>
	</tr>
	*/	

	int i;

	int CmdUseTimes[MAX_ASR_CMD_NUM] = {0};

////////////////////////

	char Seg0[1000] = 		
"\n<tr>\n\
<td>注册用户数</td>\n\
<td>%d</td>\n\
</tr>\n\
\
<tr>\n\
<td>在线用户数</td>\n\
<td>%d</td>\n\
</tr>\n\
\
<tr>\n\
<td>语音识别累计次数</td>\n\
<td>%u</td>\n\
</tr>\n\
\
<tr>\n\
<td>用户操作累计次数</td>\n\
<td>%u</td>\n\
</tr>\n\
\
<tr>\n\
<td>识别正确率</td>\n\
<td>%.2f</td>\n\
</tr>\n";

/////////////////////////////////

	char Seg[1000] = 
"\n<tr>\n\
<td>常用语言:%s</td>\n\
<td>%.2f\%</td>\n\
</tr>\n";

	sprintf(OutPutStr, "");

	long long tatol = 1;

	// 单项值统计
	{
		char TableName[100];
		int RowCount;
		
		sprintf(TableName, "AllUserASRActTbl");
		RowCount = GetTableRowCount(TableName, NULL);
		mSingleItemStat[ITEM_ALL_ASR_NUM] = RowCount;

		sprintf(TableName, "AllUserHabit");
		RowCount = GetTableRowCount(TableName, NULL);
		mSingleItemStat[ITEM_ALL_OP_NUM] = RowCount;
		
		sprintf(buf1, Seg0, 
			mSingleItemStat[ITEM_REG_USER_NUM],
			mSingleItemStat[ITEM_ACT_USER_NUM],
			mSingleItemStat[ITEM_ALL_ASR_NUM],
			mSingleItemStat[ITEM_ALL_OP_NUM],
			(float)mSingleItemStat[ITEM_ACCURACY_RATIO]
			);
		strcat(OutPutStr, buf1);
	}

	// 从表中提取各命令词的总调用次数
#if 0	
	for(i = 0; i < mASRCmdSetSize; i++)
	{
		//ASREventGenHtml_CmdStat(buf1, mASRCmdSet[i].CmdStrName);
		//strcat(OutPutStr, buf1);
		CmdUseTimes[i] = GetCmdUseTimes("ASRCmdTbl", "CallTimes", "ASRCmdDesc", mASRCmdSet[i].CmdStr);
		tatol += CmdUseTimes[i];
	}

	// 生成html文本
	for(i = 0; i < mASRCmdSetSize; i++)
	{
		sprintf(buf1, Seg, mASRCmdSet[i].CmdStr, (float)100 * CmdUseTimes[i] / tatol);
		strcat(OutPutStr, buf1);
	}
#else
	ASR_CMD ASRCmdSet[MAX_ASR_CMD_NUM];
	int 	ASRCmdSetSize;

	GetCmdAndCallTimesDesc("ASRCmdTbl", "ASRCmdName", "ASRCmdDesc", "CallTimes", ASRCmdSet, &ASRCmdSetSize);
	for(i = 0; i < ASRCmdSetSize; i++)
	{
		//ASREventGenHtml_CmdStat(buf1, mASRCmdSet[i].CmdStrName);
		//strcat(OutPutStr, buf1);
		CmdUseTimes[i] = GetCmdUseTimes("ASRCmdTbl", "CallTimes", "ASRCmdDesc", ASRCmdSet[i].CmdStr);
		tatol += CmdUseTimes[i];
	}

	// 生成html文本
	for(i = 0; i < ASRCmdSetSize; i++)
	{
		sprintf(buf1, Seg, ASRCmdSet[i].CmdStr, (float)100 * CmdUseTimes[i] / tatol);
		strcat(OutPutStr, buf1);
	}
#endif

	strcat(OutPutStr, "\n],");

	//xxxx

	// test
	/*
	if (0)
	{
		FILE * pfOut;
		char outputdata[200] = "a中华人民共和国";

		pfOut = fopen("xxx.html", "w+b");
		if (!pfOut)
		{
			printf("\nOpen output file error.");
		
			return -1;
		}

		if (outputdata)
		{
			fwrite(outputdata, 1, strlen(outputdata), pfOut);
			i = 0;
			//memcpy(outputdata, mASRCmdSet[i].CmdStr, strlen(mASRCmdSet[i].CmdStr));
			strcat(outputdata, mASRCmdSet[i].CmdStr); //, strlen(mASRCmdSet[i].CmdStr));
			fwrite(outputdata, 1, strlen(outputdata), pfOut);
		}

		fclose(pfOut);
	}
	*/

	//printf("\nooooooooooooo ASREventGenHtml_CmdUsingTimesTblStat, output:%s.", OutPutStr);
	//exit(0); // test
	
	return 1;
}

int CodeConvert(char *encFrom, char *encTo, char *src, char *dst)
{
	//char *encTo = "UNICODE//TRANSLIT";
	//char *encTo = "UNICODE//IGNORE";
	/* 源编码 */
	//char *encFrom = "UTF-8";

	/* 获得转换句柄
	*@param encTo 目标编码方式
	*@param encFrom 源编码方式
	*
	* */
	iconv_t cd = iconv_open(encTo, encFrom);
	if (cd == (iconv_t)-1)
	{
		perror ("\niconv_open error.");
		return -1;
	}

  	/* 由于iconv()函数会修改指针，所以要保存源指针 */
 	//char *srcstart = buf1;
  	//char *tempoutbuf = buf;
	size_t srclen = strlen(src);
	size_t outlen = srclen * 4;

	size_t ret = iconv (cd, &src, &srclen, &dst, &outlen);
	if (ret == -1)
	{
		perror ("\niconv conver error.");
		iconv_close(cd);
		return -1;
	}

	iconv_close(cd);

	return 1;
}

int ASRAnalyser::ASREventGenHtml_ReplaceLabel(int VARPos, int VAREndPos, char *OrgHtmlBuf, char *OutPutHtmlBuf, int *outputdatalen)
{
	// repale __VAR__xx labels


	char *varin = OrgHtmlBuf + VARPos;
	int varlen = VAREndPos - VARPos + 1;
	//memcpy(varin, OrgHtmlBuf + VARPos, VAREndPos - VARPos + 1);

	char buf[20000];
	char buf1[20000];
	char TableName[100];
	
	// test
	char xx[100];
	memcpy(xx, varin, varlen);
	xx[varlen] = 0;
	//printf("\nvarinxx:%s.", xx);
	//getchar();
	//exit(0);
	
	int timeseg = 20; // min // cfg...
	int minInOneDay = 24 * 60;
	int SegCount = minInOneDay / timeseg;

	if (strncmp(varin,"__VAR__CMD_USAGE_TBL__",varlen) == 0)
	{
		// 统计各命令的使用总数/比例
		
		ASREventGenHtml_CmdUsingTimesTblStat(buf1);

#if 0
		int l = strlen(buf1);
		char *dst = OutPutHtmlBuf + outputdatalen[0];
		memcpy(dst, buf1, l);
		outputdatalen[0] += l;
#else
		CodeConvert("gb2312", "utf-8", buf1, buf);

		int l = strlen(buf);
		char *dst = OutPutHtmlBuf + outputdatalen[0];
		memcpy(dst, buf, l);
		outputdatalen[0] += l;
#endif		

		//printf("\nooooooooooooo __VAR__CMD_USAGE_TBL__:%s, len:%d.", buf, l);

		//exit(0); // test
	}
	else if (strncmp(varin,"__VAR__linedata_OneDayTick_labels__",varlen) == 0)
	//strncmp(varin,"__VAR__linedata_Reg_Active_UserStat_labels__",varlen) == 0
	//else if (strncmp(varin,"__VAR__linedata_ASR_TRAD_OPStat_labels__",varlen) == 0)
	//else if (strncmp(varin,"__VAR__linedata_ASR_CMD_Stat_labels__",varlen) == 0)	
	{
		// 天内时间轴
		
		//printf("\n__VAR__linedata_Reg_Active_UserStat_labels__ switch.");
		//labels : ["1","2","3","4","5","6","7","8","9","10","11","12"],
		//生成:     labels : ["0:0","0:20","0:40","1:0","1:20"...],
		buf1[0] = 0;
		sprintf(buf, "labels : [");
		for(int i = 0; i < SegCount; i++)
		{
			sprintf(buf1, "\"%d:%02d\",", i / 3, (i % 3) * 20);
			strcat(buf, buf1);			
		}
		buf[strlen(buf) - 1] = 0; // 去掉最后一个','
		strcat(buf, "],");
		int l = strlen(buf);
		char *dst = OutPutHtmlBuf + outputdatalen[0];
		memcpy(dst, buf, l);
		outputdatalen[0] += l;

		// test
		//char *dst0 = dst - 2;
		//dst0[0] = ' ';
		//dst0[1] = ' ';
		//char dst1 = dst + l + 2;
		//dst1[0] = '/';
		//dst1[1] = '/';

		// test
		//printf("\nooooooooooooo:%s,outputdatalen%d:l%d.", buf, outputdatalen, l);
		//getchar();
	}
	else if (strncmp(varin,"__VAR__linedata_Reg_Active_UserStat_reguser_data__",varlen) == 0) 
	{
		// 天内注册用户数
		
		//printf("\n__VAR__linedata_Reg_Active_UserStat_reguser_data__ switch.");
		//data : [1,2,3,4,5,6,7,8,9,10,11,12]
		
		//getchar(); // test

		// TimeSegId integer(4), RegUserNum integer(4), ActiveUserNum integer(4), ASRTimes integer(4), AllActTimes integer(4)", TableName);

		// 输出到buf,  如data : [1,2,3,4,5,6,7,8,9,10,11,12]
		sprintf(buf, "data : [");
		buf1[0] = 0;	// 1,2,3,4,5,6,7,8,9,10,11,12,

		GetTableName("StatInOneDay", GetCurDateString('_'), TableName);
		GetTableField(TableName, "RegUserNum", NULL, buf1);

		strcat(buf, buf1);
		strcat(buf, "]");

		int l = strlen(buf);
		char *dst = OutPutHtmlBuf + outputdatalen[0];
		memcpy(dst, buf, l);
		outputdatalen[0] += l;

		// test
		//printf("\nooooooooooooo RegUserNum:%s.", buf);
		//getchar();
	}
	else if (strncmp(varin,"__VAR__linedata_Reg_Active_UserStat_activeuser_data__",varlen) == 0)
	{
		// 天内活动用户数

		//printf("\n__VAR__linedata_Reg_Active_UserStat_activeuser_data__.");
		//data : [1,2,3,4,5,6,7,8,9,10,11,12]

		// TimeSegId integer(4), RegUserNum integer(4), ActiveUserNum integer(4), ASRTimes integer(4), AllActTimes integer(4)", TableName);

		// 输出到buf,  如data : [1,2,3,4,5,6,7,8,9,10,11,12]
		sprintf(buf, "data : [");
		buf1[0] = 0;	// 1,2,3,4,5,6,7,8,9,10,11,12,

		GetTableName("StatInOneDay", GetCurDateString('_'), TableName);
		GetTableField(TableName, "ActiveUserNum", NULL, buf1);

		strcat(buf, buf1);
		strcat(buf, "]");

		int l = strlen(buf);
		char *dst = OutPutHtmlBuf + outputdatalen[0];
		memcpy(dst, buf, l);
		outputdatalen[0] += l;

		//printf("\nooooooooooooo ActiveUserNum:%s.", buf);
	}
	else if (strncmp(varin,"__VAR__linedata_ASR_TRAD_OPStat_ASRtimes_data__",varlen) == 0)    
	{
		// 天内ASR总调用次数

		// 输出到buf,  如data : [1,2,3,4,5,6,7,8,9,10,11,12]
		sprintf(buf, "data : [");
		buf1[0] = 0;	// 1,2,3,4,5,6,7,8,9,10,11,12,

		GetTableName("StatInOneDay", GetCurDateString('_'), TableName);
		GetTableField(TableName, "ASRTimes", NULL, buf1);

		strcat(buf, buf1);
		strcat(buf, "]");

		int l = strlen(buf);
		char *dst = OutPutHtmlBuf + outputdatalen[0];
		memcpy(dst, buf, l);
		outputdatalen[0] += l;
		
		//printf("\nooooooooooooo ASRTimes:%s.", buf);
	}
	else if (strncmp(varin,"__VAR__linedata_ASR_TRAD_OPStat_Tradtimes_data__",varlen) == 0)   
	{
		// 天内用户总行为次数

		// 输出到buf,  如data : [1,2,3,4,5,6,7,8,9,10,11,12]
		sprintf(buf, "data : [");
		buf1[0] = 0;	// 1,2,3,4,5,6,7,8,9,10,11,12,

		GetTableName("StatInOneDay", GetCurDateString('_'), TableName);
		GetTableField(TableName, "AllActTimes", NULL, buf1);

		strcat(buf, buf1);
		strcat(buf, "]");

		int l = strlen(buf);
		char *dst = OutPutHtmlBuf + outputdatalen[0];
		memcpy(dst, buf, l);
		outputdatalen[0] += l;

		//printf("\nooooooooooooo AllActTimes:%s.", buf);
	}
	else if (strncmp(varin,"__VAR__linedata_ASR_CMD_Stat_cmd1_data__,varlen) == 0)",varlen) == 0)
	{
		// 天内各CMD调用次数

		// 输出到buf,  如
		/*
		datasets : [
		{
			fillColor : "rgba(220,220,220,0.5)",
			strokeColor : "rgba(220,220,220,1)",
			pointColor : "rgba(220,220,220,1)",
			pointStrokeColor : "#fff",
			//__VAR__linedata_ASR_CMD_Stat_cmd1_data__
			data : [1,8,3,4,5,6,7,9,10,11,2,12]
		},
		*/
		ASREventGenHtml_CmdsStat(buf);

		int l = strlen(buf);
		char *dst = OutPutHtmlBuf + outputdatalen[0];
		memcpy(dst, buf, l);
		outputdatalen[0] += l;

		//printf("\nooooooooooooo AllActTimes:%s.", buf);
	}
	/*
	else if (strncmp(varin,"__VAR__linedata_ASR_CMD_Stat_cmd2_data__,varlen) == 0)",varlen) == 0)
	{
	}
	else if (strncmp(varin,"__VAR__linedata_ASR_CMD_Stat_cmd3_data__,varlen) == 0)",varlen) ==0)
	{
	}
	*/
	else if (strncmp(varin,"__VAR__linedata_RECENT30Days_tick_labels__",varlen) == 0)
	{
	}
	else if (strncmp(varin,"__VAR__linedata_RECENT30Days_Reg_Active_UserStat_data__",varlen) == 0)
	{
	}
	else if (strncmp(varin,"__VAR__linedata_RECENT30Days_Reg_Active_UserStat_data__",varlen) == 0)
	{
	}
	else if (strncmp(varin,"__VAR__linedata_RECENT30Days_tick_labels__",varlen) == 0)
	{
	}
	else if (strncmp(varin,"__VAR__linedata_RECENT30Days_ASR_TRAD_OPStat_data__",varlen) == 0)
	{
	}
	else if (strncmp(varin,"__VAR__linedata_RECENT30Days_ASR_TRAD_OPStat_data__",varlen) == 0)
	{
	}

	//printf("\nASREventGenHtml_ReplaceLabel over:%s.", xx);
	//getchar();

	return 1;

/*
	char VARLabels[][] = 
	{
		"__VAR__linedata_Reg_Active_UserStat_labels__",	
		"__VAR__linedata_Reg_Active_UserStat_reguser_data__",
		"__VAR__linedata_Reg_Active_UserStat_activeuser_data__",
		"__VAR__linedata_ASR_TRAD_OPStat_labels__",
		"__VAR__linedata_ASR_TRAD_OPStat_ASRtimes_data__",
		"__VAR__linedata_ASR_TRAD_OPStat_Tradtimes_data__",
		"__VAR__linedata_ASR_CMD_Stat_labels__",
		"__VAR__linedata_ASR_CMD_Stat_cmd1_data__,",
		"__VAR__linedata_ASR_CMD_Stat_cmd2_data__,",
		"__VAR__linedata_ASR_CMD_Stat_cmd3_data__,",
		"__VAR__linedata_RECENT30Days_Reg_Active_UserStat_labels__",
		"__VAR__linedata_RECENT30Days_Reg_Active_UserStat_data__",
		"__VAR__linedata_RECENT30Days_Reg_Active_UserStat_data__",
		"__VAR__linedata_RECENT30Days_Reg_Active_UserStat_labels__",
		"__VAR__linedata_RECENT30Days_ASR_TRAD_OPStat_data__",
		"__VAR__linedata_RECENT30Days_ASR_TRAD_OPStat_data__",
		"",
	};

	int idx = 0;
	while(VARLabels[idx] != [0])
	{
		if (strncmp(varin, VARLabels[idx]) == 0)
		{
			break;
		}
		
		idx++;
	}

	if (VARLabels[idx] == [0])
	{
		// not found
		return -1;
	}
*/	
}

// 生成主页
int ASRAnalyser::ASREventGenHtml_IndexHtml()
{
	char indexhtml[MAX_INPUT_HTMLFILE_SIZE];
	char outputdata[MAX_INPUT_HTMLFILE_SIZE];
	int outputdatalen = 0;
	
	FILE * pf;
	FILE * pfOut;

	// load template
	pf = fopen("index_temp.html", "r+b");
	if (!pf)
	{
		printf("\nOpen input file error.");
		return -1;
	}

	pfOut = fopen("index_out.html", "w+b");
	if (!pfOut)
	{
		printf("\nOpen output file error.");

		fclose(pf);
		
		return -1;
	}

	//printf("\nx1--------------------.");


	int infilesize = fread(indexhtml, 1, MAX_INPUT_HTMLFILE_SIZE, pf);
	//infilesize -= 10; // test

	int StartSearchPos = 0;
	int BufDataSize = infilesize;

	// 记录所有var label
	int VarPos[500] = {0};
	int VarEndPos[500] = {0};
	int VarCount = 0;

	//printf("\nx2--------------------infilesize:%d.", infilesize);
		
	int i = 0;
	int r = 0;
	for(i = 0; i < infilesize; i++)
	{
		//printf("\nx3.%d--------------------.", i);
		
		StartSearchPos = i;
		r = FindNextVARLabel(indexhtml, StartSearchPos, BufDataSize, VarPos + VarCount, VarEndPos + VarCount);
		if (r <= 0)
		{
			break;
		}

		i = VarEndPos[VarCount] + 1;
		VarCount++;
	}

	// test
	//printf("\nASREventGenHtml_IndexHtml(), %d VAR labels found.", VarCount);
	//for(i = 0; i < VarCount; i++)
	//{
	//	printf("\nASREventGenHtml_IndexHtml(), VAR[%d], start:%d, end:%d.", i, VarPos[i], VarEndPos[i]);
	//}

	// 处理每一个var label
	for(i = 0; i < VarCount; i++)
	{
		// copy 上一段正文
		if (i == 0)
		{
			if (VarPos[i])
			{
				memcpy(outputdata, indexhtml, VarPos[i]); //  + outputdatalen
				outputdatalen += VarPos[i];
			}
		}
		else
		{
			// copy从上一VAR段尾到本VAR段首的正文
			int copylen = VarPos[i] - VarEndPos[i-1] - 1;
			memcpy(outputdata + outputdatalen, indexhtml + VarEndPos[i-1] + 1, copylen);
			outputdatalen += copylen;
		}

		// 替换VAR label
		{
			ASREventGenHtml_ReplaceLabel(VarPos[i], VarEndPos[i], indexhtml, outputdata, &outputdatalen);
		}

		if (outputdatalen >= MAX_INPUT_HTMLFILE_SIZE)
		{
			printf("\nASREventGenHtml_IndexHtml(), Fatal error, outputdatalen: %s.", outputdatalen);
		}
	}
	// copy最后一段正文
	if (VarCount)
	{
		int copylen = infilesize - VarEndPos[i-1] - 1;
		memcpy(outputdata + outputdatalen, indexhtml + VarEndPos[VarCount-1] + 1, copylen);
		outputdatalen += copylen;
	}

	if (outputdata)
	{
		fwrite(outputdata, 1, outputdatalen, pfOut);
	}

	fclose(pf);
	fclose(pfOut);

	if (outputdata)
	{
//#ifdef THIS_IS_TEST_VER
		char cmd[200];
		sprintf(cmd, "cp index_out.html %s/%s/index.html", mHtmlRootDir, mASR_REC_CFG.HtmlSubDir); // /var/www/html/
		system(cmd);
//#endif
	}

	//printf("\nASREventGenHtml_IndexHtml() over.");

	//puts(outputdata); // test
	//exit(0); // test

	return 1;
}


int ASRAnalyser::InitDB()
{
	// connect DB
	
	// create db
	// delete proc
	// insert proc

	char buf[1000];

	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;
	
	mysql_init(&mysql);
	MYSQL * myp = mysql_real_connect(&mysql, "localhost", "root", "ddshlmstzczx", "epiano", 3306, NULL, 0);
	if (!myp)
	{
		printf("\nmysql_real_connect fail, quit.");
		//return 0;
	}

	/* 设置数据库默认字符集 */ 
    if ( mysql_set_character_set( &mysql, "gbk")) //"utf8" ) )
	{ 
        fprintf ( stderr , "错误, %s/n" , mysql_error( & mysql) ) ; 
    } 

	// 中文字符, 注，此三行在mysql中手工执行后可正常显示中文?
	if (1)
	{
		string sql = "SET character_set_client='gbk'";
		mysql_query(&mysql, sql.c_str());
		sql = "SET character_set_connection='gbk'";
		mysql_query(&mysql, sql.c_str());
		sql = "SET character_set_results='gbk'";
		mysql_query(&mysql, sql.c_str());
	}

	/*
	drop table RegUserTbl;
	drop table AllUserASRActTbl;
	drop table StatofOneDayTbl;
	*/
	if (0)
	{
		string sql = "drop database test";
		mysql_query(&mysql, sql.c_str());
		printf("\ndrop database test.");
		//mysql_free_result(result);
		
		sql = "drop table RegUserTbl";
		mysql_query(&mysql, sql.c_str());
		printf("\ndrop DB RegUserTbl.");
		//mysql_free_result(result);

		sql = "drop table AllUserASRActTbl";
		mysql_query(&mysql, sql.c_str());
		printf("\ndrop DB AllUserASRActTbl.");
		//mysql_free_result(result);

		sql = "drop table StatInOneDayTbl_2017_3_15";
		mysql_query(&mysql, sql.c_str());
		printf("\ndrop DB StatofOneDayTbl.");
		//mysql_free_result(result);
	}

	// create db
	{
		sprintf(buf, "CREATE DATABASE %s CHARACTER SET 'utf8' COLLATE 'utf8_general_ci'", mDBName); //ASR_DB_NAME);
		puts(buf);
		string sql(buf); 
		//string sql = "CREATE DATABASE test"; 
		mysql_query(&mysql, sql.c_str());
		printf("\nCreate DB.");
		//mysql_free_result(result);

		sprintf(buf, "use %s", mDBName);
		string sql2(buf); 
		mysql_query(&mysql, sql2.c_str());
		printf("\nuse db.");
		//mysql_free_result(result);
	}

	// create tble
	{
		char TableName[100];

		// 用户ASR操作全局记录表，还有按天的表
		{
			sprintf(TableName, "AllUserASRActTbl");
			sprintf(buf, "create table %s (ASRDatetime datetime NOT NULL, SessionId varchar(64) NOT NULL, UserName varchar(10) NOT NULL, ASRResultStr varchar(30), AudioFilePathName varchar(100), ASRE2EDurationInMs int, ASRRecongitioanDurationInMs int, Verified int)", 
				TableName);
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
		}

		// 用户操作习惯全局记录表，还有按天的表
		{
			sprintf(TableName, "AllUserHabit");
			sprintf(buf, "create table %s (UserName varchar(10) NOT NULL, StartTime datetime NOT NULL, ActionDesc varchar(30))", TableName);
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
		}

		// 日期记录表: 记录哪些日期本功能被打开
		{
			sprintf(TableName, "DateTbl");
			sprintf(buf, "create table %s (Date varchar(20) NOT NULL, PRIMARY KEY (Date))", TableName);
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
		}

		// 注册用户记录表
		{
			sprintf(TableName, "RegUsrTbl");
			sprintf(buf, "create table %s (Version varchar(20) NOT NULL, UserName varchar(10) NOT NULL, DateTime datetime NOT NULL, ActionNum integer(4), PRIMARY KEY (UserName))", TableName);
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
		}

		// 活动用户记录表，按天
		/*
		{
			sprintf(TableName, "ActiveUsrTbl");
			sprintf(buf, "create table %s (UserName varchar(10) NOT NULL, DateTime datetime NOT NULL)", TableName);
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
		}
		*/

		// 注册用户记录表
		{
			sprintf(TableName, "EpRegUsrTbl");
			sprintf(buf, "create table %s (UserId int(8) NOT NULL, UserName varchar(20) NOT NULL, Pwd varchar(20) NOT NULL, PhoneNum varchar(15) NOT NULL, PortraitId int(4), Roll int, LocalIp varchar(16), LocalPort int, PubIp varchar(16), PubPort int, PhoneModel varchar(20), Mac varchar(30), DateTime datetime NOT NULL, ActionNum int, NatType int, FriendDataVer int(4), LessionDataVer int(4), PRIMARY KEY (UserId))", TableName);
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
			puts("\n");
			puts(buf);
		}		

		// 师生关系表
		// 每个师生关系对应一项
		{
			sprintf(TableName, "TSTbl");
			sprintf(buf, "create table %s (TeacherUserId int(8) NOT NULL, StudentUserId int(8) NOT NULL, EstablishTime datetime, LastActTime datetime, PRIMARY KEY (TeacherUserId, StudentUserId))", TableName);
			//sprintf(buf, "create table %s (UserAId int(8) NOT NULL, UserBId int(8) NOT NULL, Relation int, EstablishTime datetime, LastActTime datetime, PRIMARY KEY (TeacherUserId, StudentUserId))", TableName);
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
			puts("\n");
			puts(buf);
		}

		// 师生课表
		// 每对师生的所有课程
		{
			sprintf(TableName, "TSLessionTbl");
			sprintf(buf, "create table %s (TeacherUserId int(8) NOT NULL, StudentUserId int(8) NOT NULL, LessionId int(8), PRIMARY KEY (TeacherUserId, StudentUserId, LessionId))", TableName);
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
			puts("\n");
			puts(buf);
		}		

		// 课描述表
		// 每一课由多个练习组成，每个练习包含一个曲子
		{
			sprintf(TableName, "LessionDescTbl");
			sprintf(buf, "create table %s (LessionId int(8) NOT NULL, ScheduleTime datetime, ExerciseId1 int, ExerciseId2 int, ExerciseId3 int, PRIMARY KEY (LessionId))", TableName);
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
			puts("\n");
			puts(buf);
		}

		// 曲练习描述表
		// 暂不完善，需要能够记录多天的练习数据
		{
			sprintf(TableName, "ExerciseDescTbl");
			sprintf(buf, "create table %s (ExerciseId int(8) NOT NULL, SongId int, StartTime datetime, ExerciseTime int, SubmitTime datetime, PRIMARY KEY (ExerciseId))", TableName);
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
			puts("\n");
			puts(buf);
		}


		// 命令词表
#if 0		
		{
			sprintf(TableName, "ASRCmdTbl");
			//sprintf(buf, "create table %s (ASRCmdName varchar(30) character set gbk NOT NULL, UseTimes integer(4), IgnoreCmd integer(4), PRIMARY KEY (ASRCmdName))", TableName);
			sprintf(buf, "create table %s (ASRCmdName varchar(30) NOT NULL,ASRCmdDesc varchar(30) NOT NULL, UseTimes integer(4), IgnoreCmd integer(4), CallTimes integer(8), PRIMARY KEY (ASRCmdName))", TableName);
			//sprintf(buf, "create table %s (StaticIdx integer(4), ASRCmdName varchar(30) NOT NULL, UseTimes integer(4), IgnoreCmd integer(4), PRIMARY KEY (StaticIdx))", TableName);
			string sql(buf);
			//cout << sql << endl;
			mysql_query(&mysql, sql.c_str());

			int i = 0;
			for(i = 0; mASRCmdSet[i].CmdStr[0] != 0; i++)
			{
				sprintf(buf, "insert into %s values ('%s','%s',0,0,0);", TableName, mASRCmdSet[i].CmdStrName, mASRCmdSet[i].CmdStr);
				//puts(buf); // test
				string sql2(buf);
				//cout << sql2 << endl;
				mysql_query(&mysql, sql2.c_str());
			}
		}
#endif
	}

	{
		pthread_t th;
		int arg = 0; //(int)this;
		//int *thread_ret;  
		printf( "\nCreating thread ... !"); 
		int ret = pthread_create( &th, NULL, StatThread, &arg );  
	    if( ret != 0 ){  
	        printf( "\nCreate thread error!");  
	        return -1;  
	    }
	    printf("\nCreating thread done." );  
	    //pthread_join( th, NULL); //(void**)&thread_ret );  		
	}

#if 0	
	{
		pthread_t th;
		int arg = 0; //(int)this;
		//int *thread_ret;  
		printf( "\nCreating UDPThread ... !"); 
		int ret = pthread_create( &th, NULL, UDPThread, &arg );  
	    if( ret != 0 ){  
	        printf( "\nCreate UDPThread error!");  
	        return -1;  
	    }
	    printf("\nCreating UDPThread done." );  
	    //pthread_join( th, NULL); //(void**)&thread_ret );  		
	}
#endif

	if (0)
	{
		pthread_t th;
		int arg = 0; //(int)this;
		//int *thread_ret;  
		printf( "\nCreating StatThread_ProcessEvent ... !"); 
		int ret = pthread_create( &th, NULL, StatThread_ProcessEvent, &arg );  
	    if( ret != 0 ){  
	        printf( "\nCreate StatThread_ProcessEvent error!");  
	        return -1;  
	    }
	    printf("\nCreating StatThread_ProcessEvent done." );  
	    //pthread_join( th, NULL); //(void**)&thread_ret );  		
	}
	
	printf("\nInit DB done.");

	return 1;
}

int ASRAnalyser::GetTableName(char *TableNamePrefix, char *Sufix, char *OutPutName)
{
	sprintf(OutPutName, "%s_%s", TableNamePrefix, Sufix);
	//puts(OutPutName); // test

	return 1;
}

int ASRAnalyser::AsrInsert(struct S_STAT_ASR_INSERT *AsrInsert)
{
	// 记录流水
	// save audio file
	// update day rec
	// update week rec
	// update month rec

	//char FileName[300];
	char buf[1000];
	char buf1[1000];
	//FILE * pf;
	char TableName[100];

	//MYSQL mysql;
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;

	if (AsrInsert->Version[0] == 0)
	{
		printf("\nAsrInsert fail, Version is null.");
		
		return -1;
	}

	if (AsrInsert->SessionId[0] == 0)
	{
		printf("\nAsrInsert fail, SessionId is null.");
		
		return -1;
	}	

	if (AsrInsert->UserName[0] == 0)
	{
		printf("\nAsrInsert fail, UserName is null.");
		
		return -1;
	}

	// tablename
	GetTableName("AllUserASRActTbl", GetCurDateString('_'), TableName);

	// 记录流水
	//printf("\nAsrInsert 1-------------------");
	{
#if 0
		// 转移到OnDayChanged()处理
		{
			// 所有用户的识别记录: 用户唯一标识, ASR操作时间, 原始录音文件, 降噪录音文件, 识别结果, 识别耗时(ms)，是否人工确认
			// CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
			// ALTER TABLE Persons
			// ADD PRIMARY KEY (Id_P)
			// ALTER TABLE Persons
			// ADD CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
			sprintf(buf, "create table %s (ASRDatetime datetime NOT NULL, SessionId varchar(33) NOT NULL, UserName varchar(10) NOT NULL, ASRResultStr varchar(30), AudioFilePathName varchar(100), ASRE2EDurationInMs int, ASRRecongitioanDurationInMs int, Verified int)", 
				TableName);
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
			puts(buf);
			//printf("\nCreate DB AllUserASRActTbl.");
			//mysql_free_result(result);
		}
#endif


		//char utfbuf[300];
		//char gbbuf_ASRResultStr[300];
		//CodeConvert("utf-8", "gb2312", AsrInsert->ASRResultStr, gbbuf_ASRResultStr);

		// insert into t1 (id, name) values (2, 'java2');
		sprintf(buf1, " values ('%s', '%s', '%s', '%s', '%s', %d, %d, %d);",
			datetime('-'),// ASRDatetime datetime NOT NULL, 
			AsrInsert->SessionId,
			AsrInsert->UserName,
			AsrInsert->ASRResultStr,
			AsrInsert->AudioFilePathName,
			//AsrInsert->AudioFilePathName_org,
			AsrInsert->ASRE2EDurationInMs,
			AsrInsert->ASRRecongitioanDurationInMs,
			0
			);

		// 写入按天的表
		{
			sprintf(buf, "insert into %s %s", TableName, buf1);
			puts("\n");
			puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());
		}

		// 写入全局表
		{
			long tick0 = get_time_ms();
				
			sprintf(buf, "insert into %s %s", "AllUserASRActTbl", buf1);
			//puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());

			//printf("\nttt------------------------ inert into AllUserASRActTbl, ticks = %d.", get_time_ms() - tick0);
		}

	}

	// save audio file
	/*
	printf("AsrInsert 2-------------------");
	if (mASR_REC_CFG.RecRootDir[0])
	{

		sprintf(FileName, "%s%cAudio%c%s_%s_%s.pcm", mASR_REC_CFG.RecRootDir, DIRDASH, DIRDASH,
			pEvent->u_data.act.AudioFilePathName, 
			pEvent->u_data.act.UserName, 
			pEvent->u_data.act.ASRResultStr 
			);
	}
	else
	{
		sprintf(FileName, "Audio%c%s_%s_%s.pcm", DIRDASH,
			pEvent->u_data.act.AudioFilePathName, 
			pEvent->u_data.act.UserName, 
			pEvent->u_data.act.ASRResultStr 
			);
	}
	pf = fopen(FileName, "w+b");
	if (!pf)
	{
		return 0;
	}
	fwrite(pEvent->u_data.act.pAudioData, pEvent->u_data.act.AudioDataLen, 1, pf);
	fclose(pf);
	*/

	// update day rec
	
	// update week rec
	
	// update month rec

	// upade 
	// 活跃用户表: 天级
	{
		//sprintf(TableName, "ActiveUsrTbl");
		GetTableName("ActiveUsrTbl", GetCurDateString('_'), TableName);
		sprintf(buf1, "values ('%s', '%s', 0);",
			AsrInsert->UserName,
			datetime('-')
			);
		sprintf(buf, "insert into %s %s", TableName, buf1);
		//puts(buf); // test
		string sql2(buf);
		//cout << sql2 << endl;
		mysql_query(&mysql, sql2.c_str());
	}

	// upade 
	// 注册用户表:
	{
		{
			sprintf(TableName, "RegUsrTbl");
			sprintf(buf1, "values ('%s', '%s', '%s', 0);",
				AsrInsert->Version,
				AsrInsert->UserName,
				datetime('-')
				);
			sprintf(buf, "insert into %s %s", TableName, buf1);
			//puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());
		}

		// 更新版本号
		{
			sprintf(buf1, " set Version = '%s' where UserName = '%s';",
				AsrInsert->Version,
				AsrInsert->UserName
				);
			sprintf(buf, "update %s %s", TableName, buf1);
			//puts("\n");
			//puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());
		}
	}

	//printf("\nAsrInsert end-------------------");
	
	return 1;
}

int ASRAnalyser::AsrUpdate(struct S_ASR_UPDATE *AsrUpdate)
{
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;
	char buf[1000];
	char buf1[1000];
	char TableName[100];

	if (AsrUpdate->SessionId[0] == 0)
	{
		printf("\nAsrUpdate fail, SessionId is null.");
		
		return -1;
	}

	// tablename
	GetTableName("AllUserASRActTbl", GetCurDateString('_'), TableName);

	// 
	//printf("\nAsrUpdate 1-------------------x.");
	{
		// 所有用户的识别记录: 用户唯一标识, ASR操作时间, 原始录音文件, 降噪录音文件, 识别结果, 识别耗时(ms)，是否人工确认
		// CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
		// ALTER TABLE Persons
		// ADD PRIMARY KEY (Id_P)
		// ALTER TABLE Persons
		// ADD CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
		//string sql = "create table AllUserASRActTbl (ASRDatetime datetime NOT NULL, SessionId varchar(33) NOT NULL, UserName varchar(10) NOT NULL, ASRResultStr varchar(30), AudioFilePathName varchar(100), ASRE2EDurationInMs int, ASRRecongitioanDurationInMs int, Verified int)";
		//mysql_query(&mysql, sql.c_str());
		//printf("\nCreate DB AllUserASRActTbl.");
		//mysql_free_result(result);

		sprintf(buf1, " set ASRE2EDurationInMs = %d where SessionId = '%s';",
			AsrUpdate->ASRE2EDurationInMs,
			AsrUpdate->SessionId
			);
		//puts(buf); // test
		//string sql2(buf);
		//cout << sql2 << endl;
		//mysql_query(&mysql, sql2.c_str());

		// 写入按天的表
		{
			sprintf(buf, "update %s %s", TableName, buf1);
			//puts("\n");
			//puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());
		}

		// 写入全局表
		{
			sprintf(buf, "update %s %s", "AllUserASRActTbl", buf1);
			//puts("\n");
			//puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());
		}
		
		//mysql_free_result(result);
	}
	
	return 1;
}

int ASRAnalyser::StatHabit(struct S_STAT_HABIT *Habit)
{
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;	
	char buf[1000];
	char buf1[1000];
	char TableName[100];

	if (Habit->ActionDesc[0] == 0)
	{
		printf("\nStatHabit fail, ActionDesc is null.");
		
		return -1;
	}

	if (Habit->UserName[0] == 0)
	{
		printf("\nStatHabit fail, UserName is null.");
		
		return -1;
	}

	// tablename
	GetTableName("AllUserHabit", GetCurDateString('_'), TableName);

	// 记录流水
	//printf("\nStatHabit 1-------------------");
	{
#if 0
		// 转移到OnDayChanged()处理
		{
			// 所有用户的识别记录: 用户唯一标识, ASR操作时间, 原始录音文件, 降噪录音文件, 识别结果, 识别耗时(ms)，是否人工确认
			// CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
			// ALTER TABLE Persons
			// ADD PRIMARY KEY (Id_P)
			// ALTER TABLE Persons
			// ADD CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
			sprintf(buf, "create table %s (UserName varchar(10) NOT NULL, StartTime datetime NOT NULL, ActionDesc varchar(30))", TableName);
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
			//printf("\nCreate DB AllUserASRActTbl.");
		}
#endif
		// insert into t1 (id, name) values (2, 'java2');
		sprintf(buf1, " values ('%s', '%s', '%s');",
			Habit->UserName,
			datetime('-'), //Habit->StartTime,
			Habit->ActionDesc
			);
		//puts(buf); // test
		//string sql2(buf);
		//cout << sql2 << endl;
		//mysql_query(&mysql, sql2.c_str());

		// 写入按天的表
		{
			sprintf(buf, "insert into %s %s", TableName, buf1);
			//puts("\n");
			//puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());
		}

		// 写入全局表
		{
			sprintf(buf, "insert into %s %s", "AllUserHabit", buf1);
			//puts("\n");
			//puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());
		}

		// 写命令词统计表
		{
			/*
			sprintf(buf, "insert into %s %s", "AllUserHabit", buf1);
			//puts("\n");
			//puts(buf); // test
			string sql2(buf);
			//cout << sql2 << endl;
			mysql_query(&mysql, sql2.c_str());
			*/
			IncCmdUseTimes("ASRCmdTbl", "CallTimes", "ASRCmdDesc", Habit->ActionDesc);
		}

	}
	
	return 1;
}

// 增加命令的调用次数
int ASRAnalyser::IncCmdUseTimes(char * TableName, char *FieldName, char *WhereFieldName, char * CmdStr)
{
	//char TableName[100];
	char buf[1000];
	char buf1[1000];
	
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;

	int RowCount = 0;
	
	sprintf(buf, "select %s from %s where %s = '%s'", FieldName, TableName, WhereFieldName, CmdStr);
	
	//puts(buf); // test
	string sql2(buf);
	//cout << sql2 << endl;
	mysql_query(&mysql, sql2.c_str());

	//string sql = "select id,name from t1;";
    //mysql_query(&mysql, sql.c_str());
    result = mysql_store_result(&mysql);
    int rowcount = mysql_num_rows(result);
    //cout << rowcount << endl;
		//mysql_free_result(result);
	int fieldcount = mysql_num_fields(result);
    //cout << fieldcount << endl;
		//mysql_free_result(result);

#if 0	
	for(int i = 0; i < fieldcount; i++)
	{
		printf("\n0.%d-------------------------",i);
		field = mysql_fetch_field_direct(result,i);
		cout << field->name << "\t\t";
	}
#endif

	//mysql_free_result(result);
	MYSQL_ROW row = NULL;
	int k = 0;	

	//printf("\nffffffffffffffff: fieldcount:%d.", fieldcount);

	while(row = mysql_fetch_row(result))
	{
		//printf("\n0.1 %d-------------------------",k);
		k++;
		
		for(int j=0; j<fieldcount; j++)
		{
			//printf("\nffffffffffffffff");
			//cout << row[j] << "\t\t";
			//printf("\neeeeeeeeeeeeeeee");

			long long v = atoll(row[j]);
			v++;

			

			sprintf(buf, "update %s set %s = %d where %s = '%s'", TableName, FieldName, v, WhereFieldName, CmdStr);
			string sql3(buf);
			//cout << sql3 << endl;
			mysql_query(&mysql, sql3.c_str());

			mysql_free_result(result);
			
			return 1;
		}
		//cout << endl;
		//row = mysql_fetch_row(result);

		break;	/////////////////
	}
	
	/**/
	mysql_free_result(result);

	return RowCount;

}

// 获取命令的调用次数
long long ASRAnalyser::GetCmdUseTimes(char * TableName, char *FieldName, char *WhereFieldName, char * CmdStr)
{
	//char TableName[100];
	char buf[1000];
	char buf1[1000];
	
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;

	int RowCount = 0;
	
	sprintf(buf, "select %s from %s where %s = '%s'", FieldName, TableName, WhereFieldName, CmdStr);
	
	//puts(buf); // test
	string sql2(buf);
	//cout << sql2 << endl;
	mysql_query(&mysql, sql2.c_str());

	//string sql = "select id,name from t1;";
    //mysql_query(&mysql, sql.c_str());
    result = mysql_store_result(&mysql);
    int rowcount = mysql_num_rows(result);
    //cout << rowcount << endl;
		//mysql_free_result(result);
	int fieldcount = mysql_num_fields(result);
    //cout << fieldcount << endl;
		//mysql_free_result(result);

#if 0	
	for(int i = 0; i < fieldcount; i++)
	{
		printf("\n0.%d-------------------------",i);
		field = mysql_fetch_field_direct(result,i);
		cout << field->name << "\t\t";
	}
#endif

	//mysql_free_result(result);
	MYSQL_ROW row = NULL;
	int k = 0;	

	//printf("\nffffffffffffffff: fieldcount:%d.", fieldcount);

	while(row = mysql_fetch_row(result))
	{
		//printf("\n0.1 %d-------------------------",k);
		k++;
		
		for(int j=0; j<fieldcount; j++)
		{
			//printf("\nffffffffffffffff");
			//cout << row[j] << "\t\t";
			//printf("\neeeeeeeeeeeeeeee");

			long long v = atoll(row[j]);
			
			mysql_free_result(result);
			
			return v;
		}
		//cout << endl;
		//row = mysql_fetch_row(result);

		break;	/////////////////
	}
	
	/**/
	mysql_free_result(result);

	return 0;

}

// 获取命令及调用次数, 降序
int ASRAnalyser::GetCmdAndCallTimesDesc(char * TableName, char *CmdNameField, char *CmdDescField, char *CallTimesField, ASR_CMD *ASRCmdSet, int *ASRCmdSetSize)
{
	//char TableName[100];
	char buf[1000];
	char buf1[1000];
	
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;

	int RowCount = 0;

	ASRCmdSetSize[0] = 0;
	
	sprintf(buf, "select %s,%s,%s from %s ORDER BY %s DESC", CmdNameField, CmdDescField, CallTimesField, TableName, CallTimesField);
	
	//puts(buf); // test
	string sql2(buf);
	//cout << sql2 << endl;
	mysql_query(&mysql, sql2.c_str());

	//string sql = "select id,name from t1;";
    //mysql_query(&mysql, sql.c_str());
    result = mysql_store_result(&mysql);
    int rowcount = mysql_num_rows(result);
    //cout << rowcount << endl;
		//mysql_free_result(result);
	int fieldcount = mysql_num_fields(result);
    //cout << fieldcount << endl;
		//mysql_free_result(result);

#if 0	
	for(int i = 0; i < fieldcount; i++)
	{
		printf("\n0.%d-------------------------",i);
		field = mysql_fetch_field_direct(result,i);
		cout << field->name << "\t\t";
	}
#endif

	//mysql_free_result(result);
	MYSQL_ROW row = NULL;
	int k = 0;	

	//printf("\nffffffffffffffff: fieldcount:%d.", fieldcount);

	while(row = mysql_fetch_row(result))
	{
		//printf("\n0.1 %d-------------------------",k);

		/*
		for(int j=0; j<fieldcount; j++)
		{
			//printf("\nffffffffffffffff");
			//cout << row[j] << "\t\t";
			//printf("\neeeeeeeeeeeeeeee");

			long long v = atoll(row[j]);
			
			//mysql_free_result(result);			
			//return v;
		}
		//cout << endl;
		//row = mysql_fetch_row(result);
		*/

		if (k >= MAX_ASR_CMD_NUM)
		{
			break;
		}

		strcpy(ASRCmdSet[k].CmdStrName, row[0]);	// CMD_xxx
		strcpy(ASRCmdSet[k].CmdStr, row[1]);	// 拔打
		ASRCmdSet[k].Times = atoll(row[2]);

		k++;
		//break;	/////////////////
	}

	ASRCmdSetSize[0] = k;
	
	/**/
	mysql_free_result(result);

	return 1;

}


// 获取表格的记录数
int ASRAnalyser::GetTableRowCount(char * TableName, char *where_condition)
{
	//char TableName[100];
	char buf[1000];
	char buf1[1000];
	
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;

	int RowCount = 0;
	
	//sprintf(TableName, "DateTbl");	
	if (where_condition)
	{
		sprintf(buf, "select count(*) from %s %s", TableName, where_condition);
	}
	else
	{
		sprintf(buf, "select count(*) from %s", TableName);
	}
	
	//puts(buf); // test
	string sql2(buf);
	//cout << sql2 << endl;
	mysql_query(&mysql, sql2.c_str());

	//string sql = "select id,name from t1;";
    //mysql_query(&mysql, sql.c_str());
    result = mysql_store_result(&mysql);
    int rowcount = mysql_num_rows(result);
    //cout << rowcount << endl;
		//mysql_free_result(result);
	int fieldcount = mysql_num_fields(result);
    //cout << fieldcount << endl;
		//mysql_free_result(result);

#if 0	
	for(int i = 0; i < fieldcount; i++)
	{
		printf("\n0.%d-------------------------",i);
		field = mysql_fetch_field_direct(result,i);
		cout << field->name << "\t\t";
	}
#endif

	//mysql_free_result(result);
	MYSQL_ROW row = NULL;
	int k = 0;	

	//printf("\nffffffffffffffff: fieldcount:%d.", fieldcount);

	while(row = mysql_fetch_row(result))
	{
		//printf("\n0.1 %d-------------------------",k);
		k++;
		
		for(int j=0; j<fieldcount; j++)
		{
			//printf("\nffffffffffffffff");
			//cout << row[j] << "\t\t";
			//printf("\neeeeeeeeeeeeeeee");

			RowCount = atoi(row[j]);
			
			break;	///////////////
		}
		//cout << endl;
		//row = mysql_fetch_row(result);

		break;	/////////////////
	}
	
	/**/
	mysql_free_result(result);

	return RowCount;

}

// 获取表格的列录数
// OutStr: 1,3, x, y, z....
int ASRAnalyser::GetTableField(char * TableName, char *FieldName, char *where_condition, char * OutStr)
{
	//char TableName[100];
	char buf[1000];
	//char buf1[1000];
	
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;

	if (!OutStr)
	{
		return -1;
	}

	OutStr[0] = 0;	

	int RowCount = 0;
	
	//sprintf(TableName, "DateTbl");
	if (where_condition)
	{
		sprintf(buf, "select %s from %s %s", FieldName, TableName, where_condition);
	}
	else
	{
		sprintf(buf, "select %s from %s", FieldName, TableName);
	}
	//puts(buf); // test
	string sql2(buf);
	//cout << sql2 << endl;
	mysql_query(&mysql, sql2.c_str());

	//string sql = "select id,name from t1;";
    //mysql_query(&mysql, sql.c_str());
    result = mysql_store_result(&mysql);
    int rowcount = mysql_num_rows(result);
    //cout << rowcount << endl;
		//mysql_free_result(result);
	int fieldcount = mysql_num_fields(result); // assert fieldcount == 1
    //cout << fieldcount << endl;
		//mysql_free_result(result);

	if (fieldcount != 1)
	{
		printf("\nGetTableField(), error, fieldcount = %d.", fieldcount);
		return -1;
	}

#if 0	
	for(int i = 0; i < fieldcount; i++)
	{
		printf("\n0.%d-------------------------",i);
		field = mysql_fetch_field_direct(result,i);
		cout << field->name << "\t\t";
	}
#endif

#if 1
	//mysql_free_result(result);
	MYSQL_ROW row = NULL;
	int k = 0;	

	//printf("\nffffffffffffffff: fieldcount:%d.", fieldcount);

	while(row = mysql_fetch_row(result))
	{
		//printf("\n0.1 %d-------------------------",k);
		k++;
		
		for(int j=0; j<fieldcount; j++)
		{
			//printf("\nffffffffffffffff");
			//cout << row[j] << "\t\t";
			//printf("\neeeeeeeeeeeeeeee");

			//RowCount = atoi(row[j]);
			//(*row)[j]

			sprintf(buf, "%s,", row[j]);
			strcat(OutStr, buf);
			
			//break;	///////////////
		}
		//cout << endl;
		//row = mysql_fetch_row(result);

		//break;	/////////////////
	}
	
	/**/
	mysql_free_result(result);
#endif

	return RowCount;

}

// 更新天内时间段统计
int ASRAnalyser::UpdateInnerOneDayStat(int CurTimeSegId)
{
	MYSQL_RES *result = NULL;
    MYSQL_FIELD *field = NULL;
	char buf[2000];
	char buf1[2000];
	char FiSet[2000];
	char TableName[100];


	int RowCount = 0;

	//printf("\nUpdateInnerOneDayStat()---------------------RegUsrTbl rowcount:%d.", mSingleItemStat[ITEM_REG_USER_NUM]);


	// 注册用户数, 活动用户数，ASR次数, 用户操作总数, 
	// 从各表格中提取

	// TimeSegId integer(4), RegUserNum integer(4), ActiveUserNum integer(4), ASRTimes integer(4), AllActTimes integer(4)", TableName);


	//sprintf(FiSet, " set ASRE2EDurationInMs = %d where SessionId = '%s';",
	sprintf(FiSet, " set ");

	{
		sprintf(TableName, "RegUsrTbl");
		RowCount = GetTableRowCount(TableName, NULL);
		mSingleItemStat[ITEM_REG_USER_NUM] = RowCount;

		//printf("\n---------------------RegUsrTbl rowcount:%d.", RowCount);
		
 		sprintf(buf1, " RegUserNum = %d", RowCount);
		strcat(FiSet, buf1);
		strcat(FiSet, ", ");
	}

	{
		GetTableName("ActiveUsrTbl", GetCurDateString('_'), TableName);
		RowCount = GetTableRowCount(TableName, NULL);
		//printf("\n---------------------%s rowcount:%d.", TableName, RowCount);
		mSingleItemStat[ITEM_ACT_USER_NUM] = RowCount;
		
 		sprintf(buf1, " ActiveUserNum = %d", RowCount);
		strcat(FiSet, buf1);
		strcat(FiSet, ", ");
	}

	{
		GetTableName("AllUserASRActTbl", GetCurDateString('_'), TableName);
		RowCount = GetTableRowCount(TableName, NULL);
		//printf("\n---------------------%s rowcount:%d.", TableName, RowCount);
		
 		sprintf(buf1, " ASRTimes = %d", RowCount);
		strcat(FiSet, buf1);
		strcat(FiSet, ", ");
	}

	{
		GetTableName("AllUserHabit", GetCurDateString('_'), TableName);
		RowCount = GetTableRowCount(TableName, NULL);
		//printf("\n---------------------%s rowcount:%d.", TableName, RowCount);
		
 		sprintf(buf1, " AllActTimes = %d", RowCount);
		strcat(FiSet, buf1);
		//strcat(FiSet, ", ");
		strcat(FiSet, ", ");
	}

	// 按CMD的统计 TBD.
	{
		GetTableName("AllUserHabit", GetCurDateString('_'), TableName);

		int i;
		char wherex[200];
		for(i = 0; i < mASRCmdSetSize; i++)
		{
			sprintf(wherex, "where ActionDesc = '%s'", mASRCmdSet[i].CmdStr);
			RowCount = GetTableRowCount(TableName, wherex);
			//printf("\n---------------------%s rowcount:%d.", TableName, RowCount);
			
	 		sprintf(buf1, " %s = %d", mASRCmdSet[i].CmdStrName, RowCount);
			strcat(FiSet, buf1);
			//strcat(FiSet, ", ");

			if (i < mASRCmdSetSize - 1)
			{
				strcat(FiSet, ",");
			}
			else
			{
				strcat(FiSet, " ");
			}
		}
	}

	if (CurTimeSegId >= 0)
	{
		// where
		{
			sprintf(buf1, " where TimeSegId = %d;", CurTimeSegId);
			strcat(FiSet, buf1);
		}

		// tablename
		GetTableName("StatInOneDay", GetCurDateString('_'), TableName);

		// 
		//printf("\nAsrUpdate 1-------------------x.");
		{
			// 写入按天的表
			{
				sprintf(buf, "update %s %s", TableName, FiSet);
				//puts("\n");
				//puts(buf); // test
				string sql2(buf);
				//cout << sql2 << endl;
				mysql_query(&mysql, sql2.c_str());
			}

			//mysql_free_result(result);
		}
	}
	
	return 1;
}


// 判断天内的时间分段是否变化, 用于生成新的时间分片表格
int ASRAnalyser::OnTimeSegChanged()
{
	char TableName[100];

	mCurTimeSegId = (mCurDateTM - mStartTMThisDay) / mTimeSegLongInSec;

	// test
#if 1
	long CurTimeSegId = (mCurDateTM - mStartTMThisDay) / 5;
	// 5分钟一片, 一天24 * 60 / 5 = 288片
	if (mLastTimeSegId != CurTimeSegId)
#else
	if (mLastTimeSegId != mCurTimeSegId)
#endif
	{
		mLastTimeSegId = mCurTimeSegId;
		//printf("\n---------------------OnTimeSegChanged(), mCurTimeSegId:%d, Ver:%d.", mCurTimeSegId, VERSION);

		// test
		/*
		if (mCurTimeSegId < 40)
		{
			printf("\n------eeeeeeeeeeeee---------------OnTimeSegChanged(), mCurTimeSegId:%d, Ver:%d. mCurDateTM:%d,mStartTMThisDay:%d,mTimeSegLongInSec:%d.", 
				mCurTimeSegId, VERSION, 
				mCurDateTM,mStartTMThisDay,mTimeSegLongInSec);
			getchar();
			return 0;
		}
		*/
		
		UpdateInnerOneDayStat(mCurTimeSegId);
	}

	return 0;
}

// 判断日期是否变化, 用于生成新的天的表格
int ASRAnalyser::OnDayChanged()
{
	char TableName[100];
	char buf[3000];
	char buf1[2000];
	char buf2[2000];

	int r = DateCmp(mLastDateTM, mCurDateTM);

	//printf("\nDateCmp:%d, pASRAnalyser->mLastDateTM:%d, pASRAnalyser->mCurDateTM:%d.", r, mLastDateTM, mCurDateTM);
	
	if (r != 0)
	{

		// 当天的开始时刻, 用于分段计算
		//mStartTMThisDay = mCurDateTM;
		struct tm *tm_beginOfThisDay;
		tm_beginOfThisDay = localtime(&mCurDateTM); 
		//tm_beginOfThisDay->tm_mday;
		tm_beginOfThisDay->tm_hour = 0;
		tm_beginOfThisDay->tm_min = 0;
		tm_beginOfThisDay->tm_sec = 0;
		mStartTMThisDay = mktime(tm_beginOfThisDay); 

		printf("\n---------------------New day %s, last day is %s, TMNow;%d, TMBeginOfDay:%d.\n", 
			GetCurDateString('_'), GetDateString(mLastDateTM, '_'),
			mCurDateTM, mStartTMThisDay
			);

		//getchar();
		
		// 保留真正的昨天, 因为mLastDate会被马上变为"今天"
		mLastDateTMReal = mLastDateTM;

		// 创建天表
		// 用户ASR活动表
		// 用户习惯表
		// 活跃用户表: 0时创建, 相当于清空
		// 创建天内统计表

		{
			//if (pASRAnalyser->mLastDateTMReal != -1)
			{
				{
					sprintf(TableName, "DateTbl");
					sprintf(buf, "insert into %s values ('%s')", TableName, GetCurDateString('_'));
					//puts(buf); // test
					string sql2(buf);
					//cout << sql2 << endl;
					mysql_query(&mysql, sql2.c_str());
				}
			}
		}
		
		{
			// 用户ASR活动表，for ecah day
			{
				// 所有用户的识别记录: 用户唯一标识, ASR操作时间, 原始录音文件, 降噪录音文件, 识别结果, 识别耗时(ms)，是否人工确认
				// CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
				// ALTER TABLE Persons
				// ADD PRIMARY KEY (Id_P)
				// ALTER TABLE Persons
				// ADD CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
				
				// tablename
				GetTableName("AllUserASRActTbl", GetCurDateString('_'), TableName);
				
				sprintf(buf, "create table %s (ASRDatetime datetime NOT NULL, SessionId varchar(64) NOT NULL, UserName varchar(10) NOT NULL, ASRResultStr varchar(30), AudioFilePathName varchar(100), ASRE2EDurationInMs int, ASRRecongitioanDurationInMs int, Verified int)", 
					TableName);
				string sql(buf);
				mysql_query(&mysql, sql.c_str());
				//puts(buf);
				//printf("\nCreate DB AllUserASRActTbl.");
				//mysql_free_result(result);
			}

			// 用户习惯表，for ecah day
			{
				// 所有用户的识别记录: 用户唯一标识, ASR操作时间, 原始录音文件, 降噪录音文件, 识别结果, 识别耗时(ms)，是否人工确认
				// CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)
				// ALTER TABLE Persons
				// ADD PRIMARY KEY (Id_P)
				// ALTER TABLE Persons
				// ADD CONSTRAINT pk_PersonID PRIMARY KEY (Id_P,LastName)

				// tablename
				GetTableName("AllUserHabit", GetCurDateString('_'), TableName);
				
				sprintf(buf, "create table %s (UserName varchar(10) NOT NULL, StartTime datetime NOT NULL, ActionDesc varchar(30))", TableName);
				string sql(buf);
				mysql_query(&mysql, sql.c_str());
				//printf("\nCreate DB AllUserASRActTbl.");
			}

			// 活跃用户表: 0时创建, 相当于清空，for ecah day
			{
				//sprintf(TableName, "ActiveUsrTbl");
				GetTableName("ActiveUsrTbl", GetCurDateString('_'), TableName);
				sprintf(buf, "create table %s (UserName varchar(10) NOT NULL, DateTime datetime NOT NULL, ActionNum integer(4), PRIMARY KEY (UserName))", TableName);
				string sql(buf);
				mysql_query(&mysql, sql.c_str());
			}
		}

		// 天内统计表, 按x分钟分段, 比如段长5分钟, 则每天会有288条记录
		{
			//sprintf(TableName, "ActiveUsrTbl");
			GetTableName("StatInOneDay", GetCurDateString('_'), TableName);

			// 拼装所有命令词字段
			char ASRCmdFields[3000] = "";
			char ASRCmdFieldVals[2000] = "";
			for(int i = 0; i < mASRCmdSetSize; i++)
			{
				sprintf(buf, ",%s integer(4)", mASRCmdSet[i].CmdStrName);
				strcat(ASRCmdFields, buf);
				sprintf(buf, ",0");
				strcat(ASRCmdFieldVals, buf);
			}
			// test
			//puts("\n");
			//puts(ASRCmdFields);

			// 拼装sql命令

			sprintf(buf, "create table %s (TimeSegId integer(4), RegUserNum integer(4), ActiveUserNum integer(4), ASRTimes integer(4), AllActTimes integer(4)", TableName);
			strcat(buf, ASRCmdFields);
			strcat(buf, ", PRIMARY KEY (TimeSegId))");
			string sql(buf);
			mysql_query(&mysql, sql.c_str());
			puts("\n");
			puts(buf); // test

			int TimeSegLongInSec = mTimeSegLongInSec;
			if (TimeSegLongInSec > 300)
			{
				//TimeSegLongInSec = 300;
			}
			int loopcount = 24 * 60 * 60 / TimeSegLongInSec;
			for(int i = 0; i < loopcount; i++)
			{
				sprintf(buf, "insert into %s values (%d,0,0,0,0", TableName, i);
				strcat(buf, ASRCmdFieldVals);
				strcat(buf, ")");
				string sql(buf);
				mysql_query(&mysql, sql.c_str());
			}
		}

		// 更新天级统计
		{
			// 生成上一天的统计
			/*
				注册用户数
				在线用户数
				ASR业务调用次数, 按命令词分解?
				用户操作次数
			*/

			if (mLastDateTMReal != -1)
			{
				
			}
		}

		{
		}
	}
	
	return 0;
}

int ASREventRecordStatCfg_Internel(ASR_REC_CFG * pASRRecCfg)
{
	//printf("\nASREventRecordStatCfg_Internel() entered.");

	printf("\nASREventRecordStatCfg_Internel():pASRRecCfg->DBName:%s, pASRRecCfg->HtmlSubDir:%s.", pASRRecCfg->DBName, pASRRecCfg->HtmlSubDir);

	if (pASRRecCfg->DBName[0] == 0)
	{
		printf("\nASREventRecordStatCfg(), error, DBName is null.");

		return -1;
	}

	// 数据库初始化
	if (!pASRAnalyser)
	{
		pthread_mutex_init(&mutex,NULL);
		
		printf("\n----------------------> pASRAnalyser Started 1. <-------------------, mutex:%d.", mutex);
		
		pASRAnalyser = new ASRAnalyser(pASRRecCfg->DBName);
		if (!pASRAnalyser)
		{
			printf("\nCreate ASRAnalyser fail.");

			return -1;
		}

		int r = pASRAnalyser->InitDB();
		if (!r)
		{
			return 0;
		}
	}
	else
	{
		//printf("\n----------------------> pASRAnalyser already inited. <-------------------.");
		
		if (pASRRecCfg->DBName[0] != 0)
		{
			printf("\nASREventRecordStatCfg(), Already cfged, deny.");

			return -1;
		}
	}
	
	pASRAnalyser->mASR_REC_CFG = pASRRecCfg[0];

	// 创建html子目录
	{
		char cmd[200];
		//sprintf(cmd, "mkdir %s/%s", pASRAnalyser->mHtmlRootDir, pASRAnalyser->mASR_REC_CFG.HtmlSubDir); // /var/www/html/
		//system(cmd);
		sprintf(cmd, "%s/%s", pASRAnalyser->mHtmlRootDir, pASRAnalyser->mASR_REC_CFG.HtmlSubDir); // /var/www/html/
		int r = pASRAnalyser->CreateDir(cmd);
		if (r <= 0)
		{
			return 0;
		}
		
		puts("\n");
		puts(cmd); // test
		puts("\n");
	}

	
	return 1;
}

// 事件记录
int ASREventRecordStatRep_Internel(char *DBName, ASR_EVENT * pEvent)
{
    time_t now;
	//char DBName2[100];

	// 数据库初始化
	if (!pASRAnalyser)
	{
		printf("\n----------------------> pASRAnalyser Not Start. <-------------------.");

		return -1;
		/*
		pthread_mutex_init(&mutex,NULL);
		
		pASRAnalyser = new ASRAnalyser(DBName);
		if (!pASRAnalyser)
		{
			printf("\nCreate ASRAnalyser fail.");

			return -1;
		}

		pASRAnalyser->InitDB();
		*/
	}

	// 缺省配置
	if (pASRAnalyser->mASR_REC_CFG.DBName[0] == 0)
	{
		printf("\nASREventRecordStatRep_Internel(), pASRAnalyser->mASR_REC_CFG.DBName[] is null.");
		
		ASR_REC_CFG ASRRecCfg;
		strcpy(ASRRecCfg.DBName, "epiano");
		strcpy(ASRRecCfg.HtmlSubDir, ""); // /var/www/html/");
		ASREventRecordStatCfg_Internel(&ASRRecCfg);
	}

	// 接收事件计数
	pASRAnalyser->mEventInCount++;
	if (pEvent->eType < EVENT_TYPE_BUTT)
	{
		pASRAnalyser->mASREventRcvStat[pEvent->eType]++;
	}
	if (pEvent->eType != EVENT_TYPE_KEEP_ALIVE)
	{
		printf("\nASREventRecordStatRep_Internel(), new event:%d.", pEvent->eType);
	}	

	// push to queue

	// 取当前时间
	//strcpy(pASRAnalyser->mCurDate, datetime('-'));
    time(&now);
	if (pASRAnalyser->mCurDateTM == -1);
	{
		pASRAnalyser->mCurDateTM = now;
	}

//#define QUEUE_MOD //如果应用层进程退出, 可导致该事件消息来不及处理/入库
#ifdef QUEUE_MOD	
	pthread_mutex_lock(&mutex);
	
	//printf("\nASREventRecordStatRep(), new zzzzzzzzzzzzzzzzzzzzzzz now:%d.", now);

	if (now == 0)
	{
		//printf("\nzzzzzzzzzzzzzzzzzzzzzzz input now is 0.");
		//exit(0); // test
		pthread_mutex_unlock(&mutex);

		return -1;
	}
	
	int r = ASR_EVENT_QUEUE_In(&gASREventQue, DBName, pEvent, now);
	if (r < 0)
	{
		printf("\nFatal error, ASR record Event queue full. Event lost.");

		pthread_mutex_unlock(&mutex);

		return -1;
	}

	pthread_mutex_unlock(&mutex);
#else
	pthread_mutex_lock(&mutex);
	
	ASREventRecordStatRepPorccess(DBName, pEvent, now);

	pthread_mutex_unlock(&mutex);
#endif
	return 1;
}


/////////////////////////// API ///////////////////////////////
// 接收应用层事件总接口


// 配置
ASR_API int ASREventRecordStatCfg(ASR_REC_CFG * pASRRecCfg)
{
	printf("\nASREventRecordStatCfg():pASRRecCfg->DBName:%s, pASRRecCfg->HtmlSubDir:%s.", pASRRecCfg->DBName, pASRRecCfg->HtmlSubDir);
	
	if (pASRRecCfg->DBName[0] == 0)
	{
		printf("\nASREventRecordStatCfg(), error, DBName is null.");

		return -1;
	}
	
	//printf("\nASREventRecordStatCfg() 2.");

	ASR_EVENT e;
	e.eType = EVENT_TYPE_CFG;
	e.u_data.Cfg = pASRRecCfg[0];

	UDPSend((char *)&e, sizeof(ASR_EVENT));
	
	return 1;
}

// 事件记录
ASR_API int ASREventRecordStatRep(char *DBName, ASR_EVENT * pEvent)
{
	/**/
	printf("\nASREventRecordStatRep(),NAME:%x %x %x, E:%x %x %x.", 
		DBName[0], DBName[1], DBName[2],
		((char*)pEvent)[0], ((char*)pEvent)[1], ((char*)pEvent)[2]);
		
	// test
	if (pEvent->eType == EVENT_TYPE_ASR_INSERT)
	{
		printf("\nASREventRecordStatRep(), EVENT_TYPE_ASR_INSERT, username:%s.", pEvent->u_data.AsrInsert.UserName);
	}
	
	
	UDPSend((char *)pEvent, sizeof(ASR_EVENT));	
    
	return 1;
}




