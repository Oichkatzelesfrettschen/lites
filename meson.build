project('lites', 'c')

bison = find_program('bison', required : false)

# Build the IPC demo
rt = cc.find_library('rt', required : false)
lites_src_dir = run_command('sh', '-c', 'echo "$LITES_SRC_DIR"').stdout().strip()
if lites_src_dir == ''
  error('LITES_SRC_DIR must be set')
endif
inc = include_directories('.', join_paths(lites_src_dir, 'include'), 'libs/ddekit', 'include')
executable('ipc-demo',
           ['libs/libposix/ipc.c', 'examples/ipc/ipc-demo.c', 'libos/vm.c'],
           include_directories : inc,
           dependencies : rt)
