/**
 * Autogenerated by Thrift Compiler (0.10.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef InstrumentAgent_TYPES_H
#define InstrumentAgent_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/TBase.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>


namespace Interface { namespace Wqimc { namespace Api { namespace Agent {

struct MeasureMode {
  enum type {
    Online = 0,
    Offine = 1
  };
};

extern const std::map<int, const char*> _MeasureMode_VALUES_TO_NAMES;

struct DataType {
  enum type {
    Bool = 0,
    Option = 1,
    Int = 2,
    Float = 3,
    String = 4,
    IntArray = 5,
    Byte = 6,
    Short = 7,
    Long = 8,
    Double = 9
  };
};

extern const std::map<int, const char*> _DataType_VALUES_TO_NAMES;

struct RoleType {
  enum type {
    Guest = 1,
    Administrator = 2,
    Engineer = 3,
    Super = 4
  };
};

extern const std::map<int, const char*> _RoleType_VALUES_TO_NAMES;

class Signal;

class Config;

class Operation;

class MeasureData;

class MeasureItem;

class MeasureWaveform;

class Diagnosis;

class Authorization;

class InstrumentFile;

typedef struct _Signal__isset {
  _Signal__isset() : name(false), value(true), format(false) {}
  bool name :1;
  bool value :1;
  bool format :1;
} _Signal__isset;

class Signal : public virtual ::apache::thrift::TBase {
 public:

  Signal(const Signal&);
  Signal& operator=(const Signal&);
  Signal() : name(), value(0), format() {
  }

  virtual ~Signal() throw();
  std::string name;
  double value;
  std::string format;

  _Signal__isset __isset;

  void __set_name(const std::string& val);

  void __set_value(const double val);

  void __set_format(const std::string& val);

  bool operator == (const Signal & rhs) const
  {
    if (!(name == rhs.name))
      return false;
    if (!(value == rhs.value))
      return false;
    if (!(format == rhs.format))
      return false;
    return true;
  }
  bool operator != (const Signal &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Signal & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(Signal &a, Signal &b);

inline std::ostream& operator<<(std::ostream& out, const Signal& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _Config__isset {
  _Config__isset() : profile(false), config(false), value(false) {}
  bool profile :1;
  bool config :1;
  bool value :1;
} _Config__isset;

class Config : public virtual ::apache::thrift::TBase {
 public:

  Config(const Config&);
  Config& operator=(const Config&);
  Config() : profile(), config(), value() {
  }

  virtual ~Config() throw();
  std::string profile;
  std::string config;
  std::string value;

  _Config__isset __isset;

  void __set_profile(const std::string& val);

  void __set_config(const std::string& val);

  void __set_value(const std::string& val);

  bool operator == (const Config & rhs) const
  {
    if (!(profile == rhs.profile))
      return false;
    if (!(config == rhs.config))
      return false;
    if (!(value == rhs.value))
      return false;
    return true;
  }
  bool operator != (const Config &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Config & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(Config &a, Config &b);

inline std::ostream& operator<<(std::ostream& out, const Config& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _Operation__isset {
  _Operation__isset() : suit(false), name(false), params(false) {}
  bool suit :1;
  bool name :1;
  bool params :1;
} _Operation__isset;

class Operation : public virtual ::apache::thrift::TBase {
 public:

  Operation(const Operation&);
  Operation& operator=(const Operation&);
  Operation() : suit(), name() {
  }

  virtual ~Operation() throw();
  std::string suit;
  std::string name;
  std::vector<std::string>  params;

  _Operation__isset __isset;

  void __set_suit(const std::string& val);

  void __set_name(const std::string& val);

  void __set_params(const std::vector<std::string> & val);

  bool operator == (const Operation & rhs) const
  {
    if (!(suit == rhs.suit))
      return false;
    if (!(name == rhs.name))
      return false;
    if (!(params == rhs.params))
      return false;
    return true;
  }
  bool operator != (const Operation &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Operation & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(Operation &a, Operation &b);

inline std::ostream& operator<<(std::ostream& out, const Operation& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _MeasureData__isset {
  _MeasureData__isset() : time(false), result(false), mode(false), type(false), target(false), optionals(false), waveforms(false), resultFormat(false) {}
  bool time :1;
  bool result :1;
  bool mode :1;
  bool type :1;
  bool target :1;
  bool optionals :1;
  bool waveforms :1;
  bool resultFormat :1;
} _MeasureData__isset;

class MeasureData : public virtual ::apache::thrift::TBase {
 public:

  MeasureData(const MeasureData&);
  MeasureData& operator=(const MeasureData&);
  MeasureData() : time(0), result(0), mode(0), type(), target(), resultFormat() {
  }

  virtual ~MeasureData() throw();
  int64_t time;
  double result;
  int32_t mode;
  std::string type;
  std::string target;
  std::vector<MeasureItem>  optionals;
  std::vector<MeasureWaveform>  waveforms;
  std::string resultFormat;

  _MeasureData__isset __isset;

  void __set_time(const int64_t val);

  void __set_result(const double val);

  void __set_mode(const int32_t val);

  void __set_type(const std::string& val);

  void __set_target(const std::string& val);

  void __set_optionals(const std::vector<MeasureItem> & val);

  void __set_waveforms(const std::vector<MeasureWaveform> & val);

  void __set_resultFormat(const std::string& val);

  bool operator == (const MeasureData & rhs) const
  {
    if (!(time == rhs.time))
      return false;
    if (!(result == rhs.result))
      return false;
    if (!(mode == rhs.mode))
      return false;
    if (!(type == rhs.type))
      return false;
    if (!(target == rhs.target))
      return false;
    if (!(optionals == rhs.optionals))
      return false;
    if (!(waveforms == rhs.waveforms))
      return false;
    if (!(resultFormat == rhs.resultFormat))
      return false;
    return true;
  }
  bool operator != (const MeasureData &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MeasureData & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(MeasureData &a, MeasureData &b);

inline std::ostream& operator<<(std::ostream& out, const MeasureData& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _MeasureItem__isset {
  _MeasureItem__isset() : name(false), value(false), unit(false) {}
  bool name :1;
  bool value :1;
  bool unit :1;
} _MeasureItem__isset;

class MeasureItem : public virtual ::apache::thrift::TBase {
 public:

  MeasureItem(const MeasureItem&);
  MeasureItem& operator=(const MeasureItem&);
  MeasureItem() : name(), value(), unit() {
  }

  virtual ~MeasureItem() throw();
  std::string name;
  std::string value;
  std::string unit;

  _MeasureItem__isset __isset;

  void __set_name(const std::string& val);

  void __set_value(const std::string& val);

  void __set_unit(const std::string& val);

  bool operator == (const MeasureItem & rhs) const
  {
    if (!(name == rhs.name))
      return false;
    if (!(value == rhs.value))
      return false;
    if (!(unit == rhs.unit))
      return false;
    return true;
  }
  bool operator != (const MeasureItem &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MeasureItem & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(MeasureItem &a, MeasureItem &b);

inline std::ostream& operator<<(std::ostream& out, const MeasureItem& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _MeasureWaveform__isset {
  _MeasureWaveform__isset() : name(false), sampleRate(false), sampleNum(false), sampleType(false), xBegin(false), xInterval(false), xUnit(false), yUnit(false), samples(false) {}
  bool name :1;
  bool sampleRate :1;
  bool sampleNum :1;
  bool sampleType :1;
  bool xBegin :1;
  bool xInterval :1;
  bool xUnit :1;
  bool yUnit :1;
  bool samples :1;
} _MeasureWaveform__isset;

class MeasureWaveform : public virtual ::apache::thrift::TBase {
 public:

  MeasureWaveform(const MeasureWaveform&);
  MeasureWaveform& operator=(const MeasureWaveform&);
  MeasureWaveform() : name(), sampleRate(0), sampleNum(0), sampleType((DataType::type)0), xBegin(0), xInterval(0), xUnit(), yUnit(), samples() {
  }

  virtual ~MeasureWaveform() throw();
  std::string name;
  double sampleRate;
  int32_t sampleNum;
  DataType::type sampleType;
  double xBegin;
  double xInterval;
  std::string xUnit;
  std::string yUnit;
  std::string samples;

  _MeasureWaveform__isset __isset;

  void __set_name(const std::string& val);

  void __set_sampleRate(const double val);

  void __set_sampleNum(const int32_t val);

  void __set_sampleType(const DataType::type val);

  void __set_xBegin(const double val);

  void __set_xInterval(const double val);

  void __set_xUnit(const std::string& val);

  void __set_yUnit(const std::string& val);

  void __set_samples(const std::string& val);

  bool operator == (const MeasureWaveform & rhs) const
  {
    if (!(name == rhs.name))
      return false;
    if (!(sampleRate == rhs.sampleRate))
      return false;
    if (!(sampleNum == rhs.sampleNum))
      return false;
    if (!(sampleType == rhs.sampleType))
      return false;
    if (!(xBegin == rhs.xBegin))
      return false;
    if (!(xInterval == rhs.xInterval))
      return false;
    if (!(xUnit == rhs.xUnit))
      return false;
    if (!(yUnit == rhs.yUnit))
      return false;
    if (!(samples == rhs.samples))
      return false;
    return true;
  }
  bool operator != (const MeasureWaveform &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MeasureWaveform & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(MeasureWaveform &a, MeasureWaveform &b);

inline std::ostream& operator<<(std::ostream& out, const MeasureWaveform& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _Diagnosis__isset {
  _Diagnosis__isset() : suit(false), name(false) {}
  bool suit :1;
  bool name :1;
} _Diagnosis__isset;

class Diagnosis : public virtual ::apache::thrift::TBase {
 public:

  Diagnosis(const Diagnosis&);
  Diagnosis& operator=(const Diagnosis&);
  Diagnosis() : suit(), name() {
  }

  virtual ~Diagnosis() throw();
  std::string suit;
  std::string name;

  _Diagnosis__isset __isset;

  void __set_suit(const std::string& val);

  void __set_name(const std::string& val);

  bool operator == (const Diagnosis & rhs) const
  {
    if (!(suit == rhs.suit))
      return false;
    if (!(name == rhs.name))
      return false;
    return true;
  }
  bool operator != (const Diagnosis &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Diagnosis & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(Diagnosis &a, Diagnosis &b);

inline std::ostream& operator<<(std::ostream& out, const Diagnosis& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _Authorization__isset {
  _Authorization__isset() : name(false), expirationDate(false) {}
  bool name :1;
  bool expirationDate :1;
} _Authorization__isset;

class Authorization : public virtual ::apache::thrift::TBase {
 public:

  Authorization(const Authorization&);
  Authorization& operator=(const Authorization&);
  Authorization() : name(), expirationDate(0) {
  }

  virtual ~Authorization() throw();
  std::string name;
  int64_t expirationDate;

  _Authorization__isset __isset;

  void __set_name(const std::string& val);

  void __set_expirationDate(const int64_t val);

  bool operator == (const Authorization & rhs) const
  {
    if (!(name == rhs.name))
      return false;
    if (!(expirationDate == rhs.expirationDate))
      return false;
    return true;
  }
  bool operator != (const Authorization &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Authorization & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(Authorization &a, Authorization &b);

inline std::ostream& operator<<(std::ostream& out, const Authorization& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _InstrumentFile__isset {
  _InstrumentFile__isset() : name(false), isFile(false), updateDate(false), type(false), size(false) {}
  bool name :1;
  bool isFile :1;
  bool updateDate :1;
  bool type :1;
  bool size :1;
} _InstrumentFile__isset;

class InstrumentFile : public virtual ::apache::thrift::TBase {
 public:

  InstrumentFile(const InstrumentFile&);
  InstrumentFile& operator=(const InstrumentFile&);
  InstrumentFile() : name(), isFile(0), updateDate(0), type(), size(0) {
  }

  virtual ~InstrumentFile() throw();
  std::string name;
  bool isFile;
  int64_t updateDate;
  std::string type;
  int64_t size;

  _InstrumentFile__isset __isset;

  void __set_name(const std::string& val);

  void __set_isFile(const bool val);

  void __set_updateDate(const int64_t val);

  void __set_type(const std::string& val);

  void __set_size(const int64_t val);

  bool operator == (const InstrumentFile & rhs) const
  {
    if (!(name == rhs.name))
      return false;
    if (!(isFile == rhs.isFile))
      return false;
    if (!(updateDate == rhs.updateDate))
      return false;
    if (!(type == rhs.type))
      return false;
    if (!(size == rhs.size))
      return false;
    return true;
  }
  bool operator != (const InstrumentFile &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const InstrumentFile & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(InstrumentFile &a, InstrumentFile &b);

inline std::ostream& operator<<(std::ostream& out, const InstrumentFile& obj)
{
  obj.printTo(out);
  return out;
}

}}}} // namespace

#endif
