gcc -o execelf execelf.c
if command -v staticx; then
for f in execelf pexec; do
  staticx --strip $f $f.static
done
fi
