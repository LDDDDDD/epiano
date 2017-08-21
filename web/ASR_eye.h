
#pragma once

#include "ASRStatApi.h"

#define ASR_DB_NAME "epiano" // "ASR_DB"
//#define ASR_DB_NAME_TEST "ASR_DB_TEST"

#define ASR_EYE_BIND_PORT 8800

#define UUIDLEN 36

#if 0
#define MAX_USER_NAME_LEN 10
#define MAX_ASR_RESULT_LEN 100
#define MAX_PATH_FILE_NAME_LEN 100
#define MAX_AUDIO_FILE_NAME_LEN 100
#define MAX_SESSIONID_LEN 33
#define MAX_ACTION_DESC_LEN 20

enum EVENT_TYPE
{
	EVENT_TYPE_NULL,

	EVENT_TYPE_USER_REG,
	EVENT_TYPE_USER_DEREG,

	EVENT_TYPE_STAT_REP,		// 周期性的报告, 比如5分钟
		
	EVENT_TYPE_ASR_INSERT,

	EVENT_TYPE_BUT,
};

/*
enum AUDIO_CMD_TYPE
{
	AUDIO_CMD_HUJIAO,
	AUDIO_CMD_LA,
	AUDIO_CMD_TIANJIA,
	AUDIO_CMD_YUANCHENGZHUOMIAN,
	AUDIO_CMD_FAQIHUIYI,

	AUDIO_CMD_BUT,
};
*/

// 字段操作方式
enum FIELD_OP
{
	FIELD_OP_INC,
	FIELD_OP_DEC,

	FIELD_OP_SET,

	FIELD_OP_BUTT,
};

// 字段定义
enum STATUS_FIELD
{
	STATUS_FIELD_REG_USER_COUNT,			// 注册用户数
	STATUS_FIELD_ACTIVE_USER_COUNT,			// 活动用户数
	STATUS_FIELD_ASR_COUNT,					// ASR调用次数
	STATUS_FIELD_ASR_FAIL_COUNT,			// ASR失败次数
	STATUS_FIELD_TRADITIONAL_ACT_COUNT,		// 传统操作次数

	STATUS_FIELD_ASR_CMD_HUJIAO,
	STATUS_FIELD_ASR_CMD_LA,
	STATUS_FIELD_ASR_CMD_TIANJIA,
	STATUS_FIELD_ASR_CMD_YUANCHENGZHUOMIAN,
	STATUS_FIELD_ASR_CMD_FAQIHUIYI,

	STATUS_FIELD_BUT,
};


#pragma   pack(push,1)

// 动态表格定义表
#define MAX_TABLE_NAME_LEN 20
#define MAX_FIELD_VAL_STR_LEN 30
typedef struct S_TABLE_DEF
{
	//int id;
	int TblId;
	char TableName[MAX_TABLE_NAME_LEN];
} TABLE_DEF;

// 动态字段定义表
#define MAX_FIELD_NAME_LEN 20
#define MAX_FIELD_VAL_STR_LEN 30
typedef struct S_FIELD_DEF
{
	//int id;
	int TblId;
	char FieldName[MAX_FIELD_NAME_LEN];
	FIELD_OP FieldOp;

	union U_FIELD_VALUE
	{
		unsigned long val_DWORD;
		char val_STR[MAX_FIELD_VAL_STR_LEN];
	} val;
} FIELD_DEF;


// 周期统计
#define MAX_AUDIO_CMD_NUM 50
struct S_ASR_AUDIO_CMD_STAT
{
	int CmdSuccessCount;
	int CmdFailureCount;
};
struct S_ASR_STAT_REP
{
	char StartTime[20];
	int RegUserNum;
	int ActiveUserNum;
	float ASRRate;
	int TraditionalActNum;
	int ASRNum; 
	int ASRFailNum; 
	struct S_ASR_AUDIO_CMD_STAT CmdStat[MAX_AUDIO_CMD_NUM];		
};



