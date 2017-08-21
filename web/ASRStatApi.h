//////////////////////////////////////////////////////////////////////////////////
//
// ASRͳ�Ʒ���API
// wanggeng, 2017.4.5
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_USER_NAME_LEN 10
#define MAX_ASR_RESULT_LEN 100
#define MAX_AUDIO_FILE_NAME_LEN 100
#define MAX_SESSIONID_LEN 64
#define MAX_ACTION_DESC_LEN 20

// Ӧ�ò���APIͨ�����¼�����
enum EVENT_TYPE
{
	EVENT_TYPE_NULL,

	//EVENT_TYPE_USER_REG,
	//EVENT_TYPE_USER_DEREG,

	//EVENT_TYPE_STAT_REP,		// �����Եı���, ����5����	
		
	EVENT_TYPE_ASR_INSERT,		// ASR insert
	EVENT_TYPE_ASR_UPDATE,		// ASR update
	EVENT_TYPE_USR_HABIT,		// user habit
	
	EVENT_TYPE_KEEP_ALIVE,		// 
	
	EVENT_TYPE_CFG,				// Configure

	EVENT_TYPE_BUTT,
};


#pragma   pack(push,1)

// ����
typedef struct S_ASR_REC_CFG
{
	//char RecRootDir[300];
	char HtmlSubDir[300];	// ��var/www/html/�µ���Ŀ¼����, ����test01, �൱��var/www/html/test01Ŀ¼
	char DBName[300];
	// ASR_CMD [] tbd
} ASR_REC_CFG;

// ASR CMD
typedef struct S_ASR_CMD
{
	long long Times;
	char CmdStrName[50];	// CMD_xx
	char CmdStr[50];		// �δ�
} ASR_CMD;

// ASR���ݼ�¼INSERT�ӿ�
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

// ASR���ݼ�¼INSERT�ӿ�
// EVENT_TYPE_ASR_INSERT
typedef struct S_STAT_ASR_INSERT
{
	char Version[15];									// �汾��
	char StartTime[20];									// ����ʱ��
	char SessionId[MAX_SESSIONID_LEN];					// �ỰID, ȫ��Ψһ
	char UserName[MAX_USER_NAME_LEN];					// �û���
	char ASRResultStr[MAX_ASR_RESULT_LEN];				// ARSʶ����/����
	char AudioFilePathName[MAX_AUDIO_FILE_NAME_LEN];	// ������audiofile�ļ���
	//char AudioFilePathName_org[MAX_AUDIO_FILE_NAME_LEN];// ������audiofile�ļ���
	int ASRE2EDurationInMs;								// �˲��֪��ASR�˵���ʱ��(����Ϊ�����������յ����������ص��ı����)
	int ASRRecongitioanDurationInMs;					// ����˸�֪��ASRʶ��ʱ��(���յ����һ֡��ʶ����ı����)
	
	//char * pAudioData;
	//unsigned long AudioDataLen;
} STAT_ASR_INSERT;
// UPDATE�ӿ�
typedef struct S_ASR_UPDATE
{
	char SessionId[MAX_SESSIONID_LEN];					// �ỰID, ȫ��Ψһ
	int ASRE2EDurationInMs;
} ASR_UPDATE;

// ����ʹ��ϰ��ͳ�ƽӿ�
typedef struct S_STAT_HABIT
{
	char UserName[MAX_USER_NAME_LEN];					// �û���
	char StartTime[20];									// ����ʱ��
	char ActionDesc[MAX_ACTION_DESC_LEN];				// ��Ϊ����
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

// �¼�֪ͨ�ӿ�
typedef struct S_ASR_EVENT
{
	enum EVENT_TYPE eType;

	union U_ASR_EVENT u_data;
} ASR_EVENT;

#pragma   pack(pop)

//#define THIS_IS_TEST_VER 1

/////////////////////////// API ///////////////////////////////

#define ASR_API 

// ����
ASR_API int ASREventRecordStatCfg(ASR_REC_CFG * pASRRecCfg);

// �¼���¼
ASR_API int ASREventRecordStatRep(char *DBName, ASR_EVENT * pEvent);

ASR_API int CodeConvert(char *encFrom, char *encTo, char *src, char *dst);

