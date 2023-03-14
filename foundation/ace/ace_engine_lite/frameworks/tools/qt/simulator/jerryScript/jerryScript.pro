CONFIG -= qt
TEMPLATE = lib

CONFIG -= app_bundle

DESTDIR = ../libs

LIBS += -lpthread libwsock32 libws2_32


DEFINES += JERRY_CPOINTER_32_BIT=0
DEFINES += JERRY_DEBUGGER=0
DEFINES += JERRY_ES2015=0
DEFINES += JERRY_EXTERNAL_CONTEXT=1
DEFINES += JERRY_FUNCTION_BACKTRACE
DEFINES += JERRY_FUNCTION_NAME
DEFINES += JERRY_GC_LIMIT=(0)
//DEFINES += JERRY_HEAPDUMP
DEFINES += JERRY_LINE_INFO=1
DEFINES += JERRY_MEM_GC_BEFORE_EACH_ALLOC=0
DEFINES += JERRY_NDEBUG
DEFINES += JERRY_PARSER=1
DEFINES += JERRY_PARSER_DUMP_BYTE_CODE=0
DEFINES += JERRY_REF_TRACKER
DEFINES += JERRY_REGEXP_DUMP_BYTE_CODE=0
DEFINES += JERRY_REGEXP_STRICT_MODE=0
DEFINES += JERRY_STACK_LIMIT=(0)
DEFINES += JERRY_SYSTEM_ALLOCATOR=0
DEFINES += JERRY_VALGRIND=0
DEFINES += JERRY_VM_EXEC_STOP=0

ROOT_PATH = ../../../../../../../..

INCLUDEPATH += \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/include \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/api \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/debugger \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ext \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/jcontext \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/jmem \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/jrt \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/lit \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/regexp \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/vm \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/debugger \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/common \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/include \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/arg \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/handle-scope \
        $${ROOT_PATH}/third_party/jerryscript/jerry-port/default/include \
        # $${ROOT_PATH}/third_party/jerryscript/jerry-libm/include


SOURCES += \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/api/jerry-debugger-transport.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/api/jerry-debugger.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/api/jerry-snapshot.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/api/jerry.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/debugger/debugger.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-alloc.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-gc.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-collection.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-conversion.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-errol.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-external-pointers.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-number.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-string.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-value.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-init-finalize.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-lcache.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-literal-storage.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-module.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/base/ecma-property-hashmap.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-array-iterator-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-array-prototype-unscopables.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-array-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-array.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-arraybuffer-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-arraybuffer.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-boolean-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-boolean.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-dataview-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-dataview.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-date-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-date.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-error-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-error.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-evalerror-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-evalerror.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-function-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-function.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-generator-function.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-generator-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-generator.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-global.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-helpers-date.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-helpers-error.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-helpers-json.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-helpers-sort.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-helpers.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-intrinsic.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-iterator-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-json.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-map-iterator-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-map-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-map.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-math.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-number-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-number.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-object-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-promise-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-promise.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-proxy.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-rangeerror-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-rangeerror.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-referenceerror-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-referenceerror.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-reflect.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-regexp-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-regexp.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-set-iterator-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-set-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-set.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-string-iterator-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-string-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-string.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-symbol-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-symbol.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-syntaxerror-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-syntaxerror.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-type-error-thrower.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-typeerror-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-typeerror.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-urierror-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-urierror.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-weakmap-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-weakmap.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-weakset-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-weakset.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtins.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-float32array-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-float32array.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-float64array-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-float64array.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-int16array-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-int16array.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-int32array-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-int32array.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-int8array-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-int8array.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-typedarray-helpers.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-typedarray-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-typedarray.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint16array-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint16array.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint32array-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint32array.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint8array-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint8array.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint8clampedarray-prototype.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint8clampedarray.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-array-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-arraybuffer-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-boolean-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-comparison.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-container-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-conversion.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-dataview-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-eval.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-exceptions.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-function-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-get-put-value.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-iterator-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-jobqueue.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-lex-env.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-number-arithmetic.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-number-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-objects-arguments.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-objects-general.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-objects.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-promise-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-proxy-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-reference.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-regexp-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-string-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-symbol-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ecma/operations/ecma-typedarray-object.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ext/ext-utils.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ext/heapdump.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/ext/tracker.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/jcontext/jcontext.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/jmem/jmem-allocator.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/jmem/jmem-heap.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/jmem/jmem-poolman.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/jrt/jrt-fatals.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/lit/lit-char-helpers.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/lit/lit-magic-strings.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/lit/lit-strings.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js/byte-code.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js/common.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js/js-lexer.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js/js-parser-expr.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js/js-parser-mem.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js/js-parser-module.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js/js-parser-statm.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js/js-parser-tagged-template-literal.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js/js-parser-util.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js/js-parser.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js/js-scanner-ops.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js/js-scanner-util.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/js/js-scanner.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/regexp/re-bytecode.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/regexp/re-compiler.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/parser/regexp/re-parser.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/vm/opcodes-ecma-arithmetics.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/vm/opcodes-ecma-bitwise.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/vm/opcodes-ecma-relational-equality.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/vm/opcodes.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/vm/vm-stack.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/vm/vm-utils.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-core/vm/vm.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/arg/arg-js-iterator-helper.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/arg/arg-transform-functions.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/arg/arg.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/debugger/debugger-common.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/debugger/debugger-rp.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/debugger/debugger-serial.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/debugger/debugger-sha1.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/debugger/debugger-tcp.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/debugger/debugger-ws.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/handle-scope/handle-scope-allocator.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/handle-scope/handle-scope.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/handler/handler-assert.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/handler/handler-gc.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/handler/handler-print.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/handler/handler-register.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/module/module.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-ext/handler/handler-resource-name.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/acos.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/acosh.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/asin.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/asinh.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/atan.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/atan2.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/atanh.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/cbrt.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/ceil.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/copysign.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/cosh.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/exp.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/expm1.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/fabs.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/finite.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/floor.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/fmod.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/isnan.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/log.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/log10.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/log1p.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/log2.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/nextafter.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/pow.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/scalbn.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/sinh.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/sqrt.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/tanh.c \
        #$${ROOT_PATH}/third_party/jerryscript/jerry-libm/trig.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-port/default/default-date.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-port/default/default-debugger.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-port/default/default-external-context.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-port/default/default-fatal.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-port/default/default-io.c \
        $${ROOT_PATH}/third_party/jerryscript/jerry-port/default/default-module.c