// ASR数据记录INSERT接口
// EVENT_TYPE_ASR_INSERT
typedef struct S_STAT_ASR_INSERT
{
	char StartTime[20];									// 发生时间
	char SessionId[MAX_SESSIONID_LEN];					// 会话ID, 全局唯一
	char UserName[MAX_USER_NAME_LEN];					// 用户名
	char ASRResultStr[MAX_ASR_RESULT_LEN];				// ARS识别结果/文字
	char AudioFilePathName[MAX_AUDIO_FILE_NAME_LEN];	// 可生成audiofile文件名
	int ASRE2EDurationInMs;
	int ASRRecongitioanDurationInMs;
	
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
	struct S_STAT_ASR_INSERT AsrInsert;
	struct S_ASR_UPDATE AsrUpdate;
	struct S_STAT_HABIT Habit;
	//struct S_ASR_STAT_REP statrep;
	//
};

// 记录事件
typedef struct S_ASR_EVENT
{
	enum EVENT_TYPE eType;

	union U_ASR_EVENT u_data;
} ASR_EVENT;


struct S_ASR_REC
{
	int RegUserCount;			// 注册用户数
	int ActUserCount;			// 活动用户数
	int ASRRate;				// 语音识别话务量, 次/秒
	int TraditionalActCount;	// 传统(键盘，鼠标)操作次数
	int ASRCount;				// 语音识别次数
	int ASRFailCount;			// 语音识别失败次数
	int ASRCmdTimes[100];		// 各命令词使用次数
	int ASRCmdFailTimes[100];	// 各命令词失败次数
	
};

#pragma   pack(pop)
#endif

typedef unsigned char  byte;

//#define HTML_ROOT_DIR "/var/www/html"
#define HTML_ROOT_DIR "/data/www/"


#define VERSION 3
#define MAX_ASR_CMD_NUM 200
#define VAR_HEAD "__VAR__"
#define VAR_END "__"

enum SINGLE_ITEM
{
	ITEM_REG_USER_NUM,
	ITEM_ACT_USER_NUM,
	ITEM_ALL_ASR_NUM,
	ITEM_ALL_OP_NUM,
	ITEM_ACCURACY_RATIO,

	SINGLE_ITEM_BUTT,
};

enum MSG_TYPE
{
	MSG_TYPE_NULL,
		
	MSG_TYPE_REG,
	MSG_TYPE_REG_ACK,
	MSG_TYPE_REG_ACK_UNREG,
	MSG_TYPE_REG_ACK_WRONG_PWD,
	MSG_TYPE_REG_TIMEOUT,

	MSG_TYPE_GET_FRIEND,
	MSG_TYPE_GET_FRIEND_ACK,
	

	MSG_TYPE_CALL,
	MSG_TYPE_CALL_ACK,	
 
	MSG_TYPE_BUTT,
};



class CUdpPk
{
public:
	int MAX_UDP_SIZE; // = 2048;
	byte *udppk; // = new byte[MAX_UDP_SIZE];
	int udppkLen; // = 0;
	int mSN; // = 0;
	int mMsgType;	// MSG_TYPE
	int mSock;

	CUdpPk(int sock)
	{
		mSock = sock;
		
		MAX_UDP_SIZE = 2048;
		udppk = new byte[MAX_UDP_SIZE];
		udppkLen = 0;
		mSN = 0;

		mLastSN = 0;
		mBadCrcPkNum = 0;
		mRcvPks = 0;
		mSorSendPksSNRewindTime = 0;
		mCurJitter = 0;
		mLostPkNum = 0;
		mLostPkNumInPeriod = 0;		
	};
	
	~CUdpPk(void)
	{
		delete udppk;
	};

	int Send(int s, void * to, char * data, int datalen); // struct sockaddr_in *to
	
