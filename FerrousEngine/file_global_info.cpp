#include "file.h"

File::GlobalInfo* File::_info = new File::GlobalInfo();

File::GlobalInfo::GlobalInfo() {
	working_dir = std::filesystem::current_path();
}