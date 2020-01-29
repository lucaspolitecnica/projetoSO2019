        @   /4000
MONITOR SC  GETARG              ; Carregar o byte superior do endereco inicial
        MM  ADDRU
        SC  GETARG              ; Carregar o byte inferior do endereco inicial
        MM  ADDRL
        IO  3                   ; Ignorar espaco
GETSIZE SC  GETARG              ; Carregar quantidade de bytes a serem impressos
        MM  SIZE
PRINT   LD  SIZE                ; Conferir se continua a imprimir
        JZ  FIM                 ; Acaba se SIZE == 0
        -   ONE                 ; Subtrair um de SIZE
        MM  SIZE
        CN  2                   ; Carregar byte a ser impresso
        LD  ADDRU
        SC  PRTBYTE             ; Imprimir byte
        LD  CHESP               ; Imprimir espaco
        IO  6
        LD  ADDRL               ; Adicionar um ao byte inferior do endereco
        +   ONE
        MM  ADDRL
        JZ  OVERFL              ; Lidar com overflow do byte inferior do endereco
        JP  PRINT               ; Recomecar loop
OVERFL  LD  ADDRU               ; Adicionar um ao byte superior do endereco
        +   ONE
        MM  ADDRU
        JP  PRINT               ; Recomecar loop
FIM     CN  0                   ; Fim do monitor
        
ONE     K   1                   ; Constante 1
CHESP   K   " "                 ; Constante espaco ASCII
ADDRU   K   0                   ; Byte superior de endereco
ADDRL   K   0                   ; Byte inferior de endereco
SIZE    K   0                   ; Quantidade de bytes a serem impressos

GETARG  $   2                   ; Subrotina para pegar e converter byte hexa do argumento ASCII para binario
GETNIB1 IO  3                   ; Pegar primeiro nibble
        -   NH3A                ; Testar se entre 0-9 ou A-F
        JN  GAASC1              ; Pular se for entre 0-9
        -   NH7                 ; Compensar para entre A-F
GAASC1  +   NHA                 ; Corrigir para entre 0-9
        *   NH10                ; Deslocamento em 4 bits para a esquerda
        MM  UPNIB               ; Salvar nibble superior
GETNIB2 IO  3                   ; Pegar segundo nibble
        -   NH3A                ; Testar se entre 0-9 ou A-F
        JN  GAASC2              ; Pular se entre 0-9
        -   NH7                 ; Compensar para entre A-F
GAASC2  +   NHA                 ; Corrigir para entre 0-9
        +   UPNIB               ; Adicionar nibble superior, completando o byte a ser retornado
        CN  2                   ; Terminar subrotina
        JP  GETARG
PRTBYTE $   2                   ; Subrotina de impressao de byte em hexadecimal ASCII
        MM  TMPBYTE             ; Guardar byte a ser dumpado
        /   NH10                ; Pegar nibble superior
        *   NH10
        MM  UPNIB               ; Armazenar nibble superior
        /   NH10
        -   NHA                 ; Testar se entre 0-9 ou A-F
        JN  PBASC1              ; Pular se for entre 0-9
        +   NH7                 ; Compensar para entre A-F
PBASC1  +   NH3A                ; Corrigir para entre 0-9
        IO  6                   ; Print do nibble convertido
        LD  TMPBYTE             ; Carregar nibble inferior
        -   UPNIB
        -   NHA                 ; Testar se entre 0-9 ou A-F
        JN  PBASC2              ; Pular se for entre 0-9
        +   NH7                 ; Compensar para entre A-F
PBASC2  +   NH3A                ; Corrigir para entre 0-9
        IO  6                   ; Print do nibble convertido
        LD  TMPBYTE             ; Carregar byte impresso antes de retornar
        CN  2                   ; Terminar subrotina
        JP  PRTBYTE
NH7     K   /7                  ; Constante numerica /7
NHA     K   /A                  ; Constante numerica /A
NH10    K   /10                 ; Constante numerica /10
NH3A    K   /3A                 ; Constante numerica /3A
TMPBYTE K   0                   ; Byte a ser impresso
UPNIB   K   0                   ; Nibble superior
        #   MONITOR
