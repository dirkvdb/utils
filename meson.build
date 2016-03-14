project('utils', 'cpp', default_options : ['cpp_std=c++14'])

utilsinc = include_directories(meson.current_source_dir() + '/inc')

utilsfiles = files(
    'inc/utils/bufferedreader.h',      'src/bufferedreader.cpp',
    'inc/utils/fileoperations.h',      'src/fileoperations.cpp',
    'inc/utils/filereader.h',          'src/filereader.cpp',
    'inc/utils/format.h',              'src/format.cpp',
    'inc/utils/functiontraits.h',
    'inc/utils/log.h',                 'src/log.cpp',
    'inc/utils/numericoperations.h',
    'inc/utils/readerinterface.h',
    'inc/utils/readerfactory.h',       'src/readerfactory.cpp',
    'inc/utils/signal.h',
    'inc/utils/simplesubscriber.h',
    'inc/utils/stringoperations.h',
    'inc/utils/subscriber.h',
    'inc/utils/timeoperations.h',
    'inc/utils/timerthread.h',
    'inc/utils/threadpool.h',          'src/threadpool.cpp',
    'inc/utils/trace.h',               'src/trace.cpp',
    'inc/utils/workerthread.h',        'src/workerthread.cpp',
    'inc/utils/backtrace.h',           'src/backtrace.cpp'
)

libxdgbasedir_dep = dependency('libxdg-basedir', required : false)
libunwind_dep = dependency('libunwind', required : false)

config = configuration_data()
config.set10('HAVE_XDGBASEDIR', libxdgbasedir_dep.found())
config.set10('HAVE_LIBUNWIND', libunwind_dep.found())

configure_file(input : 'utilsconfigmeson.h.in', output : 'utilsconfig.h', configuration : config)

utilslib = static_library('utils',
                          utilsfiles,
                          include_directories : utilsinc,
                          dependencies: [libxdgbasedir_dep, libunwind_dep],
                          cpp_args : '-DHAVE_UTILS_CONFIG')

utils_dep = declare_dependency(link_with : utilslib, include_directories : utilsinc)

subdir('test')
