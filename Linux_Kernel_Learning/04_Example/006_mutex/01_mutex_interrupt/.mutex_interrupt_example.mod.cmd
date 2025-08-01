savedcmd_mutex_interrupt_example.mod := printf '%s\n'   mutex_interrupt_example.o | awk '!x[$$0]++ { print("./"$$0) }' > mutex_interrupt_example.mod
