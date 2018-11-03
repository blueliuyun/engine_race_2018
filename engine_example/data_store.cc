// Copyright [2018] Alibaba Cloud All rights reserved
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include "util.h"
#include "data_store.h"

namespace polar_race {

static const char kDataFilePrefix[] = "DATA_";
static const int kDataFilePrefixLen = 5;
static const int kSingleFileSize = 1024 * 1024 * 100; //---@2018-10-26 TianYe 单个文件最大 100MBytes

static std::string FileName(const std::string &dir, uint32_t fileno) {
  return dir + "/" + kDataFilePrefix + std::to_string(fileno);
}

RetCode DataStore::Init() {
  if (!FileExists(dir_)
      && 0 != mkdir(dir_.c_str(), 0755)) {
    return kIOError;
  }

  std::vector<std::string> files;
  if (0 != GetDirFiles(dir_, &files)) {
    return kIOError;
  }

  uint32_t last_no = 0;
  uint32_t cur_offset = 0;

  // Get the last data file no
  std::string sindex;
  std::vector<std::string>::iterator it;
  for (it = files.begin(); it != files.end(); ++it) {
    if ((*it).compare(0, kDataFilePrefixLen, kDataFilePrefix) != 0) {
      continue;
    }
    sindex = (*it).substr(kDataFilePrefixLen);
    if (std::stoul(sindex) > last_no) {
      last_no = std::stoi(sindex); //---最终目的是拿到 序号 最大的 file 的序号
    }
  }

  // Get last data file offset
  int len = GetFileLength(FileName(dir_, last_no));
  if (len > 0) {
    cur_offset = len;
  }

  next_location_.file_no = last_no;
  next_location_.offset = cur_offset;

  // Open file
  return OpenCurFile();
}

RetCode DataStore::Append(const std::string& value, Location* location) {
  if (value.size() > kSingleFileSize) { //---@2018-10-26 TianYe 比赛中单个 value 的大小是 4KBytes 远小于 单个文件 100MBytes 的大小
    return kInvalidArgument;
  }

  if (next_location_.offset + value.size() > kSingleFileSize) { //---@2018-10-26 Tian 当前offset + 即将插入的数据长度 > 单个文件 100MBytes 的大小, 所以会再新建一个文件
    // Swtich to new file
    close(fd_);
    next_location_.file_no += 1;
    next_location_.offset = 0; //---若是新创建的 DATA 文件，则写入偏移 offset = 0.
    OpenCurFile(); //---@2018-10-26 函数执行正确的情况时，文件描述符 fd_ 会被更新.
  }

  // Append write
  if (0 != FileAppend(fd_, value)) {
    return kIOError;
  }
  location->file_no = next_location_.file_no;
  location->offset = next_location_.offset;
  location->len = value.size();

  next_location_.offset += location->len;
  return kSucc;
}

RetCode DataStore::Read(const Location& l, std::string* value) {
  int fd = open(FileName(dir_, l.file_no).c_str(), O_RDONLY, 0644);
  if (fd < 0) {
    return kIOError;
  }
  lseek(fd, l.offset, SEEK_SET);

  char* buf = new char[l.len]();
  char* pos = buf;
  uint32_t value_len = l.len;

  while (value_len > 0) {
    ssize_t r = read(fd, pos, value_len);
    if (r < 0) {
      if (errno == EINTR) {
        continue;  // Retry
      }
      close(fd);
      return kIOError;
    }
    pos += r;
    value_len -= r;
  }
  *value = std::string(buf, l.len);

  delete buf;
  close(fd);
  return kSucc;
}

RetCode DataStore::OpenCurFile() {
  std::string file_name = FileName(dir_, next_location_.file_no);
  int fd = open(file_name.c_str(), O_APPEND | O_WRONLY | O_CREAT, 0644);
  if (fd < 0) {
    return kIOError;
  }
  fd_ = fd;
  return kSucc;
}

}  // namespace polar_race
