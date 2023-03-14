# Declarative State Management

This sub-project provides most of the revised implementation of Declarative State management.

## Building just state mgmt

The implementation is in Typescript.
TS sources are built into a single JS file using `tsc`.
`tsc` must be in shell's PATH.

Before first build:
`npm install`

For every debug build
`npm run build`

The output is one JS file:
`./dist/stateMgmt.js`

For every release build
`npm run build_release`

The output is one JS file:
`./distRelease/stateMgmt.js`

The difference between debug build and release buold is the removal 
of all console.log/.debug/.info statements from the generated JS code.

## Unit tests for state mgmt using node.js / npm

To compile the entire project and unit tests, and to run all tests
`npm test`

The output is one JS file:
`./distTest/stateMgmt_test.js`

The results are written to console.
Successful run ends with the message `Passed all XX cases, YYY test asertions.`.

## Organising

`./src/lib*.ts` - implementation
`./src/index.ts` - implementation 'main' creates singletons.
`./test/utest/*` - unit tests
