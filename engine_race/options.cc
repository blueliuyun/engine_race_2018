/// Copyright [2018]

#include "options.h"

#include "comparator.h"
#include "env.h"

namespace polar_race {

Options::Options()
    : comparator(BytewiseComparator()),
      create_if_missing(false),
      error_if_exists(false),
      paranoid_checks(false),
      env(Env::Default()),
      info_log(nullptr),
      //write_buffer_size(4<<20),     //@2018-11-11 默认 4 MBytes， 调大，从而加速写入速度。 对应的是 Memtable 的大小。
      //write_buffer_size(1024<<20),  //@2018-11-11 调大到 1024 MBytes。 由于总内存 2G 的限制，切换 memtable 到 Imuable 担心内存OOM。 
                      			      //  再次恢复到 1024 MBytes。@2018-11-12 16:15 + @2018-11-12 22:22
									  //+@2018-11-13 08:54 Out of memory , 所以减小些 Memtable 的内存。
                      			      //
      write_buffer_size(768<<20),     //@2018-11-12 11:35   调到 768 MBytes。仍然出现下面的 ' 越限 ' 问题。
									  //	  In [ EngineRace::MakeRoomForWrite() ] >= config::kL0_SlowdownWritesTrigger 
									  //	  In [ EngineRace::MakeRoomForWrite() ] >= config::kL0_StopWritesTrigger 
									  //+@2018-11-13 11:30 Out of memory , 所以减小些 Memtable 的内存。
									  //
	  //write_buffer_size(1280<<20),  //@2018-11-12 17:57 仍然出现 ' 越限 ' 问题 ， 调大内存到 1280 。
	  								  //+@2018-11-13 07:44 继续使用 1280 MBytes。
      //max_open_files(1000),
      max_open_files(32768),			  //@2018-11-13 08:03 修改为 32768 。
      block_cache(nullptr),
      block_size(4096),
      block_restart_interval(16),
      //max_file_size(2<<20),     	//@2018-11-11 默认 2MBytes 可能由于比较小,且写入的数据量太大, 导致撑爆了 Linux IO 句柄数.
      //max_file_size(512<<20),   	//@2018-11-12   13:53  修改為 512 MBytes
      //max_file_size(32<<20),    	//@2018-11-12   22:14  修改為 32 MBytes， 这个值不宜过大，否则会降低 compact 的效率。
      max_file_size(64<<20),   		//@2018-11-13   00:38  修改為 64 MBytes， 这个值不宜过大，否则会降低 compact 的效率。
      compression(kSnappyCompression), //@2018-11-10 Drop up [ kSnappyCompression ] , and then use  [ kNoCompression ]
      //compression(kNoCompression),   //@2018-11-12 22:03 compaction 速度跟不上 write 的速度，尝试打开 压缩 ，降低要合并的文件。
      reuse_logs(false),
      filter_policy(nullptr) {
}

}  // namespace polar_race
