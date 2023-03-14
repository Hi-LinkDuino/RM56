  .section .note.ohos.ident,"a",%note
  .balign 4
  .type abitag, %object
abitag:
  .long 2f-1f                 // int32_t namesz
  .long 3f-2f                 // int32_t descsz
1:.ascii "OHOS\0"          // char name[]
2:.long 1  // int32_t ohos_api
3:
  .size abitag, .-abitag
