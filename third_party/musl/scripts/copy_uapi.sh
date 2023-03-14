
while getopts "o:i:t:h" arg
do
    case "${arg}" in
        "o")
            OUT_DIR=${OPTARG}
            ;;
        "i")
            SOURCE_DIR=${OPTARG}
            ;;
	"t")
	    TARGET_ARCH=${OPTARG}
	    ;;
        "h")
            echo "help"
            ;;
        ?)
            echo "unkonw argument"
            exit 1
            ;;
    esac
done

if [ ! -d "${OUT_DIR}" ];then
    mkdir -p ${OUT_DIR}
fi
cp -r ${SOURCE_DIR}/* ${OUT_DIR}

if [ ${TARGET_ARCH} = "arm" ]; then
    mv ${OUT_DIR}/asm-arm/asm ${OUT_DIR}/asm
    rm -rf ${OUT_DIR}/asm-arm64
    rm -rf ${OUT_DIR}/asm-arm
elif [ ${TARGET_ARCH} = "aarch64" ]; then
    mv ${OUT_DIR}/asm-arm64/asm ${OUT_DIR}/asm
    rm -rf ${OUT_DIR}/asm-arm64
    rm -rf ${OUT_DIR}/asm-arm
fi

