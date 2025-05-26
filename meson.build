project('lites', 'c')

bison = find_program('bison', required : false)

# Build the IPC demo
rt = cc.find_library('rt', required : false)
inc = include_directories('.',
    'include',
    'include/ARCH')
executable('ipc-demo',
           ['ipc.c', 'bin/ipc-demo.c'],
           include_directories : inc,
           dependencies : rt)
