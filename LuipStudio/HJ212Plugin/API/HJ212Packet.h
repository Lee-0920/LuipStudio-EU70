/**
 * @brief HJ212通讯包组装
 * @author haungyue
 * @date 2020/11/11
 */
#ifndef INTERFACE_HJ212_HJ212PACKET_H
#define INTERFACE_HJ212_HJ212PACKET_H

#include <memory>
#include <thread>
#include <vector>
#include <map>
#include <QString>
#include <QList>

#include "HJ212Def.h"

namespace HJ212
{

class HJ212Packet
{
public:
    HJ212Packet(ProtocolVersion version = ProtocolVersion::HJ212_2017D);
    HJ212Packet(std::string data, int len);

    //通讯包组装、查找、检查
    bool LoadFinalPacket(std::string &packet, int &packetSize, unsigned int &crc16);                    
    static bool FindFinalPacket(std::string &buffer, int &bufferSize, std::string &packet, int &packetSize); 
    static bool CheckFinalPacket(std::string &packet, int &packetSize, std::string &data, int &dataSize);    

    //数据段操作
    std::string GetData();
    std::string GetDataFormatChinese();
    int GetDataLength();

    //获取数据段中的某个字段
    std::string GetQN();      
    int  GetST();       
    std::string GetCN();      
    std::string GetPW();           
    std::string GetMN();  
    int  GetFlag();               
    int GetPUMN();      
    int GetPNO();      
    std::string GetCP(bool removeHeadTail = false);

    //获取CP字段中的参数
    bool GetCPParam(std::string name, std::string &value, std::string eof = ";", int number = 0);
    bool GetCPParam(std::string name, int &value, std::string eof = ";", int number = 0);
    bool GetCPParam(std::string name, float &value, std::string eof = ";", int number = 0);
    bool GetCPParam(std::string name, double &value, std::string eof = ";", int number = 0);
    std::string GetCPParam(std::string name, int number = 0);
    QList<QString> GetCPParams(std::string name);
    std::map<std::string, std::string> GetCPParamsMap(std::vector<char> seperators = {';'});

    //设置数据段中的某个字段
    void SetQN(std::string qn, std::string head = "", std::string tail = "");
    void SetST(int st);
    void SetCN(std::string cn);
    void SetPW(std::string pw);
    void SetMN(std::string mn);
    void SetFlag(int flag);
    void AddFlag(int flag);
    void SetSubPackage(int pnum, int pno);
    bool AddCPParam(std::string param); //往CP字段中添加参数
    bool RemoveCPLastChar();            //去除CP字段参数最后一个“；”

    //临时用的参数
    std::string GetNeedRespCN();
    std::string GetExtraRespCNInCP();
    void SetNeedRespCN(std::string cn);
    void SetExtraRespCNInCP(std::string cn);

    //其他
    ProtocolVersion GetProtocolVersion();   //根据flag获取协议版本
    CommandType GetCommandType();           //获取命令的类型（控制或者交互）
    bool IsNeedRespond();                   //是否需要回应
    bool IsSubpackage();                    //是否有分包

    void RefreshQN();

protected:
    std::string m_data;            //数据
    std::string m_needRespCN;      //需要回应包的命令编号
    std::string m_extraRespCNInCP; //CP字段中需要有的CN参数（2005协议上传分钟数据等要用到）

    std::string m_qn; //请求编号
    int m_st;         //系统编号
    std::string m_cn; //命令编号
    std::string m_pw; //密码
    std::string m_mn; //设备唯一标识
    int m_flag;       //分包标记
    int m_pnum;       //总包号
    int m_pno;        //包号
    std::string m_cp; //指令参数

    ProtocolVersion m_protocolVersion;

    //获取\设置数据段中的某个字段
    std::string GetFieldString(const std::string& str, const std::string& name);
    int    GetFieldInt(const std::string &str, const std::string &name);
    bool   SetField(std::string &str, const std::string &name, const std::string &value, std::string head = "", std::string tail = "");
    bool   SetField(std::string &str, const std::string &name, int value, std::string head = "", std::string tail = "");

public:
    //字符串操作
    bool   StringFind(const std::string &str, std::string beg, std::string end, size_t& begPos, size_t& endPos, int number = 0);
    bool   StringReplace(std::string &dest, std::string beg, std::string end, std::string source, int number = 0);
    std::string StringSub(const std::string &str, std::string beg, std::string end, int number = 0);
    std::vector<std::string> StringSplit(const std::string &str, char seperator);
    std::vector<std::string> StringSplit(const std::string &str, std::vector<char> seperators);

    //CRC16校验码
    static unsigned int CRC16Checkout(const unsigned char *pushMsg, unsigned int usDataLen);
};

/**
 * @brief HJ212数据包引用对象。使用引用计数器自动管理数据包对象的内存。
 */
typedef std::shared_ptr<HJ212Packet> HJ212PackPtr;
}

#endif 
