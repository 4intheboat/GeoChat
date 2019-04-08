AR = ['/usr/bin/ar']
ARFLAGS = ['rcs']
BINDIR = '/usr/local/bin'
CC_VERSION = ('5', '5', '0')
CFLAGS_API = ['-Werror', '-pedantic', '-pedantic-errors', '-Wall', '-Wextra', '-Wno-sign-compare', '-fstrict-aliasing', '-fstack-protector-all', '-fdata-sections', '-ffunction-sections', '-ggdb3', '-std=c++14', '-O2', '-isystem/usr/include/boost156']
COMPILER_CXX = 'g++'
CPPPATH_ST = '-I%s'
CXX = ['/usr/bin/g++-5']
CXXFLAGS_API = ['-Werror', '-pedantic', '-pedantic-errors', '-Wall', '-Wextra', '-Wno-sign-compare', '-fstrict-aliasing', '-fstack-protector-all', '-fdata-sections', '-ffunction-sections', '-ggdb3', '-std=c++14', '-O2', '-isystem/usr/include/boost156']
CXXFLAGS_MACBUNDLE = ['-fPIC']
CXXFLAGS_cxxshlib = ['-fPIC']
CXXLNK_SRC_F = []
CXXLNK_TGT_F = ['-o']
CXX_NAME = 'gcc'
CXX_SRC_F = []
CXX_TGT_F = ['-c', '-o']
DEFINES_API = ['CHECK_ALLOC', '_REENTRANT', '_FORTIFY_SOURCE=2', 'BOOST_ASIO_STRAND_IMPLEMENTATIONS=250000', 'BOOST_ASIO_ENABLE_SEQUENTIAL_STRAND_ALLOCATION']
DEFINES_API_EXTERNAL = ['CHECK_ALLOC', '_REENTRANT', '_FORTIFY_SOURCE=2', 'BOOST_ASIO_STRAND_IMPLEMENTATIONS=250000', 'BOOST_ASIO_ENABLE_SEQUENTIAL_STRAND_ALLOCATION']
DEFINES_ST = '-D%s'
DEST_BINFMT = 'elf'
DEST_CPU = 'x86_64'
DEST_OS = 'linux'
INCLUDES_API = ['/home/nastya/Technopark/GeoChat/tests/src', '../../', '../../third_party/rapidjson-1.0.2/include/', '../../third_party/']
LIBDIR = '/usr/local/lib'
LIBPATH_API = ['/usr/lib64/boost156', '/usr/lib/boost156', '/usr/lib/x86_64-linux-gnu/']
LIBPATH_API_EXTERNAL = ['/usr/lib64/boost156', '/usr/lib/boost156', '/usr/lib/x86_64-linux-gnu/']
LIBPATH_ST = '-L%s'
LIB_API = ['boost_system', 'z', 'crypto', 'ssl', 'pthread', 'rt']
LIB_API_EXTERNAL = ['boost_system', 'z', 'crypto', 'ssl', 'pthread', 'rt']
LIB_ST = '-l%s'
LINKFLAGS_API = ['-fdata-sections', '-ffunction-sections', '-Wl,--gc-sections', '-ggdb3']
LINKFLAGS_MACBUNDLE = ['-bundle', '-undefined', 'dynamic_lookup']
LINKFLAGS_cxxshlib = ['-shared']
LINKFLAGS_cxxstlib = ['-Wl,-Bstatic']
LINK_CXX = ['/usr/bin/g++-5']
PREFIX = '/usr/local'
RPATH_ST = '-Wl,-rpath,%s'
SHLIB_MARKER = '-Wl,-Bdynamic'
SONAME_ST = '-Wl,-h,%s'
STLIBPATH_API = ['../../../third_party/libproperty/src/BUILD', '../../../third_party/o2logger/src/BUILD']
STLIBPATH_ST = '-L%s'
STLIB_API = ['property', 'o2logger']
STLIB_MARKER = '-Wl,-Bstatic'
STLIB_ST = '-l%s'
cxxprogram_PATTERN = '%s'
cxxshlib_PATTERN = 'lib%s.so'
cxxstlib_PATTERN = 'lib%s.a'
macbundle_PATTERN = '%s.bundle'
