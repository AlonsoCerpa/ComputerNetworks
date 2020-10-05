Compilar el servidor con:
g++ Server.cpp -o Server.exe -lpthread

Y el cliente con:
g++ Cliente.cpp -o Cliente.exe

Luego ejecutar primero el servidor con ./Server.exe
y después el Cliente con ./Cliente.exe

Cunado se ejecute el cliente, le preguntará cual es su
operacion, escribir la operacion de la siguiente forma:
operando1 operando2 operador
Por ejemplo:
18 3 +
Después de enviar este mensaje al servidor mediante una
conexión TCP, el servidor lo procesará y nos enviará el
resultado de la siguiente forma:
Mensaje recibido del servidor: [La suma es 21]

Varios clientes pueden conectarse al servidor simultáneamente,
ya que el servidor implementa threads.
Además el servidor imprime los mensaje que le llegan en su
standard output, y esto lo hace utilizando mutexes, para
que las impresiones a la salida no se mezclen debido a los
threads.