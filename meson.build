project('lites', 'c')

bison = find_program('bison', required : false)

# Build the IPC demo
rt = cc.find_library('rt', required : false)
lites_src_dir = run_command('sh', '-c', 'echo "$LITES_SRC_DIR"').stdout().strip()
if lites_src_dir == ''
  error('LITES_SRC_DIR must be set')
endif
inc = include_directories('.', join_paths(lites_src_dir, 'include'))
executable('ipc-demo',
           ['ipc.c', 'bin/ipc-demo.c'],
           include_directories : inc,
           dependencies : rt)
