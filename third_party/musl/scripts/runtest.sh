
cd /data/tests/libc-test
rm src/REPORT 
touch src/REPORT 

for filelist in src/*
do  
    if [ -d $filelist ]; then
        for file in `ls $filelist`
        do
            if [ "$file" = "runtest" ] \
            || [ "$file" = "libdlopen_dso.so" ] \
            || [ "$file" = "libtls_init_dso.so" ] \
            || [ "$file" = "libtls_align_dso.so" ] \
            || [ "$file" = "libtls_get_new-dtv_dso.so" ]
            then
                continue
            else
                if [ "$file" = "tls_get_new-dtv" ];then
                    cd src/regression
                    /data/tests/libc-test/src/common/runtest -w '' $file >> /data/tests/libc-test/src/REPORT
                    cd /data/tests/libc-test
                else
                    src/common/runtest -w '' $filelist/$file >> src/REPORT
                fi
            fi
        done
    fi
done
