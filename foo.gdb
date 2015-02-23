target remote :3333
b main

print *((LPC_GPIO_T*)0xa0000000)
print ((LPC_GPIO_T*)0xa0000000)->B[0][16]
