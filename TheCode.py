import serial, math, platform, os, time, sys, struct
import serial.tools.list_ports as list_ports
from collections import deque
import drawings
 
class BreakInterrupt(Exception):
  pass
 
def clscr():
  if platform.system() == 'Windows':
    _=os.system('cls')
  else:
    _=os.system('clear')
 
formats = { 
  'int': '>h',
  'float': '>f'
}
def send(f, n):
  # print('!', struct.pack(formats[f.__name__], f(n)))
  arduino.write(struct.pack(formats[f.__name__], f(n))) 
  arduino.read(1)

def send_angulos(f, s):
  arduino.write(struct.pack('>f', math.degrees(float(s[0])))) # who cares about readability, right?
  arduino.write(struct.pack('>f', math.degrees(float(s[1])))) 
  arduino.read(1)
 
def send_sequence(f, s):
  send(int, len(s))
  for i in s:
    send_angulos(i[0], i[1])
  arduino.read(1)
    
 
a1 = 0    # Ângulo 1 anterior
a2 = 0    # Ângulo 2 anterior
ax1 = 0   # Ângulo 1 atual
ax2 = 0   
r1 = 30   # TODO: Definir direitinho r1 e r2 de acordo com a área de trabalho
r2 = 160
 
 
if '--com' in sys.argv:
  anm = sys.argv[sys.argv.index('--com')+1]
else:
  anm = arduino_ports = [
    p.device
    for p in list_ports.comports()
    if 'Arduino' in p.description
  ][0]
 
# Inicialização do Serial
arduino = serial.Serial(anm, 9600)
 

time.sleep(1.25)
 
 
try:
  while True:
    # send(float, input('=> '))
    # time.sleep(0.5)
    # print(arduino.read(arduino.in_waiting))
    # continue

    fase = input('Defina a fase: ')
    if fase == 'sair':
      raise BreakInterrupt()
    send(int, fase)
      
    # Recebe 2 ângulos e manda pelo serial
    if fase == '0':
        angulo1 = input('Entre o angulo1 (Digite sair para fechar): ')
        if angulo1 == "sair":
          raise BreakInterrupt()
        send(float, angulo1)
      
        angulo2 = input('Entre o angulo2 (Digite sair para fechar): ')
        if angulo2 == "sair":
          raise BreakInterrupt()
        send(float, angulo2)
  
    # Recebe um valor de coordenadas e manda os ângulos correspondentes pelo serial
    else:
      z = 10  # Valor para garanir que o programa entrará no while
      while z<r1 or z>r2:
          x = input('Entre com o valor de x (Digite sair para fechar): ');
          if x == 'sair':
              raise BreakInterrupt()
      
          y = input('Entre com o valor de y (Digite sair para fechar): ')
          if y == 'sair':
              raise BreakInterrupt()
          
          x = int(x)
          y = int(y)
        
          if 160 >= x > 0 and 160 > y > -160:  # Segunda condição de checagem para garantir
              z = math.sqrt(x*x + y*y)         # que os pontos estejam na área de trabalho
              
          clscr()
      
      if y>=0:
          angulo2 = math.acos(((x*x + y*y) - 12800) / 12800)
          #angulo1 = math.atan2(y, x) - (180 - (180 - angulo2))/2
          angulo1 = math.atan(y/x) - ((180-(180-angulo2))/2)
      else:
          angulo2 = -math.acos(((x*x + y*y) - 12800) / 12800)
          #angulo1 = math.atan2(y, x) + (180 - (180 - angulo2))/2
          angulo1 = math.atan(y/x) + ((180-(180+angulo2))/2)
 
      ax1 = angulo1   # Ângulo 1 recebe o ângulo atual
      ax2 = angulo2   # Ângulo 2 recebe o ângulo atual
      
      angulo1 = ax1 - a1  # São calculados os valores dos ângulos 1 e 2
      angulo2 = ax2 - a2  # São calculados os valores dos ângulos 1 e 2
      x1 = 80*math.cos(angulo1) + 80*math.cos(angulo1+a2)
      inverte = 0
      if x1 < 0:
        inverte = 1
      
      a1 = ax1   # a1 e a2 recebem o valor da variavel auxiliar 
      a2 = ax2   # correspondente ao angulo anteriormente digitado digitado
 
      print('angulo1 = {}\nangulo2 = {}\ninverte = {}'.format(math.degrees(angulo1), math.degrees(angulo2), inverte))
 
      if fase == '1': 
        send(int, inverte)
      send_angulos(angulo1, angulo2)
except (KeyboardInterrupt, BreakInterrupt):
  print('Stopping process...')
except serial.serialutil.SerialException:
  print('Serial exception... Open=' + str(arduino.is_open))
finally:
  if arduino.is_open: arduino.close()