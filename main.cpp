#include <iostream>  
#include <cstdlib>  
#include <sys/types.h>  
#include <cstring>  
#include <string>
#include <windows.h>

#include <stdio.h>
#include <iphlpapi.h>
#include <iptypes.h>
#include <string.h>

#pragma comment (lib,"iphlpapi")

#define MAX_FAILED_CNT 3   // ���ʧ�ܴ�����������ֵ��������֤�ͻ���
#define ADAPTER_NAME "Realtek PCIe GbE Family Controller"  // �������ƣ������������ߺ�Ż��⻥��������
#define RETRY_INTERVAL 30  // ���������λ��
#define CLIENT_PATH "D:\\SCUT_WWW\\DrMain.exe"  //��֤�ͻ���·��


using namespace std;

//����ָ��: g++ main.cpp -o scut_internet_minitor -lwsock32 -liphlpapi

bool AnalysisFile()  
{ 
  bool rState;//����״̬ 
  FILE *file; 
  char ln[80]; 
  fopen_s(&file, "returnpingdata.txt", "r"); 
  
  fgets(ln, 80, file);//������У����� 
  fgets(ln, 80, file);//����ping��Ϣ������ 
  fgets(ln, 80, file);//����ping���󷵻�ֵ���������� 
    
  string data = ln; 
  int iPos = data.find("="); 
  data = data.substr(iPos+1,3);//��ȡ�ַ��������ֽ��� 
  int n = atoi(data.c_str()); 
  rState = n > 0; 
  fclose(file); 
  return rState; 
} 

bool is_internet_cable_connected(){
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwBufLen = 0;
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        printf("Error allocating memory needed to call GetAdaptersInfo.\n");
        return 1;
    }
    pAdapterInfo->Next = (IP_ADAPTER_INFO *)65536l;
 
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory needed to call GetAdaptersInfo.\n");
            return 1;
        }
    }
 
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            printf("%s. IP: %s\n", pAdapter->Description, pAdapter->IpAddressList.IpAddress.String);
            if(strcmp(pAdapter->Description, ADAPTER_NAME) == 0){
                // printf("Realtek PCIe GbE Family Controller. IP: %s\n", pAdapter->IpAddressList.IpAddress.String);
                if(strcmp(pAdapter->IpAddressList.IpAddress.String, "0.0.0.0") == 0){
                    return false;
                } else {
                    return true;
                }
            }
            pAdapter = pAdapter->Next;
        }
    } else {
        printf("Call to GetAdaptersInfo failed.\n");
    }
 
    if (pAdapterInfo) {
        free(pAdapterInfo);
    }
    return false;
}

int main()
{
    int failed_cnt = 0;
    while (1)
    {
        if(is_internet_cable_connected() != true) {
            std::cout << "����δ����. " << RETRY_INTERVAL << "s������" << std::endl;
            failed_cnt = 0;
            Sleep(RETRY_INTERVAL*1000);
            continue;
        }
        // std::string ipAddress = "www.msftconnecttest.com";
        WinExec("cmd /c ping 223.5.5.5 -n 1 -w 1000 > returnpingdata.txt", SW_HIDE); 
        Sleep(1000);//�ȴ�1000ms 
        bool result = AnalysisFile();//���������з����ļ����õ������������ 
        if (result) {
            failed_cnt = 0;
            std::cout << "������������. ��һ�μ����" << RETRY_INTERVAL << "s��." << std::endl;
            Sleep(RETRY_INTERVAL*1000);
        } else {
            failed_cnt++;
            std::cout << "������δ����. ʧ�ܹ���" << failed_cnt << "��. ";
            if (failed_cnt >= MAX_FAILED_CNT) {
                std::cout << "������֤�ͻ���." << std::endl;
                WinExec("TASKKILL /F /IM DrMain.exe /T", SW_HIDE);
                WinExec("TASKKILL /F /IM DrClient.exe /T", SW_HIDE);
                WinExec("TASKKILL /F /IM DrUpdate.exe /T", SW_HIDE);
                Sleep(1000);
                // WinExec("route delete 192.168.53.229", SW_HIDE);
                // WinExec("route ADD -p 192.168.53.229 MASK 255.255.254.0 10.40.91.254 METRIC 1 IF 3", SW_HIDE);
                // Sleep(1000);
                std::cout << "��" <<CLIENT_PATH << std::endl;
                WinExec(CLIENT_PATH, SW_SHOW);
                Sleep(RETRY_INTERVAL*1000);
            } else {
                std::cout << "5s������." << std::endl;
                Sleep(1000*5);
            }
        }
    }
    return 0;
}
