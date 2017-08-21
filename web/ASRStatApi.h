//////////////////////////////////////////////////////////////////////////////////
//
// ASR统计分析API
// wanggeng, 2017.4.5
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_USER_NAME_LEN 10
#define MAX_ASR_RESULT_LEN 100
#define MAX_AUDIO_FILE_NAME_LEN 100
#define MAX_SESSIONID_LEN 64
#define MAX_ACTION_DESC_LEN 20

// 应用层向API通报的事件定义
enum EVENT_TYPE
{
	EVENT_TYPE_NULL,

	//EVENT_TYPE_USER_REG,
	//EVENT_TYPE_USER_DEREG,

	//EVENT_TYPE_STAT_REP,		// 周期性的报告, 比如5分钟	
		
	EVENT_TYPE_ASR_INSERT,		// ASR insert
	EVENT_TYPE_ASR_UPDATE,		// ASR update
	EVENT_TYPE_USR_HABIT,		// user habit
	
	EVENT_TYPE_KEEP_ALIVE,		// 
	
	EVENT_TYPE_CFG,				// Configure

	EVENT_TYPE_BUTT,
};


#pragma   pack(push,1)

// 配置
typedef struct S_ASR_REC_CFG
{
	//char RecRootDir[300];
	char HtmlSubDir[300];	// 在var/www/html/下的子目录名称, 比如test01, 相当于var/www/html/test01目录
	char DBName[300];
	// ASR_CMD [] tbd
} ASR_REC_CFG;

// ASR CMD
typedef struct S_ASR_CMD
{
	long long Times;
	char CmdStrName[50];	// CMD_xx
	char CmdStr[50];		// 拔打
} ASR_CMD;

// ASR数据记录INSERT接口
// EVENT_TYPE_ASR_INSERT
#define USRIDLEN  20
#define USRNAMELEN 20
#define PWDLEN  8
#define IPLEN  16
#define PHONEMODLEN 10
#define MACLEN  17			// xx:xx:xx:xx:xx:xx
typedef struct S_REG_USER_INSERT
{
	//char usrid[USRIDLEN];
	int SessionId;
	long usrid;
	char usrname[USRNAMELEN];
	char pwd[PWDLEN];
	char localIP[IPLEN];
	int localPort;
	char PubIP[IPLEN];
	int PubPort;	
	char PhoneModel[PHONEMODLEN];
	char MAC[MACLEN];
	int NatType;
	int RegAcked;
} REG_USER_INSERT;
typedef struct S_GET_USER_FRIEND
{
	//char usrid[USRIDLEN];
	int SessionId;
	long usrid;
	char usrname[USRNAMELEN];
	char PubIP[IPLEN];
	int PubPort;	
} GET_USER_FRIEND;

// ASR数据记录INSERT接口
// EVENT_TYPE_ASR_INSERT
typedef struct S_STAT_ASR_INSERT
{
	char Version[15];									// 版本号
	char StartTime[20];									// 发生时间
	char SessionId[MAX_SESSIONID_LEN];					// 会话ID, 全局唯一
	char UserName[MAX_USER_NAME_LEN];					// 用户名
	char ASRResultStr[MAX_ASR_RESULT_LEN];				// ARS识别结果/文字
	char AudioFilePathName[MAX_AUDIO_FILE_NAME_LEN];	// 可生成audiofile文件名
	//char AudioFilePathName_org[MAX_AUDIO_FILE_NAME_LEN];// 可生成audiofile文件名
	int ASRE2EDurationInMs;								// 端侧感知的ASR端到端时长(从认为收齐语音到收到服务器返回的文本结果)
	int ASRRecongitioanDurationInMs;					// 服务端感知的ASR识别时长(从收到最后一帧到识别出文本结果)
	
	//char * pAudioData;
	//unsigned long AudioDataLen;
} STAT_ASR_INSERT;
// UPDATE接口
typedef struct S_ASR_UPDATE
{
	char SessionId[MAX_SESSIONID_LEN];					// 会话ID, 全局唯一
	int ASRE2EDurationInMs;
} ASR_UPDATE;

// 个人使用习惯统计接口
typedef struct S_STAT_HABIT
{
	char UserName[MAX_USER_NAME_LEN];					// 用户名
	char StartTime[20];									// 发生时间
	char ActionDesc[MAX_ACTION_DESC_LEN];				// 行为描述
} STAT_HABIT;


union U_ASR_EVENT
{
	// EVENT_TYPE_ASR_INSERT
	struct S_ASR_REC_CFG Cfg;
	struct S_STAT_ASR_INSERT AsrInsert;
	struct S_ASR_UPDATE AsrUpdate;
	struct S_STAT_HABIT Habit;
	//struct S_ASR_STAT_REP statrep;
	//
};

// 事件通知接口
typedef struct S_ASR_EVENT
{
	enum EVENT_TYPE eType;

	union U_ASR_EVENT u_data;
} ASR_EVENT;

#pragma   pack(pop)

//#define THIS_IS_TEST_VER 1

/////////////////////////// API ///////////////////////////////

#define ASR_API 

// 配置
ASR_API int ASREventRecordStatCfg(ASR_REC_CFG * pASRRecCfg);

// 事件记录
ASR_API int ASREventRecordStatRep(char *DBName, ASR_EVENT * pEvent);

ASR_API int CodeConvert(char *encFrom, char *encTo, char *src, char *dst);