	// 把short数据value存入by + offset和by + offset + 1位置
//		void setushort(byte by[], int offset, int value)
//		{
//			by[offset] 		= (byte)(value); // & 0xFF);
//			by[offset + 1] 	= (byte)((value >> 8)); // & 0xFF);
//		}
	void setushort(byte by[], int offset, int value)
	{
		by[offset] 		= (byte)(value & 0xFF);
		by[offset + 1] 	= (byte)((value >> 8) & 0xFF);
	};
	void setint(byte by[], int offset, int value)
	{
		by[offset] 		= (byte)(value & 0xFF);
		by[offset + 1] 	= (byte)((value >> 8) & 0xFF);
		by[offset + 2] 	= (byte)((value >> 16) & 0xFF);
		by[offset + 3] 	= (byte)((value >> 24) & 0xFF);
	};	
	int getushort(byte by[], int offset)
	{
//				short v1 = (short)(by[offset] & 0xFF);
//				short v2 = (short)(by[offset + 1] & 0xFF);				
//				short value = (short)((v2 << 8) + v1);
//				int value = (int)(((by[offset + 1] & 0xff) << 8) | (by[offset] & 0xff));
//				int value = (int)(((int)((by[offset + 1]) << 8)) | (by[offset]));
		int targets = (by[offset] & 0xff) | ((by[offset + 1] << 8) & 0xff00);
		
		return targets;
	};
	int getint(byte by[], int offset)
	{			
		int targets = (by[offset] & 0xff) | ((by[offset + 1] << 8) & 0xff00)
				| ((by[offset + 2] << 16) & 0xff0000) | ((by[offset + 3] << 24) & 0xff000000);
		return targets;
	};
	int getBytes(byte by[], int offset, byte out[], int len)
	{			
		//System.arraycopy(by, offset, out, 0, len);
		memcpy(out, by + offset, len);
		
		return 1;
	}
	void AddItem(byte pudppk[], byte data[], int datalen, int MaxLen, int pos)
	{
		//int l = Math.min(data.length, MaxLen);
		//System.arraycopy(data, 0, pudppk, pos, l);
		int l = datalen < MaxLen? datalen:MaxLen;
		memcpy(pudppk + pos, data, datalen);
		SetZero((char *)pudppk, pos + l, MaxLen - l);
	}	
	// 计算crc, bytelen必须为2的倍数
	short GetCRC16(char data[], int offset, int bytelen)
	{
		/*
		short crc = 0;
//    				int shortlen = bytelen / 2;
		short shortd;
		
		for(int i = 0 + offset; i < bytelen; i += 2)
		{
			shortd = (short)((((short)data[i]) << 8) + data[i+1]);
			
			crc ^= shortd;
		}
			
		return crc;
		*/

	    char crc0 = 0;
		char crc1 = 0;
		for(int i = 0 + offset; i < bytelen; i += 2)
		{
			crc0 ^= data[i];
			crc1 ^= data[i+1];
		}

		//short crcR = (short)((short)(crc0 << 8) | crc1);
		short crcR = (short)((crc0 & 0xff) | ((crc1 << 8) & 0xff00));
		
		return crcR;
	}

	unsigned long get_time_ms();

	void SetZero(char dst[], int offset, int len) {
		for (int i = 0; i < len; i++) {
			dst[offset + i] = 0;
		}
	};

	// 生成一个udp包
	void SetUdpTick(long tick)
	{
		//setushort(udppk, 2, (int)tick);
		setint((byte*)udppk, 2, (int)tick);
	}

	// 计算udp包crc
	void CRCUdpPkt()
	{			
		int i = 0;

		// 保证长度是偶数
		if ((udppkLen % 2) == 1)
		{
			udppkLen++;
		}
		
		int CRC16 = GetCRC16((char*)udppk, 2, udppkLen - 2);
		setushort((byte*)udppk, i, CRC16);
	}

