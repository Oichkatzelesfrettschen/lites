project('lites', 'c', default_options : ['c_std=c23'])
fs = import('fs')
option('arch', type: 'string', value: 'x86_64',
       description: 'Target architecture')

arch = get_option('arch')

cargs = []
ldflags = []
if arch == 'i686'
  cargs += ['-m32']
  ldflags += ['-m32']
elif arch == 'x86_64'
  cargs += ['-m64']
  ldflags += ['-m64']
elif arch == 'ppc'
  cargs += ['-m32']
elif arch == 'ppc64'
  cargs += ['-m64']
endif

srcdir = 'src-lites-1.1-2025'
if not fs.is_dir(srcdir)
  srcdir = 'build/lites-1.1.u3'
endif

server_src = run_command('sh', '-c', 'find ' + srcdir + '/server -name "*.c"').stdout().split()
executable('lites_server', server_src,
           include_directories : [srcdir + '/include', srcdir + '/server'],
           c_args : cargs,
           link_args : ldflags)

emulator_dir = srcdir + '/emulator'
if fs.is_dir(emulator_dir)
  emulator_src = run_command('sh', '-c', 'find ' + emulator_dir + ' -name "*.c"').stdout().split()
  executable('lites_emulator', emulator_src,
             include_directories : [srcdir + '/include', emulator_dir],
             c_args : cargs,
             link_args : ldflags)
endif

tidy = find_program('clang-tidy', required : false)
if tidy.found()
  tidy_src = server_src
  if fs.is_dir(emulator_dir)
    tidy_src += emulator_src
  endif
  run_target('tidy', command : [tidy] + tidy_src)
endif
