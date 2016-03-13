utilstestfiles = files(
    'bufferedreadertest.cpp',
    'fileoperationstest.cpp',
    'gmock-gtest-all.cpp',
    'main.cpp',
    'signaltest.cpp',
    'stringoperationstest.cpp',
    'tracetest.cpp',
    'threadpooltest.cpp',
    'workerthreadtest.cpp'
)

testinc = include_directories(meson.current_build_dir() + '/..')

utilstest = executable('utilstest',
                       utilstestfiles,
                       include_directories : [utilsinc, testinc],
                       link_with : utilslib)

test('utils test', utilstest)