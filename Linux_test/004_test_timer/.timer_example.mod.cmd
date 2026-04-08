savedcmd_timer_example.mod := printf '%s\n'   timer_example.o | awk '!x[$$0]++ { print("./"$$0) }' > timer_example.mod