	// 生成一个udp包
	int FromUdpPkt(int MsgType, byte uuid[])
	{
		//long tick = System.currentTimeMillis() & 0xFFFF;
		
		int i = 0;

		// Hdr
		{
//			setushort(udppk, i, mCRC16);
			i += 2;

//			setushort(udppk, i, tick);
//			i += 2;
//			setint(udppk, i, tick);
			i += 4;

			setushort(udppk, i, mSN++);				// 包序列号
			i += 2;
			

			setushort(udppk, i, MsgType);		// 包消息类型
			i += 2;

			// send Report
			// sendbyte
//			setint(udppk, i, (int)ByteSendToal);
			i += 4;	

			// uuid
			AddItem(udppk, uuid, UUIDLEN, UUIDLEN, i);
			i += UUIDLEN;			
		}

		udppkLen = i;

		// crc计算
		//i = 0;
		//mCRC16 = GetCRC16(udppk, 2, udppkLen - 2);
		//setushort(udppk, i, mCRC16);
		
		return udppkLen;
	};

	int GetTableRowCount(char * TableName, char *where_condition);

	// 注册消息
	// byte usrname[], byte pwd[], byte localIP[], int localPort, byte
	// PhoneModel[], byte MAC[], int NatType
	int AppendRegMsgAck2UdpPkt(int ret, int SessionId, int usrid)
	{
		int offset = udppkLen;
		int i = offset;
		int l = 0;

		{
			// T
			setushort(udppk, i, 0); // (int)UDP_ITEM.UDP_ITEM_REG.ordinal());
			i += 2;

			// L
			// udppk[udppkLen + i] = lenIn;
			i += 2;

			// V

			/*
			// id, 用户名, PWD, Local IP, port, phone model, MAC, NAT type,
			l = Math.min(usrid.length, USRIDLEN);
			System.arraycopy(usrid, 0, udppk, i, l);
			SetZero(udppk, i + l, USRIDLEN - l);
			i += USRIDLEN;

			l = Math.min(usrname.length, USRNAMELEN);
			System.arraycopy(usrname, 0, udppk, i, l);
			SetZero(udppk, i + l, USRNAMELEN - l);
			i += USRNAMELEN;

			l = Math.min(pwd.length, PWDLEN);
			System.arraycopy(pwd, 0, udppk, i, l);
			SetZero(udppk, i + l, PWDLEN - l);
			i += PWDLEN;
			*/
			setint(udppk, i, (int)SessionId);
			i += 4;	
			
			udppk[i] = ret; // 0: success, 1: unreged user, 2: bad pwd
			i += 1;
			
			setint(udppk, i, (int)usrid);
			i += 4;	
		}

		int itemLen = i - offset; // 长度含T L V

		// L
		setushort(udppk, offset + 2, itemLen);

		udppkLen += itemLen;

		return itemLen;
	}

	// 获取朋友消息
	// byte usrname[], byte pwd[], byte localIP[], int localPort, byte
	// PhoneModel[], byte MAC[], int NatType
	int AppendGetFriendsMsgAck2UdpPkt(int UserId, char * UserName)
	{
		int offset = udppkLen;
		int i = offset;
		int l = 0;

		{
			// T
			setushort(udppk, i, 0); // (int)UDP_ITEM.UDP_ITEM_REG.ordinal());
			i += 2;

			// L
			// udppk[udppkLen + i] = lenIn;
			i += 2;

			// V

			/*
			// id, 用户名, PWD, Local IP, port, phone model, MAC, NAT type,
			l = Math.min(usrid.length, USRIDLEN);
			System.arraycopy(usrid, 0, udppk, i, l);
			SetZero(udppk, i + l, USRIDLEN - l);
			i += USRIDLEN;

			l = Math.min(usrname.length, USRNAMELEN);
			System.arraycopy(usrname, 0, udppk, i, l);
			SetZero(udppk, i + l, USRNAMELEN - l);
			i += USRNAMELEN;

			l = Math.min(pwd.length, PWDLEN);
			System.arraycopy(pwd, 0, udppk, i, l);
			SetZero(udppk, i + l, PWDLEN - l);
			i += PWDLEN;
			*/
			//setint(udppk, i, (int)SessionId);
			//i += 4;	
			
			//udppk[i] = ret; // 0: success, 1: unreged user, 2: bad pwd
			//i += 1;

			setint((byte*)udppk, i, (int)UserId);
			i += 4;
			
			AddItem(udppk, (byte *)UserName, strlen(UserName), USRNAMELEN, i);
			i += USRNAMELEN;			
			
		}

		int itemLen = i - offset; // 长度含T L V

		// L
		setushort(udppk, offset + 2, itemLen);

		printf("\n--------------L:%d", itemLen);

		udppkLen += itemLen;

		return itemLen;
	}

