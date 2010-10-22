//! \file
//! \brief Набор файлов, включаемых в предварительно скомпилированный
//!   файл заголовка
//!
//! Дата: 13.09.2010\n
//! Дата создания: 13.09.2010

#ifndef irspchH
#define irspchH

#include <irsdefs.h>
#ifdef __BORLANDC__
#include <vcl.h>

#include <iomanip>
#include <iostream>
#include <new>
#include <sstream>
#include <string>
#include <strstream>
#include <algorithm>
#include <deque>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <utility>
#include <vector>
#include <fstream>
#ifndef __embedded_cplusplus
#include <locale>
#include <limits>
#endif //__embedded_cplusplus
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
#include <stdexcept>
#endif //IRS_FULL_STDCPPLIB_SUPPORT

#endif // __BORLANDC__

#endif // irspchH
