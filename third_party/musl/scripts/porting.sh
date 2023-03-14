SRC_DIR=
DST_DIR=
PORTING=

while getopts "i:o:p:h" arg
do 
    case "${arg}" in
        "i")
            SRC_DIR=${OPTARG}
	    ;;
        "o")
            DST_DIR=${OPTARG}
	    ;;
        "p")
	    PORTING=${OPTARG}
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

if [ ! -d ${DST_DIR} ]; then
    mkdir -p ${DST_DIR}
fi
cp -rfp ${SRC_DIR}/* ${DST_DIR}
cp -rfp ${SRC_DIR}/porting/${PORTING}/user/* ${DST_DIR}
