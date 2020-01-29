        @   /8001
WORK    LD  ZERO
        MM  ITER
LOOP    WK  10
        LD  ITER
        +   UM
        MM  ITER
        -   VEZES
        JZ  FORA
        JP  LOOP
FORA    CN  0
VEZES   K   50
ITER    K   0
ZERO    K   0
UM      K   1
        #   WORK