	/*
    public void UdpSend(InetAddress address, int port) // , byte data[], int datalen)
	{  
    	//if (false)
    	{
	    	try {  
//	            InetAddress address = InetAddress.getByName("192.168.1.80");  //服务器地址  
//	            int port = 8080;  //服务器的端口号  
	            //创建发送方的数据报信息  
	            DatagramPacket dataGramPacket = new DatagramPacket(udppk, udppkLen, address, port);  
	              
//	            DatagramSocket socket = new DatagramSocket();  //创建套接字  
	            socket.send(dataGramPacket);  //通过套接字发送数据  
//	            socket.close();
//	        } catch (UnknownHostException e) {  
//	            e.printStackTrace();  
	        } catch (IOException e) {  
	            e.printStackTrace();  
	        }
    	}
    }
    */

	/*
	// 向udp包追加反向信元
	int AppendReverseItem2UdpPkt(long tick)
	{
		int offset = udppkLen;
		int i = offset;
		
		// B向A反馈
		{
			// T
			setushort(udppk, i, (int)UDP_ITEM.UDP_ITEM_BA.ordinal());
			i += 2;

			// L
			//udppk[udppkLen + i] = lenIn;
			i += 2;

			// V
			
			//setushort(udppk, i, decctl.IFrmReq);				// I帧请求
			//i += 2;
			//setushort(udppk, i, decctl.ActRcvBw);	// 
			//i += 2;
			//setushort(udppk, i, decctl.ActLostRatio * 100);	// 
			//i += 2;
			//setushort(udppk, i, decctl.ActLostBurst);	// 
			//i += 2;
			udppk[i] = (byte)decctl.IFrmReq;
			i++;
			//int rcvbw = mStat.GetRcvBw(3) / 100;
			int rcvbw = decctl.BwCtl / 100;			// 控制发端带宽
			setushort(udppk, i, rcvbw); // decctl.ActRcvBw / 100);	// 
			i += 2;
			udppk[i] = (byte)decctl.ActLostRatio;
			i++;
			udppk[i] = (byte)decctl.ActLostBurst;
			i++;

//				Log.e(TAG, "AppendReverseItem2UdpPkt: rcvbw: " + rcvbw
//					+ ", ActLostRatio: " + decctl.ActLostRatio
//					);
			
			setushort(udppk, i, decctl.mAckFrmId);	// 
			i += 2;				
			// TTL测量, 本端为B端, 向A端发送估计的A端时刻, A端用该时刻与最新时刻比较, 得到TTL
			if (decctl.TTL_Sor != -1)
			{
				int tick_hold = GetUsTickDiff((int)tick, (int)decctl.TTL_Local);
				int SorEstmate = (int)(decctl.TTL_Sor) + tick_hold;
				setint(udppk, i, SorEstmate);	// 

				// important print
//		    		Log.i(TAG, "SendOnePkt()" 
//						+ ", tickA:" + tick
//						+ ", tickB_local:" + decctl.TTL_Local
//						+ ", TTL_Sor:" + decctl.TTL_Sor
//						+ ", tick_hold:" + tick_hold
//						+ ", decctl.TTL_Local:" + decctl.TTL_Local
//						+ ", SorEstmate:" + SorEstmate
//						+ ", est ttl:" + (GetUsTickDiff((int)tick, SorEstmate))
//						);

				// curve output
				//String s1 = String.valueOf(tick_hold);
				if (IntTestSwitch) IntTestOut("ttl_hold", String.valueOf(tick_hold)); // CURVE_TYPE_NUMBER, 
			}
			else
			{
				setint(udppk, i, 0);	// 
			}
			decctl.TTL_Sor = -1;
			i += 4;
		}

		int itemLen = i - offset; // 长度含T L V

		// L
		setushort(udppk, offset + 2, itemLen);

		udppkLen += itemLen;

		return itemLen;

	}
	*/

