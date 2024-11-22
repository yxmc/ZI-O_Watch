#include "MyRtc.h"


u16 MyRTC_Time[] = { 2024, 11, 6, 14, 10, 0, 3 };	//����ȫ�ֵ�ʱ������,�������ݷֱ�Ϊ�ꡢ�¡��ա�ʱ���֡��롢����

void MyRTC_SetTime(void);				//��������


/**
  * ��    ��:RTC��ʼ��
  * ��    ��:��
  * �� �� ֵ:��
  */
void MyRTC_Init(void)
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); //����PWR��ʱ�ӡ�BKP��ʱ��


	/*���ݼĴ�������ʹ��*/
	PWR_BackupAccessCmd(ENABLE);							//ʹ��PWR�����Ա��ݼĴ����ķ���

	/*����RTCʱ��*/
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)			//ͨ��д�뱸�ݼĴ����ı�־λ,�ж�RTC�Ƿ��ǵ�һ������
	{
		/* ���� LSE */
		RCC_LSEConfig(RCC_LSE_ON);

		/* �ȴ� LSE ׼������ */
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
		{
		}

		/* ѡ�� LSE ��Ϊ RTC ʱ��Դ */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

		/* ʹ�� RTC ʱ�� */
		RCC_RTCCLKCmd(ENABLE);

		RTC_WaitForSynchro();								//�ȴ�ͬ��
		RTC_WaitForLastTask();								//�ȴ���һ�β������

		RTC_SetPrescaler(32768 - 1);						//����RTCԤ��Ƶ��,Ԥ��Ƶ��ļ���Ƶ��Ϊ1Hz
		RTC_WaitForLastTask();								//�ȴ���һ�β������

		MyRTC_SetTime();									//����ʱ��,���ô˺���,ȫ��������ʱ��ֵˢ�µ�RTCӲ����·

		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);			//�ڱ��ݼĴ���д���Լ��涨�ı�־λ,�����ж�RTC�ǲ��ǵ�һ��ִ������
	}
	else													//RTC���ǵ�һ������
	{
		RTC_WaitForSynchro();								//�ȴ�ͬ��
		RTC_WaitForLastTask();								//�ȴ���һ�β������
	}


}

//���LSE�޷������³������ڳ�ʼ��������
//�ɽ���ʼ�������滻Ϊ��������,ʹ��LSI����RTCCLK
//LSI�޷��ɱ��õ�Դ����,����Դ����ʱ,RTC��ʱ����ͣ

//void MyRTC_Init(void)
//{
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
//	
//	PWR_BackupAccessCmd(ENABLE);
//	
//	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
//	{
//		RCC_LSICmd(ENABLE);
//		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);
//		
//		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
//		RCC_RTCCLKCmd(ENABLE);
//		
//		RTC_WaitForSynchro();
//		RTC_WaitForLastTask();
//		
//		RTC_SetPrescaler(42340 - 1);
//		RTC_WaitForLastTask();
//		
//		MyRTC_SetTime();
//		
//		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
//	}
//	else
//	{
//		RCC_LSICmd(ENABLE);				//��ʹ���ǵ�һ������,Ҳ��Ҫ�ٴο���LSIʱ��
//		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);
//		
//		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
//		RCC_RTCCLKCmd(ENABLE);
//		
//		RTC_WaitForSynchro();
//		RTC_WaitForLastTask();
//	}
//}

/**
  * ��    ��:RTC����ʱ��
  * ��    ��:��
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,ȫ��������ʱ��ֵ��ˢ�µ�RTCӲ����·
  */
void MyRTC_SetTime(void)
{
	time_t time_cnt;		//�������������������
	struct tm time_date;	//��������ʱ����������

	time_date.tm_year = MyRTC_Time[0] - 1900;		//�������ʱ�丳ֵ������ʱ��ṹ��
	time_date.tm_mon = MyRTC_Time[1] - 1;
	time_date.tm_mday = MyRTC_Time[2];
	time_date.tm_hour = MyRTC_Time[3];
	time_date.tm_min = MyRTC_Time[4];
	time_date.tm_sec = MyRTC_Time[5];

	time_cnt = mktime(&time_date) - 8 * 60 * 60;	//����mktime����,������ʱ��ת��Ϊ���������ʽ
	//- 8 * 60 * 60Ϊ��������ʱ������

	RTC_SetCounter(time_cnt);						//���������д�뵽RTC��CNT��
	RTC_WaitForLastTask();							//�ȴ���һ�β������
}

/**
  * ��    ��:RTC��ȡʱ��
  * ��    ��:��
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,RTCӲ����·��ʱ��ֵ��ˢ�µ�ȫ������
  */
void MyRTC_ReadTime(void)
{
	time_t time_cnt;		//�������������������
	struct tm time_date;	//��������ʱ����������

	time_cnt = RTC_GetCounter() + 8 * 60 * 60;		//��ȡRTC��CNT,��ȡ��ǰ���������
	//+ 8 * 60 * 60Ϊ��������ʱ������

	time_date = *localtime(&time_cnt);				//ʹ��localtime����,���������ת��Ϊ����ʱ���ʽ

	MyRTC_Time[0] = time_date.tm_year + 1900;		//������ʱ��ṹ�帳ֵ�������ʱ��
	MyRTC_Time[1] = time_date.tm_mon + 1;
	MyRTC_Time[2] = time_date.tm_mday;
	MyRTC_Time[3] = time_date.tm_hour;
	MyRTC_Time[4] = time_date.tm_min;
	MyRTC_Time[5] = time_date.tm_sec;
	MyRTC_Time[6] = time_date.tm_wday;

}





/**
  * ��    ��:RTC��������
  * ��    ��:��
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,ȫ��������ʱ��ֵ��ˢ�µ�RTCӲ����·
  */
  // void MyRTC_SetAlarm(void)
  // {
  // 	RTC_AlarmTypeDef RTC_AlarmStructure;
  // 	time_t time_cnt;		//�������������������
  // 	struct tm time_date;	//��������ʱ����������

  // 	time_date.tm_year = MyRTC_Time[0] - 1900;		//�������ʱ�丳ֵ������ʱ��ṹ��
  // 	time_date.tm_mon = MyRTC_Time[1] - 1;
  // 	time_date.tm_mday = MyRTC_Time[2];
  // 	time_date.tm_hour = MyRTC_Time[3];
  // 	time_date.tm_min = MyRTC_Time[4];
  // 	time_date.tm_sec = MyRTC_Time[5];

  // 	time_cnt = mktime(&time_date) - 8 * 60 * 60;	//����mktime����,������ʱ��ת��Ϊ���������ʽ
  // 	//- 8 * 60 * 60Ϊ��������ʱ������

  // 	RTC_AlarmStructure.AlarmTime.Hours = 0;
  // 	RTC_AlarmStructure.AlarmTime.Minutes = 0;
  // 	RTC_AlarmStructure.AlarmTime.Seconds = 0;
  // 	RTC_AlarmStructure.AlarmMask = RTC_AlarmMask_DateWeekDay;
  // 	RTC_AlarmStructure.AlarmDateWeekDay = RTC_AlarmDateWeekDay_Date;
  // 	RTC_AlarmStructure.AlarmDate.Date = 1;
  // 	RTC_AlarmStructure.AlarmDate.Month = 1;
  // 	RTC_AlarmStructure.AlarmDate.Year = 2024;

  // 	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
  // 	RTC_WaitForLastTask();
  // }



