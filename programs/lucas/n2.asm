        @   /6010
INIC    LD  UM
        MM  CONT
        MM  IMPAR
        MM  N2
LOOP    LD  CONT
        -   N
        JZ  FORA
        LD  CONT
        +   UM
        MM  CONT
        LD  IMPAR
        +   DOIS
        MM  IMPAR
        +   N2
        MM  N2
        JP  LOOP
FORA    CN  0
UM      K   1
DOIS    K   2
IMPAR   K   0
N       K   4
N2      K   0
CONT    K   0
        #   INIC
