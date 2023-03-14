#! /bin/sh

rm REPORT-SANITIZE
touch REPORT-SANITIZE

num_s=0
num_f=0
num_skip=0

function skip {
    echo skip
    let num_skip=$num_skip+1
}


for case in `ls sanitize/asan`
do
    echo [Runing test] : sanitize/asan/$case
    if [ "$case" = "use-after-return" ]; then
        ASAN_OPTIONS=detect_stack_use_after_return=1 sanitize/asan/$case 2>tmp
    elif [ "$case" = "IO-a" ] | [ "$case" = "IO-b" ]; then
        ASAN_OPTIONS=check_initialization_order=true sanitize/asan/$case 2>tmp
    else
        ASAN_OPTIONS='' sanitize/asan/$case 2>tmp
    fi

    # check result
    if [ "$case" = "memory-leaks" ];then
        grep -nr 'SUMMARY: ' tmp | grep "leaked in"
    else
        grep -nr 'SUMMARY: ' tmp | grep $case
    fi
    res=$?

    # Specially
    if [ res -ne 0 ] && [ "$case" = "IO-b" ];then
        echo "$case succeeded."
        let num_s=$num_s+1
    elif [ res -ne 0 ];then
        echo "[ERROR] asan/$case failed." | tee -a REPORT-SANITIZE
        sanitize/asan/$case >> REPORT-SANITIZE
        let num_f=$num_f+1
    else
        echo "$case succeeded."
        let num_s=$num_s+1
    fi
done

