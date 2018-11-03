// Copyright [2018] Alibaba Cloud All rights reserved
#include "engine_race.h"

namespace polar_race {

RetCode Engine::Open(const std::string& name, Engine** eptr) {
  return EngineRace::Open(name, eptr);
}

Engine::~Engine() {
}

/*
 * Complete the functions below to implement you own engine
 */

// 1. Open engine
RetCode EngineRace::Open(const std::string& name, Engine** eptr) {
  *eptr = NULL;
  
  //--o.s1. 创建 engine_race 对象, 返回 ''数据库引擎" 对外暴露的操作对象.
  EngineRace *engine_race = new EngineRace(name); //--@2018-11-03 暂时不考虑 options
  //--加锁
  engine_race->mutex_.Lock();

  //--o.s2. Recover(...) 恢复数据库。如果存在数据库，则 Load 数据库数据，并对日志进行恢复，否则，创建新的数据。  
  Status s = impl->Recover(&edit, &save_manifest);

  //--释放锁
  if(1) {
    //--s.ok()--*eptr 接管 engine_race 申请的内存空间数据, 并传递给外部调用的函数。
    *eptr = engine_race;	
  } else {
    delete eptr;        
  }
  
  return kSucc;
}

// 2. Close engine
EngineRace::~EngineRace() {
}

// 3. Write a key-value pair into engine
RetCode EngineRace::Write(const PolarString& key, const PolarString& value) {
  return kSucc;
}

// 4. Read value of a key
RetCode EngineRace::Read(const PolarString& key, std::string* value) {
  return kSucc;
}

/*
 * NOTICE: Implement 'Range' in quarter-final,
 *         you can skip it in preliminary.
 */
// 5. Applies the given Vistor::Visit function to the result
// of every key-value pair in the key range [first, last),
// in order
// lower=="" is treated as a key before all keys in the database.
// upper=="" is treated as a key after all keys in the database.
// Therefore the following call will traverse the entire database:
//   Range("", "", visitor)
RetCode EngineRace::Range(const PolarString& lower, const PolarString& upper,
    Visitor &visitor) {
  return kSucc;
}

}  // namespace polar_race
