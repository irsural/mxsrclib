#ifndef fs_cppbuilderH
#define fs_cppbuilderH

#include <irsdefs.h>

#include <irsfs.h>
#include <irscpp.h>

#include <irsfinal.h>

namespace irs {

std::wstring utf8_to_wstring(const std::string& utf8);
std::string wstring_to_utf8(const std::wstring& wstr);

fs_t* fs_cppbuilder();

} // namespace irs

#endif //fs_cppbuilderH