	int GetItemAB(byte Udpdata[], int offset, long tick)
	{
		int i = offset; // offset: item V的开始位置
		
		int TTL_Sor = getint(Udpdata, i); // & 0xFFFF;
		i += 4;
		
		return i - offset;
	}

	// 解码一个udp包
	int mLastSN;
	int mBadCrcPkNum;
	int mRcvPks;
	int mSorSendPksSNRewindTime;
	int mCurJitter;
	int mLostPkNum;
	int mLostPkNumInPeriod;
	byte uuid[UUIDLEN + 1];
	//String uuidS;

	int GetUserIdFromTbl(char * UserName);
	char * GetUserNameFromTbl(int userid);	
	int RegMsgHandler(byte * uuid, int i, byte * Udpdata, int udplen, char * PeerPubIp, int PeerPubPort, struct sockaddr_in * from);
	int GetFriendMsgHandler(byte * uuid, int i, byte * Udpdata, int udplen, char * PeerPubIp, int PeerPubPort, struct sockaddr_in * from);
	int RegUsrInsert(byte * uuid, REG_USER_INSERT *AsrInsert, struct sockaddr_in *from);
	int GetUserFriends(byte * uuid, GET_USER_FRIEND *AsrInsert, struct sockaddr_in *from);
	
	int ProccessUdpPkt(byte * Udpdata, int udplen, char * PeerPubIp, int PeerPubPort, struct sockaddr_in * from)
	{
		int i = 0;
		
		short mCRC16;
		
		short mCRC16Calc;
		int mSN;
		int mMsgType;
		long SorSendByte;

		// 计算包的crc值
		mCRC16Calc = GetCRC16((char*)Udpdata, 2, udplen - 2);
		//byte xx[] = new byte[2];
		//setushort(xx, 0, mCRC16Calc);				
		
		long tick = get_time_ms(); // System.currentTimeMillis();
		//int tickcut0 = (int)((short)tick & 0xFFFF);
		//int tickcut = (int)(tick & 0xFFFF);
		long tickcut = tick & 0xFFFFFFFF;
		
		i = 0;

		int Tag = 0;
		int L = 0; // 含 T L V总长
		
		// 取发端crc
		mCRC16 = (short)getushort(Udpdata, i);
		i += 2;
		
		//int ts = getushort(Udpdata, i); // & 0xFFFF;
		int ts = getint(Udpdata, i); // & 0xFFFF;
		i += 4;
		
		// 反馈: 本端B向A
		// TTL测量, 对端A发起TTL测量, 本端B记录A端时刻和本端收到消息的时刻
		/*
		if (TTL_Sor == -1)
		{
			TTL_Sor = ts;
			TTL_Local = tickcut;
		}
		*/
		
		// test
//				{
//					int r = GetUsTickDiffSign(5, 65505);
//					int z = r;
//				}
		
		mSN = getushort(Udpdata, i); // & 0xFFFF;
		i += 2;
		
		mMsgType = getushort(Udpdata, i); // & 0xFFFF;
		i += 2;

		// send report
		SorSendByte = getint(Udpdata, i); // & 0xFFFF;
		SorSendByte *= 10;
		i += 4;

		if (mCRC16Calc != mCRC16)
		{
			mBadCrcPkNum++;
			//mStat.pushval(NETSTAT.NETSTAT_RcvBadCrcPks, mBadCrcPkNum);

			/*
			Log.i(TAG, "rcv udp pks, crc check fail!, "
					+ ", mCRC16Calc:" + mCRC16Calc
					+ ", mCRC16:" + mCRC16
            		+ ", mSN:" + mSN
//		            		+ ", m264FrmId:" + m264FrmId
//		            		+ ", m264PkIdInGrp:" + m264PkIdInGrp
////		            		+ ", mFecIdInGrp:" + mFecIdInGrp
//		            		+ ", OrgPkNum_N:" + OrgPkNum_N
//		            		+ ", FecPkNum_n:" + FecPkNum_n
            		);
            */
            printf("\nrcv udp pks, crc check fail!(Calc:0x%x_0x%x) mSN:%d, mMsgType:%d.", 
            	mCRC16Calc, mCRC16, mSN, mMsgType);
			
			return 0;
		}

		// uuid			
		getBytes(Udpdata, i, uuid, UUIDLEN);
		//uuidS = uuid.toString();
		uuid[UUIDLEN] = 0;
		i += UUIDLEN;

		mRcvPks++;

		
		// jitter更新
		//mCurJitter = jitter.GetJitter(ts, tickcut);
		// curve output
		//if (IntTestSwitch) IntTestOut("Jitter", String.valueOf((int)mCurJitter)); // CURVE_TYPE_NUMBER, 
		
		
		// 源端发送总字节数
		//if (mSorSendByte < SorSendByte)
		//{
		//	mSorSendByte = SorSendByte;
		//}
		
		// mSN回滚计数
		if (mLastSN > mSN)
		{
			if (mLastSN - mSN >= 32768)
			{
				mSorSendPksSNRewindTime++;
			}
		}
		// 源端发送总包数
		//mSorSendPks = mSorSendPksSNRewindTime * 65536 + mSN;
		
		// 丢包统计
		if (mLastSN == -1)
		{
			
		}
		else
		{
			int lostnum = 0;
			if (mSN > mLastSN) // 忽略回滚和乱序
			{
				lostnum = mSN - mLastSN;
				lostnum--;
			}
//					if (lostnum <= 0)
//					{
//						lostnum = -lostnum;
//					}
//					if (lostnum > 32768)
//					{
//						lostnum = 0xFFFF - lostnum;
//					}					
			mLostPkNum += lostnum;
			mLostPkNumInPeriod += lostnum;
		}
		mLastSN = mSN;

		// 解包
		int dataleft = udplen - i;
		switch(mMsgType)
		{
		case MSG_TYPE_REG:
			RegMsgHandler(uuid, i, Udpdata, udplen, PeerPubIp, PeerPubPort, from);
			break;
		case MSG_TYPE_GET_FRIEND:
			GetFriendMsgHandler(uuid, i, Udpdata, udplen, PeerPubIp, PeerPubPort, from);
			break;
		default:
			printf("\nrcv udp pks, unknown msg: %d.", mMsgType);
			break;
		}
		

		// important printf, may highly encrease delay
		// 264data: Udpdata + i;
//				if (mSN % 10 == 0)
//				{
//					Log.e(TAG, "rcv udp pks"
//		            		+ ", mSN:" + mSN
//		            		+ ", SorSendByte:" + SorSendByte
//	//	            		+ ", mFrmType(I1,P2):" + mFrmType
//	//	            		+ ", mNalCount:" + mNalCount
//	//	            		+ ", m264FrmId:" + m264FrmId
//	//	            		+ ", m264PkIdInGrp:" + m264PkIdInGrp
//	//	            		+ ", OrgPkNum_N:" + OrgPkNum_N
//	//	            		+ ", FecPkNum_n:" + FecPkNum_n
//	//	            		+ ", TTL_Sor:" + TTL_Sor
//	//	            		+ ", TTL_Local:" + TTL_Local
//		            		+ ", rtt:" + encctl.TTL_Result
//		            		+ ", jt:" + mCurJitter
//		            		+ ", lospks:" + mLostPkNum
//		            		);
//				}

		return 1;
	}
};

