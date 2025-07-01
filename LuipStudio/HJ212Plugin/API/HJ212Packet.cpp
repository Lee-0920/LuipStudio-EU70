#include <QDebug>
#include <QTextCodec>
#include <QDateTime>
#include "HJ212Packet.h"

using namespace std;

namespace HJ212
{

HJ212Packet::HJ212Packet(ProtocolVersion version)
{
    m_protocolVersion = version;
    if (version == ProtocolVersion::HJ212_2005)
        m_data = "ST=;CN=;PW=;MN=;CP=&&&&";
    else
        m_data = "QN=;ST=;CN=;PW=;MN=;Flag=;CP=&&&&";

    m_needRespCN = "-1";

    m_qn = "";
    m_st = 0;
    m_cn = "";
    m_pw = "";
    m_mn = "";
    m_flag = 0;
    m_pnum = 0;
    m_pno = 0;
    m_cp = "&&&&";
}

HJ212Packet::HJ212Packet(string data, int len)
{
    //解析数据段
    m_data = data.substr(0, len);
    m_needRespCN = "-1";

    m_qn   = GetFieldString(m_data, "QN");
    m_st   = GetFieldInt(m_data, "ST");
    m_cn   = GetFieldString(m_data, "CN");
    m_pw   = GetFieldString(m_data, "PW");
    m_mn   = GetFieldString(m_data, "MN");
    m_flag = GetFieldInt(m_data, ";Flag");  //旧代码这里为什么要加;
    m_pnum = GetFieldInt(m_data, "PNUM");
    m_pno  = GetFieldInt(m_data, "PNO");
    m_cp   = GetFieldString(m_data, "CP");

    //根据flag获取协议版本
    if (m_flag & 0x08)
        m_protocolVersion = ProtocolVersion::HJ212_2017D;
    else if (m_flag & 0x04)
        m_protocolVersion = ProtocolVersion::HJ212_2017;
    else
        m_protocolVersion = ProtocolVersion::HJ212_2005;

}

string HJ212Packet::GetData()
{
    return m_data;
}

string HJ212Packet::GetDataFormatChinese()
{
    //如果是地表水2017并且是日志，则需要进行中文转换
    if (m_protocolVersion == ProtocolVersion::HJ212_2017D && GetCN() == "3020")
    {
        QString qstr = QTextCodec::codecForName("GB2312")->toUnicode(m_data.c_str(), m_data.length());
        return qstr.toStdString();
    }
    else
        return m_data;
}

int HJ212Packet::GetDataLength()
{
    return m_data.length();
}

string HJ212Packet::GetQN()
{
    return m_qn;
}

int HJ212Packet::GetST()
{
    return m_st;
}

string HJ212Packet::GetCN()
{
    return m_cn;
}

string HJ212Packet::GetPW()
{
    return m_pw;
}

string HJ212Packet::GetMN()
{
    return m_mn;
}

int HJ212Packet::GetFlag()
{
    return m_flag;
}

int HJ212Packet::GetPUMN()
{
    return m_pnum;
}

int HJ212Packet::GetPNO()
{
    return m_pno;
}

string HJ212Packet::GetCP(bool removeHeadTail)
{
    string cp = m_cp;
    if (cp.empty())
        return cp;
    else if(removeHeadTail)
    {
        cp.erase(0, cp.find_first_not_of("&"));
        cp.erase(cp.find_last_not_of("&") + 1);
    }
    return cp;
}

string HJ212Packet::GetNeedRespCN()
{
    return m_needRespCN;
}

string HJ212Packet::GetExtraRespCNInCP()
{
    return m_extraRespCNInCP;
}

void HJ212Packet::SetQN(string qn, string head, string tail)
{
    if (!SetField(m_data, "QN", qn, head, tail))
    {
        //旧代码中，如果替换失败，则QN强行插入
        m_data.insert(0, "QN=" + qn + ";");
    }
    m_qn = qn;
}

void HJ212Packet::RefreshQN()
{
    if (m_qn.empty())
        return;
    std::string qn = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz").toStdString();
    SetQN(qn);
}

void HJ212Packet::SetST(int st)
{
    if (SetField(m_data, "ST", st))
        m_st = st;
}

void HJ212Packet::SetCN(string cn)
{
    if (SetField(m_data, "CN", cn))
        m_cn = cn;
}

void HJ212Packet::SetPW(string pw)
{
    if (SetField(m_data, "PW", pw))
        m_pw = pw;
}

void HJ212Packet::SetMN(string mn)
{
    if (SetField(m_data, "MN", mn))
        m_mn = mn;
}

void HJ212Packet::SetFlag(int flag)
{
    if (SetField(m_data, "Flag", flag, "MN=" + m_mn + ";", "CP="))
        m_flag = flag;
}

void HJ212Packet::AddFlag(int flag)
{
    this->SetFlag(m_flag|flag);
}

void HJ212Packet::SetSubPackage(int pnum, int pno)
{
    string value = std::to_string(pnum) + ";PNO=" + std::to_string(pno);
    //2017协议
    if (SetField(m_data, "PNUM", value, "Flag=" + std::to_string(m_flag) + ";", "CP="))
    {
        m_pnum = pnum;
        m_pno = pno;
    }
    //2005协议
    else if (SetField(m_data, "PNUM", value, "MN=" + m_mn + ";", "CP="))
    {
        m_pnum = pnum;
        m_pno = pno;
    }
}

void HJ212Packet::SetNeedRespCN(string cn)
{
    m_needRespCN = cn;
}

void HJ212Packet::SetExtraRespCNInCP(string cn)
{
    m_extraRespCNInCP = cn;
}

bool HJ212Packet::RemoveCPLastChar()
{
    if(m_cp.size() > 4 && m_data.size() > 4)
    {
        //判断最后第三个字符是不是;
        if (m_cp[m_cp.size()-3] != ';')
            return true;

        m_data.erase(m_data.size()-3, 1);
        m_cp.erase(m_cp.size()-3, 1);
        return true;
    }
    return false;
}

bool HJ212Packet::AddCPParam(string param)
{
    if((m_data.length() + param.size() < HJ212_DATA_MAXSIZE) &&
        (m_cp.length() + param.size()) < HJ212_CP_MAXSIZE)
    {
        m_data.insert(m_data.length() - 2, param);
        m_cp.insert(m_cp.length() - 2, param);
        return true;
    }
    else
    {
        qDebug("AddCPParam [%s] fail, out of length limit.", param.c_str());
        return false;
    }
}

string HJ212Packet::GetCPParam(string name, int number)
{
    string value = StringSub(m_cp, name + "=", ";", number);
    if (!value.empty())
        return value;
    value = StringSub(m_cp, name + "=", "&", number);
    return value;
}

QList<QString> HJ212Packet::GetCPParams(string name)
{
    int num = 0;
    QList<QString> ret;
    while(true)
    {
        string value;
        if (!GetCPParam(name, value, ";", num))
        {
            if(GetCPParam(name, value, "&", num))
                ret.push_back(QString::fromStdString(value));
            break;
        }
        num++;
        ret.push_back(QString::fromStdString(value));
    };
    return ret;
}

bool HJ212Packet::GetCPParam(string name, string& value, string eof, int number)
{
    value = StringSub(m_cp, name + "=", eof, number);
    if (!value.empty())
        return true;
    return false;
}

bool HJ212Packet::GetCPParam(string name, int& value, string eof, int number)
{
    string svalue = "";
    if (GetCPParam(name, svalue, eof, number))
    {
        value = atoi(svalue.c_str());
        return true;
    }
    value = 0;
    return false;
}

bool HJ212Packet::GetCPParam(string name, float& value, string eof, int number)
{
    string svalue = "";
    if (GetCPParam(name, svalue, eof, number))
    {
        value = atof(svalue.c_str());
        return true;
    }
    value = 0;
    return false;
}

bool HJ212Packet::GetCPParam(string name, double& value, string eof, int number)
{
    string svalue = "";
    if (GetCPParam(name, svalue, eof, number))
    {
        char *ptr;
        value = strtod(svalue.c_str(), &ptr);
        return true;
    }
    value = 0;
    return false;
}

std::map<string,string> HJ212Packet::GetCPParamsMap(std::vector<char> seperators)
{
    std::map<string,string> ret;
    if (m_cp.empty())
        return ret;

    string temp = m_cp;
    temp.erase(0, temp.find_first_not_of("&"));
    temp.erase(temp.find_last_not_of("&") + 1);

    std::vector<string> array = StringSplit(temp, seperators);
    for (int i = 0; i < (int)array.size(); i++)
    {
        std::vector<string> item = StringSplit(array[i], '=');
        if (item.size() != 2)
            continue;

        if (ret.find(item[0]) != ret.end())
            continue;
        ret.insert(pair<string,string>(item[0], item[1]));
    }
    return ret;
}

ProtocolVersion HJ212Packet::GetProtocolVersion()
{
    return m_protocolVersion;
}

CommandType HJ212Packet::GetCommandType()
{
    if (atoi(m_cn.c_str()) < 9000)
        return CommandType::ControlCommand;

    return CommandType::InteractionCommand;
}

bool HJ212Packet::IsNeedRespond()
{
    return (m_flag & 0x01);
}

bool HJ212Packet::IsSubpackage()
{
    return (m_flag & 0x02);
}

/**
 * @brief 装载最终的通讯包
 * @param packet 返回的包
 * @param packetSize 包大小
 * @param crc16 CRC校验码
 * @note  通讯包结构：包头（2字节##）+ 数据段长度（4字节左补0）+ 数据（最长1024）+ CRC16（4字节）+ 包尾（2字节<CR><LF>）
 */
bool HJ212Packet::LoadFinalPacket(string& packet, int& packetSize, unsigned int& crc16)
{
    // 判断长度
    int dataLen = GetDataLength();
    packetSize = 2 + 4 + dataLen + 4 + 2;
    if (packetSize > HJ212_FINAL_PACKET_MAXSIZE || dataLen > 1024)
    {
        return false;
    }

    string buffer = "";
    // 包头
    buffer += HJ212_FINAL_PACKET_HEADER;

    // 数据段长度
    char str[5];
    sprintf(str, "%04d", dataLen);
    buffer += str;

    // 数据
    buffer += m_data;

    //CRC16
    crc16 = CRC16Checkout((const unsigned char *)m_data.c_str(), dataLen);
    sprintf(str, "%04X", crc16);
    buffer += str;

    // 包尾
    buffer += HJ212_FINAL_PACKET_TAIL;

    packet = buffer;
    return true;
}

/**
 * @brief 从缓存中查找通讯包（接收时用）
 * @param buffer 缓存
 * @param bufferSize 缓存大小
 * @param packet 返回的通讯包
 * @param packetSize 通讯包大小
 */
bool HJ212Packet::FindFinalPacket(string& buffer, int& bufferSize, string& packet, int& packetSize)
{
    if (bufferSize < 12)
        return false;

    // 格式化缓存，前面如果不是包头，则为垃圾数据
    size_t headerIdx = buffer.find(HJ212_FINAL_PACKET_HEADER);
    if (headerIdx == string::npos)
    {
        buffer = "";
        bufferSize = 0;
        return false;
    }
    else if(headerIdx != 0)
    {
        buffer = buffer.substr(headerIdx);
        bufferSize = bufferSize - headerIdx;
    }

    // 查找包尾
    size_t endIdx = buffer.find(HJ212_FINAL_PACKET_TAIL);
    if (endIdx == string::npos)
        return false;
    endIdx += 2;

    // 截取
    packet = buffer.substr(0, endIdx);

    //判断包前面是否有多余的包头（##...##...\r\n）
    int headerLength = string(HJ212_FINAL_PACKET_HEADER).length();
    headerIdx = packet.find_last_of(HJ212_FINAL_PACKET_HEADER) - (headerLength - 1);
    if (headerIdx > 0)
        packet = packet.substr(headerIdx);

    // 缓存裁剪
    buffer = buffer.substr(endIdx);

    packetSize = packet.length();
    bufferSize = buffer.length();
    return true;
}

/**
 * @brief 检查通讯包
 * @param packet 通讯包缓存
 * @param packetSize 通讯包缓存大小
 * @param data 通讯包中的数据段
 * @param dataSize 通讯包中的数据段长度
 */
bool HJ212Packet::CheckFinalPacket(string& packet, int& packetSize, string& data, int& dataSize)
{
    if (packetSize < 12)
        return false;

    string lenStr = packet.substr(2, 4);
    string crcStr = packet.substr(packetSize - 6, 4);
    string dataStr = packet.substr(6, packetSize - 12);

    //获取数据段长度
    int len = atoi(lenStr.c_str());

    //获取CRC校验码
    int crc = 0;
    sscanf(crcStr.c_str(), "%x", &crc);

    int ret = true;
    //验证数据长度
    if (packetSize - 12 != len)
    {
        ret = false;
        //("OnReceive data error len");
    }

    //验证CRC
    int check = CRC16Checkout((unsigned char*)dataStr.c_str(), len);
    if (check != crc)
    {
        ret = false;
        //("OnReceive data error crc");
    }

    if (ret)
    {
        data = dataStr;
        dataSize = len;
    }
    return ret;
}

/**
 * @brief 获取字段
 * @param str 数据段
 * @param name 字段名
 * @note 1. 一般字段都是以“=”开始，“;”结尾的
 *       2. CP字段中的内容用“&&”开始，“&&”结尾，所以用“=&&”和“&&”来裁剪，不过好像直接用“=”和“;”也行
 *       3. 2005协议的QN可能在CP字段里面，如果是CP=&&QN=xxxx&&,那么就需要用“=”和“&”来裁剪
 */
string HJ212Packet::GetFieldString(const string &str, const string &name)
{
    string ret = "";
    if (name == "CP")
        ret = "&&" + StringSub(str, name + "=&&", "&&") + "&&";
    else
        ret = StringSub(str, name + "=", ";");

    if (ret.empty() && name == "QN")
        ret = StringSub(str, name + "=", "&");
    return ret;
}

int HJ212Packet::GetFieldInt(const string &str, const string &name)
{
    string value = GetFieldString(str, name);
    return atoi(value.c_str());
}

/**
 * @brief 设置字段
 * @param str 数据段
 * @param name 字段名
 * @param value 设置的值
 * @param head 自定义的插入起始位置(2005的QN字段才用)
 * @param tail 自定义的插入结束位置(2005的QN字段才用)
 */
bool HJ212Packet::SetField(string &str, const string &name, const string &value, string head, string tail)
{
    if (StringReplace(str, name + "=", ";", value))
        return true;
    if (head.size() > 0 && tail.size() > 0)
    {
        if (StringReplace(str, head, tail, name + "=" + value + ";"))
            return true;
    }
    return false;
}

bool HJ212Packet::SetField(string &str, const string &name, int value, string head, string tail)
{
    string svalue = std::to_string(value);
    return SetField(str, name, svalue, head, tail);
}

/**
 * @brief 超找子串位置
 * @param str 字符串
 * @param beg 起始标志
 * @param end 结束标志
 * @param begPos 起始位置
 * @param endPos 结束位置
 * @param number 第几个子串
 */
bool HJ212Packet::StringFind(const string &str, string beg, string end, size_t& begPos, size_t& endPos, int number)
{
    bool ret = false;
    begPos = 0;
    endPos = string::npos;

    //定位起始位置
    for (int i = 0; i <= number; i++)
    {
        begPos = str.find(beg, begPos);
        if (begPos == string::npos)
            return ret;
        begPos += beg.length();
    }

    //定位结束位置
    if (begPos != string::npos)
    {
        endPos = str.find(end, begPos);
        if (endPos != string::npos)
            ret = true;
    }
    return ret;
}

/**
 * @brief 获取子串
 * @param str 字符串
 * @param beg 起始标志
 * @param end 结束标志
 * @param number 第几个子串
 */
string HJ212Packet::StringSub(const string &str, string beg, string end, int number)
{
    string result = "";
    size_t begPos = 0;
    size_t endPos = 0;

    if (StringFind(str, beg, end, begPos, endPos, number))
        result = str.substr(begPos, endPos - begPos);
    return result;
}

/**
 * @brief 替换子串
 * @param dest 字符串
 * @param beg 起始标志
 * @param end 结束标志
 * @param source 待替换的子串
 * @param number 第几个子串
 */
bool HJ212Packet::StringReplace(string &dest, string beg, string end, string source, int number)
{
    size_t begPos = 0;
    size_t endPos = 0;
    if (StringFind(dest, beg, end, begPos, endPos, number))
    {
        if (begPos == endPos)
            dest.insert(begPos, source);
        else
            m_data.replace(begPos, endPos - begPos, source);
        return true;
    }
    return false;
}

std::vector<string> HJ212Packet::StringSplit(const string &str, char seperator)
{
    std::vector<std::string> ret;
    auto string_find_first_not = [str, seperator](size_t pos = 0) -> size_t {
        for (size_t i = pos; i < str.size(); i++)
        {
            if (str[i] != seperator)
                return i;
        }
        return std::string::npos;
    };

    size_t lastPos = string_find_first_not(0);
    size_t pos = str.find(seperator, lastPos);
    while (lastPos != std::string::npos)
    {
        ret.emplace_back(str.substr(lastPos, pos - lastPos));
        lastPos = string_find_first_not(pos);
        pos = str.find(seperator, lastPos);
    }

    return ret;
}

std::vector<string> HJ212Packet::StringSplit(const string &str, std::vector<char> seperators)
{
    std::vector<std::string> ret;
    if (str.empty())
        return ret;
    if (seperators.size() <= 0)
    {
        ret.push_back(str);
        return ret;
    }

    char sep = seperators[0];
    std::vector<std::string> array = StringSplit(str, sep);
    if (seperators.size() <= 1)
    {
        ret = array;
        return ret;
    }

    seperators.erase(seperators.begin());
    for (int i = 0; i < (int)array.size(); i++)
    {
        std::vector<std::string> subArray = StringSplit(array[i], seperators);
        for (int j = 0; j < (int)subArray.size(); j++)
        {
            ret.push_back(subArray[j]);
        }
    }
    return ret;
}

/**
 * @brief 获取CRC16校验码
 * @param pushMsg 待校验缓存
 * @param usDataLen 缓存大小
 */
unsigned int HJ212Packet::CRC16Checkout(const unsigned char *pushMsg, unsigned int usDataLen)
{
    unsigned int i, j, crc_reg, check;
    crc_reg = 0xFFFF;
    for(i = 0; i < usDataLen; i++)
    {
        crc_reg = (crc_reg >> 8) ^ pushMsg[i];
        for( j = 0; j < 8; j++)
        {
            check = crc_reg & 0x0001;
            crc_reg >>= 1;
            if(check==0x0001)
            {
                crc_reg ^= 0xA001;
            }
        }
    }
    return crc_reg;
}


}
