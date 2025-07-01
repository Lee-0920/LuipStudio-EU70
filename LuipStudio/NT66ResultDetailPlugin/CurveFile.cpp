#include "CurveFile.h"
#include "LuaEngine/LuaEngine.h"
#include "Log.h"

using namespace System;
using namespace Controller::API;
using namespace Lua;
using namespace OOLUA;
using namespace Controller;

namespace  ResultDetail
{

CurveFile::CurveFile(String path, Uint32 dataMaxLength, Version ver, String curveType, Uint32 curveMaxNumber):
    m_file(path.c_str()),m_path(path), m_dataMaxLength(dataMaxLength), m_curveMaxNumber(curveMaxNumber),m_setCurveVersion(ver),
    m_setCurveType(curveType)
{
    memset(&m_FileHeader, 0, sizeof(m_FileHeader));
    m_fileHeaderSize = sizeof(m_FileHeader);
}

CurveFile::~CurveFile()
{
    if (m_file.isOpen())
    {
        m_file.close();
    }
}

bool CurveFile::SaveTo()
{
    SetCurveType(m_setCurveType);
    SetCurveVersion(m_setCurveVersion);
    SetIndex(0);

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();
    String model;
    luaEngine->GetLuaValue(state, "config.info.instrument.model", model);

    SetTarget(model);

    m_file.seek(0);
    m_file.write((char*) &m_FileHeader, sizeof(m_FileHeader));
    m_file.flush();
    return true;
}

bool CurveFile::Save()
{
    bool ret = false;
    if (m_file.open(QIODevice::ReadWrite))
    {
        ret = SaveTo();
        m_file.flush();
    }
    return ret;
}

bool CurveFile::LoadFrom()
{
    m_file.seek(0);
    m_file.read((char*) &m_FileHeader, sizeof(m_FileHeader));

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();
    String model;
    luaEngine->GetLuaValue(state, "config.info.instrument.model", model);

    if (model != GetTarget())
    {
        logger->warn("%s filetarget error, defaulttarget：%s, filetarget：%s", this->m_path.c_str(),
                     model.c_str(), GetTarget().c_str());
        return false;
    }

    if (GetCurveType() != m_setCurveType)
    {
        logger->warn("%s Curve Type error, defaultType：%s, fileType：%s", this->m_path.c_str(),
                     m_setCurveType.c_str(), GetCurveType().c_str());
        return false;
    }

    return true;
}

System::Int64 CurveFile::GetPos(System::Uint32 index)
{
    return (index % m_curveMaxNumber * m_dataMaxLength + m_fileHeaderSize);
}

bool CurveFile::Load()
{
    bool ret = false;
    if (!m_file.exists())
    {
        ret = Save();
    }
    else
    {
        if (m_file.open(QIODevice::ReadWrite))
        {
            ret = LoadFrom();
        }
    }
    return ret;
}

String CurveFile::GetCurveType() const
{
    return String(m_FileHeader.curveType);
}

bool CurveFile::SetCurveType(String type)
{
    memcpy(m_FileHeader.curveType, type.c_str(), sizeof(m_FileHeader.curveType));
    return true;
}

Version CurveFile::GetCurveVersion() const
{
    return Version(m_FileHeader.curveVersion, 16);
}

bool CurveFile::SetCurveVersion(Version ver)
{
    ver.GetData(m_FileHeader.curveVersion, 16);
    return true;
}

String CurveFile::GetTarget() const
{
    return String(m_FileHeader.target);
}

bool CurveFile::SetTarget(String target)
{
    memcpy(m_FileHeader.target, target.c_str(), sizeof(m_FileHeader.target));
    return true;
}

System::Uint64 CurveFile::GetIndex() const
{
    return m_FileHeader.index;
}

bool CurveFile::SetIndex(System::Uint64 index)
{
    m_FileHeader.index = index;
    return true;
}

bool CurveFile::ClearCurve(void)
{
    bool status = false;
    if (m_file.isOpen())
    {
        m_file.close();
    }
    if (m_file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        status = SaveTo();
        m_file.flush();
    }
    return status;
}

bool CurveFile::MakeIndex(System::Uint64 index)
{
    if (m_file.isOpen())
    {
        m_FileHeader.index = index;
        m_file.seek(sizeof(m_FileHeader.curveType) + sizeof(m_FileHeader.curveVersion) + sizeof(m_FileHeader.target));
        return m_file.write((char*) &m_FileHeader.index, sizeof(m_FileHeader.index));
    }
    else
    {
        return false;
    }
}

Uint32 CurveFile::Addcurve(CurveData curve)
{
    int index = -1;
    m_mutex.lock();
    Uint32 writeLength = curve.GetSize() + sizeof(int) + sizeof(float) * 2;
    if (m_file.isOpen() && writeLength <= m_dataMaxLength)
    {
        Uint64 writePos;
        if (m_file.size() == m_fileHeaderSize)
        {
            writePos = m_fileHeaderSize;
        }
        else
        {
            m_FileHeader.index++;
            writePos = GetPos(m_FileHeader.index);
        }
        m_file.seek(writePos);
        int length = curve.GetLength();
        float begin = curve.GetBegin();
        float end = curve.GetEnd();

        m_file.write((const char *)&length, sizeof(int));
        m_file.write((const char *)&begin, sizeof(float));
        m_file.write((const char *)&end, sizeof(float));
        m_file.write((const char *)curve.GetWholeData(), curve.GetSize());

        writePos += m_dataMaxLength;

        if (writeLength != m_dataMaxLength)
        {
            m_file.seek(writePos - 1);
            m_file.write("\n");
        }
        MakeIndex(m_FileHeader.index);
        m_file.flush();
        index = m_FileHeader.index;
    }
    m_mutex.unlock();
    return index;
}

CurveData CurveFile::Getcurve(Uint32 index)
{
    Uint32 min = 0;
    Uint32 max = m_FileHeader.index;

    if (m_FileHeader.index < m_curveMaxNumber)
    {
        min = 0;
    }
    else
    {
        min = m_FileHeader.index - m_curveMaxNumber + 1;

    }

    if (index >= min && index <= max)
    {
        Uint64 readPos = GetPos(index);
        int length;
        float begin;
        float end;

        m_file.seek(readPos);

        m_file.read((char *)&length, sizeof(int));
        m_file.read((char *)&begin, sizeof(float));
        m_file.read((char *)&end, sizeof(float));

        CurveData curve(begin, end, length);
        m_file.read((char *)curve.GetWholeData(), curve.GetSize());
        return curve;
    }
    else
    {
        CurveData curve(0, 0, 0);
        return curve;
    }
}
}
