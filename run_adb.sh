
TARGET_DIR=/data/local/tmp

adb $@ push ./src/main/puscon ${TARGET_DIR}
adb $@ push ./src/kernel/kernel ${TARGET_DIR}
adb $@ shell "cd ${TARGET_DIR} && ./puscon --ansi-color --level 7 ./kernel ."
