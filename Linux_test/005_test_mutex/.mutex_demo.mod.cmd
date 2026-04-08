savedcmd_mutex_demo.mod := printf '%s\n'   mutex_demo.o | awk '!x[$$0]++ { print("./"$$0) }' > mutex_demo.mod
