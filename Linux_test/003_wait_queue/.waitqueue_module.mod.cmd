savedcmd_waitqueue_module.mod := printf '%s\n'   waitqueue_module.o | awk '!x[$$0]++ { print("./"$$0) }' > waitqueue_module.mod
