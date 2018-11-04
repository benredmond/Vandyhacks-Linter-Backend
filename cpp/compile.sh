CC=g++
CFLAGS=-O
input_file=$1
shift # pull off first arg
args="$*"
filename=${input_file%%.cpp}

$CC -o ./cpp/$filename.out $CFLAGS ./cpp/$input_file --sysroot=/app/.apt
rc=$?

if [[ $rc -eq 0 ]]; then
   ./cpp/$filename.out $args
   exit $?
fi

exit $rc