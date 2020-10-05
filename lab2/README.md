########################  COMPILACIÓN  ##########################

Compilar el servidor con:
g++ server.cpp -o server.exe -lpthread

Y el cliente con:
g++ client.cpp -o client.exe -lpthread

#########################  EJECUCIÓN  ############################

Luego ejecutar primero el servidor con ./server.exe
y después el cliente con ./client.exe

##################  CÓMO USAR EL PROGRAMA  #######################

Cuando se ejecute el cliente, le preguntará:
"Ingrese su mensaje :"
A continuación deberá escribir su nickname:
Por ejemplo:
julio
Después de enviar este mensaje al servidor,
el servidor registrará su nickname.
Después le seguirá preguntando: "Ingrese su mensaje: ",
ante esto puede escribir:
    lista --> El servidor le retornará una lista de nicknames conectados
    alonso Este es un mensaje --> El servidor enviará un mensaje al nickname alonso (si es que existe)
     --> Si envia una cadena vacia como mensaje, permitirá imprimir en estandar output los mensajes que esta recibiendo del servidor. 

###########################  DETALLES  ##############################

Se usan mutexes para:
    1) manejar las impresiones en std output
    2) para el ingreso y eliminacion de elementos en la estructura "map" que controla los nicknames y Socket File Descriptors.