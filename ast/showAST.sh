ROSE_HOME=/home/anson/Code/Github/rose_build_dir

${ROSE_HOME}/bin/dotGeneratorWholeASTGraph -c test.c
xdot test.c_AST.dot &
