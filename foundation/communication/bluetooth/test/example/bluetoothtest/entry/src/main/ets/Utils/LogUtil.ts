import BaseModel from './BaseModel';

let LogLevel = {
    /**
       * debug
       */
    DEBUG: 3,

    /**
       * info
       */
    INFO: 4,

    /**
       * warn
       */
    WARN: 5,

    /**
       * error
       */
    ERROR: 6,

    /**
       * fatal
       */
    FATAL: 7,
};

const LOG_LEVEL = LogLevel.INFO

/**
 *  log package tool class
 */
export class LogUtil extends BaseModel{
    debug(msg): void {
        console.info(msg)
        if (LogLevel.DEBUG >= LOG_LEVEL) {
            console.info(msg);
        }
    }

    log(msg): void {
        console.info(msg)
        if (LogLevel.INFO >= LOG_LEVEL) {
            console.info(msg);
        }
    }

    info(msg): void {
        console.info(msg)
        if (LogLevel.INFO >= LOG_LEVEL) {
            console.info(msg);
        }
    }

    warn(msg): void {
        console.info(msg)
        if (LogLevel.WARN >= LOG_LEVEL) {
            console.warn(msg);
        }
    }

    error(msg): void {
        if (LogLevel.ERROR >= LOG_LEVEL) {
            console.error(msg);
        }
    }
}

let mLogUtil = new LogUtil();
export default mLogUtil as LogUtil
;