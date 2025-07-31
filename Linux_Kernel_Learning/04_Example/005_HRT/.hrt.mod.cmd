savedcmd_hrt.mod := printf '%s\n'   hrt.o | awk '!x[$$0]++ { print("./"$$0) }' > hrt.mod