class ASRAnalyser
{
public:
	ASRAnalyser(char *DBName);
	~ASRAnalyser();


	ASR_REC_CFG mASR_REC_CFG;
	char mHtmlRootDir[300];	// var/www/html/
	
	ASR_CMD mASRCmdSet[MAX_ASR_CMD_NUM];
	int 	mASRCmdSetSize;

	// 事件统计
	unsigned int mASREventRcvStat[EVENT_TYPE_BUTT];
	unsigned int mEventInCount;	// 接收事件总数
	unsigned int mEventProcessCount;	// 处理事件总数
	float mEventInSpeed;	// 接收事件速率
	float mEventPrcessSpeed;// 处理事件速率

	char mDBName[100];

	char mLastDateTime[30];
	int mLastTimeInSec;
	char mCurDateTime[30];

	long mLastDateTM;		// linux localtime
	long mLastDateTMReal;	// linux localtime
	
	int mLastDate;
	int mLastDateReal;		// 昨天
	int mLastHour;
	int mLastMin;
	int mLastSec;

	int mLastTimeSegId;		// 每天按片(比如5分钟)分段, 第x片
	int mCurTimeSegId;		// 每天按片(比如5分钟)分段, 第x片
	int mTimeSegLongInSec;	// 时分分片长度: 300秒
	long mStartTMThisDay;	// linux localtime

