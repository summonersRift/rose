TileK Evaluation Tools
======================

```bash
source $ROSE_BUILDDIR/projects/Tilek/tilek.rc

mkdir test
cd test

touch code-cfg_1.c # and edit
touch code-cfg_2.c # and edit
touch code-cfg_3.c # and edit

> code-configs.csv
echo "cfg_1" >> code-configs.csv
echo "cfg_2" >> code-configs.csv
echo "cfg_3" >> code-configs.csv

> code-args.csv
echo "arg_1,16" >> code-args.csv
echo "arg_2,32" >> code-args.csv
echo "arg_3,64" >> code-args.csv

tilek-compile --stem code --opts "-I/user/headers/directory/"
tilek-eval --stem code --reps 5 --timeout 30s
tilek-collect --stem code
```