for case in `ls sanitize/scudo`
do
    cat /dev/null > tmp
    echo [Runing test] : sanitize/scudo/$case
    if [ "$case" = "aligned-new" ];then
        sanitize/scudo/$case valid 2>tmp
        r1=$?
        SCUDO_OPTIONS=allocator_may_return_null=1 sanitize/scudo/$case invalid 2>tmp
        r2=$?
        SCUDO_OPTIONS=allocator_may_return_null=0 sanitize/scudo/$case invalid 2>tmp #not
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r3=$?
        let res=$r1+$r2+$r3
        res2=$r1+$r2+$r3
    elif [ "$case" = "alignment" ];then
        sanitize/scudo/$case pointers 2>tmp #not
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        res=$?
    elif [ "$case" = "dealloc-race" ];then
        SCUDO_OPTIONS="QuarantineChunksUpToSize=0" sanitize/scudo/$case 2>tmp
        res=$?
    elif [ "$case" = "double-free" ];then
        sanitize/scudo/$case malloc 2>tmp
        grep -nr "ERROR: invalid chunk state" tmp >/dev/null
        r1=$?
        sanitize/scudo/$case new 2>tmp
        grep -nr "ERROR: invalid chunk state" tmp >/dev/null
        r2=$?
        sanitize/scudo/$case newarray 2>tmp
        grep -nr "ERROR: invalid chunk state" tmp >/dev/null
        r3=$?
        let res=$r1+$r2+$r3
        res2=$r1+$r2+$r3
    elif [ "$case" = "fsanitize" ];then
        skip #build error
        continue
    elif [ "$case" = "interface" ];then
        sanitize/scudo/$case ownership 2>tmp
        r1=$?
        sanitize/scudo/$case ownership-and-size 2>tmp
        r2=$?
        sanitize/scudo/$case heap-size 2>tmp
        r3=$?
        SCUDO_OPTIONS="allocator_may_return_null=1" sanitize/scudo/$case soft-limit 2>tmp
        r4=$?
        SCUDO_OPTIONS="allocator_may_return_null=1" sanitize/scudo/$case hard-limit 2>tmp
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r5=$?
        let res=$r1+$r2+$r3+$r4+$r5
        res2=$r1+$r2+$r3+$r4+$r5
    elif [ "$case" = "memalign" ];then
        skip
        continue
        # sanitize/scudo/$case valid 2>tmp
        # sanitize/scudo/$case invalid 2>tmp
        # scudo_opts=allocator_may_return_null=1 sanitize/scudo/$case invalid 2>tmp
        # sanitize/scudo/$case double-free 2>tmp
        # scudo_opts=DeallocationTypeMismatch=1  sanitize/scudo/$case realloc 2>tmp 
        # scudo_opts=DeallocationTypeMismatch=0  sanitize/scudo/$case realloc 2>tmp
    elif [ "$case" = "mismatch" ];then
        SCUDO_OPTIONS=DeallocationTypeMismatch=1 sanitize/scudo/$case mallocdel 2>tmp #not
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r1=$?
        SCUDO_OPTIONS=DeallocationTypeMismatch=0 sanitize/scudo/$case mallocdel 2>tmp
        r2=$?
        SCUDO_OPTIONS=DeallocationTypeMismatch=1 sanitize/scudo/$case newfree   2>tmp #not
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r3=$?
        SCUDO_OPTIONS=DeallocationTypeMismatch=0 sanitize/scudo/$case newfree   2>tmp
        r4=$?
        let res=$r1+$r2+$r3+$r4
        res2=$r1+$r2+$r3+$r4
    elif [ "$case" = "preinit" ];then
        skip # UNSUPPORTED
        continue
    elif [ "$case" = "options" ];then
        sanitize/scudo/$case 2>tmp
        r1=$?
        SCUDO_OPTIONS=DeallocationTypeMismatch=0 sanitize/scudo/$case 2>tmp
        r2=$?
        SCUDO_OPTIONS=DeallocationTypeMismatch=1 sanitize/scudo/$case 2>tmp #not
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r3=$?
        let res=$r1+$r2+$r3
        res2=$r1+$r2+$r3
    elif [ "$case" = "preload" ];then
        skip
        continue
        # env LD_PRELOAD=%shared_libscudo    not %run %t 2>tmp | FileCheck %s
        # env LD_PRELOAD=%shared_minlibscudo not %run %t 2>tmp | FileCheck %s
    elif [ "$case" = "realloc" ];then
        skip
        continue
        # sanitize/scudo/$case pointers   2>tmp
        # sanitize/scudo/$case contents   2>tmp
        # sanitize/scudo/$case usablesize 2>tmp
    elif [ "$case" = "rss" ];then
        sanitize/scudo/$case 2>tmp
        r1=$?
        SCUDO_OPTIONS="soft_rss_limit_mb=128" sanitize/scudo/$case 2>tmp
        r2=$?
        SCUDO_OPTIONS="hard_rss_limit_mb=128" sanitize/scudo/$case 2>tmp
        r3=$?
        SCUDO_OPTIONS="soft_rss_limit_mb=32:allocator_may_return_null=0" sanitize/scudo/$case 2>tmp
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r4=$? #not
        SCUDO_OPTIONS="soft_rss_limit_mb=32:allocator_may_return_null=1" sanitize/scudo/$case 2>tmp
        r5=$?
        SCUDO_OPTIONS="soft_rss_limit_mb=32:allocator_may_return_null=0:can_use_proc_maps_statm=0" sanitize/scudo/$case 2>tmp
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r6=$? #not 
        SCUDO_OPTIONS="soft_rss_limit_mb=32:allocator_may_return_null=1:can_use_proc_maps_statm=0" sanitize/scudo/$case 2>tmp
        r7=$?
        SCUDO_OPTIONS="hard_rss_limit_mb=32:allocator_may_return_null=0" sanitize/scudo/$case 2>tmp
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r8=$? #not 
        SCUDO_OPTIONS="hard_rss_limit_mb=32:allocator_may_return_null=1" sanitize/scudo/$case 2>tmp
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r9=$? #not 
        SCUDO_OPTIONS="hard_rss_limit_mb=32:allocator_may_return_null=0:can_use_proc_maps_statm=0" sanitize/scudo/$case 2>tmp
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r10=$? #not 
        SCUDO_OPTIONS="hard_rss_limit_mb=32:allocator_may_return_null=1:can_use_proc_maps_statm=0" sanitize/scudo/$case 2>tmp
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r11=$? #not 
        let res=$r1+$r2+$r3+$r4+$r5+$r6+$r7+$r8+$r9+$r10+$r11
        res2=$r1+$r2+$r3+$r4+$r5+$r6+$r7+$r8+$r9+$r10+$r11
    elif [ "$case" = "sized-delete" ];then
        SCUDO_OPTIONS=DeleteSizeMismatch=1 sanitize/scudo/$case gooddel    2>tmp 
        r1=$?
        SCUDO_OPTIONS=DeleteSizeMismatch=1 sanitize/scudo/$case baddel     2>tmp  #not
        grep -nr 'Scudo ERROR: ' tmp >/dev/null 
        r2=$?
        SCUDO_OPTIONS=DeleteSizeMismatch=0 sanitize/scudo/$case baddel     2>tmp
        r3=$?
        SCUDO_OPTIONS=DeleteSizeMismatch=1 sanitize/scudo/$case gooddelarr 2>tmp
        r4=$?
        SCUDO_OPTIONS=DeleteSizeMismatch=1 sanitize/scudo/$case baddelarr  2>tmp  #not
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r5=$?
        SCUDO_OPTIONS=DeleteSizeMismatch=0 sanitize/scudo/$case baddelarr  2>tmp
        r6=$?
        let res=$r1+$r2+$r3+$r4+$r5+$r6
        res2=$r1+$r2+$r3+$r4+$r5+$r6
    elif [ "$case" = "sizes" ];then
        SCUDO_OPTIONS=allocator_may_return_null=0 sanitize/scudo/$case malloc 2>tmp #not
        grep -nr 'Scudo ERROR: ' tmp >/dev/null 
        r1=$? 
        SCUDO_OPTIONS=allocator_may_return_null=1 sanitize/scudo/$case malloc 2>tmp
        r2=$?
        SCUDO_OPTIONS=allocator_may_return_null=0 sanitize/scudo/$case calloc 2>tmp #not 
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r3=$?
        SCUDO_OPTIONS=allocator_may_return_null=1 sanitize/scudo/$case calloc 2>tmp
        r4=$?
        SCUDO_OPTIONS=allocator_may_return_null=0 sanitize/scudo/$case new 2>tmp #not
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r5=$?
        SCUDO_OPTIONS=allocator_may_return_null=1 sanitize/scudo/$case new 2>tmp #not
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r6=$?
        SCUDO_OPTIONS=allocator_may_return_null=0 sanitize/scudo/$case new-nothrow 2>tmp #not
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        r7=$?
        SCUDO_OPTIONS=allocator_may_return_null=1 sanitize/scudo/$case new-nothrow 2>tmp
        r8=$?
        sanitize/scudo/$case usable 2>tmp
        r9=$?
        let res=$r1+$r2+$r3+$r4+$r5+$r6+$r7+$r8+$r9
        res2=$r1+$r2+$r3+$r4+$r5+$r6+$r7+$r8+$r9
    else
        sanitize/scudo/$case 2>tmp
        grep -nr 'Scudo ERROR: ' tmp >/dev/null
        res=$?
    fi

    if [ res -ne 0 ];then
        # echo res
        echo "[ERROR] scudo/$case failed." | tee -a REPORT-SANITIZE
        let num_f=$num_f+1
        continue
    else
        echo "scudo/$case succeeded."
        let num_s=$num_s+1
        continue
    fi    
    echo
done

for case in `ls sanitize/ubsan`
do
    echo [Runing test] : sanitize/ubsan/$case
    sanitize/ubsan/$case 2>tmp
    # check result
    grep -nr 'SUMMARY: UndefinedBehaviorSanitizer' tmp
    res=$?
    if [ res -ne 0 ];then
        echo "[ERROR] ubsan/$case failed." | tee -a REPORT-SANITIZE
        sanitize/ubsan/$case >> REPORT-SANITIZE
        let num_f=$num_f+1
    else
        echo "ubsan/$case succeeded."
        let num_s=$num_s+1
    fi
done

let sum_case=$num_s+$num_f+$num_skip
echo "All:$sum_case Succeeded:$num_s   Failed:$num_f    Skiped:$num_skip"
if [ $num_f -eq 0 ];then
    echo "No test failed, REPORT-SANITIZE not create."
fi
rm tmp