	long mCurDateTM;		// linux localtime
	
	int mCurDate;
	int mCurHour;
	int mCurMin;
	int mCurSec;

	int mSingleItemStat[SINGLE_ITEM_BUTT];

	int ASREventRecordASRAct(ASR_EVENT * pEvent);

	int InitDB();

	int CreateDir(char *sPathName);	

	// 注册表格
	//int RegTable(TABLE_DEF * pTableDef);
	// 注册"表格"字段
	//int RegField(FIELD_DEF * pFieldDef);

	// no use
	int ASREventRecordCfg(ASR_REC_CFG * pCfg);

	int ASREventRecordStatRep(ASR_EVENT * pEvent);

	int ASREventRecord(ASR_EVENT * pEvent);

	int ASREventGenHtml();
	int ASREventGenHtml_IndexHtml();
	// 查询VarLable位置
	int FindNextVARLabel(char *buf, int StartSearchPos, int BufDataSize, int *VarPos, int *VarEndPos);
	int ASREventGenHtml_ReplaceLabel(int VARPos, int VAREndPos, char *OrgHtmlBuf, char *OutPutHtmlBuf, int *outputdatalen);
	int ASREventGenHtml_CmdStat(char *OutPutStr, char * CmdName);
	int ASREventGenHtml_CmdsStat(char *OutPutStr);
	// 统计各命令的使用总数/比例
	int ASREventGenHtml_CmdUsingTimesTblStat(char *OutPutStr);

	//int CodeConvert(char *encFrom, char *encTo, char *src, char *dst);

	// 增加命令的调用次数
	int IncCmdUseTimes(char * TableName, char *FieldName, char *WhereFieldName, char * CmdStr);
	// 获取命令的调用次数
	long long GetCmdUseTimes(char * TableName, char *FieldName, char *WhereFieldName, char * CmdStr);	

	// 获取命令及调用次数, 降序
	int GetCmdAndCallTimesDesc(char * TableName, char *CmdNameField, char *CmdDescField, char *CallTimesField, ASR_CMD *ASRCmdSet, int *ASRCmdSetSize);

	// 获取表格的记录数
	int GetTableRowCount(char * TableName, char *where_condition);
	int GetTableField(char * TableName, char *FieldName, char *where_condition, char * OutStr);

	// 判断天内的时间分段是否变化, 用于生成新的时间分片表格
	int OnTimeSegChanged();

	int UpdateInnerOneDayStat(int CurTimeSegId);
	
	// 判断日期是否变化, 用于生成新的天的表格
	int OnDayChanged();

	int GetTableName(char *TableNamePrefix, char *Sufix, char *OutPutName);

	////////////////////////////

	int AsrInsert(struct S_STAT_ASR_INSERT *AsrInsert);

	int AsrUpdate(struct S_ASR_UPDATE *AsrUpdate);

	int StatHabit(struct S_STAT_HABIT *Habit);

};

