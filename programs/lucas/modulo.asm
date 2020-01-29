        @   /6000   ; Endereço inicial 0x6000
MODULO  LD  N       ; Carregar variável a ser testada
        JN  NEG     ; Testar se negativa
        JP  FIM     ; Nada a ser feito se positiva
NEG     *   NUM     ; Multiplicar por menos um se negativa
        MM  N       ; Salvar novo valor de N
FIM     CN  0       ; Terminar execução do programa
NUM     K   -1      ; Constante -1
N       K   -10     ; Variável a ser tirado o módulo, mudar antes da montagem
        #   MODULO  ; Fim do programa, iniciar execução em MODULO
